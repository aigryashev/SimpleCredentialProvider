#include <QStringList>
#include <QDebug>
#include <QCoreApplication>
#include <QThread>

#include "ProcessWatchdog.hpp"

namespace crm
{
namespace credential_provider
{

ProcessWatchdog::ProcessWatchdog()
    : QThread(nullptr)
{
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
}

ProcessWatchdog::~ProcessWatchdog()
{
    quit();
}


void ProcessWatchdog::start(const QString & command, const QStringList & arguments)
{
    std::lock_guard<std::recursive_mutex> guard(mProcessMutex);
    if (nullptr != mProcess)
        return;

    mCommand = command;
    mArguments = arguments;

    QThread::start();
}

void ProcessWatchdog::run()
{
    startProcess();
    QThread::run();
}

void ProcessWatchdog::startProcess()
{
    std::lock_guard<std::recursive_mutex> guard(mProcessMutex);
    std::unique_ptr<QProcess> process(std::make_unique<QProcess>());
    connect(process.get(), SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int, QProcess::ExitStatus)), Qt::DirectConnection);
    connect(this, SIGNAL(killSignal()),
            process.get(), SLOT(kill()));

    process->start(mCommand, mArguments);

    if (!process->waitForStarted())
    {
        /// just in case
        process->kill();
        qDebug() << "The process" << mCommand << " can't be started";
        return;
    }

    mProcess = std::move(process);
}

void ProcessWatchdog::cleanupProcess()
{
    std::lock_guard<std::recursive_mutex> guard(mProcessMutex);
    if (nullptr == mProcess)
        return;

    mProcess->disconnect();
    emit killSignal();
//    mProcess->kill();
    mProcess->waitForFinished();
    mProcess.reset();
}

void ProcessWatchdog::stopProcess()
{
    std::lock_guard<std::recursive_mutex> guard(mProcessMutex);
    cleanupProcess();
}


void ProcessWatchdog::quit()
{
    std::lock_guard<std::recursive_mutex> guard(mProcessMutex);

    stopProcess();

    QThread::quit();
}

void ProcessWatchdog::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    std::lock_guard<std::recursive_mutex> guard(mProcessMutex);
    if (!mProcess)
    {
        return;
    }

    qDebug() << "The process " << mCommand << " has been finished"
             << ". Exit code: " << exitCode
             << ". ExitStatus: " << exitStatus;

    cleanupProcess();
    if (0 == exitCode)
    {
        quit();
        return;
    }

    qDebug() << "The finished process " << mCommand << " has to be restarted";

    cleanupProcess();
    startProcess();
}

} // ~namespace credential_provider
} // ~namespace CRM

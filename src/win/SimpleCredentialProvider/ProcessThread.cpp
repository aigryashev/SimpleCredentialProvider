#include <QStringList>
#include <QDebug>
#include <QCoreApplication>
#include <QThread>

#include "ProcessWatchdog.hpp"

namespace crm
{
namespace credential_provider
{

namespace
{

class ProcessThread : public QThread
{
    Q_OBJECT
public:
    ProcessThread() = default;

    bool startProcess(const QString & command, const QStringList & arguments)
    {
        if (isRunning())
            return false;

        std::unique_ptr<QProcess> process(std::make_unique<QProcess>());
        qDebug() << __FILE__ << __LINE__ << connect(process.get(), SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(onProcessFinished(int, QProcess::ExitStatus)), Qt::DirectConnection);

    qDebug() << __FILE__ << __LINE__;    process->start(command, arguments);

    qDebug() << __FILE__ << __LINE__;    if (!process->waitForStarted())
        {
            /// just in case
    qDebug() << __FILE__ << __LINE__;        process->kill();
    qDebug() << __FILE__ << __LINE__;        return false;
    qDebug() << __FILE__ << __LINE__;    }

    qDebug() << __FILE__ << __LINE__;    mCommand = command;
    qDebug() << __FILE__ << __LINE__;    mArguments = arguments;
        //TODO!!! позже ненужный вариант удалить
    qDebug() << __FILE__ << __LINE__;    mProcess = std::move(process);
    //    mProcess = startProcess(command, arguments);
    //    if (nullptr == mProcess)
    //        return false;

    qDebug() << __FILE__ << __LINE__;    qDebug() << "The process" << command << " has been started";

        //TODO!!! позже ненужное удалить
    //    auto processWatchdogCallback = [this, command, arguments]()
    //    {
    //qDebug() << __FILE__ << __LINE__;        while (true)
    //        {
    //qDebug() << __FILE__ << __LINE__;            {
    //qDebug() << __FILE__ << __LINE__;                std::lock_guard<std::recursive_mutex> guard(mProcessMutex);
    //qDebug() << __FILE__ << __LINE__;                if (nullptr == mProcess)
    //                    return;

    //qDebug() << __FILE__ << __LINE__ << "state" << mProcess->state();                if (QProcess::NotRunning == mProcess->state())
    //                {
    //qDebug() << __FILE__ << __LINE__;                    qDebug() << "The process " << command << " has been finished. We have to restsrt it";

    //                    /// We have to restart the process it has been stopped
    //qDebug() << __FILE__ << __LINE__;                    mProcess = startProcess(command, arguments);
    //qDebug() << __FILE__ << __LINE__;                }
    //qDebug() << __FILE__ << __LINE__;            }

    //qDebug() << __FILE__ << __LINE__;            QThread::currentThread()->sleep(2);
    //qDebug() << __FILE__ << __LINE__;        }
    //qDebug() << __FILE__ << __LINE__;    };

    //    globalThreadPool->start(processWatchdogCallback);
        return true;

    }

    void run()
    {
        QThread::run();
    }

protected:
    using QThread::start;

private:
    //TODO!!! позже удалить, если не будет нужно
//    const QString mCommand;
//    const QStringList mArguments;

    std::unique_ptr<QProcess> mProcess;
};

} // ~anonymous namespace

ProcessWatchdog::ProcessWatchdog()
    : QObject(nullptr)
{
}

ProcessWatchdog::~ProcessWatchdog()
{
    stopSync();
}


bool ProcessWatchdog::start(const QString & command, const QStringList & arguments)
{
    std::lock_guard<std::recursive_mutex> guard(mProcessMutex);
    if (isRunning())
        return false;

    std::unique_ptr<QThread> processThread(nullptr);



    std::unique_ptr<QProcess> process(std::make_unique<QProcess>());
    //TODO!!! продолжить отсюда, понять почему не приходят сигналы от QProcess и исправить
    process->moveToThread(QCoreApplication::instance()->thread());
    qDebug() << __FILE__ << __LINE__ << connect(process.get(), SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int, QProcess::ExitStatus)), Qt::DirectConnection);

qDebug() << __FILE__ << __LINE__;    process->start(command, arguments);

qDebug() << __FILE__ << __LINE__;    if (!process->waitForStarted())
    {
        /// just in case
qDebug() << __FILE__ << __LINE__;        process->kill();
qDebug() << __FILE__ << __LINE__;        return false;
qDebug() << __FILE__ << __LINE__;    }

qDebug() << __FILE__ << __LINE__;    mCommand = command;
qDebug() << __FILE__ << __LINE__;    mArguments = arguments;
    //TODO!!! позже ненужный вариант удалить
qDebug() << __FILE__ << __LINE__;    mProcess = std::move(process);
//    mProcess = startProcess(command, arguments);
//    if (nullptr == mProcess)
//        return false;

qDebug() << __FILE__ << __LINE__;    qDebug() << "The process" << command << " has been started";

    //TODO!!! позже ненужное удалить
//    auto processWatchdogCallback = [this, command, arguments]()
//    {
//qDebug() << __FILE__ << __LINE__;        while (true)
//        {
//qDebug() << __FILE__ << __LINE__;            {
//qDebug() << __FILE__ << __LINE__;                std::lock_guard<std::recursive_mutex> guard(mProcessMutex);
//qDebug() << __FILE__ << __LINE__;                if (nullptr == mProcess)
//                    return;

//qDebug() << __FILE__ << __LINE__ << "state" << mProcess->state();                if (QProcess::NotRunning == mProcess->state())
//                {
//qDebug() << __FILE__ << __LINE__;                    qDebug() << "The process " << command << " has been finished. We have to restsrt it";

//                    /// We have to restart the process it has been stopped
//qDebug() << __FILE__ << __LINE__;                    mProcess = startProcess(command, arguments);
//qDebug() << __FILE__ << __LINE__;                }
//qDebug() << __FILE__ << __LINE__;            }

//qDebug() << __FILE__ << __LINE__;            QThread::currentThread()->sleep(2);
//qDebug() << __FILE__ << __LINE__;        }
//qDebug() << __FILE__ << __LINE__;    };

//    globalThreadPool->start(processWatchdogCallback);
    return true;
}

bool ProcessWatchdog::isRunning() const
{
    std::lock_guard<std::recursive_mutex> guard(mProcessMutex);
    return (nullptr != mProcessThread
            && mProcessThread->isRunning()
            && nullptr != mProcess);
}

void ProcessWatchdog::stopSync()
{
    std::lock_guard<std::recursive_mutex> guard(mProcessMutex);
    if (!isRunning())
        return;

    qDebug() << "Finishing the process" << mProcess->program();
    mProcess->disconnect(this);
    mProcess->kill();
    mProcess->waitForFinished();
    qDebug() << "The process" << mProcess->program() << " has been finished";

    mProcess.reset();
}

//TODO!!! позже ненужное удалить
//bool ProcessWatchdog::startProcess()
//{
//    std::unique_ptr<QProcess> process(std::make_unique<QProcess>());
//    process->start(command, arguments);

//    if (!process->waitForStarted())
//    {
//        /// just in case
//        process->kill();
//        return false;
//    }


//    return result;
//}

//TODO!!! позже ненужное удалить
//void ProcessWatchdog::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
//{
//qDebug() << __FILE__ << __LINE__;    std::lock_guard<std::recursive_mutex> guard(mProcessMutex);
//qDebug() << __FILE__ << __LINE__;    if (!mProcess)
//        return;

//qDebug() << __FILE__ << __LINE__;    qDebug() << "The process " << mCommand << " has been finished. We have to restsrt it"
//             << ". Exit code: " << exitCode
//             << ". ExitStatus: " << exitStatus;

//qDebug() << __FILE__ << __LINE__;    mProcess->disconnect(this);
//qDebug() << __FILE__ << __LINE__;    mProcess.reset();

//qDebug() << __FILE__ << __LINE__;    if (!start(mCommand, mArguments))
//    {
//qDebug() << __FILE__ << __LINE__;        qDebug() << "Can't restart the process " << mCommand;
//qDebug() << __FILE__ << __LINE__;        return;
//    }

//qDebug() << __FILE__ << __LINE__;    qDebug() << "The process " << mCommand << " successfully restarted";
//qDebug() << __FILE__ << __LINE__;}

} // ~namespace credential_provider
} // ~namespace CRM

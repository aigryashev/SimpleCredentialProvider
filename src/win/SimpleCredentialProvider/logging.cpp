#include <process.h>

#include <atomic>
#include <thread>
#include <mutex>
#include <cassert>
#include <unordered_map>
#include <sstream>

#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QStandardPaths>
#include <QLoggingCategory>

#include "logging.hpp"

namespace crm
{
namespace logging
{

namespace
{

int getPID()
{
    static int pid(_getpid());
    return pid;
}

const std::unordered_map<QtMsgType, char> gLogTypeToStringMap
{
    {QtMsgType::QtInfoMsg, 'I'},
    {QtMsgType::QtDebugMsg, 'D'},
    {QtMsgType::QtWarningMsg, 'W'},
    {QtMsgType::QtCriticalMsg, 'C'},
    {QtMsgType::QtFatalMsg, 'F'}
};

std::unique_ptr<QFile> gLogFileUPtr;
std::recursive_mutex gLogFileUPtrMutex;
} // ~anonymous namespace

void handleCRMLogMessage(QtMsgType type, const QMessageLogContext & context, const QString & msg)
{
    std::lock_guard<std::recursive_mutex> lock(gLogFileUPtrMutex);

    if (nullptr == gLogFileUPtr)
        return;

    auto it = gLogTypeToStringMap.find(type);
    assert(gLogTypeToStringMap.end() != it);

    std::stringstream ss;
    ss << std::this_thread::get_id();

    QTextStream out(gLogFileUPtr.get());
    out << QDateTime::currentDateTime().toString("yyyy.MM.ddThh:mm:ss.zzz")
        << ' '
        << ss.str().c_str()
        << it->second
        << ' '
        << context.category
        << ": "
        << msg
        << Qt::endl;

    out.flush();
}

void initCRMLogging(const QString & binaryName)
{
    std::lock_guard<std::recursive_mutex> lock(gLogFileUPtrMutex);

    if (nullptr != gLogFileUPtr)
        return;

    std::string loggingDirPath(std::getenv("ALLUSERSPROFILE"));
    loggingDirPath += "\\crm\\log";

    QDir dir(loggingDirPath.c_str());
    if (!dir.exists())
        return;

    const QString filePath(QString("%1/%2-%3.log")
                           .arg(dir.absolutePath())
                           .arg(binaryName)
                           .arg(getPID()));

    gLogFileUPtr.reset(new QFile(filePath));
    gLogFileUPtr->open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text);

    qInstallMessageHandler(handleCRMLogMessage);
}

} // ~namespace logging
} // ~namespace theabyss


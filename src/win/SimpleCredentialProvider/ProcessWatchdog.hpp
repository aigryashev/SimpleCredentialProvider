#pragma once

#include <memory>
#include <mutex>

#include <QStringList>
#include <QProcess>
#include <QThread>

#include "SimpleCredentialProviderCredentialBase.hpp"

namespace crm
{
namespace credential_provider
{

class ProcessWatchdog : public QThread
{
    Q_OBJECT

public:
    ProcessWatchdog();
    virtual ~ProcessWatchdog();

    void start(const QString & command, const QStringList & arguments = QStringList());
    void quit();

signals:
    void killSignal();

protected:
    void startProcess();
    void cleanupProcess();
    void stopProcess();
    void run() override;
    using QThread::start;

protected slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QString mCommand = QString();
    QStringList mArguments = QStringList();
    std::unique_ptr<QProcess> mProcess = nullptr;
    mutable std::recursive_mutex mProcessMutex;
};

} // ~namespace credential_provider
} // ~namespace CRM

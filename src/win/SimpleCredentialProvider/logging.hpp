#pragma once

#include <QLoggingCategory>

class QString;

namespace crm
{
namespace logging
{

void initCRMLogging(const QString & binaryName);
void handleCRMLogMessage(QtMsgType type, const QMessageLogContext & context, const QString & msg);

} // ~namespace logging
} // ~namespace theabyss


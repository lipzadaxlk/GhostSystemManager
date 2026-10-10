#include "SafetyEngine.h"
#include "WindowsConfiguration.h"
#include <QCoreApplication>
#include <QTextStream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    const auto unsupported = SafetyEngine::networkAddressCapability(QString());
    const auto invalidName = WindowsConfiguration::applyComputerName(QString("bad name with spaces"));
    QTextStream(stdout) << unsupported.reason << " | " << invalidName.message << Qt::endl;
    return !unsupported.supported && !invalidName.success ? 0 : 1;
}

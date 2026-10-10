#include "HardwareInventory.h"
#include "NetworkConfiguration.h"
#include <QCoreApplication>
#include <QDir>
#include <QProcessEnvironment>
#include <iostream>

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    const auto records = HardwareInventory().scan();
    QString adapterGuid;
    for (const auto& item : records) if (item.category == "Network" && item.name.endsWith(" GUID")) { adapterGuid = item.value; break; }
    if (adapterGuid.isEmpty()) { std::cout << "FAILED | no network adapter GUID was observed\n"; return 1; }
    const auto env = QProcessEnvironment::systemEnvironment();
    if (env.value("GHOST_RUN_NETWORK_INTEGRATION") != "1") {
        std::cout << "NOT_TESTED | no authorized test adapter was supplied; no Windows configuration was changed\n";
        return 0;
    }
    const QString backup = QDir::tempPath() + "/ghost-network-integration.json";
    const auto plan = NetworkConfiguration::generatePlan(adapterGuid);
    const auto applied = NetworkConfiguration::apply(plan, backup);
    if (!applied.success) { std::cout << "FAILED | " << applied.message.toStdString() << "\n"; return 2; }
    const auto verified = NetworkConfiguration::verify(plan);
    const auto restored = NetworkConfiguration::restore(adapterGuid, backup);
    std::cout << (verified.success ? "SUCCESS" : "PARTIAL") << " | observed active MAC after apply; restore=" << (restored.success ? "SUCCESS" : "FAILED") << "\n";
    return verified.success && restored.success ? 0 : 3;
}


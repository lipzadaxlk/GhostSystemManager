#include "HardwareInventory.h"
#include <QCoreApplication>
#include <QTextStream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    const auto records = HardwareInventory().scan();
    QTextStream output(stdout);
    bool motherboard = false; bool uuid = false; bool bios = false;
    for (const auto& record : records) {
        output << record.category << " | " << record.name << " | " << record.value << Qt::endl;
        motherboard = motherboard || (record.category == "Motherboard" && record.name == "Product" && !record.value.isEmpty());
        uuid = uuid || (record.name == "System UUID" && !record.value.isEmpty());
        bios = bios || (record.category == "BIOS" && record.name == "Version" && !record.value.isEmpty());
    }
    return motherboard && uuid && bios ? 0 : 1;
}

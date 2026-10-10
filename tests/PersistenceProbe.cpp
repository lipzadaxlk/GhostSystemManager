#include "PersistenceVerification.h"
#include <QCoreApplication>
#include <QTextStream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    std::vector<IdentityRecord> before{{"Windows", "Computer name", "OLD", Mutability::SupportedConfiguration, "test", "test"}};
    std::vector<IdentityRecord> after{{"Windows", "Computer name", "NEW", Mutability::SupportedConfiguration, "test", "test"}};
    const auto a = PersistenceVerification::fromRecords("a", "BEFORE", before);
    const auto b = PersistenceVerification::fromRecords("b", "AFTER", after);
    const auto changed = PersistenceVerification::compare(a, b, false);
    const auto persistent = PersistenceVerification::compare(a, b, true);
    QTextStream(stdout) << verificationStatusText(changed.front().status) << " | " << verificationStatusText(persistent.front().status) << Qt::endl;
    return changed.size() == 1 && changed.front().status == VerificationStatus::Changed && persistent.front().status == VerificationStatus::Persistent ? 0 : 1;
}

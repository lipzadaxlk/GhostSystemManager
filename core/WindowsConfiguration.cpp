#include "WindowsConfiguration.h"
#include "SafetyEngine.h"

#include <windows.h>
#include <QRegularExpression>

QString WindowsConfiguration::currentComputerName() {
    wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 1]{}; DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    return GetComputerNameExW(ComputerNamePhysicalDnsHostname, buffer, &size) ? QString::fromWCharArray(buffer, static_cast<int>(size)) : QString();
}

OperationResult WindowsConfiguration::applyComputerName(const QString& proposedName) {
    const QString name = proposedName.trimmed();
    if (name.isEmpty() || name.size() > MAX_COMPUTERNAME_LENGTH || name.contains(QRegularExpression("[^A-Za-z0-9-]"))) return {false, "Computer name must contain only letters, numbers and hyphens and be at most 15 characters."};
    const auto safety = SafetyEngine::validate(SafetyEngine::computerNameCapability());
    if (!safety.success) return safety;
    const std::wstring wideName = name.toStdWString();
    if (!SetComputerNameExW(ComputerNamePhysicalDnsHostname, wideName.c_str())) return {false, QString("SetComputerNameExW failed with Windows error %1.").arg(GetLastError())};
    return {true, "Computer name changed. Windows may require a restart; no restart was initiated."};
}

OperationResult WindowsConfiguration::verifyComputerName(const QString& proposedName) {
    const QString observed = currentComputerName();
    return {observed.compare(proposedName.trimmed(), Qt::CaseInsensitive) == 0, observed.isEmpty() ? "Unable to read the current computer name." : "Observed computer name: " + observed};
}

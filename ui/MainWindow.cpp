#include "MainWindow.h"
#include "NetworkConfiguration.h"
#include "PersistenceVerification.h"

#include <QComboBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

namespace {
QFrame* panel(QWidget* parent) { auto* frame = new QFrame(parent); frame->setObjectName("panel"); return frame; }
QLabel* caption(const QString& text, QWidget* parent) { auto* label = new QLabel(text, parent); label->setObjectName("caption"); return label; }
}

MainWindow::MainWindow(const QString& rootPath, QWidget* parent) : QMainWindow(parent), logger_(rootPath + "/logs/operations.log"), rootPath_(rootPath) {
    setWindowTitle("Ghost System Manager v1.0.3 | Device Identity"); resize(1240, 780);
    auto* central = new QWidget(this); auto* shell = new QHBoxLayout(central); shell->setContentsMargins(0, 0, 0, 0); shell->setSpacing(0);
    auto* rail = new QFrame(central); rail->setObjectName("rail"); rail->setFixedWidth(78); auto* railLayout = new QVBoxLayout(rail); railLayout->setContentsMargins(10, 18, 10, 18); railLayout->setSpacing(12);
    auto* mark = new QLabel("G\nS", rail); mark->setObjectName("mark"); mark->setAlignment(Qt::AlignCenter); railLayout->addWidget(mark);
    auto* home = new QPushButton("HOME", rail); auto* device = new QPushButton("DEVICE", rail); device->setObjectName("selectedRail"); auto* history = new QPushButton("LOGS", rail); railLayout->addWidget(home); railLayout->addWidget(device); railLayout->addWidget(history); railLayout->addStretch(); shell->addWidget(rail);
    auto* page = new QWidget(central); auto* pageLayout = new QVBoxLayout(page); pageLayout->setContentsMargins(28, 22, 28, 18); pageLayout->setSpacing(16);
    auto* top = new QHBoxLayout(); auto* logo = new QLabel("GHOST <span>///</span> SYSTEM MANAGER", page); logo->setObjectName("logo"); top->addWidget(logo); top->addStretch(); connectionBadge_ = new QLabel("  ●  LOCAL SYSTEM CONNECTED  ", page); connectionBadge_->setObjectName("connected"); top->addWidget(connectionBadge_); supportBadge_ = new QLabel("  SUPPORTED CONFIG: MAC  ", page); supportBadge_->setObjectName("support"); top->addWidget(supportBadge_); pageLayout->addLayout(top);
    auto* heading = new QHBoxLayout(); auto* headingText = new QVBoxLayout(); headingText->addWidget(caption("DEVICE / LIVE OBSERVATION", page)); deviceName_ = new QLabel("Unknown device", page); deviceName_->setObjectName("deviceName"); headingText->addWidget(deviceName_); deviceSubtitle_ = new QLabel("Scan hardware to load verified identifiers", page); deviceSubtitle_->setObjectName("muted"); headingText->addWidget(deviceSubtitle_); heading->addLayout(headingText); heading->addStretch(); auto* scan = new QPushButton("SCAN HARDWARE", page); scan->setObjectName("primary"); connect(scan, &QPushButton::clicked, this, &MainWindow::scanHardware); heading->addWidget(scan); pageLayout->addLayout(heading);
    auto* content = new QHBoxLayout(); content->setSpacing(16);
    auto* identityPanel = panel(page); auto* identityLayout = new QVBoxLayout(identityPanel); identityLayout->setContentsMargins(20, 18, 20, 18); identityLayout->addWidget(caption("SERIAL NUMBERS", page)); table_ = new QTableWidget(identityPanel); table_->setColumnCount(3); table_->setHorizontalHeaderLabels({"TYPE", "SOURCE", "OBSERVED VALUE"}); table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); table_->verticalHeader()->setVisible(false); table_->setShowGrid(false); table_->setEditTriggers(QAbstractItemView::NoEditTriggers); identityLayout->addWidget(table_, 1); content->addWidget(identityPanel, 3);
    auto* factsPanel = panel(page); auto* factsLayout = new QVBoxLayout(factsPanel); factsLayout->setContentsMargins(20, 18, 20, 18); factsLayout->addWidget(caption("DEVICE PROFILE", page)); auto* facts = new QLabel("Motherboard model:\n—\n\nCPU:\n—\n\nSystem UUID:\n—\n\nWindows MachineGuid:\n—\n\nTPM state:\n—", factsPanel); facts->setObjectName("facts"); facts->setWordWrap(true); factsLayout->addWidget(facts); factsLayout->addStretch(); factsLayout->addWidget(caption("NETWORK ADAPTER", page)); adapters_ = new QComboBox(factsPanel); adapters_->setToolTip("Adapter GUID selected for supported NetworkAddress configuration."); factsLayout->addWidget(adapters_); content->addWidget(factsPanel, 2); pageLayout->addLayout(content, 1);
    auto* bottom = new QHBoxLayout(); auto* actions = new QHBoxLayout(); for (const auto& pair : std::initializer_list<std::pair<QString, void (MainWindow::*)()>>{{"CHECK COMPATIBILITY", &MainWindow::checkCompatibility}, {"GENERATE CONFIGURATION", &MainWindow::generateConfiguration}, {"VERIFY", &MainWindow::verifyChanges}, {"BACKUP", &MainWindow::backupConfiguration}, {"RESTORE", &MainWindow::restoreConfiguration}, {"VIEW LOGS", &MainWindow::viewLogs}}) { auto* button = new QPushButton(pair.first, page); connect(button, &QPushButton::clicked, this, pair.second); actions->addWidget(button); } applyButton_ = new QPushButton("APPLY IDENTITY", page); applyButton_->setObjectName("primary"); applyButton_->setEnabled(true); connect(applyButton_, &QPushButton::clicked, this, &MainWindow::applyIdentityOneClick); actions->addWidget(applyButton_); bottom->addLayout(actions, 1); status_ = new QTextEdit(page); status_->setReadOnly(true); status_->setPlaceholderText("Operation history appears here..."); status_->setMaximumHeight(74); bottom->addWidget(status_, 1); pageLayout->addLayout(bottom); shell->addWidget(page, 1); setCentralWidget(central);
    home->setEnabled(false); history->setEnabled(false);
    setStyleSheet("QMainWindow,QWidget { background:#090a0d; color:#eeeaf4; font-family:'Segoe UI'; } #rail { background:#0d0f14; border-right:1px solid #24242d; } #mark { color:#ff6b3d; font-size:24px; font-weight:900; padding:8px; } QPushButton { background:#151820; color:#aaa6b6; border:1px solid #282b35; border-radius:6px; padding:9px 12px; font-size:10px; font-weight:700; } QPushButton:hover { border-color:#ff6b3d; color:#fff; } #selectedRail { background:#3a201b; border-color:#ff6b3d; color:#ff9b79; } #primary { background:#bd4c2c; color:white; border-color:#f57d59; } #logo { color:#ff6b3d; font-size:18px; font-weight:900; letter-spacing:2px; } #logo span { color:#63d07f; } #connected { color:#75e28c; background:#102319; border:1px solid #245d33; border-radius:14px; padding:7px; font-size:10px; font-weight:700; } #support { color:#ffc18d; background:#2a1b15; border:1px solid #71402e; border-radius:14px; padding:7px; font-size:10px; font-weight:700; } #panel { background:#101217; border:1px solid #242832; border-radius:10px; } #caption { color:#817d8b; font-size:11px; font-weight:800; letter-spacing:2px; } #deviceName { color:#f8f5fb; font-size:28px; font-weight:600; } #muted { color:#777481; font-size:12px; } #facts { color:#ddd7e5; font-size:13px; line-height:1.7; padding-top:12px; } QTableWidget,QTextEdit,QComboBox { background:#0b0d11; border:1px solid #252a34; color:#eeeaf4; selection-background-color:#51261d; border-radius:5px; } QHeaderView::section { background:#151820; color:#888391; border:0; padding:8px; font-size:10px; font-weight:800; } QTableWidget::item { padding:9px; border-bottom:1px solid #1c2028; }");
}
QString MainWindow::backupPath() const { return rootPath_ + "/backup/network-address.json"; }
QString MainWindow::snapshotPath() const { return rootPath_ + "/backup/system-before.json"; }
void MainWindow::showRecords(const std::vector<IdentityRecord>& records) { table_->setRowCount(0); QString computer, cpu, motherboard, uuid, machineGuid, tpm; for (const auto& item : records) { int row = table_->rowCount(); table_->insertRow(row); table_->setItem(row, 0, new QTableWidgetItem(item.category)); table_->setItem(row, 1, new QTableWidgetItem(item.name)); table_->setItem(row, 2, new QTableWidgetItem(item.value)); if (item.name == "Computer name") computer = item.value; if (item.name == "ProcessorNameString") cpu = item.value; if (item.category == "Motherboard" && item.name == "Product") motherboard = item.value; if (item.name == "System UUID") uuid = item.value; if (item.name == "MachineGuid") machineGuid = item.value; if (item.category == "TPM") tpm = item.value; } deviceName_->setText(computer.isEmpty() ? "Windows device" : computer); deviceSubtitle_->setText(motherboard.isEmpty() ? "Verified hardware profile" : motherboard); auto* facts = findChild<QLabel*>("facts"); if (facts) facts->setText(QString("Motherboard model:\n%1\n\nCPU:\n%2\n\nSystem UUID:\n%3\n\nWindows MachineGuid:\n%4\n\nTPM state:\n%5").arg(motherboard.isEmpty() ? "Not reported" : motherboard, cpu.isEmpty() ? "Not reported" : cpu, uuid.isEmpty() ? "Not exposed by SMBIOS/WMI" : uuid, machineGuid.isEmpty() ? "Not reported" : machineGuid, tpm.isEmpty() ? "Not exposed by WMI/TBS" : tpm)); }
void MainWindow::scanHardware() { records_ = inventory_.scan(); showRecords(records_); adapters_->clear(); for (const auto& item : records_) if (item.category == "Network" && item.name.endsWith(" GUID")) adapters_->addItem(item.name, item.value); OperationResult result{!records_.empty(), records_.empty() ? "No identity records were returned." : QString("Loaded %1 verified identifiers and serial values.").arg(records_.size())}; logger_.write("SCAN HARDWARE", result); status_->append(result.message); }
void MainWindow::checkCompatibility() { if (records_.empty()) scanHardware(); int configurable = 0; for (const auto& item : records_) if (item.mutability == Mutability::SupportedConfiguration) ++configurable; OperationResult result{true, QString("Compatibility review: %1 supported configuration candidates. Hardware, firmware, TPM, storage, UUID and Windows IDs remain read-only.").arg(configurable)}; logger_.write("CHECK COMPATIBILITY", result); status_->append(result.message); }
void MainWindow::generateConfiguration() { if (records_.empty()) scanHardware(); if (adapters_->currentData().toString().isEmpty() && adapters_->count() > 0) adapters_->setCurrentIndex(0); plan_ = NetworkConfiguration::generatePlan(adapters_->currentData().toString()); applyButton_->setEnabled(plan_.valid); OperationResult result{plan_.valid, plan_.valid ? plan_.detail : "No network adapter exposes a usable GUID for supported configuration."}; logger_.write("GENERATE CONFIGURATION", result); status_->append(result.message); }
void MainWindow::applyChanges() { applyIdentityOneClick(); }
void MainWindow::applyIdentityOneClick() {
    if (records_.empty()) scanHardware();
    if (adapters_->count() == 0) { OperationResult result{false, "APPLY IDENTITY failed: no network adapter GUID is available."}; logger_.write("APPLY IDENTITY", result); status_->append(result.message); return; }
    if (!plan_.valid) generateConfiguration();
    if (!plan_.valid) { OperationResult result{false, "APPLY IDENTITY failed: no supported logical adapter configuration was generated."}; logger_.write("APPLY IDENTITY", result); status_->append(result.message); return; }
    const auto applied = NetworkConfiguration::apply(plan_, backupPath()); logger_.write("APPLY IDENTITY", applied); status_->append("APPLY: " + applied.message);
    if (!applied.success) return;
    const auto verified = NetworkConfiguration::verify(plan_); logger_.write("VERIFY", verified); status_->append("VERIFY: " + verified.message);
    if (!verified.success) status_->append("RESULT: PARTIAL — the driver did not expose the requested value as active."); else status_->append("RESULT: SUCCESS — the active adapter value matches the generated configuration.");
}
void MainWindow::verifyChanges() {
    if (records_.empty()) scanHardware();
    auto networkResult = NetworkConfiguration::verify(plan_);
    if (!beforeSnapshot_.fields.empty()) {
        const auto after = PersistenceVerification::fromRecords(QStringLiteral("after"), QStringLiteral("After operation"), records_);
        const auto comparisons = PersistenceVerification::compare(beforeSnapshot_, after, false);
        int changed = 0;
        for (const auto& item : comparisons) if (item.status == VerificationStatus::Changed) ++changed;
        networkResult.message += QStringLiteral(" Snapshot verification: %1 changed field(s); reboot is required before persistence can be classified.").arg(changed);
        networkResult.success = networkResult.success && changed >= 0;
    }
    logger_.write("VERIFY", networkResult); status_->append(networkResult.message);
}
void MainWindow::backupConfiguration() {
    if (records_.empty()) scanHardware();
    beforeSnapshot_ = PersistenceVerification::fromRecords(QStringLiteral("before"), QStringLiteral("Before operation"), records_);
    QString error;
    const bool saved = SnapshotStore::write(snapshotPath(), beforeSnapshot_, &error);
    OperationResult result{saved, saved ? QStringLiteral("Snapshot BEFORE saved. It contains %1 verified fields and is used for independent verification.").arg(beforeSnapshot_.fields.size()) : QStringLiteral("Could not save the BEFORE snapshot: %1").arg(error)};
    logger_.write("BACKUP", result); status_->append(result.message);
}
void MainWindow::restoreConfiguration() { auto result = NetworkConfiguration::restore(adapters_->currentData().toString(), backupPath()); logger_.write("RESTORE", result); status_->append(result.message); }
void MainWindow::viewLogs() { status_->setPlainText(logger_.readAll()); }

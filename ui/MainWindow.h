#pragma once

#include "AppTypes.h"
#include "HardwareInventory.h"
#include "OperationLogger.h"
#include "SnapshotStore.h"
#include <QMainWindow>

class QComboBox;
class QTableWidget;
class QTextEdit;
class QPushButton;
class QLabel;

class MainWindow final : public QMainWindow {
public:
    explicit MainWindow(const QString& rootPath, QWidget* parent = nullptr);
private:
    void scanHardware();
    void checkCompatibility();
    void generateConfiguration();
    void applyChanges();
    void applyIdentityOneClick();
    void verifyChanges();
    void backupConfiguration();
    void restoreConfiguration();
    void viewLogs();
    void showRecords(const std::vector<IdentityRecord>& records);
    QString backupPath() const;
    QString snapshotPath() const;
    HardwareInventory inventory_;
    OperationLogger logger_;
    QString rootPath_;
    std::vector<IdentityRecord> records_;
    ChangePlan plan_;
    SystemSnapshot beforeSnapshot_;
    QTableWidget* table_ = nullptr;
    QComboBox* adapters_ = nullptr;
    QTextEdit* status_ = nullptr;
    QPushButton* applyButton_ = nullptr;
    QLabel* deviceName_ = nullptr;
    QLabel* deviceSubtitle_ = nullptr;
    QLabel* connectionBadge_ = nullptr;
    QLabel* supportBadge_ = nullptr;
};

#include "MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    const QString applicationDirectory = QCoreApplication::applicationDirPath();
    QString root = applicationDirectory.endsWith("/release") || applicationDirectory.endsWith("\\release")
        ? QDir::cleanPath(applicationDirectory + "/../..")
        : QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(root);
    MainWindow window(root);
    window.show();
    return application.exec();
}

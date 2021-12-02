#include <QApplication>

#ifndef QT_NO_SYSTEMTRAYICON

#include <QMessageBox>
#include <QSystemTrayIcon>

#include "config.h"
#include "window.h"

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(resource);

    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QApplication::setQuitOnLastWindowClosed(false);

    Window window{};
    window.show();
    return app.exec();
}

#else

#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto label = new QLabel(QObject::tr("QSystemTrayIcon is not supported on this platform"));
    label->setWordWrap(true);
    label->show();

    app.exec();
}

#endif
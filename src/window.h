#ifndef SEC21_STARTER_WINDOW_H
#define SEC21_STARTER_WINDOW_H

#include <QSystemTrayIcon>

#ifndef QT_NO_SYSTEMTRAYICON

#include <QDialog>

class QStateMachine;
class QAction;
class QSystemTrayIcon;

class Window final : public QDialog
{
    Q_OBJECT
public:
    Window();

    void setVisible(bool visible) override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:

    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void createActions();

    QStateMachine *machine{nullptr};

    QAction *minimizeAction{nullptr};
    QAction *maximizeAction{nullptr};
    QAction *restoreAction{nullptr};
    QAction *quitAction{nullptr};

    QSystemTrayIcon *trayIcon{nullptr};
};

#endif // QT_NO_SYSTEMTRAYICON

#endif
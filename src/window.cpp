#include "window.h"
#include "config.h"
#include "generic_event.h"
#include "generic_transition.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QStateMachine>
#include <QSettings>
#include <QFileDialog>
#include <QDebug>

#include <fstream>
#include <string>

constexpr char TOKEN[] = "token";
constexpr char EMAIL[] = "email";
constexpr char PASSWORD[] = "password";

auto readFile(auto const& filename)
{
    std::ifstream ifs{filename};
    return std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
}

bool isTokenValid(auto const& text) noexcept
{
    return text.size() == 36;
}

Window::Window()
{
    // setup UI
    createActions();

    auto trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    //! \todo replace icon
    const auto icon = QIcon(":/images/heart.png");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(icon);
    trayIcon->show();

    auto buttonBrowseToken = new QPushButton(tr("Browse..."), this);
    auto buttonConnect = new QPushButton(tr("Connect..."), this);
    buttonConnect->setDefault(true);
    auto lineEditToken = new QLineEdit("", this);
    lineEditToken->setMaxLength(36);

    auto lineEditEmail = new QLineEdit("", this);
    auto lineEditPassword = new QLineEdit("", this);

    auto labelToken = new QLabel(tr("Token:"), this);
    auto labelEmail = new QLabel(tr("eMail:"), this);
    auto labelPassword = new QLabel(tr("Password:"), this);

    auto checkRememberPassword = new QCheckBox(tr("Remember Password"), this);

    auto border = new QGroupBox();
    auto messageLayout = new QGridLayout;
    messageLayout->addWidget(labelToken, 2, 0);
    messageLayout->addWidget(lineEditToken, 2, 1, 1, 3);
    messageLayout->addWidget(buttonBrowseToken, 2, 4);
    messageLayout->addWidget(labelEmail, 3, 0);
    messageLayout->addWidget(lineEditEmail, 3, 1, 1, 4);
    messageLayout->addWidget(labelPassword, 4, 0);
    messageLayout->addWidget(lineEditPassword, 4, 1, 1, 4);
    messageLayout->addWidget(checkRememberPassword, 5, 1);
    messageLayout->addWidget(buttonConnect, 6, 4);
    messageLayout->setColumnStretch(3, 1);
    messageLayout->setRowStretch(4, 1);
    border->setLayout(messageLayout);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(border);
    setLayout(mainLayout);

    // load settings
    {
        QSettings settings;
        lineEditToken->setText(settings.value(TOKEN).toString());
        lineEditEmail->setText(settings.value(EMAIL).toString());
        lineEditPassword->setText(settings.value(PASSWORD).toString());
    }

    // setup state machine
    machine = new QStateMachine{this};
    auto s1 = new QState();
    auto s2 = new QState();

    s1->assignProperty(lineEditEmail, "enabled", false);
    s1->assignProperty(lineEditPassword, "enabled", false);
    s1->assignProperty(checkRememberPassword, "enabled", false);
    s1->assignProperty(buttonConnect, "enabled", false);

    s2->assignProperty(lineEditEmail, "enabled", true);
    s2->assignProperty(lineEditPassword, "enabled", true);
    s2->assignProperty(checkRememberPassword, "enabled", true);
    s2->assignProperty(buttonConnect, "enabled", true);

    using validTokenEvent = GenericEvent<bool, 2>;

    auto guardedTransistionValidToken = new GenericTransition<validTokenEvent>{ true };
    guardedTransistionValidToken->setTargetState(s2);
    s1->addTransition(guardedTransistionValidToken);

    auto guardedTransistionInvalidToken = new GenericTransition<validTokenEvent>{ false };
    guardedTransistionInvalidToken->setTargetState(s1);
    s2->addTransition(guardedTransistionInvalidToken);

    machine->addState(s1);
    machine->addState(s2);
    machine->setInitialState(isTokenValid(lineEditToken->text()) ? s2 : s1);
    machine->start();

    // connect signals
    QObject::connect(trayIcon, &QSystemTrayIcon::activated, this, &Window::iconActivated);

    QObject::connect(lineEditToken, &QLineEdit::textChanged, [&](auto const& text){
        qDebug() << "is valid:" << isTokenValid(text);
        machine->postEvent(new validTokenEvent{ isTokenValid(text) });
    });

    QObject::connect(buttonBrowseToken, &QPushButton::clicked, [this, lineEditToken]() {
        const auto fileName = QFileDialog::getOpenFileName(this, tr("Open token"), "", tr("Token files (*.stk)"));
        qDebug() << "read from file: " << fileName;
        if (not fileName.isEmpty())
        {
            lineEditToken->setText(QString::fromStdString(readFile(fileName.toStdString())).trimmed());
        }
    });
    QObject::connect(buttonConnect, &QPushButton::clicked, [lineEditToken, lineEditEmail, lineEditPassword, checkRememberPassword]()
    {
        const auto token = lineEditToken->text();
        const auto email = lineEditEmail->text();
        const auto password = lineEditPassword->text();

        if (checkRememberPassword->isChecked())
        {
            QSettings settings;
            settings.setValue(TOKEN, token);
            settings.setValue(EMAIL, email);
            settings.setValue(PASSWORD, password);
        }

        //! \todo open
    });

    // window settings
    setWindowIcon(icon);
    setWindowTitle(APPLICATION_NAME);
    resize(500, 150);
}

void Window::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}

void Window::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_OSX
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}


void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::DoubleClick:
        case QSystemTrayIcon::Trigger:
            showMaximized();
            break;
        default:
            ;
    }
}

void Window::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    QObject::connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    QObject::connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction(tr("&Restore"), this);
    QObject::connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    QObject::connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

#endif
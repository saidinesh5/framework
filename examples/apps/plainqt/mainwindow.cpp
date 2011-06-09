#include "mainwindow.h"

#include <QX11Info>
#include <stdlib.h>
#include <QtCore>
#include <QtGui>

namespace {
    const char * const TextPrompt = "Double-click this text area to invoke virtual keyboard ...";
    const char * const ImModuleEnv = "QT_IM_MODULE";
    const char * const ExpectedImModule = "MInputContext";


    QString checkForErrors()
    {
        const char * const imModule = getenv(ImModuleEnv);
        QString errorMsg;

        if (not imModule
            || (0 != strcmp(getenv(ImModuleEnv), ExpectedImModule))) {
            errorMsg.append(QString("You will not be able to use the virtual keyboard. The " \
                                    "environment variable %1 must be set to %2.")
                            .arg(ImModuleEnv)
                            .arg(ExpectedImModule));
        }

        if (not QX11Info::isCompositingManagerRunning()) {
            if (not errorMsg.isEmpty()) {
                errorMsg.append("\n");
            }

            errorMsg.append("No compositing window manager found. You might be able to run the " \
                            "MeeGo Input Method server in self-compositing mode:\n" \
                            "$ meego-im-uiserver -use-self-composition -manual-redirection");
        }

        return errorMsg;
    }
}

class MyTextEdit
    : public QTextEdit
{
private:
    bool wasFocused;

public:
    MyTextEdit()
        : QTextEdit(TextPrompt)
        , wasFocused(false)
    {}

protected:
    void focusInEvent(QFocusEvent *e)
    {
        toPlainText();
        // On first text edit, clear pre-set TextPrompt:
        if (not wasFocused && toPlainText() == QString(TextPrompt)) {
            wasFocused = true;
            setText("");
        }

        QTextEdit::focusInEvent(e);
    }
};

MainWindow::MainWindow()
    : QMainWindow()
    , serverProcess(new QProcess(this))
    , orientationIndex(0)
    , startServerButton(new QPushButton)
    , rotateKeyboardButton(new QPushButton)
    , textEdit(new QTextEdit)
{
    serverProcess->setProcessChannelMode(QProcess::ForwardedChannels);
    connect(serverProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onServerError()));
    connect(serverProcess, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onServerStateChanged()));

    connect(startServerButton, SIGNAL(clicked()),
            this, SLOT(onStartServerClicked()));
    connect(rotateKeyboardButton, SIGNAL(clicked()),
            this, SLOT(onRotateKeyboardClicked()));

    initUI();
    onServerStateChanged();

    const QString &errorMsg(checkForErrors());
    if (not errorMsg.isEmpty()) {
        textEdit->setText(errorMsg);
        qCritical() << "CRITICAL:" << errorMsg;
    }
}

void MainWindow::initUI()
{
    setWindowTitle("Maliit test application");

    QVBoxLayout *vbox = new QVBoxLayout();

    rotateKeyboardButton->setText("Rotate keyboard");
    vbox->addWidget(startServerButton);
    vbox->addWidget(rotateKeyboardButton);

    // Clicking the button will steal focus from the text edit, thus hiding
    // the virtual keyboard:
    QPushButton *hideVkb = new QPushButton("Hide virtual keyboard");
    vbox->addWidget(hideVkb);

    vbox->addWidget(textEdit);

    QPushButton *closeApp = new QPushButton("Close application");
    vbox->addWidget(closeApp);
    connect(closeApp, SIGNAL(clicked()),
            this,     SLOT(close()));

    setCentralWidget(new QWidget);
    centralWidget()->setLayout(vbox);
    show();
}

MainWindow::~MainWindow()
{
    serverProcess->terminate();
}

void MainWindow::onStartServerClicked()
{
    const QString programName("meego-im-uiserver");
    QStringList arguments;
    arguments << "-use-self-composition" << "-bypass-wm-hint";

    if (serverProcess->state() != QProcess::NotRunning) {
        serverProcess->terminate();
    } else {
        serverProcess->start(programName, arguments);
    }
}

void MainWindow::onServerError()
{
    textEdit->setText(serverProcess->errorString());
}

void MainWindow::onServerStateChanged()
{
    switch (serverProcess->state()) {
    case QProcess::Running:
        startServerButton->setText("(running) Stop input method server");
        break;
    case QProcess::Starting:
        startServerButton->setText("(starting) Stop input method server");
        break;
    case QProcess::NotRunning:
        startServerButton->setText("(stopped) Start input method server");
    default:
        break;
    }
}

// Copied from minputmethodnamespace.h
namespace MInputMethod {
    enum OrientationAngle { Angle0=0, Angle90=90, Angle180=180, Angle270=270 };
}

void MainWindow::onRotateKeyboardClicked()
{
    const MInputMethod::OrientationAngle orientations[] = {MInputMethod::Angle0,
                                                           MInputMethod::Angle90,
                                                           MInputMethod::Angle180,
                                                           MInputMethod::Angle270};
    orientationIndex++;
    if (orientationIndex >= 4) {
        orientationIndex = 0;
    }
    const MInputMethod::OrientationAngle angle = orientations[orientationIndex];

    QMetaObject::invokeMethod(qApp->inputContext(), "notifyOrientationChanged",
                              Q_ARG(MInputMethod::OrientationAngle, angle));
}
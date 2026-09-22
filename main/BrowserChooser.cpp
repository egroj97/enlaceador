#include "BrowserChooser.hpp"
#include "BrowserButton.hpp"

#include <QDebug>
#include <QGuiApplication>
#include <QLabel>
#include <QProcess>
#include <QScreen>
#include <QVBoxLayout>
#include <Windows.h>

BrowserChooser::BrowserChooser(QVector<BrowserButton *> browsers, QString url,
                               QWidget *parent)
    : QWidget(parent), m_browsers(std::move(browsers)), m_url(std::move(url)) {
  this->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
  this->setWindowTitle(tr("Choose a browser"));
  this->setAttribute(Qt::WA_DeleteOnClose);

  this->setStyleSheet("background-color: rgb(45, 45, 45);");

  auto *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(2);

  if (this->m_browsers.isEmpty()) {
    auto *noLabel = new QLabel(tr("No browsers found"), this);
    noLabel->setStyleSheet("color: rgb(128, 128, 128); font-size: 12px;");
    mainLayout->addWidget(noLabel);
  } else {
    for (auto *btn : std::as_const(this->m_browsers)) {
      btn->setParent(this);
      btn->setStyleSheet("background-color: rgb(64, 64, 64); color: white; border: none; padding-left: 15px; font-size: 12px;");
      this->connect(btn, &QPushButton::released, this, &BrowserChooser::handleChosenButton);
      mainLayout->addWidget(btn);
    }
  }
  mainLayout->addStretch();

  this->setFixedWidth(320);
  this->adjustSize();

  const QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
  const int x = (screenGeometry.width() - this->width()) / 2;
  const int y = (screenGeometry.height() - this->height()) / 2;
  this->move(x, y);
}

BrowserChooser::~BrowserChooser() {
  qDeleteAll(this->m_browsers);
}

void BrowserChooser::handleChosenButton() {
  if (auto *btn = static_cast<BrowserButton *>(this->sender())) {
    qDebug() << "User chose browser:" << btn->text() << "path:" << btn->getPath();
    if (!QProcess::startDetached(btn->getPath(), {this->m_url})) {
      qCritical() << "Failed to launch browser:" << btn->getPath();
    }
    this->close();
    QCoreApplication::quit();
  }
}

void BrowserChooser::showAndActivate() {
  this->show();

  HWND chooserWindowHandle = (HWND)this->winId();
  HWND currentForegroundWindow = GetForegroundWindow();
  DWORD foregroundWindowThreadId =
      GetWindowThreadProcessId(currentForegroundWindow, nullptr);
  DWORD currentThreadId = GetCurrentThreadId();

  if (foregroundWindowThreadId != currentThreadId)
    AttachThreadInput(foregroundWindowThreadId, currentThreadId, TRUE);

  BringWindowToTop(chooserWindowHandle);
  SetForegroundWindow(chooserWindowHandle);
  ShowWindow(chooserWindowHandle, SW_SHOW);
  this->raise();
  this->activateWindow();

  if (foregroundWindowThreadId != currentThreadId)
    AttachThreadInput(foregroundWindowThreadId, currentThreadId, FALSE);
}

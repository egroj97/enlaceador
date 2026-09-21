#include "BrowserChooser.hpp"
#include "BrowserButton.hpp"

#include <QGuiApplication>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QScreen>
#include <QVBoxLayout>

BrowserChooser::BrowserChooser(const QVector<BrowserButton *> &browsers,
                               const QString &url, QWidget *parent)
    : QWidget(parent), m_browsers(browsers), m_url(url) {
  setWindowFlags(Qt::WindowStaysOnTopHint);
  setWindowTitle("Elige un navegador");
  setAttribute(Qt::WA_DeleteOnClose);

  setStyleSheet("background-color: rgb(45, 45, 45);");

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(2);

  if (m_browsers.isEmpty()) {
    QLabel *noLabel = new QLabel("No se encontraron navegadores", this);
    noLabel->setStyleSheet("color: rgb(128, 128, 128); font-size: 12px;");
    mainLayout->addWidget(noLabel);
  } else {
    for (BrowserButton *btn : m_browsers) {
      btn->setParent(this);
      btn->setStyleSheet("background-color: rgb(64, 64, 64); color: white; border: none; padding-left: 15px; font-size: 12px;");
      connect(btn, &QPushButton::released, this, &BrowserChooser::handleChosenButton);
      mainLayout->addWidget(btn);
    }
  }
  mainLayout->addStretch();

  setFixedWidth(320);
  adjustSize();

  QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
  int x = (screenGeometry.width() - width()) / 2;
  int y = (screenGeometry.height() - height()) / 2;
  move(x, y);
}

BrowserChooser::~BrowserChooser() {
  qDeleteAll(m_browsers);
}

void BrowserChooser::handleChosenButton() {
  BrowserButton *btn = static_cast<BrowserButton *>(sender());
  if (btn) {
    QProcess::startDetached(btn->getPath(), {m_url});
    close();
  }
}

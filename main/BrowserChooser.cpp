#include "BrowserChooser.hpp"
#include "BrowserButton.hpp"

#include <QGuiApplication>
#include <QLabel>
#include <QProcess>
#include <QScreen>
#include <QVBoxLayout>

BrowserChooser::BrowserChooser(QVector<BrowserButton *> browsers, QString url,
                               QWidget *parent)
    : QWidget(parent), m_browsers(std::move(browsers)), m_url(std::move(url)) {
  setWindowFlags(Qt::WindowStaysOnTopHint);
  setWindowTitle("Elige un navegador");
  setAttribute(Qt::WA_DeleteOnClose);

  setStyleSheet("background-color: rgb(45, 45, 45);");

  auto *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(2);

  if (m_browsers.isEmpty()) {
    auto *noLabel = new QLabel("No se encontraron navegadores", this);
    noLabel->setStyleSheet("color: rgb(128, 128, 128); font-size: 12px;");
    mainLayout->addWidget(noLabel);
  } else {
    for (auto *btn : std::as_const(m_browsers)) {
      btn->setParent(this);
      btn->setStyleSheet("background-color: rgb(64, 64, 64); color: white; border: none; padding-left: 15px; font-size: 12px;");
      connect(btn, &QPushButton::released, this, &BrowserChooser::handleChosenButton);
      mainLayout->addWidget(btn);
    }
  }
  mainLayout->addStretch();

  setFixedWidth(320);
  adjustSize();

  const QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
  const int x = (screenGeometry.width() - width()) / 2;
  const int y = (screenGeometry.height() - height()) / 2;
  move(x, y);
}

BrowserChooser::~BrowserChooser() {
  qDeleteAll(m_browsers);
}

void BrowserChooser::handleChosenButton() {
  if (auto *btn = static_cast<BrowserButton *>(sender())) {
    QProcess::startDetached(btn->getPath(), {m_url});
    close();
  }
}

#ifndef BROWSERBUTTON_H
#define BROWSERBUTTON_H

#include <QEnterEvent>
#include <QPushButton>

class BrowserButton : public QPushButton {
public:
  BrowserButton(QString name, QString path, QWidget *parent = nullptr)
      : QPushButton(name, parent), m_path(std::move(path)) {
    setMinimumHeight(36);
    setCursor(Qt::PointingHandCursor);
  }

  const QString &getPath() const { return m_path; }

protected:
  void enterEvent(QEnterEvent *event) override {
    event->accept();
    setStyleSheet("background-color: rgb(91, 188, 181);");
  }

  void leaveEvent(QEvent *event) override {
    event->accept();
    setStyleSheet("background-color: rgb(64, 64, 64);");
  }

private:
  QString m_path;
};

#endif

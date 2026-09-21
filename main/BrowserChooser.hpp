#ifndef BROWSERCHOOSER_H
#define BROWSERCHOOSER_H

#include <QWidget>

class BrowserButton;

class BrowserChooser : public QWidget {
  Q_OBJECT

public:
  explicit BrowserChooser(const QVector<BrowserButton *> &browsers,
                          const QString &url, QWidget *parent = nullptr);
  ~BrowserChooser();

private slots:
  void handleChosenButton();

private:
  QVector<BrowserButton *> m_browsers;
  QString m_url;
};

#endif

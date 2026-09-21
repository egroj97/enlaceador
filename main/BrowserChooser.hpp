#ifndef BROWSERCHOOSER_H
#define BROWSERCHOOSER_H

#include <QString>
#include <QVector>
#include <QWidget>

class BrowserButton;

class BrowserChooser : public QWidget {
  Q_OBJECT

public:
  explicit BrowserChooser(QVector<BrowserButton *> browsers, QString url,
                          QWidget *parent = nullptr);
  ~BrowserChooser() override;

private slots:
  void handleChosenButton();

private:
  QVector<BrowserButton *> m_browsers;
  QString m_url;
};

#endif

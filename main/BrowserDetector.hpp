#ifndef BROWSERDETECTOR_H
#define BROWSERDETECTOR_H

#include <QVector>

class BrowserButton;

class BrowserDetector {
public:
  static QVector<BrowserButton *> getInstalledBrowsers();
};

#endif

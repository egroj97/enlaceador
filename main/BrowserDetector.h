#ifndef BROWSERDETECTOR_H
#define BROWSERDETECTOR_H

#include <QString>
#include <QVector>

struct Browser {
    QString name;
    QString path;
};

class BrowserDetector {
public:
    static QVector<Browser> getInstalledBrowsers();
};

#endif

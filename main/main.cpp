#include <QApplication>
#include "BrowserChooser.h"
#include "BrowserDetector.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QStringList args = QCoreApplication::arguments();
    if (args.size() < 2) return 0;

    QString url = args[1];

    if (url.startsWith('"') && url.endsWith('"')) {
        url = url.mid(1, url.length() - 2);
    }

    if (!url.startsWith("http://") && !url.startsWith("https://") && !url.startsWith("file://")) {
        url = "https://" + url;
    }

    QVector<Browser> browsers = BrowserDetector::getInstalledBrowsers();

    BrowserChooser chooser(browsers, url);
    chooser.show();

    return app.exec();
}

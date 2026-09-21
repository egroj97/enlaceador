#include <QApplication>
#include "BrowserChooser.hpp"
#include "BrowserDetector.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    const QStringList args = QCoreApplication::arguments();
    QString url = args.size() < 2 ? "www.example.com" : args[1];

    if (url.startsWith('"') && url.endsWith('"')) {
        url = url.mid(1, url.length() - 2);
    }

    if (!url.startsWith("http://") && !url.startsWith("https://") && !url.startsWith("file://")) {
        url = "https://" + url;
    }

    const auto browsers = BrowserDetector::getInstalledBrowsers();

    BrowserChooser chooser(browsers, url);
    chooser.show();

    return app.exec();
}

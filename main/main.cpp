#include <QApplication>
#include <QLocale>
#include <QTimer>
#include <QTranslator>
#include "BrowserChooser.hpp"
#include "BrowserDetector.hpp"
#include "Logger.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Logger::init();

    QTranslator translator;
    const QString systemLocale = QLocale::system().name();
    qDebug() << "System locale:" << systemLocale;

    if (translator.load(":/i18n/" + systemLocale + ".qm")) {
        app.installTranslator(&translator);
        qDebug() << "Loaded translation for locale:" << systemLocale;
    } else if (translator.load(":/i18n/es.qm")) {
        app.installTranslator(&translator);
        qDebug() << "Loaded fallback translation: es";
    }

    const QStringList args = QCoreApplication::arguments();
    QString url = args.size() < 2 ? "www.example.com" : args[1];

    qDebug() << "Raw URL argument:" << url;

    if (url.startsWith('"') && url.endsWith('"')) {
        url = url.mid(1, url.length() - 2);
    }

    if (!url.startsWith("http://") && !url.startsWith("https://") && !url.startsWith("file://")) {
        url = "https://" + url;
    }

    qDebug() << "Resolved URL:" << url;

    const auto browsers = BrowserDetector::getInstalledBrowsers();

    qDebug() << "Detected" << browsers.size() << "browser(s)";

    BrowserChooser chooser(browsers, url);
    chooser.showAndActivate();

    QTimer::singleShot(300000, &app, &QCoreApplication::quit);

    return app.exec();
}

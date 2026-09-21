#include "BrowserDetector.hpp"
#include "BrowserButton.hpp"
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QSettings>
#include <QSet>

QVector<BrowserButton *> BrowserDetector::getInstalledBrowsers() {
  QVector<BrowserButton *> browsers;
  QSet<QString> seenPaths;

  auto scanKey = [&](const QString &regPath) {
    QSettings reg(regPath, QSettings::NativeFormat);

    for (const QString &browserKey : reg.childGroups()) {
      QString displayName = reg.value(browserKey + "/.").toString();
      if (displayName.isEmpty())
        displayName = browserKey;

      QString cmdValue =
          reg.value(browserKey + "/shell/open/command/.").toString();

      if (!cmdValue.isEmpty()) {
        int start = cmdValue.indexOf('"');
        if (start != -1) {
          int end = cmdValue.indexOf('"', start + 1);
          if (end != -1) {
            QString exePath = cmdValue.mid(start + 1, end - start - 1);

            if (!seenPaths.contains(exePath)) {
              QFileInfo fi(exePath);
              if (fi.exists() && fi.isFile()) {
                seenPaths.insert(exePath);
                browsers.append(new BrowserButton(displayName, exePath));
              }
            }
          }
        }
      }
    }
  };

  scanKey(QString::fromLatin1(
      "HKEY_LOCAL_MACHINE\\SOFTWARE\\Clients\\StartMenuInternet"));
  scanKey(QString::fromLatin1(
      "HKEY_CURRENT_USER\\Software\\Clients\\StartMenuInternet"));

  struct CommonPath {
    const char *envVar;
    const char *relPath;
    const char *name;
  };

  CommonPath paths[] = {
      {"PROGRAMFILES", "Google\\Chrome\\Application\\chrome.exe",
       "Google Chrome"},
      {"PROGRAMFILES", "Mozilla Firefox\\firefox.exe", "Mozilla Firefox"},
      {"PROGRAMFILES", "Internet Explorer\\iexplore.exe", "Internet Explorer"},
      {"PROGRAMFILES(X86)", "Google\\Chrome\\Application\\chrome.exe",
       "Google Chrome"},
      {"PROGRAMFILES(X86)", "Mozilla Firefox\\firefox.exe", "Mozilla Firefox"},
      {"PROGRAMFILES(X86)", "Microsoft\\Edge\\Application\\msedge.exe",
       "Microsoft Edge"},
      {"LOCALAPPDATA", "Google\\Chrome\\Application\\chrome.exe",
       "Google Chrome"},
      {"LOCALAPPDATA", "Microsoft\\Edge\\Application\\msedge.exe",
       "Microsoft Edge"},
  };

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  for (const auto &p : paths) {
    QString envValue = env.value(QString::fromLatin1(p.envVar));
    if (envValue.isEmpty())
      continue;

    QString fullPath = envValue + "\\" + QString::fromLatin1(p.relPath);

    if (!seenPaths.contains(fullPath)) {
      QFileInfo fi(fullPath);
      if (fi.exists() && fi.isFile()) {
        seenPaths.insert(fullPath);
        browsers.append(new BrowserButton(QString::fromLatin1(p.name), fullPath));
      }
    }
  }

  return browsers;
}

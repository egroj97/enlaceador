#include "BrowserDetector.hpp"
#include "BrowserButton.hpp"
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QSet>
#include <windows.h>

static QString readRegString(HKEY hKey, const wchar_t *path,
                             const wchar_t *valueName) {
  HKEY key = nullptr;
  if (RegOpenKeyExW(hKey, path, 0, KEY_READ, &key) != ERROR_SUCCESS)
    return QString();

  DWORD type = 0, size = 0;
  if (RegQueryValueExW(key, valueName, nullptr, &type, nullptr, &size) !=
          ERROR_SUCCESS ||
      type != REG_SZ) {
    RegCloseKey(key);
    return QString();
  }

  std::wstring result((size / sizeof(wchar_t)) - 1, L'\0');
  if (RegQueryValueExW(key, valueName, nullptr, &type,
                       reinterpret_cast<LPBYTE>(result.data()),
                       &size) == ERROR_SUCCESS) {
    size_t pos = result.find_last_not_of(L'\0');
    if (pos != std::wstring::npos)
      result.resize(pos + 1);
  }
  RegCloseKey(key);
  return QString::fromStdWString(result);
}

QVector<BrowserButton *> BrowserDetector::getInstalledBrowsers() {
  QVector<BrowserButton *> browsers;
  QSet<QString> seenPaths;

  auto scanKey = [&](HKEY hive, const wchar_t *basePath) {
    HKEY clientsKey = nullptr;
    if (RegOpenKeyExW(hive, basePath, 0, KEY_READ, &clientsKey) !=
        ERROR_SUCCESS)
      return;

    wchar_t subkeyName[256];
    DWORD nameLen = 256;
    for (DWORD i = 0;
         RegEnumKeyExW(clientsKey, i, subkeyName, &nameLen, nullptr, nullptr,
                       nullptr, nullptr) == ERROR_SUCCESS;
         ++i, nameLen = 256) {

      std::wstring subkeyPath = std::wstring(basePath) + L"\\" + subkeyName;
      QString displayName = readRegString(hive, subkeyPath.c_str(), nullptr);
      if (displayName.isEmpty())
        displayName = QString::fromWCharArray(subkeyName);

      std::wstring cmdPath = subkeyPath + L"\\shell\\open\\command";
      QString cmdValue = readRegString(hive, cmdPath.c_str(), nullptr);

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
    RegCloseKey(clientsKey);
  };

  scanKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Clients\\StartMenuInternet");
  scanKey(HKEY_CURRENT_USER, L"Software\\Clients\\StartMenuInternet");

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

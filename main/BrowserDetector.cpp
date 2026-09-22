#include "BrowserDetector.hpp"
#include "BrowserButton.hpp"
#include <QDebug>
#include <QSet>
#include <Windows.h>

QVector<BrowserButton *> BrowserDetector::getInstalledBrowsers() {
  QVector<BrowserButton *> browsers;
  browsers.reserve(10);
  QSet<QString> seenPaths;

  auto fileExists = [](const QString &path) -> bool {
    DWORD attrs = GetFileAttributesW(reinterpret_cast<LPCWSTR>(path.utf16()));
    return attrs != INVALID_FILE_ATTRIBUTES &&
           !(attrs & FILE_ATTRIBUTE_DIRECTORY);
  };

  auto readRegistryValue = [](HKEY hKey, const QString &subKey,
                              const QString &valueName) -> QString {
    HKEY hSubKey;
    if (RegOpenKeyExW(hKey,
                      reinterpret_cast<LPCWSTR>(subKey.utf16()), 0, KEY_READ,
                      &hSubKey) != ERROR_SUCCESS)
      return QString();

    wchar_t buffer[1024];
    DWORD bufferSize = sizeof(buffer);
    DWORD type;
    LONG result = RegQueryValueExW(
        hSubKey, reinterpret_cast<LPCWSTR>(valueName.utf16()), nullptr, &type,
        reinterpret_cast<LPBYTE>(buffer), &bufferSize);
    RegCloseKey(hSubKey);

    if (result != ERROR_SUCCESS || type != REG_SZ)
      return QString();

    return QString::fromWCharArray(buffer, bufferSize / sizeof(wchar_t) - 1);
  };

  auto scanRegistryKey = [&](HKEY rootKey, const QString &rootKeyName) {
    const QString basePath = "SOFTWARE\\Clients\\StartMenuInternet";

    qDebug() << "Scanning registry:" << rootKeyName + "\\" + basePath;

    HKEY hKey;
    if (RegOpenKeyExW(rootKey, reinterpret_cast<LPCWSTR>(basePath.utf16()), 0,
                      KEY_READ, &hKey) != ERROR_SUCCESS)
      return;

    DWORD index = 0;
    wchar_t keyName[256];
    DWORD keyNameSize = 256;

    while (RegEnumKeyExW(hKey, index, keyName, &keyNameSize, nullptr, nullptr,
                         nullptr, nullptr) == ERROR_SUCCESS) {
      QString browserKey = QString::fromWCharArray(keyName, keyNameSize);

      QString displayName = readRegistryValue(hKey, browserKey, "");
      if (displayName.isEmpty())
        displayName = browserKey;

      QString commandPath = browserKey + "\\shell\\open\\command";
      QString cmdValue = readRegistryValue(hKey, commandPath, "");

      if (!cmdValue.isEmpty()) {
        int start = cmdValue.indexOf('"');
        if (start != -1) {
          int end = cmdValue.indexOf('"', start + 1);
          if (end != -1) {
            QString exePath = cmdValue.mid(start + 1, end - start - 1);

            if (!seenPaths.contains(exePath)) {
              if (fileExists(exePath)) {
                qDebug() << "Found browser:" << displayName << "at" << exePath;
                seenPaths.insert(exePath);
                browsers.append(new BrowserButton(displayName, exePath));
              }
            }
          }
        }
      }

      index++;
      keyNameSize = 256;
    }

    RegCloseKey(hKey);
  };

  scanRegistryKey(HKEY_LOCAL_MACHINE, "HKEY_LOCAL_MACHINE");
  scanRegistryKey(HKEY_CURRENT_USER, "HKEY_CURRENT_USER");

  struct CommonPath {
    LPCWSTR envVar;
    LPCWSTR relPath;
    const char *name;
  };

  CommonPath paths[] = {
      {L"PROGRAMFILES", L"Google\\Chrome\\Application\\chrome.exe",
       "Google Chrome"},
      {L"PROGRAMFILES", L"Mozilla Firefox\\firefox.exe", "Mozilla Firefox"},
      {L"PROGRAMFILES", L"Internet Explorer\\iexplore.exe", "Internet Explorer"},
      {L"PROGRAMFILES(X86)", L"Google\\Chrome\\Application\\chrome.exe",
       "Google Chrome"},
      {L"PROGRAMFILES(X86)", L"Mozilla Firefox\\firefox.exe", "Mozilla Firefox"},
      {L"PROGRAMFILES(X86)", L"Microsoft\\Edge\\Application\\msedge.exe",
       "Microsoft Edge"},
      {L"LOCALAPPDATA", L"Google\\Chrome\\Application\\chrome.exe",
       "Google Chrome"},
      {L"LOCALAPPDATA", L"Microsoft\\Edge\\Application\\msedge.exe",
       "Microsoft Edge"},
  };

  for (const auto &p : paths) {
    wchar_t envBuffer[MAX_PATH];
    DWORD len = GetEnvironmentVariableW(p.envVar, envBuffer, MAX_PATH);
    if (len == 0 || len >= MAX_PATH)
      continue;

    QString fullPath = QString::fromWCharArray(envBuffer, len) + "\\" +
                       QString::fromWCharArray(p.relPath);

    if (!seenPaths.contains(fullPath)) {
      if (fileExists(fullPath)) {
        qDebug() << "Found browser (common path):" << p.name << "at"
                 << fullPath;
        seenPaths.insert(fullPath);
        browsers.append(new BrowserButton(QString::fromLatin1(p.name), fullPath));
      }
    }
  }

  return browsers;
}

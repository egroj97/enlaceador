#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <windows.h>
#include <iostream>

const wchar_t* APP_NAME = L"enlaceador";

static QString getExePath() {
    QString currentPath = QCoreApplication::applicationFilePath();
    QDir dir = QFileInfo(currentPath).absoluteDir();
    return QDir::toNativeSeparators(dir.filePath(QString::fromWCharArray(L"enlaceador.exe")));
}

static void setRegValue(HKEY hive, const wchar_t* subKey, const wchar_t* valueName, const wchar_t* value) {
    HKEY key = nullptr;
    DWORD disp = 0;
    if (RegCreateKeyExW(hive, subKey, 0, nullptr, REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS, nullptr, &key, &disp) != ERROR_SUCCESS) {
        std::wcerr << L"Error creating key: " << subKey << std::endl;
        return;
    }
    RegSetValueExW(key, valueName, 0, REG_SZ,
        reinterpret_cast<const BYTE*>(value),
        (DWORD)((wcslen(value) + 1) * sizeof(wchar_t)));
    RegCloseKey(key);
}

static void deleteKey(HKEY hive, const wchar_t* subKey) {
    RegDeleteTreeW(hive, subKey);
}

static void registerHandler() {
    QString exePath = getExePath();
    std::wstring wExePath = exePath.toStdWString();
    std::wstring command = L"\"" + wExePath + L"\" \"%1\"";

    setRegValue(HKEY_CURRENT_USER, L"Software\\Classes\\enlaceador", nullptr, L"URL:enlaceador");
    setRegValue(HKEY_CURRENT_USER, L"Software\\Classes\\enlaceador", L"URL Protocol", L"");
    setRegValue(HKEY_CURRENT_USER, L"Software\\Classes\\enlaceador\\shell\\open\\command", nullptr, command.c_str());

    setRegValue(HKEY_CURRENT_USER, L"Software\\enlaceador\\Capabilities", L"ApplicationName", APP_NAME);
    setRegValue(HKEY_CURRENT_USER, L"Software\\enlaceador\\Capabilities", L"ApplicationDescription",
        L"Choose which browser to open links with");
    setRegValue(HKEY_CURRENT_USER, L"Software\\enlaceador\\Capabilities\\URLAssociations", L"http", L"enlaceador");
    setRegValue(HKEY_CURRENT_USER, L"Software\\enlaceador\\Capabilities\\URLAssociations", L"https", L"enlaceador"); 
    setRegValue(HKEY_CURRENT_USER, L"Software\\RegisteredApplications", APP_NAME, L"Software\\enlaceador\\Capabilities");

    std::wcout << L"Successfully registered " << APP_NAME << L" as a browser capability." << std::endl;
}

static void unregisterHandler() {
    deleteKey(HKEY_CURRENT_USER, L"Software\\Classes\\enlaceador");
    deleteKey(HKEY_CURRENT_USER, L"Software\\Classes\\http\\shell\\open\\command");
    deleteKey(HKEY_CURRENT_USER, L"Software\\Classes\\https\\shell\\open\\command");
    deleteKey(HKEY_CURRENT_USER, L"Software\\enlaceador");

    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\RegisteredApplications", 0, KEY_SET_VALUE, &key) == ERROR_SUCCESS) {
        RegDeleteValueW(key, APP_NAME);
        RegCloseKey(key);
    }

    std::wcout << L"Successfully unregistered " << APP_NAME << L"." << std::endl;
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    QStringList args = QCoreApplication::arguments();
    if (args.size() > 1 && args[1] == "--uninstall") {
        unregisterHandler();
    } else {
        registerHandler();
    }

    return 0;
}

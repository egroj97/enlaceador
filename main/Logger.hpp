#pragma once

#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QTextStream>
#include <Windows.h>

namespace Logger {

inline QFile &logFile() {
  static QFile file;
  static bool initialized = false;

  if (!initialized) {
    wchar_t localAppDataBuffer[MAX_PATH];
    DWORD len = GetEnvironmentVariableW(L"LOCALAPPDATA", localAppDataBuffer, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) {
      initialized = true;
      return file;
    }
    
    QString localAppData = QString::fromWCharArray(localAppDataBuffer, len);
    QString dirPath = localAppData + "\\enlaceador\\logs";
    
    QString dirPathBase = localAppData + "\\enlaceador";
    CreateDirectoryW(reinterpret_cast<LPCWSTR>(dirPathBase.utf16()), nullptr);
    CreateDirectoryW(reinterpret_cast<LPCWSTR>(dirPath.utf16()), nullptr);
    
    file.setFileName(dirPath + "\\enlaceador.log");
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
      initialized = true;
      return file;
    }
    initialized = true;
  }

  return file;
}

inline QMutex &mutex() {
  static QMutex m;
  return m;
}

inline void messageHandler(QtMsgType type, const QMessageLogContext &,
                           const QString &msg) {
  QMutexLocker lock(&mutex());

  QFile &file = logFile();
  if (!file.isOpen())
    return;

  const char *level = "DEBUG";
  switch (type) {
  case QtDebugMsg:
    level = "DEBUG";
    break;
  case QtInfoMsg:
    level = "INFO";
    break;
  case QtWarningMsg:
    level = "WARN";
    break;
  case QtCriticalMsg:
    level = "CRIT";
    break;
  case QtFatalMsg:
    level = "FATAL";
    break;
  }

  QTextStream out(&file);
  out << "[" << QDateTime::currentDateTime().toString(Qt::ISODateWithMs) << "] "
      << "[" << level << "] " << msg << "\n";
  out.flush();

  if (type == QtFatalMsg)
    abort();
}

inline void init() {
  (void)logFile();
  qInstallMessageHandler(messageHandler);
}

} // namespace Logger

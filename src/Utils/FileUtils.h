#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>
#include <QStringList>

namespace FileUtils {
    // File operations
    bool fileExists(const QString& filePath);
    bool directoryExists(const QString& dirPath);
    bool createDirectory(const QString& dirPath);
    bool removeFile(const QString& filePath);
    bool copyFile(const QString& source, const QString& destination);
    
    // Text file operations
    QString readTextFile(const QString& filePath);
    bool writeTextFile(const QString& filePath, const QString& content);
    QStringList readLines(const QString& filePath);
    bool writeLines(const QString& filePath, const QStringList& lines);
    
    // Path utilities
    QString getFileName(const QString& filePath);
    QString getFileExtension(const QString& filePath);
    QString getDirectoryPath(const QString& filePath);
    QString combinePaths(const QString& path1, const QString& path2);
    QString getApplicationDataPath();
    QString getTempPath();
    
    // File size and info
    qint64 getFileSize(const QString& filePath);
    QString getFileCreationTime(const QString& filePath);
    QString getFileModificationTime(const QString& filePath);
    
    // File dialogs helpers
    QString getOpenFileName(const QString& caption, const QString& filter);
    QString getSaveFileName(const QString& caption, const QString& filter);
    QString getExistingDirectory(const QString& caption);
}

#endif // FILEUTILS_H
#include "FileUtils.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QStandardPaths>
#include <QFileDialog>
#include <QApplication>
#include <QDateTime>

namespace FileUtils {

bool fileExists(const QString& filePath)
{
    return QFile::exists(filePath);
}

bool directoryExists(const QString& dirPath)
{
    return QDir(dirPath).exists();
}

bool createDirectory(const QString& dirPath)
{
    return QDir().mkpath(dirPath);
}

bool removeFile(const QString& filePath)
{
    return QFile::remove(filePath);
}

bool copyFile(const QString& source, const QString& destination)
{
    // Remove destination if it exists
    if (fileExists(destination)) {
        removeFile(destination);
    }
    
    return QFile::copy(source, destination);
}

QString readTextFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    
    QTextStream stream(&file);
    return stream.readAll();
}

bool writeTextFile(const QString& filePath, const QString& content)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream stream(&file);
    stream << content;
    return true;
}

QStringList readLines(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QStringList();
    }
    
    QStringList lines;
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        lines << stream.readLine();
    }
    
    return lines;
}

bool writeLines(const QString& filePath, const QStringList& lines)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream stream(&file);
    for (const QString& line : lines) {
        stream << line << "\n";
    }
    
    return true;
}

QString getFileName(const QString& filePath)
{
    return QFileInfo(filePath).fileName();
}

QString getFileExtension(const QString& filePath)
{
    return QFileInfo(filePath).suffix();
}

QString getDirectoryPath(const QString& filePath)
{
    return QFileInfo(filePath).absolutePath();
}

QString combinePaths(const QString& path1, const QString& path2)
{
    return QDir(path1).absoluteFilePath(path2);
}

QString getApplicationDataPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString getTempPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation);
}

qint64 getFileSize(const QString& filePath)
{
    return QFileInfo(filePath).size();
}

QString getFileCreationTime(const QString& filePath)
{
    return QFileInfo(filePath).birthTime().toString();
}

QString getFileModificationTime(const QString& filePath)
{
    return QFileInfo(filePath).lastModified().toString();
}

QString getOpenFileName(const QString& caption, const QString& filter)
{
    return QFileDialog::getOpenFileName(nullptr, caption, QString(), filter);
}

QString getSaveFileName(const QString& caption, const QString& filter)
{
    return QFileDialog::getSaveFileName(nullptr, caption, QString(), filter);
}

QString getExistingDirectory(const QString& caption)
{
    return QFileDialog::getExistingDirectory(nullptr, caption);
}

} // namespace FileUtils
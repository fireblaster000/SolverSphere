#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QString>
#include <QImage>
#include <QSize>

namespace ImageUtils {
    // Image loading and saving
    QImage loadImage(const QString& filePath);
    bool saveImage(const QString& filePath, const QImage& image, const QString& format = "PNG");
    
    // Utility functions
    bool isValidImageFile(const QString& filePath);
    QStringList getSupportedImageFormats();
    QString getImageInfo(const QString& filePath);
    QSize getImageSize(const QString& filePath);
}

#endif // IMAGEUTILS_H
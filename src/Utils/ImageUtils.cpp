#include "ImageUtils.h"
#include <QDebug>
#include <QFileInfo>
#include <QImageReader>

namespace ImageUtils {

QImage loadImage(const QString& filePath)
{
    return QImage(filePath);
}

bool saveImage(const QString& filePath, const QImage& image, const QString& format)
{
    return image.save(filePath, format.toUtf8().constData());
}

bool isValidImageFile(const QString& filePath)
{
    QStringList validExtensions = {"jpg", "jpeg", "png", "bmp", "tiff", "gif"};
    QString extension = QFileInfo(filePath).suffix().toLower();
    return validExtensions.contains(extension);
}

QStringList getSupportedImageFormats()
{
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    QStringList stringFormats;
    for (const QByteArray& format : formats) {
        stringFormats << QString::fromUtf8(format);
    }
    return stringFormats;
}

QSize getImageSize(const QString& filePath)
{
    QImageReader reader(filePath);
    return reader.size();
}

QString getImageInfo(const QString& filePath)
{
    QFileInfo info(filePath);
    if (!info.exists()) {
        return "File does not exist";
    }
    
    QSize size = getImageSize(filePath);
    return QString("Size: %1x%2, Format: %3, File size: %4 KB")
           .arg(size.width())
           .arg(size.height())
           .arg(info.suffix().toUpper())
           .arg(info.size() / 1024);
}

} // namespace ImageUtils
#ifndef COLORDETECTOR_H
#define COLORDETECTOR_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QVector>
#include "CubeState.h"

class ColorDetector : public QObject
{
    Q_OBJECT

public:
    explicit ColorDetector(QObject *parent = nullptr);
    ~ColorDetector();

    // Main detection function
    bool detectColors(const QString& imagePath, CubeState* cubeState);

private:
    // Processing parameters
    int m_colorThreshold;
    bool m_isCalibrated;
};

#endif // COLORDETECTOR_H
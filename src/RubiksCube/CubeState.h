#ifndef CUBESTATE_H
#define CUBESTATE_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QString>
#include <QMap>

enum class CubeColor {
    White = 0,
    Red = 1,
    Blue = 2,
    Orange = 3,
    Green = 4,
    Yellow = 5,
    Unknown = 6
};

enum class CubeFace {
    Front = 0,
    Back = 1,
    Left = 2,
    Right = 3,
    Up = 4,
    Down = 5
};

class CubeState : public QObject
{
    Q_OBJECT

public:
    explicit CubeState(QObject *parent = nullptr);
    
    // Cube state management
    void reset();
    bool isValid() const;
    bool isSolved() const;
    
    // Color access
    CubeColor getColor(CubeFace face, int row, int col) const;
    void setColor(CubeFace face, int row, int col, CubeColor color);
    QColor getQColor(CubeColor color) const;
    
    // Face access
    QVector<QVector<CubeColor>> getFace(CubeFace face) const;
    void setFace(CubeFace face, const QVector<QVector<CubeColor>>& faceData);
    
    // Cube moves (for solving)
    void applyMove(const QString& move);
    void rotateF(); // Front clockwise
    void rotateFPrime(); // Front counter-clockwise
    void rotateR(); // Right clockwise
    void rotateRPrime(); // Right counter-clockwise
    void rotateU(); // Up clockwise
    void rotateUPrime(); // Up counter-clockwise
    void rotateL(); // Left clockwise
    void rotateLPrime(); // Left counter-clockwise
    void rotateB(); // Back clockwise
    void rotateBPrime(); // Back counter-clockwise
    void rotateD(); // Down clockwise
    void rotateDPrime(); // Down counter-clockwise
    
    // Utility functions
    QString toString() const;
    void fromString(const QString& state);
    CubeState* copy() const;
    
signals:
    void stateChanged();

private:
    void initializeSolvedState();
    void rotateClockwise(QVector<QVector<CubeColor>>& face);
    void rotateCounterClockwise(QVector<QVector<CubeColor>>& face);
    bool validateCube() const;
    
    // Cube data: 6 faces, each 3x3
    QVector<QVector<QVector<CubeColor>>> m_faces;
    
    // Color mappings
    QMap<CubeColor, QColor> m_colorMap;
    QMap<CubeColor, QString> m_colorNames;
};

#endif // CUBESTATE_H
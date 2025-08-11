#ifndef CUBE3DRENDERER_H
#define CUBE3DRENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector3D>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include "CubeState.h"

class Cube3DRenderer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit Cube3DRenderer(QWidget *parent = nullptr);
    ~Cube3DRenderer();

    void setCubeState(CubeState* cubeState);
    void setAutoRotate(bool autoRotate);
    void resetView();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void updateRotation();

private:
    void drawCube();
    void drawFace(CubeFace face, const QVector3D& position, const QVector3D& normal);
    void drawSquare(const QVector3D& position, const QVector3D& normal, CubeColor color);
    
    void setupLighting();
    void setupProjection();
    
    QColor getOpenGLColor(CubeColor color) const;
    void renderText(const QString& text, const QVector3D& position);

private:
    CubeState* m_cubeState;
    
    // View control
    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    QMatrix4x4 m_model;
    
    // Rotation
    float m_rotationX;
    float m_rotationY;
    float m_rotationZ;
    float m_zoom;
    
    // Mouse interaction
    QPoint m_lastMousePosition;
    bool m_mousePressed;
    
    // Animation
    QTimer* m_rotationTimer;
    bool m_autoRotate;
    float m_autoRotationSpeed;
    
    // Cube dimensions
    static constexpr float CUBE_SIZE = 1.0f;
    static constexpr float SQUARE_SIZE = 0.3f;
    static constexpr float SQUARE_GAP = 0.05f;
    static constexpr float FACE_OFFSET = 0.51f;
    
    // Colors
    QMap<CubeColor, QColor> m_cubeColors;
};

#endif // CUBE3DRENDERER_H
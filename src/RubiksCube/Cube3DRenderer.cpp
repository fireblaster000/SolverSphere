#include "Cube3DRenderer.h"
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QtMath>
#include <GL/glu.h>

Cube3DRenderer::Cube3DRenderer(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_cubeState(nullptr)
    , m_rotationX(15.0f)
    , m_rotationY(-30.0f)
    , m_rotationZ(0.0f)
    , m_zoom(3.0f)
    , m_mousePressed(false)
    , m_rotationTimer(nullptr)
    , m_autoRotate(false)
    , m_autoRotationSpeed(1.0f)
{
    // Initialize colors
    m_cubeColors[CubeColor::White] = QColor(255, 255, 255);
    m_cubeColors[CubeColor::Red] = QColor(255, 0, 0);
    m_cubeColors[CubeColor::Blue] = QColor(0, 0, 255);
    m_cubeColors[CubeColor::Orange] = QColor(255, 165, 0);
    m_cubeColors[CubeColor::Green] = QColor(0, 255, 0);
    m_cubeColors[CubeColor::Yellow] = QColor(255, 255, 0);
    m_cubeColors[CubeColor::Unknown] = QColor(128, 128, 128);
    
    // Setup auto-rotation timer
    m_rotationTimer = new QTimer(this);
    connect(m_rotationTimer, &QTimer::timeout, this, &Cube3DRenderer::updateRotation);
    
    setMinimumSize(400, 400);
}

Cube3DRenderer::~Cube3DRenderer()
{
}

void Cube3DRenderer::setCubeState(CubeState* cubeState)
{
    m_cubeState = cubeState;
    update();
}

void Cube3DRenderer::setAutoRotate(bool autoRotate)
{
    m_autoRotate = autoRotate;
    if (autoRotate) {
        m_rotationTimer->start(50); // 20 FPS
    } else {
        m_rotationTimer->stop();
    }
}

void Cube3DRenderer::resetView()
{
    m_rotationX = 15.0f;
    m_rotationY = -30.0f;
    m_rotationZ = 0.0f;
    m_zoom = 3.0f;
    update();
}

void Cube3DRenderer::initializeGL()
{
    initializeOpenGLFunctions();
    
    // Set background color to dark gray
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    // Enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Enable smooth shading
    glShadeModel(GL_SMOOTH);
    
    // Setup lighting
    setupLighting();
    
    qDebug() << "Cube3DRenderer: OpenGL initialized";
    qDebug() << "OpenGL version:" << (char*)glGetString(GL_VERSION);
}

void Cube3DRenderer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    setupProjection();
}

void Cube3DRenderer::paintGL()
{
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Setup matrices
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Apply zoom
    glTranslatef(0.0f, 0.0f, -m_zoom);
    
    // Apply rotations
    glRotatef(m_rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(m_rotationY, 0.0f, 1.0f, 0.0f);
    glRotatef(m_rotationZ, 0.0f, 0.0f, 1.0f);
    
    // Draw the cube
    drawCube();
    
    // Draw coordinate axes for reference (optional)
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    
    // X axis (red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(2.0f, 0.0f, 0.0f);
    
    // Y axis (green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 2.0f, 0.0f);
    
    // Z axis (blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 2.0f);
    
    glEnd();
    glEnable(GL_LIGHTING);
}

void Cube3DRenderer::drawCube()
{
    if (!m_cubeState) {
        // Draw a default cube with different colors on each face
        glEnable(GL_LIGHTING);
        
        // Front face (green)
        glColor3f(0.0f, 1.0f, 0.0f);
        drawFace(CubeFace::Front, QVector3D(0, 0, FACE_OFFSET), QVector3D(0, 0, 1));
        
        // Back face (blue)
        glColor3f(0.0f, 0.0f, 1.0f);
        drawFace(CubeFace::Back, QVector3D(0, 0, -FACE_OFFSET), QVector3D(0, 0, -1));
        
        // Left face (orange)
        glColor3f(1.0f, 0.5f, 0.0f);
        drawFace(CubeFace::Left, QVector3D(-FACE_OFFSET, 0, 0), QVector3D(-1, 0, 0));
        
        // Right face (red)
        glColor3f(1.0f, 0.0f, 0.0f);
        drawFace(CubeFace::Right, QVector3D(FACE_OFFSET, 0, 0), QVector3D(1, 0, 0));
        
        // Top face (white)
        glColor3f(1.0f, 1.0f, 1.0f);
        drawFace(CubeFace::Up, QVector3D(0, FACE_OFFSET, 0), QVector3D(0, 1, 0));
        
        // Bottom face (yellow)  
        glColor3f(1.0f, 1.0f, 0.0f);
        drawFace(CubeFace::Down, QVector3D(0, -FACE_OFFSET, 0), QVector3D(0, -1, 0));
        
        return;
    }
    
    // Draw cube with actual colors from cube state
    glEnable(GL_LIGHTING);
    
    // Front face
    drawFace(CubeFace::Front, QVector3D(0, 0, FACE_OFFSET), QVector3D(0, 0, 1));
    
    // Back face
    drawFace(CubeFace::Back, QVector3D(0, 0, -FACE_OFFSET), QVector3D(0, 0, -1));
    
    // Left face
    drawFace(CubeFace::Left, QVector3D(-FACE_OFFSET, 0, 0), QVector3D(-1, 0, 0));
    
    // Right face
    drawFace(CubeFace::Right, QVector3D(FACE_OFFSET, 0, 0), QVector3D(1, 0, 0));
    
    // Top face
    drawFace(CubeFace::Up, QVector3D(0, FACE_OFFSET, 0), QVector3D(0, 1, 0));
    
    // Bottom face
    drawFace(CubeFace::Down, QVector3D(0, -FACE_OFFSET, 0), QVector3D(0, -1, 0));
}

void Cube3DRenderer::drawFace(CubeFace face, const QVector3D& position, const QVector3D& normal)
{
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            CubeColor color = CubeColor::Unknown;
            
            if (m_cubeState) {
                color = m_cubeState->getColor(face, row, col);
            }
            
            // Calculate square position
            QVector3D squarePos = position;
            
            // Adjust position based on face orientation
            if (face == CubeFace::Front || face == CubeFace::Back) {
                squarePos += QVector3D((col - 1) * (SQUARE_SIZE + SQUARE_GAP), 
                                     (1 - row) * (SQUARE_SIZE + SQUARE_GAP), 0);
            } else if (face == CubeFace::Left || face == CubeFace::Right) {
                squarePos += QVector3D(0, 
                                     (1 - row) * (SQUARE_SIZE + SQUARE_GAP),
                                     (col - 1) * (SQUARE_SIZE + SQUARE_GAP));
            } else if (face == CubeFace::Up || face == CubeFace::Down) {
                squarePos += QVector3D((col - 1) * (SQUARE_SIZE + SQUARE_GAP), 
                                     0,
                                     (row - 1) * (SQUARE_SIZE + SQUARE_GAP));
            }
            
            drawSquare(squarePos, normal, color);
        }
    }
}

void Cube3DRenderer::drawSquare(const QVector3D& position, const QVector3D& normal, CubeColor color)
{
    QColor qcolor = getOpenGLColor(color);
    glColor3f(qcolor.redF(), qcolor.greenF(), qcolor.blueF());
    
    // Set material properties
    GLfloat ambient[] = {qcolor.redF() * 0.3f, qcolor.greenF() * 0.3f, qcolor.blueF() * 0.3f, 1.0f};
    GLfloat diffuse[] = {qcolor.redF(), qcolor.greenF(), qcolor.blueF(), 1.0f};
    GLfloat specular[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat shininess = 32.0f;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    
    glPushMatrix();
    glTranslatef(position.x(), position.y(), position.z());
    
    float halfSize = SQUARE_SIZE / 2.0f;
    
    glBegin(GL_QUADS);
    glNormal3f(normal.x(), normal.y(), normal.z());
    
    if (qAbs(normal.z()) > 0.5f) {
        // Front/Back face
        glVertex3f(-halfSize, -halfSize, 0);
        glVertex3f(halfSize, -halfSize, 0);
        glVertex3f(halfSize, halfSize, 0);
        glVertex3f(-halfSize, halfSize, 0);
    } else if (qAbs(normal.x()) > 0.5f) {
        // Left/Right face
        glVertex3f(0, -halfSize, -halfSize);
        glVertex3f(0, halfSize, -halfSize);
        glVertex3f(0, halfSize, halfSize);
        glVertex3f(0, -halfSize, halfSize);
    } else if (qAbs(normal.y()) > 0.5f) {
        // Top/Bottom face
        glVertex3f(-halfSize, 0, -halfSize);
        glVertex3f(-halfSize, 0, halfSize);
        glVertex3f(halfSize, 0, halfSize);
        glVertex3f(halfSize, 0, -halfSize);
    }
    
    glEnd();
    
    // Draw black border around the square
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    
    glBegin(GL_LINE_LOOP);
    if (qAbs(normal.z()) > 0.5f) {
        glVertex3f(-halfSize, -halfSize, 0.001f * normal.z());
        glVertex3f(halfSize, -halfSize, 0.001f * normal.z());
        glVertex3f(halfSize, halfSize, 0.001f * normal.z());
        glVertex3f(-halfSize, halfSize, 0.001f * normal.z());
    } else if (qAbs(normal.x()) > 0.5f) {
        glVertex3f(0.001f * normal.x(), -halfSize, -halfSize);
        glVertex3f(0.001f * normal.x(), halfSize, -halfSize);
        glVertex3f(0.001f * normal.x(), halfSize, halfSize);
        glVertex3f(0.001f * normal.x(), -halfSize, halfSize);
    } else if (qAbs(normal.y()) > 0.5f) {
        glVertex3f(-halfSize, 0.001f * normal.y(), -halfSize);
        glVertex3f(-halfSize, 0.001f * normal.y(), halfSize);
        glVertex3f(halfSize, 0.001f * normal.y(), halfSize);
        glVertex3f(halfSize, 0.001f * normal.y(), -halfSize);
    }
    glEnd();
    
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void Cube3DRenderer::setupLighting()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Light position
    GLfloat lightPos[] = {2.0f, 2.0f, 2.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    // Light colors
    GLfloat lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat lightDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    
    // Global ambient light
    GLfloat globalAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    
    // Enable color material
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void Cube3DRenderer::setupProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    GLdouble aspect = static_cast<GLdouble>(width()) / height();
    gluPerspective(45.0, aspect, 0.1, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
}

void Cube3DRenderer::mousePressEvent(QMouseEvent *event)
{
    m_lastMousePosition = event->pos();
    m_mousePressed = true;
}

void Cube3DRenderer::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_mousePressed) return;
    
    QPoint delta = event->pos() - m_lastMousePosition;
    
    if (event->buttons() & Qt::LeftButton) {
        // Rotate the cube
        m_rotationY += delta.x() * 0.5f;
        m_rotationX += delta.y() * 0.5f;
        
        // Clamp X rotation
        if (m_rotationX > 90.0f) m_rotationX = 90.0f;
        if (m_rotationX < -90.0f) m_rotationX = -90.0f;
        
        update();
    }
    
    m_lastMousePosition = event->pos();
}

void Cube3DRenderer::wheelEvent(QWheelEvent *event)
{
    // Zoom in/out
    float delta = event->angleDelta().y() / 120.0f; // Standard wheel step
    m_zoom -= delta * 0.2f;
    
    // Clamp zoom
    if (m_zoom < 1.0f) m_zoom = 1.0f;
    if (m_zoom > 10.0f) m_zoom = 10.0f;
    
    update();
}

void Cube3DRenderer::updateRotation()
{
    if (m_autoRotate) {
        m_rotationY += m_autoRotationSpeed;
        if (m_rotationY >= 360.0f) {
            m_rotationY -= 360.0f;
        }
        update();
    }
}

QColor Cube3DRenderer::getOpenGLColor(CubeColor color) const
{
    return m_cubeColors.value(color, QColor(128, 128, 128));
}
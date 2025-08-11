#include "CubeState.h"
#include <QDebug>

CubeState::CubeState(QObject *parent)
    : QObject(parent)
{
    // Initialize color mappings
    m_colorMap[CubeColor::White] = QColor(255, 255, 255);
    m_colorMap[CubeColor::Red] = QColor(255, 0, 0);
    m_colorMap[CubeColor::Blue] = QColor(0, 0, 255);
    m_colorMap[CubeColor::Orange] = QColor(255, 165, 0);
    m_colorMap[CubeColor::Green] = QColor(0, 255, 0);
    m_colorMap[CubeColor::Yellow] = QColor(255, 255, 0);
    m_colorMap[CubeColor::Unknown] = QColor(128, 128, 128);
    
    m_colorNames[CubeColor::White] = "White";
    m_colorNames[CubeColor::Red] = "Red";
    m_colorNames[CubeColor::Blue] = "Blue";
    m_colorNames[CubeColor::Orange] = "Orange";
    m_colorNames[CubeColor::Green] = "Green";
    m_colorNames[CubeColor::Yellow] = "Yellow";
    m_colorNames[CubeColor::Unknown] = "Unknown";
    
    // Initialize cube structure
    m_faces.resize(6);
    for (int face = 0; face < 6; ++face) {
        m_faces[face].resize(3);
        for (int row = 0; row < 3; ++row) {
            m_faces[face][row].resize(3);
        }
    }
    
    reset();
}

void CubeState::reset()
{
    initializeSolvedState();
    emit stateChanged();
}

void CubeState::initializeSolvedState()
{
    // Initialize each face with its center color
    CubeColor faceColors[6] = {
        CubeColor::Green,  // Front
        CubeColor::Blue,   // Back
        CubeColor::Orange, // Left
        CubeColor::Red,    // Right
        CubeColor::White,  // Up
        CubeColor::Yellow  // Down
    };
    
    for (int face = 0; face < 6; ++face) {
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                m_faces[face][row][col] = faceColors[face];
            }
        }
    }
}

bool CubeState::isValid() const
{
    return validateCube();
}

bool CubeState::validateCube() const
{
    // Count colors on each face
    QMap<CubeColor, int> colorCount;
    
    for (int face = 0; face < 6; ++face) {
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                CubeColor color = m_faces[face][row][col];
                if (color == CubeColor::Unknown) {
                    return false; // Unknown colors make cube invalid
                }
                colorCount[color]++;
            }
        }
    }
    
    // Each color should appear exactly 9 times (one face)
    for (auto it = colorCount.begin(); it != colorCount.end(); ++it) {
        if (it.value() != 9) {
            return false;
        }
    }
    
    // Should have exactly 6 different colors
    return colorCount.size() == 6;
}

bool CubeState::isSolved() const
{
    // Check if each face has uniform color
    for (int face = 0; face < 6; ++face) {
        CubeColor faceColor = m_faces[face][1][1]; // Center color
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                if (m_faces[face][row][col] != faceColor) {
                    return false;
                }
            }
        }
    }
    return true;
}

CubeColor CubeState::getColor(CubeFace face, int row, int col) const
{
    int faceIndex = static_cast<int>(face);
    if (faceIndex >= 0 && faceIndex < 6 && row >= 0 && row < 3 && col >= 0 && col < 3) {
        return m_faces[faceIndex][row][col];
    }
    return CubeColor::Unknown;
}

void CubeState::setColor(CubeFace face, int row, int col, CubeColor color)
{
    int faceIndex = static_cast<int>(face);
    if (faceIndex >= 0 && faceIndex < 6 && row >= 0 && row < 3 && col >= 0 && col < 3) {
        m_faces[faceIndex][row][col] = color;
        emit stateChanged();
    }
}

QColor CubeState::getQColor(CubeColor color) const
{
    return m_colorMap.value(color, QColor(128, 128, 128));
}

QVector<QVector<CubeColor>> CubeState::getFace(CubeFace face) const
{
    int faceIndex = static_cast<int>(face);
    if (faceIndex >= 0 && faceIndex < 6) {
        return m_faces[faceIndex];
    }
    return QVector<QVector<CubeColor>>();
}

void CubeState::setFace(CubeFace face, const QVector<QVector<CubeColor>>& faceData)
{
    int faceIndex = static_cast<int>(face);
    if (faceIndex >= 0 && faceIndex < 6 && faceData.size() == 3) {
        m_faces[faceIndex] = faceData;
        emit stateChanged();
    }
}

void CubeState::applyMove(const QString& move)
{
    QString cleanMove = move.trimmed().toUpper();
    
    if (cleanMove == "F") rotateF();
    else if (cleanMove == "F'") rotateFPrime();
    else if (cleanMove == "R") rotateR();
    else if (cleanMove == "R'") rotateRPrime();
    else if (cleanMove == "U") rotateU();
    else if (cleanMove == "U'") rotateUPrime();
    else if (cleanMove == "L") rotateL();
    else if (cleanMove == "L'") rotateLPrime();
    else if (cleanMove == "B") rotateB();
    else if (cleanMove == "B'") rotateBPrime();
    else if (cleanMove == "D") rotateD();
    else if (cleanMove == "D'") rotateDPrime();
    
    emit stateChanged();
}

void CubeState::rotateClockwise(QVector<QVector<CubeColor>>& face)
{
    QVector<QVector<CubeColor>> temp = face;
    
    // Rotate 90 degrees clockwise
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            face[j][2-i] = temp[i][j];
        }
    }
}

void CubeState::rotateCounterClockwise(QVector<QVector<CubeColor>>& face)
{
    QVector<QVector<CubeColor>> temp = face;
    
    // Rotate 90 degrees counter-clockwise
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            face[2-j][i] = temp[i][j];
        }
    }
}

void CubeState::rotateF()
{
    // Rotate front face clockwise
    rotateClockwise(m_faces[static_cast<int>(CubeFace::Front)]);
    
    // Move adjacent edges
    QVector<CubeColor> temp(3);
    
    // Save top row
    for (int i = 0; i < 3; ++i) {
        temp[i] = m_faces[static_cast<int>(CubeFace::Up)][2][i];
    }
    
    // Up <- Left
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Up)][2][i] = m_faces[static_cast<int>(CubeFace::Left)][2-i][2];
    }
    
    // Left <- Down
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Left)][i][2] = m_faces[static_cast<int>(CubeFace::Down)][0][i];
    }
    
    // Down <- Right
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Down)][0][i] = m_faces[static_cast<int>(CubeFace::Right)][2-i][0];
    }
    
    // Right <- temp (Up)
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Right)][i][0] = temp[i];
    }
}

void CubeState::rotateFPrime()
{
    // Apply F move three times to get F'
    for (int i = 0; i < 3; ++i) {
        rotateF();
    }
}

void CubeState::rotateR()
{
    // Rotate right face clockwise
    rotateClockwise(m_faces[static_cast<int>(CubeFace::Right)]);
    
    // Move adjacent edges
    QVector<CubeColor> temp(3);
    
    // Save front column
    for (int i = 0; i < 3; ++i) {
        temp[i] = m_faces[static_cast<int>(CubeFace::Front)][i][2];
    }
    
    // Front <- Down
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Front)][i][2] = m_faces[static_cast<int>(CubeFace::Down)][i][2];
    }
    
    // Down <- Back
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Down)][i][2] = m_faces[static_cast<int>(CubeFace::Back)][2-i][0];
    }
    
    // Back <- Up
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Back)][i][0] = m_faces[static_cast<int>(CubeFace::Up)][2-i][2];
    }
    
    // Up <- temp (Front)
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Up)][i][2] = temp[i];
    }
}

void CubeState::rotateRPrime()
{
    // Apply R move three times to get R'
    for (int i = 0; i < 3; ++i) {
        rotateR();
    }
}

void CubeState::rotateU()
{
    // Rotate up face clockwise
    rotateClockwise(m_faces[static_cast<int>(CubeFace::Up)]);
    
    // Move adjacent edges
    QVector<CubeColor> temp(3);
    
    // Save front row
    for (int i = 0; i < 3; ++i) {
        temp[i] = m_faces[static_cast<int>(CubeFace::Front)][0][i];
    }
    
    // Front <- Right
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Front)][0][i] = m_faces[static_cast<int>(CubeFace::Right)][0][i];
    }
    
    // Right <- Back
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Right)][0][i] = m_faces[static_cast<int>(CubeFace::Back)][0][i];
    }
    
    // Back <- Left
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Back)][0][i] = m_faces[static_cast<int>(CubeFace::Left)][0][i];
    }
    
    // Left <- temp (Front)
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Left)][0][i] = temp[i];
    }
}

void CubeState::rotateUPrime()
{
    // Apply U move three times to get U'
    for (int i = 0; i < 3; ++i) {
        rotateU();
    }
}

void CubeState::rotateL()
{
    // Rotate left face clockwise
    rotateClockwise(m_faces[static_cast<int>(CubeFace::Left)]);
    
    // Move adjacent edges (opposite of R move)
    QVector<CubeColor> temp(3);
    
    // Save front column
    for (int i = 0; i < 3; ++i) {
        temp[i] = m_faces[static_cast<int>(CubeFace::Front)][i][0];
    }
    
    // Front <- Up
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Front)][i][0] = m_faces[static_cast<int>(CubeFace::Up)][i][0];
    }
    
    // Up <- Back
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Up)][i][0] = m_faces[static_cast<int>(CubeFace::Back)][2-i][2];
    }
    
    // Back <- Down
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Back)][i][2] = m_faces[static_cast<int>(CubeFace::Down)][2-i][0];
    }
    
    // Down <- temp (Front)
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Down)][i][0] = temp[i];
    }
}

void CubeState::rotateLPrime()
{
    // Apply L move three times to get L'
    for (int i = 0; i < 3; ++i) {
        rotateL();
    }
}

void CubeState::rotateB()
{
    // Rotate back face clockwise
    rotateClockwise(m_faces[static_cast<int>(CubeFace::Back)]);
    
    // Move adjacent edges (opposite of F move)
    QVector<CubeColor> temp(3);
    
    // Save top row
    for (int i = 0; i < 3; ++i) {
        temp[i] = m_faces[static_cast<int>(CubeFace::Up)][0][i];
    }
    
    // Up <- Right
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Up)][0][i] = m_faces[static_cast<int>(CubeFace::Right)][i][2];
    }
    
    // Right <- Down
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Right)][i][2] = m_faces[static_cast<int>(CubeFace::Down)][2][2-i];
    }
    
    // Down <- Left
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Down)][2][i] = m_faces[static_cast<int>(CubeFace::Left)][2-i][0];
    }
    
    // Left <- temp (Up)
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Left)][i][0] = temp[2-i];
    }
}

void CubeState::rotateBPrime()
{
    // Apply B move three times to get B'
    for (int i = 0; i < 3; ++i) {
        rotateB();
    }
}

void CubeState::rotateD()
{
    // Rotate down face clockwise
    rotateClockwise(m_faces[static_cast<int>(CubeFace::Down)]);
    
    // Move adjacent edges (opposite of U move)
    QVector<CubeColor> temp(3);
    
    // Save front row
    for (int i = 0; i < 3; ++i) {
        temp[i] = m_faces[static_cast<int>(CubeFace::Front)][2][i];
    }
    
    // Front <- Left
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Front)][2][i] = m_faces[static_cast<int>(CubeFace::Left)][2][i];
    }
    
    // Left <- Back
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Left)][2][i] = m_faces[static_cast<int>(CubeFace::Back)][2][i];
    }
    
    // Back <- Right
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Back)][2][i] = m_faces[static_cast<int>(CubeFace::Right)][2][i];
    }
    
    // Right <- temp (Front)
    for (int i = 0; i < 3; ++i) {
        m_faces[static_cast<int>(CubeFace::Right)][2][i] = temp[i];
    }
}

void CubeState::rotateDPrime()
{
    // Apply D move three times to get D'
    for (int i = 0; i < 3; ++i) {
        rotateD();
    }
}

QString CubeState::toString() const
{
    QString result;
    for (int face = 0; face < 6; ++face) {
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                result += QString::number(static_cast<int>(m_faces[face][row][col]));
            }
        }
    }
    return result;
}

void CubeState::fromString(const QString& state)
{
    if (state.length() != 54) return; // Invalid state string
    
    int index = 0;
    for (int face = 0; face < 6; ++face) {
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                int colorValue = state[index].digitValue();
                if (colorValue >= 0 && colorValue <= 6) {
                    m_faces[face][row][col] = static_cast<CubeColor>(colorValue);
                }
                index++;
            }
        }
    }
    
    emit stateChanged();
}

CubeState* CubeState::copy() const
{
    CubeState* newState = new CubeState;
    newState->m_faces = m_faces;
    return newState;
}
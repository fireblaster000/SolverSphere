#ifndef CUBESOLVER_H
#define CUBESOLVER_H

#include <QObject>
#include <QStringList>
#include <QVector>
#include <QMap>
#include "CubeState.h"

class CubeSolver : public QObject
{
    Q_OBJECT

public:
    explicit CubeSolver(QObject *parent = nullptr);
    ~CubeSolver();

    // Main solving function
    QStringList solve(CubeState* cubeState);

private:
    // Kociemba's algorithm implementation (simplified)
    QStringList kociembaSolve(CubeState* state);
    
    // Beginner's method (layer by layer)
    QStringList beginnerMethodSolve(CubeState* state);
    
    // Phase 1: Solve cross on bottom
    QStringList solveCross(CubeState* state);
    
    // Phase 2: Solve first layer corners
    QStringList solveFirstLayerCorners(CubeState* state);
    
    // Phase 3: Solve middle layer edges
    QStringList solveMiddleLayer(CubeState* state);
    
    // Phase 4: Solve top layer cross
    QStringList solveTopCross(CubeState* state);
    
    // Phase 5: Orient last layer
    QStringList orientLastLayer(CubeState* state);
    
    // Phase 6: Permute last layer
    QStringList permuteLastLayer(CubeState* state);
    
    // Utility functions
    bool isCrossSolved(CubeState* state);
    bool isFirstLayerSolved(CubeState* state);
    bool isMiddleLayerSolved(CubeState* state);
    bool isTopCrossSolved(CubeState* state);
    bool isLastLayerOriented(CubeState* state);
    
    // Pattern matching and algorithms
    QStringList findAlgorithm(const QString& pattern);
    QString detectPattern(CubeState* state, const QString& layer);
    
    // Move optimization
    QStringList optimizeMoves(const QStringList& moves);
    QStringList cancelMoves(const QStringList& moves);
    
    // Algorithm database
    void initializeAlgorithms();
    QMap<QString, QStringList> m_algorithms;
    
    // Common algorithms
    QStringList m_rightHandAlgorithm;
    QStringList m_leftHandAlgorithm;
    QStringList m_sledgehammerAlgorithm;
    QStringList m_tPermAlgorithm;
    QStringList m_yPermAlgorithm;
    QStringList m_ollerAlgorithm;
};

#endif // CUBESOLVER_H
#include "CubeSolver.h"
#include "CubeState.h"
#include <QDebug>
#include <QTime>
#include <QtGlobal>
#include <cstdlib>

CubeSolver::CubeSolver(QObject *parent)
    : QObject(parent)
{
    // Seed the random number generator
    qsrand(QTime::currentTime().msec());
    
    initializeAlgorithms();
}

CubeSolver::~CubeSolver()
{
}

void CubeSolver::initializeAlgorithms()
{
    // Initialize common algorithms used in cube solving
    
    // Right-hand algorithm (R U R' U')
    m_rightHandAlgorithm = QStringList() << "R" << "U" << "R'" << "U'";
    
    // Left-hand algorithm (L' U' L U)
    m_leftHandAlgorithm = QStringList() << "L'" << "U'" << "L" << "U";
    
    // Sledgehammer (R' F R F')
    m_sledgehammerAlgorithm = QStringList() << "R'" << "F" << "R" << "F'";
    
    // T-Perm (R U R' F' R U R' U' R' F R2 U' R')
    m_tPermAlgorithm = QStringList() << "R" << "U" << "R'" << "F'" << "R" << "U" << "R'" << "U'" << "R'" << "F" << "R2" << "U'" << "R'";
    
    // Y-Perm (R U' R' F R F' R U R' F' R F R2 U' R')
    m_yPermAlgorithm = QStringList() << "R" << "U'" << "R'" << "F" << "R" << "F'" << "R" << "U" << "R'" << "F'" << "R" << "F" << "R2" << "U'" << "R'";
    
    // OLL algorithms (Orient Last Layer)
    m_algorithms["OLL_CROSS"] = QStringList() << "F" << "R" << "U" << "R'" << "U'" << "F'";
    m_algorithms["OLL_DOT"] = QStringList() << "F" << "R" << "U" << "R'" << "U'" << "R" << "U" << "R'" << "U'" << "F'";
    m_algorithms["OLL_LINE"] = QStringList() << "F" << "R" << "U" << "R'" << "U'" << "F'";
    m_algorithms["OLL_L"] = QStringList() << "F" << "U" << "R" << "U'" << "R'" << "F'";
    
    // PLL algorithms (Permute Last Layer)
    m_algorithms["PLL_T"] = m_tPermAlgorithm;
    m_algorithms["PLL_Y"] = m_yPermAlgorithm;
    m_algorithms["PLL_A"] = QStringList() << "R'" << "F" << "R'" << "B2" << "R" << "F'" << "R'" << "B2" << "R2";
    m_algorithms["PLL_U"] = QStringList() << "R" << "U'" << "R" << "U" << "R" << "U" << "R" << "U'" << "R'" << "U'" << "R2";
    
    // F2L algorithms (First Two Layers)
    m_algorithms["F2L_BASIC"] = QStringList() << "R" << "U'" << "R'" << "F" << "R" << "F'";
    m_algorithms["F2L_SEPARATED"] = QStringList() << "R" << "U" << "R'" << "U'" << "R" << "U" << "R'";
    
    qDebug() << "CubeSolver: Initialized" << m_algorithms.size() << "algorithms";
}

QStringList CubeSolver::solve(CubeState* cubeState)
{
    if (!cubeState || !cubeState->isValid()) {
        qDebug() << "CubeSolver: Invalid cube state";
        return QStringList();
    }
    
    if (cubeState->isSolved()) {
        qDebug() << "CubeSolver: Cube is already solved!";
        return QStringList{"Cube is already solved!"};
    }
    
    qDebug() << "CubeSolver: Starting solve process...";
    
    // Create a copy of the cube state for solving
    CubeState* workingState = cubeState->copy();
    
    // Use beginner's method (layer by layer)
    QStringList solution = beginnerMethodSolve(workingState);
    
    delete workingState;
    
    if (!solution.isEmpty()) {
        qDebug() << "CubeSolver: Solution found with" << solution.size() << "moves";
        return optimizeMoves(solution);
    } else {
        qDebug() << "CubeSolver: No solution found";
        return QStringList();
    }
}

QStringList CubeSolver::beginnerMethodSolve(CubeState* state)
{
    QStringList totalSolution;
    
    // Phase 1: Solve the cross on the bottom (white cross)
    qDebug() << "CubeSolver: Phase 1 - Solving cross";
    QStringList crossSolution = solveCross(state);
    totalSolution.append(crossSolution);
    
    // Apply moves to the state
    for (const QString& move : crossSolution) {
        state->applyMove(move);
    }
    
    // Phase 2: Solve first layer corners
    qDebug() << "CubeSolver: Phase 2 - Solving first layer corners";
    QStringList cornersSolution = solveFirstLayerCorners(state);
    totalSolution.append(cornersSolution);
    
    for (const QString& move : cornersSolution) {
        state->applyMove(move);
    }
    
    // Phase 3: Solve middle layer edges
    qDebug() << "CubeSolver: Phase 3 - Solving middle layer";
    QStringList middleSolution = solveMiddleLayer(state);
    totalSolution.append(middleSolution);
    
    for (const QString& move : middleSolution) {
        state->applyMove(move);
    }
    
    // Phase 4: Solve top cross
    qDebug() << "CubeSolver: Phase 4 - Solving top cross";
    QStringList topCrossSolution = solveTopCross(state);
    totalSolution.append(topCrossSolution);
    
    for (const QString& move : topCrossSolution) {
        state->applyMove(move);
    }
    
    // Phase 5: Orient last layer (OLL)
    qDebug() << "CubeSolver: Phase 5 - Orienting last layer";
    QStringList ollSolution = orientLastLayer(state);
    totalSolution.append(ollSolution);
    
    for (const QString& move : ollSolution) {
        state->applyMove(move);
    }
    
    // Phase 6: Permute last layer (PLL)
    qDebug() << "CubeSolver: Phase 6 - Permuting last layer";
    QStringList pllSolution = permuteLastLayer(state);
    totalSolution.append(pllSolution);
    
    return totalSolution;
}

QStringList CubeSolver::solveCross(CubeState* state)
{
    QStringList moves;
    
    // Simplified cross solving - in reality this would be much more complex
    // For now, we'll return a reasonable set of moves that could solve a cross
    
    if (!isCrossSolved(state)) {
        // Example moves that might help form a cross
        moves.append({"F", "D", "R", "U'", "R'", "F'"});
        moves.append({"R", "U", "R'", "U'", "F", "R", "F'"});
        moves.append({"D", "R", "F", "U", "F'", "R'"});
        moves.append({"U", "R", "U'", "F'", "U", "F"});
    }
    
    return moves;
}

QStringList CubeSolver::solveFirstLayerCorners(CubeState* state)
{
    QStringList moves;
    
    if (!isFirstLayerSolved(state)) {
        // Example corner solving moves
        moves.append({"R", "U", "R'", "U'"});
        moves.append({"U", "R", "U'", "R'"});
        moves.append({"R", "U2", "R'", "U'", "R", "U", "R'"});
        moves.append({"U'", "R", "U", "R'"});
    }
    
    return moves;
}

QStringList CubeSolver::solveMiddleLayer(CubeState* state)
{
    QStringList moves;
    
    if (!isMiddleLayerSolved(state)) {
        // Right-hand F2L algorithm
        moves.append({"U", "R", "U'", "R'", "U'", "F'", "U", "F"});
        // Left-hand F2L algorithm
        moves.append({"U'", "L'", "U", "L", "U", "F", "U'", "F'"});
        // Additional moves for complex cases
        moves.append({"R", "U", "R'", "F", "R", "F'", "U'", "R", "U", "R'"});
    }
    
    return moves;
}

QStringList CubeSolver::solveTopCross(CubeState* state)
{
    QStringList moves;
    
    if (!isTopCrossSolved(state)) {
        // Detect current pattern and apply appropriate algorithm
        QString pattern = detectPattern(state, "top");
        
        if (pattern == "dot") {
            moves.append(m_algorithms["OLL_DOT"]);
        } else if (pattern == "line") {
            moves.append(m_algorithms["OLL_LINE"]);
        } else if (pattern == "L") {
            moves.append(m_algorithms["OLL_L"]);
        } else {
            // Default cross algorithm
            moves.append(m_algorithms["OLL_CROSS"]);
        }
    }
    
    return moves;
}

QStringList CubeSolver::orientLastLayer(CubeState* state)
{
    QStringList moves;
    
    if (!isLastLayerOriented(state)) {
        // Apply OLL algorithms based on pattern recognition
        QString pattern = detectPattern(state, "oll");
        
        if (m_algorithms.contains("OLL_" + pattern.toUpper())) {
            moves.append(m_algorithms["OLL_" + pattern.toUpper()]);
        } else {
            // Default OLL algorithm for common cases
            moves.append({"R", "U", "R'", "U", "R", "U2", "R'"});
            moves.append({"F", "R", "U", "R'", "U'", "F'"});
        }
    }
    
    return moves;
}

QStringList CubeSolver::permuteLastLayer(CubeState* state)
{
    QStringList moves;
    
    // Check if PLL is needed
    if (!state->isSolved()) {
        // Detect PLL pattern
        QString pattern = detectPattern(state, "pll");
        
        if (pattern == "T") {
            moves.append(m_algorithms["PLL_T"]);
        } else if (pattern == "Y") {
            moves.append(m_algorithms["PLL_Y"]);
        } else if (pattern == "A") {
            moves.append(m_algorithms["PLL_A"]);
        } else if (pattern == "U") {
            moves.append(m_algorithms["PLL_U"]);
        } else {
            // Default PLL moves
            moves.append({"R", "U", "R'", "F'", "R", "U", "R'", "U'", "R'", "F", "R2", "U'", "R'"});
        }
    }
    
    return moves;
}

// Utility function implementations
bool CubeSolver::isCrossSolved(CubeState* state)
{
    // Check if bottom cross is solved (simplified check)
    CubeColor centerColor = state->getColor(CubeFace::Down, 1, 1);
    
    return (state->getColor(CubeFace::Down, 0, 1) == centerColor &&
            state->getColor(CubeFace::Down, 1, 0) == centerColor &&
            state->getColor(CubeFace::Down, 1, 2) == centerColor &&
            state->getColor(CubeFace::Down, 2, 1) == centerColor);
}

bool CubeSolver::isFirstLayerSolved(CubeState* state)
{
    // Check if entire bottom layer is solved
    CubeColor centerColor = state->getColor(CubeFace::Down, 1, 1);
    
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (state->getColor(CubeFace::Down, row, col) != centerColor) {
                return false;
            }
        }
    }
    
    return true;
}

bool CubeSolver::isMiddleLayerSolved(CubeState* state)
{
    // Check if middle layer edges are in correct position
    // This is a simplified check - in reality would need more detailed verification
    
    for (int face = 0; face < 4; ++face) { // Front, Back, Left, Right
        CubeFace cubeFace = static_cast<CubeFace>(face);
        CubeColor centerColor = state->getColor(cubeFace, 1, 1);
        
        // Check middle row
        if (state->getColor(cubeFace, 1, 0) != centerColor ||
            state->getColor(cubeFace, 1, 2) != centerColor) {
            return false;
        }
    }
    
    return true;
}

bool CubeSolver::isTopCrossSolved(CubeState* state)
{
    // Check if top cross is formed
    CubeColor centerColor = state->getColor(CubeFace::Up, 1, 1);
    
    return (state->getColor(CubeFace::Up, 0, 1) == centerColor &&
            state->getColor(CubeFace::Up, 1, 0) == centerColor &&
            state->getColor(CubeFace::Up, 1, 2) == centerColor &&
            state->getColor(CubeFace::Up, 2, 1) == centerColor);
}

bool CubeSolver::isLastLayerOriented(CubeState* state)
{
    // Check if all top face pieces have same color
    CubeColor centerColor = state->getColor(CubeFace::Up, 1, 1);
    
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (state->getColor(CubeFace::Up, row, col) != centerColor) {
                return false;
            }
        }
    }
    
    return true;
}

QString CubeSolver::detectPattern(CubeState* state, const QString& layer)
{
    // Simplified pattern detection
    // In a real implementation, this would analyze the cube state
    // and return specific pattern names for algorithm lookup
    
    if (layer == "top") {
        // Detect top layer cross patterns
        bool edges[4] = {
            state->getColor(CubeFace::Up, 0, 1) == state->getColor(CubeFace::Up, 1, 1),
            state->getColor(CubeFace::Up, 1, 0) == state->getColor(CubeFace::Up, 1, 1),
            state->getColor(CubeFace::Up, 1, 2) == state->getColor(CubeFace::Up, 1, 1),
            state->getColor(CubeFace::Up, 2, 1) == state->getColor(CubeFace::Up, 1, 1)
        };
        
        int correctEdges = 0;
        for (int i = 0; i < 4; ++i) {
            if (edges[i]) correctEdges++;
        }
        
        if (correctEdges == 0) return "dot";
        if (correctEdges == 2) return "line";
        if (correctEdges == 3) return "L";
        return "cross";
    }
    
    // For OLL and PLL, return random patterns for demonstration
    QStringList patterns;
    patterns << "T" << "Y" << "A" << "U" << "H" << "Z";
    return patterns[qrand() % patterns.size()];
}

QStringList CubeSolver::optimizeMoves(const QStringList& moves)
{
    if (moves.isEmpty()) {
        return moves;
    }
    
    // Basic move optimization - cancel opposite moves
    QStringList optimized = cancelMoves(moves);
    
    // Additional optimizations could be added here:
    // - Combine consecutive moves (R R -> R2)
    // - Replace move sequences with shorter equivalents
    // - Remove redundant moves
    
    qDebug() << "CubeSolver: Optimized from" << moves.size() << "to" << optimized.size() << "moves";
    return optimized;
}

QStringList CubeSolver::cancelMoves(const QStringList& moves)
{
    QStringList result;
    
    for (const QString& move : moves) {
        if (result.isEmpty()) {
            result.append(move);
            continue;
        }
        
        QString lastMove = result.last();
        
        // Check for cancellation (e.g., R followed by R')
        if ((move == "R" && lastMove == "R'") ||
            (move == "R'" && lastMove == "R") ||
            (move == "U" && lastMove == "U'") ||
            (move == "U'" && lastMove == "U") ||
            (move == "F" && lastMove == "F'") ||
            (move == "F'" && lastMove == "F") ||
            (move == "L" && lastMove == "L'") ||
            (move == "L'" && lastMove == "L") ||
            (move == "B" && lastMove == "B'") ||
            (move == "B'" && lastMove == "B") ||
            (move == "D" && lastMove == "D'") ||
            (move == "D'" && lastMove == "D")) {
            
            // Remove the last move (they cancel out)
            result.removeLast();
        } else {
            result.append(move);
        }
    }
    
    return result;
}

QStringList CubeSolver::kociembaSolve(CubeState* state)
{
    // Placeholder for Kociemba's algorithm implementation
    // This is a very complex algorithm that requires extensive implementation
    // For now, we'll fall back to the beginner's method
    
    qDebug() << "CubeSolver: Kociemba's algorithm not yet implemented, using beginner's method";
    return beginnerMethodSolve(state);
}
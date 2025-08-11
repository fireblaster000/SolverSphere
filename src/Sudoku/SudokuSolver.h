#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QPoint>
#include <QMap>

class SudokuGrid;

struct SolutionStep {
    int row;
    int col;
    int value;
    QString reason;
    QString technique;
};

class SudokuSolver : public QObject
{
    Q_OBJECT

public:
    explicit SudokuSolver(QObject *parent = nullptr);
    ~SudokuSolver();

    // Main solving functions
    bool solve(SudokuGrid* grid);
    QVector<SolutionStep> solveWithSteps(SudokuGrid* grid);
    bool isUniqueSolution(SudokuGrid* grid);

private:
    // Backtracking algorithm
    bool backtrackSolve(SudokuGrid* grid, int row, int col);
    QPoint findNextEmptyCell(SudokuGrid* grid, int startRow = 0, int startCol = 0);
    
    // Human-like solving techniques
    bool applyNakedSingles(SudokuGrid* grid, QVector<SolutionStep>& steps);
    bool applyHiddenSingles(SudokuGrid* grid, QVector<SolutionStep>& steps);
    bool applyNakedPairs(SudokuGrid* grid, QVector<SolutionStep>& steps);
    bool applyPointingPairs(SudokuGrid* grid, QVector<SolutionStep>& steps);
    bool applyBoxLineReduction(SudokuGrid* grid, QVector<SolutionStep>& steps);
    
    // Constraint propagation
    bool eliminateByConstraints(SudokuGrid* grid, QVector<SolutionStep>& steps);
    bool propagateConstraints(SudokuGrid* grid);
    
    // Logical deduction
    QString explainNakedSingle(int row, int col, int value, const QVector<int>& validValues);
    QString explainHiddenSingle(int row, int col, int value, const QString& unit);
    QString explainConstraintElimination(int row, int col, int value);
    
    // Cell analysis
    QVector<int> getCandidates(SudokuGrid* grid, int row, int col);
    bool hasOnlyOneCandidate(SudokuGrid* grid, int row, int col, int& candidate);
    bool isOnlyPlaceInUnit(SudokuGrid* grid, int row, int col, int value, const QString& unit);
    
    // Unit analysis (rows, columns, blocks)
    QVector<QPoint> getRowCells(int row);
    QVector<QPoint> getColumnCells(int col);
    QVector<QPoint> getBlockCells(int blockRow, int blockCol);
    
    // Strategy selection
    bool applySolvingTechniques(SudokuGrid* grid, QVector<SolutionStep>& steps);
    QString selectBestTechnique(SudokuGrid* grid);
    
    // Difficulty assessment
    int assessDifficulty(const QVector<SolutionStep>& steps);
    QString getDifficultyLevel(int difficulty);
    
    // Validation and verification
    bool validateStep(SudokuGrid* grid, const SolutionStep& step);
    bool checkUniqueness(SudokuGrid* grid);
    
    // Utility functions
    SudokuGrid* copyGrid(SudokuGrid* original);
    bool gridsEqual(SudokuGrid* grid1, SudokuGrid* grid2);
    bool generateSamplePuzzle(SudokuGrid* grid);
    void countSolutions(SudokuGrid* grid, int row, int col);
    
private:
    // Solving statistics
    int m_solutionCount;
    int m_backtrackCount;
    int m_maxDepth;
    
    // Technique preferences
    QStringList m_techniqueOrder;
    QMap<QString, int> m_techniqueDifficulty;
    
    // Configuration
    bool m_useLogicalTechniques;
    bool m_explainSteps;
    int m_maxSolutions;
};

#endif // SUDOKUSOLVER_H
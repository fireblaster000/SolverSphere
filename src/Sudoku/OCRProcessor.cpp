#include "OCRProcessor.h"
#include "SudokuGrid.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

OCRProcessor::OCRProcessor(QObject *parent)
    : QObject(parent)
    , m_language("eng")
    , m_saveDebugImages(false)
{
    // Set up debug path
    m_debugPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/sudoku_debug/";
    QDir().mkpath(m_debugPath);
    
    qDebug() << "OCRProcessor: Initialized in demo mode";
}

OCRProcessor::~OCRProcessor()
{
}

bool OCRProcessor::processPuzzle(const QString& imagePath, SudokuGrid* grid)
{
    if (!grid) {
        qDebug() << "OCRProcessor: Invalid grid pointer";
        return false;
    }
    
    qDebug() << "OCRProcessor: Processing image:" << imagePath << "(demo mode)";
    
    return generateSamplePuzzle(grid);
}

bool OCRProcessor::generateSamplePuzzle(SudokuGrid* grid)
{
    // Generate a sample Sudoku puzzle for demonstration
    qDebug() << "OCRProcessor: Generating sample puzzle for demonstration";
    
    // Sample puzzle (solvable)
    int puzzle[9][9] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };
    
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            grid->setValue(row, col, puzzle[row][col]);
        }
    }
    
    return true;
}
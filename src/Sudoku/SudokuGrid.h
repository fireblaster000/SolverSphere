#ifndef SUDOKUGRID_H
#define SUDOKUGRID_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QPoint>

class SudokuGrid : public QObject
{
    Q_OBJECT

public:
    explicit SudokuGrid(QObject *parent = nullptr);
    
    // Grid state management
    void reset();
    bool isValid() const;
    bool isSolved() const;
    bool isEmpty() const;
    
    // Value access
    int getValue(int row, int col) const;
    void setValue(int row, int col, int value);
    
    // Grid operations
    void clearCell(int row, int col);
    void clearAll();
    
    // Validation methods
    bool isValidValue(int row, int col, int value) const;
    bool hasConflict(int row, int col, int value) const;
    QVector<QPoint> getConflicts(int row, int col, int value) const;
    
    // Constraint checking
    bool isRowValid(int row) const;
    bool isColumnValid(int col) const;
    bool isBlockValid(int blockRow, int blockCol) const;
    
    // Utility functions
    QVector<int> getValidValues(int row, int col) const;
    QVector<QPoint> getEmptyCells() const;
    int getEmptyCount() const;
    
    // Block utilities
    int getBlockIndex(int row, int col) const;
    QPoint getBlockStartPosition(int blockIndex) const;
    QVector<QPoint> getBlockCells(int blockIndex) const;
    
    // String representation
    QString toString() const;
    void fromString(const QString& gridString);
    
    // Grid state
    SudokuGrid* copy() const;
    bool equals(const SudokuGrid* other) const;

signals:
    void valueChanged(int row, int col, int newValue, int oldValue);
    void gridChanged();

private:
    void initializeGrid();
    bool isValidPosition(int row, int col) const;
    bool hasRowConflict(int row, int col, int value) const;
    bool hasColumnConflict(int row, int col, int value) const;
    bool hasBlockConflict(int row, int col, int value) const;

private:
    QVector<QVector<int>> m_grid;  // 9x9 grid, 0 = empty
    static constexpr int GRID_SIZE = 9;
    static constexpr int BLOCK_SIZE = 3;
};

#endif // SUDOKUGRID_H
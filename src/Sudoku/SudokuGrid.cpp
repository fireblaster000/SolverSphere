#include "SudokuGrid.h"
#include <QDebug>
#include <QSet>

SudokuGrid::SudokuGrid(QObject *parent)
    : QObject(parent)
{
    initializeGrid();
}

void SudokuGrid::initializeGrid()
{
    m_grid.resize(GRID_SIZE);
    for (int i = 0; i < GRID_SIZE; ++i) {
        m_grid[i].resize(GRID_SIZE);
        for (int j = 0; j < GRID_SIZE; ++j) {
            m_grid[i][j] = 0; // 0 represents empty cell
        }
    }
}

void SudokuGrid::reset()
{
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            m_grid[row][col] = 0;
        }
    }
    emit gridChanged();
}

bool SudokuGrid::isValid() const
{
    // Check all rows, columns, and blocks for conflicts
    for (int i = 0; i < GRID_SIZE; ++i) {
        if (!isRowValid(i) || !isColumnValid(i)) {
            return false;
        }
    }
    
    // Check all 3x3 blocks
    for (int blockRow = 0; blockRow < 3; ++blockRow) {
        for (int blockCol = 0; blockCol < 3; ++blockCol) {
            if (!isBlockValid(blockRow, blockCol)) {
                return false;
            }
        }
    }
    
    return true;
}

bool SudokuGrid::isSolved() const
{
    if (!isValid()) {
        return false;
    }
    
    // Check if all cells are filled
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (m_grid[row][col] == 0) {
                return false;
            }
        }
    }
    
    return true;
}

bool SudokuGrid::isEmpty() const
{
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (m_grid[row][col] != 0) {
                return false;
            }
        }
    }
    return true;
}

int SudokuGrid::getValue(int row, int col) const
{
    if (isValidPosition(row, col)) {
        return m_grid[row][col];
    }
    return 0;
}

void SudokuGrid::setValue(int row, int col, int value)
{
    if (!isValidPosition(row, col)) {
        return;
    }
    
    if (value < 0 || value > 9) {
        return;
    }
    
    int oldValue = m_grid[row][col];
    m_grid[row][col] = value;
    
    emit valueChanged(row, col, value, oldValue);
    emit gridChanged();
}

void SudokuGrid::clearCell(int row, int col)
{
    setValue(row, col, 0);
}

void SudokuGrid::clearAll()
{
    reset();
}

bool SudokuGrid::isValidValue(int row, int col, int value) const
{
    if (!isValidPosition(row, col) || value < 1 || value > 9) {
        return false;
    }
    
    return !hasConflict(row, col, value);
}

bool SudokuGrid::hasConflict(int row, int col, int value) const
{
    return hasRowConflict(row, col, value) ||
           hasColumnConflict(row, col, value) ||
           hasBlockConflict(row, col, value);
}

QVector<QPoint> SudokuGrid::getConflicts(int row, int col, int value) const
{
    QVector<QPoint> conflicts;
    
    if (!isValidPosition(row, col) || value < 1 || value > 9) {
        return conflicts;
    }
    
    // Check row conflicts
    for (int c = 0; c < GRID_SIZE; ++c) {
        if (c != col && m_grid[row][c] == value) {
            conflicts.append(QPoint(row, c));
        }
    }
    
    // Check column conflicts
    for (int r = 0; r < GRID_SIZE; ++r) {
        if (r != row && m_grid[r][col] == value) {
            conflicts.append(QPoint(r, col));
        }
    }
    
    // Check block conflicts
    int blockStartRow = (row / BLOCK_SIZE) * BLOCK_SIZE;
    int blockStartCol = (col / BLOCK_SIZE) * BLOCK_SIZE;
    
    for (int r = blockStartRow; r < blockStartRow + BLOCK_SIZE; ++r) {
        for (int c = blockStartCol; c < blockStartCol + BLOCK_SIZE; ++c) {
            if ((r != row || c != col) && m_grid[r][c] == value) {
                conflicts.append(QPoint(r, c));
            }
        }
    }
    
    return conflicts;
}

bool SudokuGrid::isRowValid(int row) const
{
    if (row < 0 || row >= GRID_SIZE) {
        return false;
    }
    
    QSet<int> seen;
    for (int col = 0; col < GRID_SIZE; ++col) {
        int value = m_grid[row][col];
        if (value != 0) {
            if (seen.contains(value)) {
                return false; // Duplicate found
            }
            seen.insert(value);
        }
    }
    return true;
}

bool SudokuGrid::isColumnValid(int col) const
{
    if (col < 0 || col >= GRID_SIZE) {
        return false;
    }
    
    QSet<int> seen;
    for (int row = 0; row < GRID_SIZE; ++row) {
        int value = m_grid[row][col];
        if (value != 0) {
            if (seen.contains(value)) {
                return false; // Duplicate found
            }
            seen.insert(value);
        }
    }
    return true;
}

bool SudokuGrid::isBlockValid(int blockRow, int blockCol) const
{
    if (blockRow < 0 || blockRow >= 3 || blockCol < 0 || blockCol >= 3) {
        return false;
    }
    
    QSet<int> seen;
    int startRow = blockRow * BLOCK_SIZE;
    int startCol = blockCol * BLOCK_SIZE;
    
    for (int row = startRow; row < startRow + BLOCK_SIZE; ++row) {
        for (int col = startCol; col < startCol + BLOCK_SIZE; ++col) {
            int value = m_grid[row][col];
            if (value != 0) {
                if (seen.contains(value)) {
                    return false; // Duplicate found
                }
                seen.insert(value);
            }
        }
    }
    return true;
}

QVector<int> SudokuGrid::getValidValues(int row, int col) const
{
    QVector<int> validValues;
    
    if (!isValidPosition(row, col) || m_grid[row][col] != 0) {
        return validValues; // Cell is not empty
    }
    
    for (int value = 1; value <= 9; ++value) {
        if (isValidValue(row, col, value)) {
            validValues.append(value);
        }
    }
    
    return validValues;
}

QVector<QPoint> SudokuGrid::getEmptyCells() const
{
    QVector<QPoint> emptyCells;
    
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (m_grid[row][col] == 0) {
                emptyCells.append(QPoint(row, col));
            }
        }
    }
    
    return emptyCells;
}

int SudokuGrid::getEmptyCount() const
{
    int count = 0;
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (m_grid[row][col] == 0) {
                count++;
            }
        }
    }
    return count;
}

int SudokuGrid::getBlockIndex(int row, int col) const
{
    if (!isValidPosition(row, col)) {
        return -1;
    }
    
    return (row / BLOCK_SIZE) * 3 + (col / BLOCK_SIZE);
}

QPoint SudokuGrid::getBlockStartPosition(int blockIndex) const
{
    if (blockIndex < 0 || blockIndex >= 9) {
        return QPoint(-1, -1);
    }
    
    int blockRow = blockIndex / 3;
    int blockCol = blockIndex % 3;
    
    return QPoint(blockRow * BLOCK_SIZE, blockCol * BLOCK_SIZE);
}

QVector<QPoint> SudokuGrid::getBlockCells(int blockIndex) const
{
    QVector<QPoint> cells;
    QPoint startPos = getBlockStartPosition(blockIndex);
    
    if (startPos.x() == -1) {
        return cells;
    }
    
    for (int row = startPos.x(); row < startPos.x() + BLOCK_SIZE; ++row) {
        for (int col = startPos.y(); col < startPos.y() + BLOCK_SIZE; ++col) {
            cells.append(QPoint(row, col));
        }
    }
    
    return cells;
}

QString SudokuGrid::toString() const
{
    QString result;
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            result += QString::number(m_grid[row][col]);
        }
    }
    return result;
}

void SudokuGrid::fromString(const QString& gridString)
{
    if (gridString.length() != 81) {
        qDebug() << "SudokuGrid: Invalid grid string length:" << gridString.length();
        return;
    }
    
    int index = 0;
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            QChar ch = gridString[index];
            int value = ch.digitValue();
            if (value >= 0 && value <= 9) {
                m_grid[row][col] = value;
            } else {
                m_grid[row][col] = 0;
            }
            index++;
        }
    }
    
    emit gridChanged();
}

SudokuGrid* SudokuGrid::copy() const
{
    SudokuGrid* newGrid = new SudokuGrid;
    newGrid->m_grid = m_grid;
    return newGrid;
}

bool SudokuGrid::equals(const SudokuGrid* other) const
{
    if (!other) {
        return false;
    }
    
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (m_grid[row][col] != other->m_grid[row][col]) {
                return false;
            }
        }
    }
    
    return true;
}

// Private helper methods

bool SudokuGrid::isValidPosition(int row, int col) const
{
    return row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE;
}

bool SudokuGrid::hasRowConflict(int row, int col, int value) const
{
    for (int c = 0; c < GRID_SIZE; ++c) {
        if (c != col && m_grid[row][c] == value) {
            return true;
        }
    }
    return false;
}

bool SudokuGrid::hasColumnConflict(int row, int col, int value) const
{
    for (int r = 0; r < GRID_SIZE; ++r) {
        if (r != row && m_grid[r][col] == value) {
            return true;
        }
    }
    return false;
}

bool SudokuGrid::hasBlockConflict(int row, int col, int value) const
{
    int blockStartRow = (row / BLOCK_SIZE) * BLOCK_SIZE;
    int blockStartCol = (col / BLOCK_SIZE) * BLOCK_SIZE;
    
    for (int r = blockStartRow; r < blockStartRow + BLOCK_SIZE; ++r) {
        for (int c = blockStartCol; c < blockStartCol + BLOCK_SIZE; ++c) {
            if ((r != row || c != col) && m_grid[r][c] == value) {
                return true;
            }
        }
    }
    return false;
}
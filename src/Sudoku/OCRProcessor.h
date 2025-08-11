#ifndef OCRPROCESSOR_H
#define OCRPROCESSOR_H

#include <QObject>
#include <QString>
#include <QRect>

class SudokuGrid;

class OCRProcessor : public QObject
{
    Q_OBJECT

public:
    explicit OCRProcessor(QObject *parent = nullptr);
    ~OCRProcessor();

    // Main processing function
    bool processPuzzle(const QString& imagePath, SudokuGrid* grid);

private:
    // Generate sample puzzle for demo
    bool generateSamplePuzzle(SudokuGrid* grid);
    
    // Configuration parameters
    QString m_language;
    bool m_saveDebugImages;
    QString m_debugPath;
};

#endif // OCRPROCESSOR_H
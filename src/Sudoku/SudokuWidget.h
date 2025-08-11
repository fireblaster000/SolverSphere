#ifndef SUDOKUWIDGET_H
#define SUDOKUWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QProgressBar>
#include <QGroupBox>
#include <QSlider>
#include <QSpinBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QTableWidget>

class SudokuGrid;
class SudokuGrid;
class OCRProcessor;
class SudokuSolver;

// Forward declare the struct
struct SolutionStep;

class SudokuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SudokuWidget(QWidget *parent = nullptr);
    ~SudokuWidget();

private slots:
    void loadImage();
    void scanPuzzle();
    void solvePuzzle();
    void resetPuzzle();
    void nextStep();
    void prevStep();
    void playAnimation();
    void pauseAnimation();
    void stepChanged(int step);
    void animationSpeedChanged(int speed);
    void cellChanged(int row, int col);
    void validatePuzzle();

private:
    void setupUI();
    void setupControlPanel();
    void setupGridPanel();
    void setupSolutionPanel();
    void updateGridDisplay();
    void updateSolutionDisplay();
    void updateNavigationControls();
    void highlightCurrentStep();

    // UI Components
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_contentLayout;
    
    // Left panel - Controls
    QGroupBox *m_controlGroup;
    QPushButton *m_loadImageButton;
    QPushButton *m_scanButton;
    QPushButton *m_solveButton;
    QPushButton *m_resetButton;
    QPushButton *m_validateButton;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    
    // Center panel - Sudoku Grid
    QGroupBox *m_gridGroup;
    QTableWidget *m_gridTable;
    
    // Right panel - Solution steps
    QGroupBox *m_solutionGroup;
    QTextEdit *m_solutionText;
    QPushButton *m_prevButton;
    QPushButton *m_nextButton;
    QPushButton *m_playButton;
    QPushButton *m_pauseButton;
    QSlider *m_stepSlider;
    QLabel *m_stepLabel;
    QSlider *m_speedSlider;
    QLabel *m_speedLabel;
    
    // Core components
    SudokuGrid *m_sudokuGrid;
    OCRProcessor *m_ocrProcessor;
    SudokuSolver *m_sudokuSolver;
    
    // Solution data
    QVector<SolutionStep> m_solutionSteps;
    int m_currentStep;
    bool m_isPlaying;
    QTimer *m_animationTimer;
    
    QString m_currentImagePath;
};

#endif // SUDOKUWIDGET_H
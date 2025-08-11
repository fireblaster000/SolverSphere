#ifndef RUBIKSCUBEWIDGET_H
#define RUBIKSCUBEWIDGET_H

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

class CubeState;
class ColorDetector;
class CubeSolver;
class Cube3DRenderer;

class RubiksCubeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RubiksCubeWidget(QWidget *parent = nullptr);
    ~RubiksCubeWidget();

private slots:
    void loadImage();
    void scanCube();
    void solveCube();
    void resetCube();
    void nextStep();
    void prevStep();
    void playAnimation();
    void pauseAnimation();
    void stepChanged(int step);
    void animationSpeedChanged(int speed);

private:
    void setupUI();
    void setupControlPanel();
    void setupVisualizationPanel();
    void setupSolutionPanel();
    void updateCubeDisplay();
    void updateSolutionDisplay();
    void updateNavigationControls();

    // UI Components
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_contentLayout;
    
    // Left panel - Controls
    QGroupBox *m_controlGroup;
    QPushButton *m_loadImageButton;
    QPushButton *m_scanButton;
    QPushButton *m_solveButton;
    QPushButton *m_resetButton;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    
    // Center panel - 3D Cube Visualization
    QGroupBox *m_visualizationGroup;
    Cube3DRenderer *m_cubeRenderer;
    
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
    CubeState *m_cubeState;
    ColorDetector *m_colorDetector;
    CubeSolver *m_cubeSolver;
    
    // Solution data
    QStringList m_solutionSteps;
    int m_currentStep;
    bool m_isPlaying;
    QTimer *m_animationTimer;
    
    QString m_currentImagePath;
};

#endif // RUBIKSCUBEWIDGET_H
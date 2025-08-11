#include "RubiksCubeWidget.h"
#include "CubeState.h"
#include "ColorDetector.h"
#include "CubeSolver.h"
#include "Cube3DRenderer.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QThread>
#include <QSplitter>

RubiksCubeWidget::RubiksCubeWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_contentLayout(nullptr)
    , m_controlGroup(nullptr)
    , m_visualizationGroup(nullptr)
    , m_solutionGroup(nullptr)
    , m_cubeRenderer(nullptr)
    , m_cubeState(nullptr)
    , m_colorDetector(nullptr)
    , m_cubeSolver(nullptr)
    , m_currentStep(0)
    , m_isPlaying(false)
    , m_animationTimer(nullptr)
{
    // Initialize core components
    m_cubeState = new CubeState(this);
    m_colorDetector = new ColorDetector(this);
    m_cubeSolver = new CubeSolver(this);
    m_animationTimer = new QTimer(this);
    
    setupUI();
    
    // Connect timer
    connect(m_animationTimer, &QTimer::timeout, this, &RubiksCubeWidget::nextStep);
    m_animationTimer->setInterval(1000); // 1 second default
    
    // Initialize display
    updateCubeDisplay();
    updateNavigationControls();
}

RubiksCubeWidget::~RubiksCubeWidget()
{
}

void RubiksCubeWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Title
    QLabel *titleLabel = new QLabel("🧊 Rubik's Cube Solver");
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #FF5252; margin: 10px; padding: 10px;");
    m_mainLayout->addWidget(titleLabel);
    
    // Create splitter for resizable panels
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    m_mainLayout->addWidget(splitter);
    
    setupControlPanel();
    setupVisualizationPanel();
    setupSolutionPanel();
    
    splitter->addWidget(m_controlGroup);
    splitter->addWidget(m_visualizationGroup);
    splitter->addWidget(m_solutionGroup);
    
    // Set initial sizes (25%, 50%, 25%)
    splitter->setSizes({250, 500, 250});
}

void RubiksCubeWidget::setupControlPanel()
{
    m_controlGroup = new QGroupBox("Controls");
    m_controlGroup->setMinimumWidth(200);
    
    QVBoxLayout *layout = new QVBoxLayout(m_controlGroup);
    layout->setSpacing(15);
    
    // Load Image Button
    m_loadImageButton = new QPushButton("📁 Load Cube Image");
    m_loadImageButton->setMinimumHeight(40);
    m_loadImageButton->setStyleSheet(
        "QPushButton { background: #4CAF50; color: white; border: none; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background: #45a049; }"
    );
    connect(m_loadImageButton, &QPushButton::clicked, this, &RubiksCubeWidget::loadImage);
    
    // Scan Button
    m_scanButton = new QPushButton("🔍 Scan Cube Colors");
    m_scanButton->setMinimumHeight(40);
    m_scanButton->setEnabled(false);
    m_scanButton->setStyleSheet(
        "QPushButton { background: #2196F3; color: white; border: none; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background: #1976D2; }"
        "QPushButton:disabled { background: #666; }"
    );
    connect(m_scanButton, &QPushButton::clicked, this, &RubiksCubeWidget::scanCube);
    
    // Solve Button
    m_solveButton = new QPushButton("🧠 Solve Cube");
    m_solveButton->setMinimumHeight(40);
    m_solveButton->setEnabled(false);
    m_solveButton->setStyleSheet(
        "QPushButton { background: #FF9800; color: white; border: none; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background: #F57C00; }"
        "QPushButton:disabled { background: #666; }"
    );
    connect(m_solveButton, &QPushButton::clicked, this, &RubiksCubeWidget::solveCube);
    
    // Reset Button
    m_resetButton = new QPushButton("🔄 Reset");
    m_resetButton->setMinimumHeight(40);
    m_resetButton->setStyleSheet(
        "QPushButton { background: #f44336; color: white; border: none; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background: #d32f2f; }"
    );
    connect(m_resetButton, &QPushButton::clicked, this, &RubiksCubeWidget::resetCube);
    
    // Status Label
    m_statusLabel = new QLabel("Ready to load cube image...");
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setStyleSheet("color: #666; padding: 10px; background: #f5f5f5; border-radius: 5px;");
    
    // Progress Bar
    m_progressBar = new QProgressBar;
    m_progressBar->setVisible(false);
    m_progressBar->setStyleSheet(
        "QProgressBar { border: 2px solid grey; border-radius: 5px; background: white; }"
        "QProgressBar::chunk { background: #4CAF50; }"
    );
    
    layout->addWidget(m_loadImageButton);
    layout->addWidget(m_scanButton);
    layout->addWidget(m_solveButton);
    layout->addWidget(m_resetButton);
    layout->addWidget(m_statusLabel);
    layout->addWidget(m_progressBar);
    layout->addStretch();
}

void RubiksCubeWidget::setupVisualizationPanel()
{
    m_visualizationGroup = new QGroupBox("Cube Visualization");
    m_visualizationGroup->setMinimumWidth(400);
    
    QVBoxLayout *layout = new QVBoxLayout(m_visualizationGroup);
    
    // 3D Cube Renderer
    m_cubeRenderer = new Cube3DRenderer;
    m_cubeRenderer->setMinimumSize(400, 400);
    layout->addWidget(m_cubeRenderer);
    
    // Instructions
    QLabel *instructLabel = new QLabel("💡 Instructions:\n"
                                      "1. Load an image of your Rubik's Cube\n"
                                      "2. Scan all six sides for color detection\n"
                                      "3. Solve to see the step-by-step solution\n"
                                      "4. Use controls to navigate through steps");
    instructLabel->setStyleSheet("color: #888; font-size: 12px; padding: 10px; background: #f9f9f9; border-radius: 5px;");
    instructLabel->setWordWrap(true);
    layout->addWidget(instructLabel);
}

void RubiksCubeWidget::setupSolutionPanel()
{
    m_solutionGroup = new QGroupBox("Solution Steps");
    m_solutionGroup->setMinimumWidth(200);
    
    QVBoxLayout *layout = new QVBoxLayout(m_solutionGroup);
    
    // Solution text display
    m_solutionText = new QTextEdit;
    m_solutionText->setMaximumHeight(150);
    m_solutionText->setReadOnly(true);
    m_solutionText->setPlaceholderText("Solution steps will appear here...");
    
    // Navigation controls
    QHBoxLayout *navLayout = new QHBoxLayout;
    m_prevButton = new QPushButton("◀ Prev");
    m_nextButton = new QPushButton("Next ▶");
    m_prevButton->setEnabled(false);
    m_nextButton->setEnabled(false);
    navLayout->addWidget(m_prevButton);
    navLayout->addWidget(m_nextButton);
    
    connect(m_prevButton, &QPushButton::clicked, this, &RubiksCubeWidget::prevStep);
    connect(m_nextButton, &QPushButton::clicked, this, &RubiksCubeWidget::nextStep);
    
    // Playback controls
    QHBoxLayout *playLayout = new QHBoxLayout;
    m_playButton = new QPushButton("▶ Play");
    m_pauseButton = new QPushButton("⏸ Pause");
    m_pauseButton->setEnabled(false);
    playLayout->addWidget(m_playButton);
    playLayout->addWidget(m_pauseButton);
    
    connect(m_playButton, &QPushButton::clicked, this, &RubiksCubeWidget::playAnimation);
    connect(m_pauseButton, &QPushButton::clicked, this, &RubiksCubeWidget::pauseAnimation);
    
    // Step slider
    m_stepSlider = new QSlider(Qt::Horizontal);
    m_stepSlider->setEnabled(false);
    m_stepLabel = new QLabel("Step: 0/0");
    connect(m_stepSlider, &QSlider::valueChanged, this, &RubiksCubeWidget::stepChanged);
    
    // Speed control
    QLabel *speedLabelText = new QLabel("Animation Speed:");
    m_speedSlider = new QSlider(Qt::Horizontal);
    m_speedSlider->setRange(1, 10);
    m_speedSlider->setValue(5);
    m_speedLabel = new QLabel("1.0x");
    connect(m_speedSlider, &QSlider::valueChanged, this, &RubiksCubeWidget::animationSpeedChanged);
    
    layout->addWidget(m_solutionText);
    layout->addLayout(navLayout);
    layout->addLayout(playLayout);
    layout->addWidget(m_stepLabel);
    layout->addWidget(m_stepSlider);
    layout->addWidget(speedLabelText);
    layout->addWidget(m_speedSlider);
    layout->addWidget(m_speedLabel);
    layout->addStretch();
}

void RubiksCubeWidget::loadImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Load Cube Image", "", "Image Files (*.png *.jpg *.jpeg *.bmp)");
    
    if (!fileName.isEmpty()) {
        m_currentImagePath = fileName;
        m_statusLabel->setText("Image loaded: " + QFileInfo(fileName).fileName());
        m_scanButton->setEnabled(true);
        
        // TODO: Display image preview in the visualization panel
        QMessageBox::information(this, "Image Loaded", 
            "Cube image loaded successfully!\nClick 'Scan Cube Colors' to detect colors.");
    }
}

void RubiksCubeWidget::scanCube()
{
    if (m_currentImagePath.isEmpty()) {
        QMessageBox::warning(this, "No Image", "Please load a cube image first.");
        return;
    }
    
    m_statusLabel->setText("Scanning cube colors...");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 100);
    
    // Simulate scanning progress
    for (int i = 0; i <= 100; i += 20) {
        m_progressBar->setValue(i);
        QApplication::processEvents();
        QThread::msleep(100);
    }
    
    // Use color detector to analyze the image
    bool success = m_colorDetector->detectColors(m_currentImagePath, m_cubeState);
    
    m_progressBar->setVisible(false);
    
    if (success) {
        m_statusLabel->setText("Cube colors detected successfully!");
        m_solveButton->setEnabled(true);
        updateCubeDisplay();
        QMessageBox::information(this, "Scan Complete", 
            "Cube colors detected successfully!\nClick 'Solve Cube' to find the solution.");
    } else {
        m_statusLabel->setText("Failed to detect cube colors. Please try another image.");
        QMessageBox::warning(this, "Scan Failed", 
            "Could not detect cube colors properly. Please ensure:\n"
            "• All six faces are visible\n"
            "• Good lighting conditions\n"
            "• Clear image quality");
    }
}

void RubiksCubeWidget::solveCube()
{
    if (!m_cubeState->isValid()) {
        QMessageBox::warning(this, "Invalid Cube", "Please scan a valid cube first.");
        return;
    }
    
    m_statusLabel->setText("Solving cube...");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0); // Indeterminate progress
    
    QApplication::processEvents();
    
    // Use cube solver to find solution
    QStringList solution = m_cubeSolver->solve(m_cubeState);
    
    m_progressBar->setVisible(false);
    
    if (!solution.isEmpty()) {
        m_solutionSteps = solution;
        m_currentStep = 0;
        m_statusLabel->setText(QString("Solution found! %1 moves required.").arg(solution.size()));
        
        updateSolutionDisplay();
        updateNavigationControls();
        
        QMessageBox::information(this, "Solution Found", 
            QString("Solution found with %1 moves!\nUse the navigation controls to see each step.").arg(solution.size()));
    } else {
        m_statusLabel->setText("Could not find solution. Cube may be invalid.");
        QMessageBox::warning(this, "No Solution", 
            "Could not find a solution for this cube configuration.\n"
            "Please check if the cube was scanned correctly.");
    }
}

void RubiksCubeWidget::resetCube()
{
    m_cubeState->reset();
    m_solutionSteps.clear();
    m_currentStep = 0;
    m_currentImagePath.clear();
    
    m_scanButton->setEnabled(false);
    m_solveButton->setEnabled(false);
    m_statusLabel->setText("Ready to load cube image...");
    
    updateCubeDisplay();
    updateSolutionDisplay();
    updateNavigationControls();
    
    if (m_isPlaying) {
        pauseAnimation();
    }
}

void RubiksCubeWidget::nextStep()
{
    if (m_currentStep < m_solutionSteps.size() - 1) {
        m_currentStep++;
        updateSolutionDisplay();
        updateNavigationControls();
        updateCubeDisplay();
    } else if (m_isPlaying) {
        pauseAnimation();
    }
}

void RubiksCubeWidget::prevStep()
{
    if (m_currentStep > 0) {
        m_currentStep--;
        updateSolutionDisplay();
        updateNavigationControls();
        updateCubeDisplay();
    }
}

void RubiksCubeWidget::playAnimation()
{
    if (!m_solutionSteps.isEmpty() && m_currentStep < m_solutionSteps.size() - 1) {
        m_isPlaying = true;
        m_playButton->setEnabled(false);
        m_pauseButton->setEnabled(true);
        m_animationTimer->start();
    }
}

void RubiksCubeWidget::pauseAnimation()
{
    m_isPlaying = false;
    m_playButton->setEnabled(true);
    m_pauseButton->setEnabled(false);
    m_animationTimer->stop();
}

void RubiksCubeWidget::stepChanged(int step)
{
    if (step >= 0 && step < m_solutionSteps.size()) {
        m_currentStep = step;
        updateSolutionDisplay();
        updateNavigationControls();
        updateCubeDisplay();
    }
}

void RubiksCubeWidget::animationSpeedChanged(int speed)
{
    double speedMultiplier = speed / 5.0; // 0.2x to 2.0x speed
    int interval = static_cast<int>(1000 / speedMultiplier);
    m_animationTimer->setInterval(interval);
    m_speedLabel->setText(QString("%1x").arg(speedMultiplier, 0, 'f', 1));
}

void RubiksCubeWidget::updateCubeDisplay()
{
    if (m_cubeRenderer) {
        m_cubeRenderer->setCubeState(m_cubeState);
        m_cubeRenderer->update();
    }
}

void RubiksCubeWidget::updateSolutionDisplay()
{
    if (m_solutionSteps.isEmpty()) {
        m_solutionText->clear();
        m_solutionText->setPlaceholderText("Solution steps will appear here...");
        return;
    }
    
    QString text = "Solution Steps:\n\n";
    for (int i = 0; i < m_solutionSteps.size(); ++i) {
        if (i == m_currentStep) {
            text += QString(">>> %1. %2 <<<\n").arg(i + 1).arg(m_solutionSteps[i]);
        } else {
            text += QString("%1. %2\n").arg(i + 1).arg(m_solutionSteps[i]);
        }
    }
    
    m_solutionText->setText(text);
    
    // Scroll to current step
    QTextCursor cursor = m_solutionText->textCursor();
    cursor.movePosition(QTextCursor::Start);
    for (int i = 0; i <= m_currentStep; ++i) {
        cursor.movePosition(QTextCursor::Down);
    }
    m_solutionText->setTextCursor(cursor);
}

void RubiksCubeWidget::updateNavigationControls()
{
    bool hasSolution = !m_solutionSteps.isEmpty();
    
    m_prevButton->setEnabled(hasSolution && m_currentStep > 0);
    m_nextButton->setEnabled(hasSolution && m_currentStep < m_solutionSteps.size() - 1);
    m_playButton->setEnabled(hasSolution && m_currentStep < m_solutionSteps.size() - 1 && !m_isPlaying);
    m_pauseButton->setEnabled(m_isPlaying);
    
    m_stepSlider->setEnabled(hasSolution);
    if (hasSolution) {
        m_stepSlider->setRange(0, m_solutionSteps.size() - 1);
        m_stepSlider->setValue(m_currentStep);
        m_stepLabel->setText(QString("Step: %1/%2").arg(m_currentStep + 1).arg(m_solutionSteps.size()));
    } else {
        m_stepSlider->setRange(0, 0);
        m_stepSlider->setValue(0);
        m_stepLabel->setText("Step: 0/0");
    }
}
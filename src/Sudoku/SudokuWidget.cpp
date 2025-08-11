#include "SudokuWidget.h"
#include "SudokuGrid.h"
#include "OCRProcessor.h" 
#include "SudokuSolver.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QThread>
#include <QSplitter>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QFileInfo>

SudokuWidget::SudokuWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_contentLayout(nullptr)
    , m_controlGroup(nullptr)
    , m_gridGroup(nullptr)
    , m_solutionGroup(nullptr)
    , m_gridTable(nullptr)
    , m_sudokuGrid(nullptr)
    , m_ocrProcessor(nullptr)
    , m_sudokuSolver(nullptr)
    , m_currentStep(0)
    , m_isPlaying(false)
    , m_animationTimer(nullptr)
{
    // Initialize core components
    m_sudokuGrid = new SudokuGrid(this);
    m_ocrProcessor = new OCRProcessor(this);
    m_sudokuSolver = new SudokuSolver(this);
    m_animationTimer = new QTimer(this);
    
    setupUI();
    
    // Connect timer for animation
    connect(m_animationTimer, &QTimer::timeout, this, &SudokuWidget::nextStep);
    m_animationTimer->setInterval(1000); // 1 second default
    
    // Initialize display
    updateGridDisplay();
    updateNavigationControls();
}

SudokuWidget::~SudokuWidget()
{
}

void SudokuWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Title
    QLabel *titleLabel = new QLabel("🔢 Sudoku Solver");
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #26C6DA; margin: 10px; padding: 10px;");
    m_mainLayout->addWidget(titleLabel);
    
    // Create splitter for resizable panels
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    m_mainLayout->addWidget(splitter);
    
    setupControlPanel();
    setupGridPanel();
    setupSolutionPanel();
    
    splitter->addWidget(m_controlGroup);
    splitter->addWidget(m_gridGroup);
    splitter->addWidget(m_solutionGroup);
    
    // Set initial sizes (25%, 50%, 25%)
    splitter->setSizes({250, 500, 250});
}

void SudokuWidget::setupControlPanel()
{
    m_controlGroup = new QGroupBox("Controls");
    m_controlGroup->setMinimumWidth(200);
    
    QVBoxLayout *layout = new QVBoxLayout(m_controlGroup);
    layout->setSpacing(15);
    
    // Load Image Button
    m_loadImageButton = new QPushButton("📁 Load Puzzle Image");
    m_loadImageButton->setMinimumHeight(40);
    m_loadImageButton->setStyleSheet(
        "QPushButton { background: #4CAF50; color: white; border: none; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background: #45a049; }"
    );
    connect(m_loadImageButton, &QPushButton::clicked, this, &SudokuWidget::loadImage);
    
    // Scan Button
    m_scanButton = new QPushButton("🔍 Scan Numbers");
    m_scanButton->setMinimumHeight(40);
    m_scanButton->setEnabled(false);
    m_scanButton->setStyleSheet(
        "QPushButton { background: #2196F3; color: white; border: none; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background: #1976D2; }"
        "QPushButton:disabled { background: #666; }"
    );
    connect(m_scanButton, &QPushButton::clicked, this, &SudokuWidget::scanPuzzle);
    
    // Validate Button
    m_validateButton = new QPushButton("✓ Validate Puzzle");
    m_validateButton->setMinimumHeight(40);
    m_validateButton->setStyleSheet(
        "QPushButton { background: #9C27B0; color: white; border: none; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background: #7B1FA2; }"
    );
    connect(m_validateButton, &QPushButton::clicked, this, &SudokuWidget::validatePuzzle);
    
    // Solve Button
    m_solveButton = new QPushButton("🧠 Solve Puzzle");
    m_solveButton->setMinimumHeight(40);
    m_solveButton->setEnabled(false);
    m_solveButton->setStyleSheet(
        "QPushButton { background: #FF9800; color: white; border: none; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background: #F57C00; }"
        "QPushButton:disabled { background: #666; }"
    );
    connect(m_solveButton, &QPushButton::clicked, this, &SudokuWidget::solvePuzzle);
    
    // Reset Button
    m_resetButton = new QPushButton("🔄 Reset");
    m_resetButton->setMinimumHeight(40);
    m_resetButton->setStyleSheet(
        "QPushButton { background: #f44336; color: white; border: none; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background: #d32f2f; }"
    );
    connect(m_resetButton, &QPushButton::clicked, this, &SudokuWidget::resetPuzzle);
    
    // Status Label
    m_statusLabel = new QLabel("Ready to load puzzle image...");
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
    layout->addWidget(m_validateButton);
    layout->addWidget(m_solveButton);
    layout->addWidget(m_resetButton);
    layout->addWidget(m_statusLabel);
    layout->addWidget(m_progressBar);
    layout->addStretch();
}

void SudokuWidget::setupGridPanel()
{
    m_gridGroup = new QGroupBox("Sudoku Grid");
    m_gridGroup->setMinimumWidth(400);
    
    QVBoxLayout *layout = new QVBoxLayout(m_gridGroup);
    
    // Create 9x9 grid table
    m_gridTable = new QTableWidget(9, 9);
    m_gridTable->setMinimumSize(400, 400);
    m_gridTable->setMaximumSize(450, 450);
    
    // Set up table appearance
    m_gridTable->horizontalHeader()->setVisible(false);
    m_gridTable->verticalHeader()->setVisible(false);
    m_gridTable->setShowGrid(true);
    m_gridTable->setGridStyle(Qt::SolidLine);
    
    // Make cells square
    for (int i = 0; i < 9; ++i) {
        m_gridTable->setRowHeight(i, 45);
        m_gridTable->setColumnWidth(i, 45);
    }
    
    // Style the grid with thick borders for 3x3 blocks
    m_gridTable->setStyleSheet(
        "QTableWidget {"
        "    gridline-color: black;"
        "    border: 2px solid black;"
        "}"
        "QTableWidget::item {"
        "    border: 1px solid #888;"
        "    text-align: center;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #e3f2fd;"
        "}"
    );
    
    // Initialize cells
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem("");
            item->setTextAlignment(Qt::AlignCenter);
            m_gridTable->setItem(row, col, item);
            
            // Color 3x3 blocks differently
            if (((row / 3) + (col / 3)) % 2 == 0) {
                item->setBackground(QColor(240, 240, 240));
            } else {
                item->setBackground(QColor(255, 255, 255));
            }
        }
    }
    
    connect(m_gridTable, &QTableWidget::cellChanged, this, &SudokuWidget::cellChanged);
    
    layout->addWidget(m_gridTable);
    
    // Instructions
    QLabel *instructLabel = new QLabel("💡 Instructions:\n"
                                      "1. Load an image of a Sudoku puzzle\n"
                                      "2. Scan to detect numbers using OCR\n"
                                      "3. Manually edit any incorrect numbers\n"
                                      "4. Validate the puzzle constraints\n"
                                      "5. Solve to see step-by-step solution");
    instructLabel->setStyleSheet("color: #888; font-size: 12px; padding: 10px; background: #f9f9f9; border-radius: 5px;");
    instructLabel->setWordWrap(true);
    layout->addWidget(instructLabel);
}

void SudokuWidget::setupSolutionPanel()
{
    m_solutionGroup = new QGroupBox("Solution Steps");
    m_solutionGroup->setMinimumWidth(200);
    
    QVBoxLayout *layout = new QVBoxLayout(m_solutionGroup);
    
    // Solution text display
    m_solutionText = new QTextEdit;
    m_solutionText->setMaximumHeight(200);
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
    
    connect(m_prevButton, &QPushButton::clicked, this, &SudokuWidget::prevStep);
    connect(m_nextButton, &QPushButton::clicked, this, &SudokuWidget::nextStep);
    
    // Playback controls
    QHBoxLayout *playLayout = new QHBoxLayout;
    m_playButton = new QPushButton("▶ Play");
    m_pauseButton = new QPushButton("⏸ Pause");
    m_pauseButton->setEnabled(false);
    playLayout->addWidget(m_playButton);
    playLayout->addWidget(m_pauseButton);
    
    connect(m_playButton, &QPushButton::clicked, this, &SudokuWidget::playAnimation);
    connect(m_pauseButton, &QPushButton::clicked, this, &SudokuWidget::pauseAnimation);
    
    // Step slider
    m_stepSlider = new QSlider(Qt::Horizontal);
    m_stepSlider->setEnabled(false);
    m_stepLabel = new QLabel("Step: 0/0");
    connect(m_stepSlider, &QSlider::valueChanged, this, &SudokuWidget::stepChanged);
    
    // Speed control
    QLabel *speedLabelText = new QLabel("Animation Speed:");
    m_speedSlider = new QSlider(Qt::Horizontal);
    m_speedSlider->setRange(1, 10);
    m_speedSlider->setValue(5);
    m_speedLabel = new QLabel("1.0x");
    connect(m_speedSlider, &QSlider::valueChanged, this, &SudokuWidget::animationSpeedChanged);
    
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

void SudokuWidget::loadImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Load Sudoku Image", "", "Image Files (*.png *.jpg *.jpeg *.bmp)");
    
    if (!fileName.isEmpty()) {
        m_currentImagePath = fileName;
        m_statusLabel->setText("Image loaded: " + QFileInfo(fileName).fileName());
        m_scanButton->setEnabled(true);
        
        QMessageBox::information(this, "Image Loaded", 
            "Sudoku image loaded successfully!\nClick 'Scan Numbers' to detect digits.");
    }
}

void SudokuWidget::scanPuzzle()
{
    if (m_currentImagePath.isEmpty()) {
        QMessageBox::warning(this, "No Image", "Please load a puzzle image first.");
        return;
    }
    
    m_statusLabel->setText("Scanning puzzle numbers...");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 100);
    
    // Simulate OCR progress
    for (int i = 0; i <= 100; i += 10) {
        m_progressBar->setValue(i);
        QApplication::processEvents();
        QThread::msleep(100);
    }
    
    // Use OCR processor to analyze the image
    bool success = m_ocrProcessor->processPuzzle(m_currentImagePath, m_sudokuGrid);
    
    m_progressBar->setVisible(false);
    
    if (success) {
        m_statusLabel->setText("Numbers detected successfully! You can edit any incorrect values.");
        m_solveButton->setEnabled(true);
        updateGridDisplay();
        QMessageBox::information(this, "Scan Complete", 
            "Numbers detected successfully!\nPlease verify the results and edit any incorrect values manually.\nThen click 'Solve Puzzle' to find the solution.");
    } else {
        m_statusLabel->setText("Failed to detect numbers. Please try another image.");
        QMessageBox::warning(this, "Scan Failed", 
            "Could not detect numbers properly. Please ensure:\n"
            "• Clear image with good contrast\n"
            "• Sudoku grid is properly aligned\n"
            "• Numbers are clearly visible");
    }
}

void SudokuWidget::validatePuzzle()
{
    if (m_sudokuGrid->isValid()) {
        m_statusLabel->setText("Puzzle is valid! Ready to solve.");
        QMessageBox::information(this, "Validation Success", "The Sudoku puzzle is valid and can be solved!");
    } else {
        m_statusLabel->setText("Puzzle contains invalid entries. Please check for duplicates.");
        QMessageBox::warning(this, "Validation Failed", 
            "The puzzle is invalid. Please check for:\n"
            "• Duplicate numbers in rows\n"
            "• Duplicate numbers in columns\n"
            "• Duplicate numbers in 3x3 blocks");
    }
}

void SudokuWidget::solvePuzzle()
{
    if (!m_sudokuGrid->isValid()) {
        QMessageBox::warning(this, "Invalid Puzzle", "Please validate the puzzle first.");
        return;
    }
    
    m_statusLabel->setText("Solving puzzle...");
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0); // Indeterminate progress
    
    QApplication::processEvents();
    
    // Use solver to find solution with steps
    m_solutionSteps = m_sudokuSolver->solveWithSteps(m_sudokuGrid);
    
    m_progressBar->setVisible(false);
    
    if (!m_solutionSteps.isEmpty()) {
        m_currentStep = 0;
        m_statusLabel->setText(QString("Solution found! %1 steps required.").arg(m_solutionSteps.size()));
        
        updateSolutionDisplay();
        updateNavigationControls();
        
        QMessageBox::information(this, "Solution Found", 
            QString("Solution found with %1 steps!\nUse the navigation controls to see each reasoning step.").arg(m_solutionSteps.size()));
    } else {
        m_statusLabel->setText("Could not find solution. Puzzle may be invalid or too difficult.");
        QMessageBox::warning(this, "No Solution", 
            "Could not find a solution for this puzzle.\n"
            "Please check if the puzzle was scanned correctly or is solvable.");
    }
}

void SudokuWidget::resetPuzzle()
{
    m_sudokuGrid->reset();
    m_solutionSteps.clear();
    m_currentStep = 0;
    m_currentImagePath.clear();
    
    m_scanButton->setEnabled(false);
    m_solveButton->setEnabled(false);
    m_statusLabel->setText("Ready to load puzzle image...");
    
    updateGridDisplay();
    updateSolutionDisplay();
    updateNavigationControls();
    
    if (m_isPlaying) {
        pauseAnimation();
    }
}

void SudokuWidget::nextStep()
{
    if (m_currentStep < m_solutionSteps.size() - 1) {
        m_currentStep++;
        updateSolutionDisplay();
        updateNavigationControls();
        highlightCurrentStep();
    } else if (m_isPlaying) {
        pauseAnimation();
    }
}

void SudokuWidget::prevStep()
{
    if (m_currentStep > 0) {
        m_currentStep--;
        updateSolutionDisplay();
        updateNavigationControls();
        highlightCurrentStep();
    }
}

void SudokuWidget::playAnimation()
{
    if (!m_solutionSteps.isEmpty() && m_currentStep < m_solutionSteps.size() - 1) {
        m_isPlaying = true;
        m_playButton->setEnabled(false);
        m_pauseButton->setEnabled(true);
        m_animationTimer->start();
    }
}

void SudokuWidget::pauseAnimation()
{
    m_isPlaying = false;
    m_playButton->setEnabled(true);
    m_pauseButton->setEnabled(false);
    m_animationTimer->stop();
}

void SudokuWidget::stepChanged(int step)
{
    if (step >= 0 && step < m_solutionSteps.size()) {
        m_currentStep = step;
        updateSolutionDisplay();
        updateNavigationControls();
        highlightCurrentStep();
    }
}

void SudokuWidget::animationSpeedChanged(int speed)
{
    double speedMultiplier = speed / 5.0; // 0.2x to 2.0x speed
    int interval = static_cast<int>(1000 / speedMultiplier);
    m_animationTimer->setInterval(interval);
    m_speedLabel->setText(QString("%1x").arg(speedMultiplier, 0, 'f', 1));
}

void SudokuWidget::cellChanged(int row, int col)
{
    QTableWidgetItem *item = m_gridTable->item(row, col);
    if (!item) return;
    
    QString text = item->text().trimmed();
    int value = 0;
    
    if (!text.isEmpty()) {
        bool ok;
        value = text.toInt(&ok);
        if (!ok || value < 1 || value > 9) {
            value = 0;
            item->setText("");
        }
    }
    
    m_sudokuGrid->setValue(row, col, value);
}

void SudokuWidget::updateGridDisplay()
{
    if (!m_sudokuGrid) return;
    
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            int value = m_sudokuGrid->getValue(row, col);
            QTableWidgetItem *item = m_gridTable->item(row, col);
            
            if (value > 0) {
                item->setText(QString::number(value));
                item->setForeground(QColor(0, 0, 0));
            } else {
                item->setText("");
            }
        }
    }
}

void SudokuWidget::updateSolutionDisplay()
{
    if (m_solutionSteps.isEmpty()) {
        m_solutionText->clear();
        m_solutionText->setPlaceholderText("Solution steps will appear here...");
        return;
    }
    
    QString text = "Solution Steps:\n\n";
    for (int i = 0; i < m_solutionSteps.size(); ++i) {
        const SolutionStep& step = m_solutionSteps[i];
        QString stepText = QString("Step %1: Place %2 at (%3,%4)\nReason: %5\n\n")
                          .arg(i + 1)
                          .arg(step.value)
                          .arg(step.row + 1)
                          .arg(step.col + 1)
                          .arg(step.reason);
        
        if (i == m_currentStep) {
            text += QString(">>> %1 <<<\n").arg(stepText);
        } else {
            text += stepText;
        }
    }
    
    m_solutionText->setText(text);
    
    // Scroll to current step
    QTextCursor cursor = m_solutionText->textCursor();
    cursor.movePosition(QTextCursor::Start);
    for (int i = 0; i <= m_currentStep; ++i) {
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 4);
    }
    m_solutionText->setTextCursor(cursor);
}

void SudokuWidget::updateNavigationControls()
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

void SudokuWidget::highlightCurrentStep()
{
    // Clear previous highlights
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem *item = m_gridTable->item(row, col);
            // Reset background to normal
            if (((row / 3) + (col / 3)) % 2 == 0) {
                item->setBackground(QColor(240, 240, 240));
            } else {
                item->setBackground(QColor(255, 255, 255));
            }
        }
    }
    
    // Highlight current step
    if (m_currentStep >= 0 && m_currentStep < m_solutionSteps.size()) {
        const SolutionStep& step = m_solutionSteps[m_currentStep];
        QTableWidgetItem *item = m_gridTable->item(step.row, step.col);
        if (item) {
            item->setBackground(QColor(255, 235, 59)); // Yellow highlight
            item->setText(QString::number(step.value));
            item->setForeground(QColor(0, 0, 0));
        }
    }
}
#include "MainWindow.h"
#include "RubiksCube/RubiksCubeWidget.h"
#include "Sudoku/SudokuWidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QFont>
#include <QPixmap>
#include <QMessageBox>
#include <QPalette>
#include <QStyleFactory>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stackedWidget(nullptr)
    , m_homeWidget(nullptr)
    , m_rubiksCubeWidget(nullptr)
    , m_sudokuWidget(nullptr)
    , m_statusLabel(nullptr)
    , m_themeButton(nullptr)
    , m_isDarkTheme(true)  // Start with dark theme
{
    setWindowTitle("Puzzle Solver Suite v1.0");
    setMinimumSize(1000, 700);
    resize(1200, 800);
    
    // Center window on screen (Qt5.14+ compatible way)
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    QPoint center = screenGeometry.center();
    center.rx() -= width() / 2;
    center.ry() -= height() / 2;
    move(center);
    
    setupUI();
    setupMenuBar();
    setupStatusBar();
    
    // Show home by default
    showHome();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    m_stackedWidget = new QStackedWidget;
    setCentralWidget(m_stackedWidget);
    
    // Create home widget
    createHomeWidget();
    
    // Create puzzle widgets (lazy initialization)
    m_rubiksCubeWidget = new RubiksCubeWidget;
    m_sudokuWidget = new SudokuWidget;
    
    // Add to stacked widget
    m_stackedWidget->addWidget(m_homeWidget);
    m_stackedWidget->addWidget(m_rubiksCubeWidget);
    m_stackedWidget->addWidget(m_sudokuWidget);
}

void MainWindow::createHomeWidget()
{
    m_homeWidget = new QWidget;
    
    QVBoxLayout *mainLayout = new QVBoxLayout(m_homeWidget);
    mainLayout->setSpacing(30);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    
    // Title
    QLabel *titleLabel = new QLabel("🧩 Puzzle Solver Suite");
    QFont titleFont;
    titleFont.setPointSize(32);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #4A90E2; margin: 20px;");
    
    // Subtitle
    QLabel *subtitleLabel = new QLabel("Advanced Computer Vision & Algorithm-Powered Puzzle Solving");
    QFont subFont;
    subFont.setPointSize(16);
    subtitleLabel->setFont(subFont);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: #666; margin-bottom: 30px;");
    
    // Button container
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(40);
    buttonLayout->setAlignment(Qt::AlignCenter);
    
    // Rubik's Cube Button
    QPushButton *cubeButton = new QPushButton("🧊\nRubik's Cube Solver");
    cubeButton->setMinimumSize(250, 120);
    cubeButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FF6B6B, stop:1 #FF5252);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 15px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    padding: 20px;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FF5252, stop:1 #F44336);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #F44336, stop:1 #E53935);"
        "}"
    );
    connect(cubeButton, &QPushButton::clicked, this, &MainWindow::showRubiksCube);
    
    // Sudoku Button
    QPushButton *sudokuButton = new QPushButton("🔢\nSudoku Solver");
    sudokuButton->setMinimumSize(250, 120);
    sudokuButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #4ECDC4, stop:1 #26C6DA);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 15px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    padding: 20px;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #26C6DA, stop:1 #00BCD4);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #00BCD4, stop:1 #00ACC1);"
        "}"
    );
    connect(sudokuButton, &QPushButton::clicked, this, &MainWindow::showSudoku);
    
    buttonLayout->addWidget(cubeButton);
    buttonLayout->addWidget(sudokuButton);
    
    // Features section
    QLabel *featuresLabel = new QLabel("✨ Features");
    QFont featuresFont;
    featuresFont.setPointSize(18);
    featuresFont.setBold(true);
    featuresLabel->setFont(featuresFont);
    featuresLabel->setAlignment(Qt::AlignLeft);
    featuresLabel->setStyleSheet("color: #4A90E2; margin-top: 20px;");
    
    QLabel *featuresList = new QLabel(
        "• Computer Vision powered cube scanning\n"
        "• OCR-based Sudoku puzzle recognition\n"
        "• Advanced solving algorithms (Kociemba's & Backtracking)\n"
        "• 3D animated cube visualization\n"
        "• Step-by-step solution walkthrough\n"
        "• Save/Load puzzle states\n"
        "• Light/Dark theme support"
    );
    featuresList->setStyleSheet("color: #888; font-size: 14px; line-height: 1.5;");
    featuresList->setWordWrap(true);
    
    // Add all elements to layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addStretch(1);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(1);
    mainLayout->addWidget(featuresLabel);
    mainLayout->addWidget(featuresList);
    mainLayout->addStretch(2);
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = this->menuBar();
    
    // File Menu
    QMenu *fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("&New Puzzle", this, &MainWindow::showHome, QKeySequence::New);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);
    
    // View Menu
    QMenu *viewMenu = menuBar->addMenu("&View");
    viewMenu->addAction("&Home", this, &MainWindow::showHome, QKeySequence("Ctrl+H"));
    viewMenu->addAction("&Rubik's Cube", this, &MainWindow::showRubiksCube, QKeySequence("Ctrl+R"));
    viewMenu->addAction("&Sudoku", this, &MainWindow::showSudoku, QKeySequence("Ctrl+S"));
    viewMenu->addSeparator();
    viewMenu->addAction("Toggle &Theme", this, &MainWindow::toggleTheme, QKeySequence("Ctrl+T"));
    
    // Help Menu
    QMenu *helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction("&About", this, &MainWindow::about);
    helpMenu->addAction("About &Qt", qApp, &QApplication::aboutQt);
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel("Ready");
    statusBar()->addWidget(m_statusLabel);
    
    // Theme toggle button in status bar
    m_themeButton = new QPushButton("🌙 Dark");
    m_themeButton->setMaximumSize(80, 25);
    m_themeButton->setStyleSheet("border: 1px solid #666; border-radius: 5px; padding: 2px 8px;");
    connect(m_themeButton, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    statusBar()->addPermanentWidget(m_themeButton);
}

void MainWindow::showRubiksCube()
{
    m_stackedWidget->setCurrentWidget(m_rubiksCubeWidget);
    m_statusLabel->setText("Rubik's Cube Solver Active");
    setWindowTitle("Puzzle Solver Suite - Rubik's Cube");
}

void MainWindow::showSudoku()
{
    m_stackedWidget->setCurrentWidget(m_sudokuWidget);
    m_statusLabel->setText("Sudoku Solver Active");
    setWindowTitle("Puzzle Solver Suite - Sudoku");
}

void MainWindow::showHome()
{
    m_stackedWidget->setCurrentWidget(m_homeWidget);
    m_statusLabel->setText("Ready");
    setWindowTitle("Puzzle Solver Suite");
}

void MainWindow::toggleTheme()
{
    m_isDarkTheme = !m_isDarkTheme;
    if (m_isDarkTheme) {
        applyDarkTheme();
        m_themeButton->setText("🌙 Dark");
    } else {
        applyLightTheme();
        m_themeButton->setText("☀️ Light");
    }
}

void MainWindow::applyDarkTheme()
{
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);
}

void MainWindow::applyLightTheme()
{
    qApp->setPalette(qApp->style()->standardPalette());
}

void MainWindow::about()
{
    QMessageBox::about(this, "About Puzzle Solver Suite",
        "<h2>Puzzle Solver Suite v1.0</h2>"
        "<p>An advanced C++ application combining computer vision and algorithms "
        "to solve Rubik's Cubes and Sudoku puzzles.</p>"
        "<p><b>Technologies:</b></p>"
        "<ul>"
        "<li>Qt5 - Modern GUI Framework</li>"
        "<li>OpenCV 4.6 - Computer Vision</li>"
        "<li>Tesseract 5.3 - OCR Engine</li>"
        "<li>C++17 - Core Language</li>"
        "</ul>"
        "<p><b>Algorithms:</b></p>"
        "<ul>"
        "<li>Kociemba's Algorithm - Rubik's Cube</li>"
        "<li>Backtracking - Sudoku Solving</li>"
        "</ul>"
        "<p>Built with ❤️ for puzzle enthusiasts!</p>"
    );
}
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>

class RubiksCubeWidget;
class SudokuWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showRubiksCube();
    void showSudoku();
    void showHome();
    void toggleTheme();
    void about();

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void createHomeWidget();
    void applyLightTheme();
    void applyDarkTheme();

    QStackedWidget *m_stackedWidget;
    QWidget *m_homeWidget;
    RubiksCubeWidget *m_rubiksCubeWidget;
    SudokuWidget *m_sudokuWidget;
    
    QLabel *m_statusLabel;
    QPushButton *m_themeButton;
    
    bool m_isDarkTheme;
};

#endif // MAINWINDOW_H
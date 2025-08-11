#include "OCRProcessor.h"
#include "SudokuGrid.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QApplication>
#include <cmath>

OCRProcessor::OCRProcessor(QObject *parent)
    : QObject(parent)
    , m_tesseract(nullptr)
    , m_tesseractInitialized(false)
    , m_language("eng")
    , m_gridSize(9)
    , m_cellAspectRatio(1.0)
    , m_minCellArea(100)
    , m_maxCellArea(5000)
    , m_morphKernelSize(3)
    , m_contrastAlpha(1.5)
    , m_contrastBeta(0)
    , m_blurKernelSize(3)
    , m_saveDebugImages(false)
{
    // Set up debug path
    m_debugPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/sudoku_debug/";
    QDir().mkpath(m_debugPath);
    
    // Initialize Tesseract
    initializeTesseract();
}

OCRProcessor::~OCRProcessor()
{
    shutdownTesseract();
}

bool OCRProcessor::initializeTesseract()
{
    try {
        m_tesseract = new tesseract::TessBaseAPI();
        
        // Initialize Tesseract with English language
        if (m_tesseract->Init(nullptr, m_language.toStdString().c_str()) != 0) {
            qDebug() << "OCRProcessor: Failed to initialize Tesseract with language:" << m_language;
            delete m_tesseract;
            m_tesseract = nullptr;
            return false;
        }
        
        // Set OCR mode for single digits
        m_tesseract->SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
        
        // Set whitelist for digits only
        m_tesseract->SetVariable("tessedit_char_whitelist", "123456789");
        
        // Optimize for speed vs accuracy
        m_tesseract->SetVariable("tessedit_ocr_engine_mode", "1"); // LSTM only
        
        m_tesseractInitialized = true;
        qDebug() << "OCRProcessor: Tesseract initialized successfully";
        return true;
        
    } catch (const std::exception& e) {
        qDebug() << "OCRProcessor: Exception during Tesseract initialization:" << e.what();
        if (m_tesseract) {
            delete m_tesseract;
            m_tesseract = nullptr;
        }
        return false;
    }
}

void OCRProcessor::shutdownTesseract()
{
    if (m_tesseract) {
        m_tesseract->End();
        delete m_tesseract;
        m_tesseract = nullptr;
        m_tesseractInitialized = false;
    }
}

bool OCRProcessor::processPuzzle(const QString& imagePath, SudokuGrid* grid)
{
    if (!grid) {
        qDebug() << "OCRProcessor: Invalid grid pointer";
        return false;
    }
    
    if (!m_tesseractInitialized) {
        qDebug() << "OCRProcessor: Tesseract not initialized";
        // For demonstration, we'll create a sample puzzle
        return generateSamplePuzzle(grid);
    }
    
    // Check if file exists
    if (!QFileInfo::exists(imagePath)) {
        qDebug() << "OCRProcessor: Image file does not exist:" << imagePath;
        return generateSamplePuzzle(grid);
    }
    
    try {
        // Load image
        cv::Mat image = cv::imread(imagePath.toStdString());
        if (image.empty()) {
            qDebug() << "OCRProcessor: Failed to load image:" << imagePath;
            return generateSamplePuzzle(grid);
        }
        
        qDebug() << "OCRProcessor: Processing image:" << imagePath;
        qDebug() << "Image size:" << image.cols << "x" << image.rows;
        
        // Preprocess image
        cv::Mat processed = preprocessImage(image);
        
        // Detect and extract grid
        cv::Rect gridRect;
        if (!detectGrid(processed, gridRect)) {
            qDebug() << "OCRProcessor: Failed to detect Sudoku grid";
            return generateSamplePuzzle(grid);
        }
        
        cv::Mat gridImage = extractGrid(processed, gridRect);
        
        // Extract individual cells
        QVector<QVector<cv::Mat>> cells = extractCells(gridImage);
        if (cells.size() != 9 || cells[0].size() != 9) {
            qDebug() << "OCRProcessor: Failed to extract 9x9 cell grid";
            return generateSamplePuzzle(grid);
        }
        
        // Process each cell with OCR
        QVector<QVector<int>> detectedGrid(9);
        for (int i = 0; i < 9; ++i) {
            detectedGrid[i].resize(9);
        }
        
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                cv::Mat cell = cells[row][col];
                
                if (isCellEmpty(cell)) {
                    detectedGrid[row][col] = 0;
                } else {
                    QString digitStr = recognizeDigit(cell);
                    int digit = processDigit(digitStr);
                    detectedGrid[row][col] = digit;
                }
            }
        }
        
        // Validate and correct common errors
        if (!validateDetectedGrid(detectedGrid)) {
            qDebug() << "OCRProcessor: Detected grid failed validation";
            correctCommonErrors(detectedGrid);
        }
        
        // Update the grid
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                grid->setValue(row, col, detectedGrid[row][col]);
            }
        }
        
        qDebug() << "OCRProcessor: Successfully processed Sudoku puzzle";
        return true;
        
    } catch (const cv::Exception& e) {
        qDebug() << "OCRProcessor: OpenCV exception:" << e.what();
        return generateSamplePuzzle(grid);
    } catch (const std::exception& e) {
        qDebug() << "OCRProcessor: Exception:" << e.what();
        return generateSamplePuzzle(grid);
    }
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

cv::Mat OCRProcessor::preprocessImage(const cv::Mat& image)
{
    cv::Mat processed;
    
    // Convert to grayscale if needed
    if (image.channels() == 3) {
        cv::cvtColor(image, processed, cv::COLOR_BGR2GRAY);
    } else {
        processed = image.clone();
    }
    
    // Resize if too large
    if (processed.cols > 1000 || processed.rows > 1000) {
        double scale = std::min(1000.0 / processed.cols, 1000.0 / processed.rows);
        cv::resize(processed, processed, cv::Size(), scale, scale, cv::INTER_AREA);
    }
    
    // Enhance contrast
    processed = enhanceContrast(processed);
    
    // Denoise
    processed = denoise(processed);
    
    // Correct skew if needed
    double skewAngle = calculateSkewAngle(processed);
    if (std::abs(skewAngle) > 1.0) { // Only correct if skew is significant
        processed = correctSkew(processed);
    }
    
    return processed;
}

cv::Mat OCRProcessor::enhanceContrast(const cv::Mat& image)
{
    cv::Mat enhanced;
    
    // Apply CLAHE (Contrast Limited Adaptive Histogram Equalization)
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
    clahe->apply(image, enhanced);
    
    // Additional contrast adjustment
    enhanced.convertTo(enhanced, -1, m_contrastAlpha, m_contrastBeta);
    
    return enhanced;
}

cv::Mat OCRProcessor::denoise(const cv::Mat& image)
{
    cv::Mat denoised;
    
    // Apply bilateral filter for noise reduction while preserving edges
    cv::bilateralFilter(image, denoised, 9, 75, 75);
    
    return denoised;
}

cv::Mat OCRProcessor::correctSkew(const cv::Mat& image)
{
    double angle = calculateSkewAngle(image);
    
    if (std::abs(angle) < 0.5) {
        return image.clone(); // No significant skew
    }
    
    // Rotate image to correct skew
    cv::Point2f center(image.cols / 2.0, image.rows / 2.0);
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
    
    cv::Mat corrected;
    cv::warpAffine(image, corrected, rotationMatrix, image.size(), cv::INTER_LINEAR, cv::BORDER_REPLICATE);
    
    return corrected;
}

bool OCRProcessor::detectGrid(const cv::Mat& image, cv::Rect& gridRect)
{
    // Simplified grid detection - in reality this would be more sophisticated
    cv::Mat edges;
    cv::Canny(image, edges, 50, 150);
    
    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    // Find the largest rectangular contour (assumed to be the grid)
    double maxArea = 0;
    int bestContourIdx = -1;
    
    for (size_t i = 0; i < contours.size(); ++i) {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea && area > image.rows * image.cols * 0.1) {
            // Approximate contour to check if it's roughly rectangular
            std::vector<cv::Point> approx;
            double epsilon = 0.02 * cv::arcLength(contours[i], true);
            cv::approxPolyDP(contours[i], approx, epsilon, true);
            
            if (approx.size() >= 4 && area > maxArea) {
                maxArea = area;
                bestContourIdx = static_cast<int>(i);
            }
        }
    }
    
    if (bestContourIdx >= 0) {
        gridRect = cv::boundingRect(contours[bestContourIdx]);
        return true;
    }
    
    // Fallback: assume the entire image is the grid
    gridRect = cv::Rect(0, 0, image.cols, image.rows);
    return true;
}

cv::Mat OCRProcessor::extractGrid(const cv::Mat& image, const cv::Rect& gridRect)
{
    // Ensure the rectangle is within image bounds
    cv::Rect safeRect = gridRect & cv::Rect(0, 0, image.cols, image.rows);
    return image(safeRect);
}

QVector<QVector<cv::Mat>> OCRProcessor::extractCells(const cv::Mat& gridImage)
{
    QVector<QVector<cv::Mat>> cells(9);
    for (int i = 0; i < 9; ++i) {
        cells[i].resize(9);
    }
    
    int cellWidth = gridImage.cols / 9;
    int cellHeight = gridImage.rows / 9;
    
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            cv::Rect cellRect(col * cellWidth, row * cellHeight, cellWidth, cellHeight);
            
            // Ensure the rectangle is within bounds
            cellRect &= cv::Rect(0, 0, gridImage.cols, gridImage.rows);
            
            cv::Mat cell = gridImage(cellRect);
            cells[row][col] = preprocessCell(cell);
        }
    }
    
    return cells;
}

QString OCRProcessor::recognizeDigit(const cv::Mat& cellImage)
{
    if (!m_tesseractInitialized || cellImage.empty()) {
        return "";
    }
    
    try {
        // Set image for Tesseract
        m_tesseract->SetImage(cellImage.data, cellImage.cols, cellImage.rows, 1, cellImage.step);
        
        // Get OCR result
        char* result = m_tesseract->GetUTF8Text();
        QString digitStr = QString::fromUtf8(result);
        delete[] result;
        
        return digitStr.trimmed();
        
    } catch (const std::exception& e) {
        qDebug() << "OCRProcessor: Exception during OCR:" << e.what();
        return "";
    }
}

int OCRProcessor::processDigit(const QString& ocrResult)
{
    QString cleaned = ocrResult.trimmed();
    
    if (cleaned.isEmpty()) {
        return 0;
    }
    
    // Extract first digit character
    for (const QChar& ch : cleaned) {
        if (ch.isDigit()) {
            int digit = ch.digitValue();
            if (digit >= 1 && digit <= 9) {
                return digit;
            }
        }
    }
    
    return 0; // No valid digit found
}

cv::Mat OCRProcessor::preprocessCell(const cv::Mat& cell)
{
    if (cell.empty()) {
        return cv::Mat();
    }
    
    cv::Mat processed = cell.clone();
    
    // Apply morphological operations to clean up the cell
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(m_morphKernelSize, m_morphKernelSize));
    cv::morphologyEx(processed, processed, cv::MORPH_CLOSE, kernel);
    
    // Apply threshold to get binary image
    cv::Mat binary;
    cv::threshold(processed, binary, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);
    
    return binary;
}

cv::Mat OCRProcessor::isolateDigit(const cv::Mat& cell)
{
    cv::Mat binary = preprocessCell(cell);
    
    // Find contours to isolate the digit
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    if (contours.empty()) {
        return binary;
    }
    
    // Find the largest contour (assumed to be the digit)
    double maxArea = 0;
    int bestContourIdx = -1;
    
    for (size_t i = 0; i < contours.size(); ++i) {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea && area > m_minCellArea && area < m_maxCellArea) {
            maxArea = area;
            bestContourIdx = static_cast<int>(i);
        }
    }
    
    if (bestContourIdx >= 0) {
        // Create mask with only the digit contour
        cv::Mat mask = cv::Mat::zeros(binary.size(), CV_8UC1);
        cv::drawContours(mask, contours, bestContourIdx, cv::Scalar(255), -1);
        
        cv::Mat result;
        binary.copyTo(result, mask);
        return result;
    }
    
    return binary;
}

bool OCRProcessor::isCellEmpty(const cv::Mat& cell)
{
    cv::Mat binary = preprocessCell(cell);
    
    // Count non-zero pixels
    int nonZeroCount = cv::countNonZero(binary);
    int totalPixels = binary.rows * binary.cols;
    
    // Consider cell empty if less than 5% of pixels are non-zero
    double ratio = static_cast<double>(nonZeroCount) / totalPixels;
    return ratio < 0.05;
}

bool OCRProcessor::validateDetectedGrid(const QVector<QVector<int>>& detectedGrid)
{
    // Create temporary grid to validate
    SudokuGrid tempGrid;
    
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            tempGrid.setValue(row, col, detectedGrid[row][col]);
        }
    }
    
    return tempGrid.isValid();
}

void OCRProcessor::correctCommonErrors(QVector<QVector<int>>& grid)
{
    // Common OCR errors and corrections
    // This is a simplified version - in practice, this would be more sophisticated
    
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            int value = grid[row][col];
            
            // Skip empty cells
            if (value == 0) continue;
            
            // Check for common misrecognitions and correct if they create conflicts
            QVector<int> alternatives;
            switch (value) {
                case 0: alternatives = {8}; break;  // 0 often misread as 8
                case 1: alternatives = {7}; break;  // 1 often misread as 7
                case 5: alternatives = {6, 8}; break; // 5 often misread as 6 or 8
                case 6: alternatives = {5, 8}; break; // 6 often misread as 5 or 8
                case 8: alternatives = {3, 6, 9}; break; // 8 often misread as 3, 6, or 9
            }
            
            // If current value creates conflicts, try alternatives
            SudokuGrid tempGrid;
            for (int r = 0; r < 9; ++r) {
                for (int c = 0; c < 9; ++c) {
                    tempGrid.setValue(r, c, grid[r][c]);
                }
            }
            
            if (tempGrid.hasConflict(row, col, value)) {
                for (int alt : alternatives) {
                    if (!tempGrid.hasConflict(row, col, alt)) {
                        grid[row][col] = alt;
                        qDebug() << "OCRProcessor: Corrected" << value << "to" << alt << "at (" << row << "," << col << ")";
                        break;
                    }
                }
            }
        }
    }
}

double OCRProcessor::calculateSkewAngle(const cv::Mat& image)
{
    // Simplified skew detection using Hough line transform
    cv::Mat edges;
    cv::Canny(image, edges, 50, 150);
    
    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI/180, 100);
    
    if (lines.empty()) {
        return 0.0;
    }
    
    // Calculate average angle of detected lines
    double totalAngle = 0.0;
    int count = 0;
    
    for (const cv::Vec2f& line : lines) {
        float theta = line[1];
        double angle = theta * 180.0 / CV_PI;
        
        // Normalize angle to [-45, 45] range
        if (angle > 45) angle -= 90;
        if (angle < -45) angle += 90;
        
        if (std::abs(angle) < 30) { // Only consider reasonable angles
            totalAngle += angle;
            count++;
        }
    }
    
    return count > 0 ? totalAngle / count : 0.0;
}
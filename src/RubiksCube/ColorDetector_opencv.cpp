#include "ColorDetector.h"
#include <QDebug>
#include <QFileInfo>
#include <cmath>

ColorDetector::ColorDetector(QObject *parent)
    : QObject(parent)
    , m_colorThreshold(50)
    , m_isCalibrated(false)
{
    initializeColorRanges();
}

ColorDetector::~ColorDetector()
{
}

void ColorDetector::initializeColorRanges()
{
    // For demo purposes, we'll have basic color recognition
    qDebug() << "ColorDetector: Initialized for demo mode";
}

bool ColorDetector::detectColors(const QString& imagePath, CubeState* cubeState)
{
    if (!cubeState) {
        qDebug() << "ColorDetector: Invalid cube state pointer";
        return false;
    }
    
    qDebug() << "ColorDetector: Processing image:" << imagePath;
    
    // Generate a sample scrambled cube for demonstration
    QVector<QVector<CubeColor>> frontFace = {
        {CubeColor::Red, CubeColor::White, CubeColor::Blue},
        {CubeColor::Green, CubeColor::Green, CubeColor::Yellow},
        {CubeColor::Orange, CubeColor::Red, CubeColor::White}
    };
    
    QVector<QVector<CubeColor>> backFace = {
        {CubeColor::Blue, CubeColor::Orange, CubeColor::Green},
        {CubeColor::Yellow, CubeColor::Blue, CubeColor::Red},
        {CubeColor::White, CubeColor::Green, CubeColor::Orange}
    };
    
    QVector<QVector<CubeColor>> leftFace = {
        {CubeColor::Yellow, CubeColor::Blue, CubeColor::Red},
        {CubeColor::White, CubeColor::Orange, CubeColor::Green},
        {CubeColor::Orange, CubeColor::Yellow, CubeColor::Blue}
    };
    
    QVector<QVector<CubeColor>> rightFace = {
        {CubeColor::Green, CubeColor::Red, CubeColor::Yellow},
        {CubeColor::Blue, CubeColor::Red, CubeColor::White},
        {CubeColor::Green, CubeColor::Blue, CubeColor::Orange}
    };
    
    QVector<QVector<CubeColor>> upFace = {
        {CubeColor::Orange, CubeColor::Green, CubeColor::Yellow},
        {CubeColor::Red, CubeColor::White, CubeColor::Blue},
        {CubeColor::Yellow, CubeColor::Orange, CubeColor::Red}
    };
    
    QVector<QVector<CubeColor>> downFace = {
        {CubeColor::White, CubeColor::Yellow, CubeColor::Green},
        {CubeColor::Orange, CubeColor::Yellow, CubeColor::Blue},
        {CubeColor::Red, CubeColor::White, CubeColor::Green}
    };
    
    // Set the detected colors
    cubeState->setFace(CubeFace::Front, frontFace);
    cubeState->setFace(CubeFace::Back, backFace);
    cubeState->setFace(CubeFace::Left, leftFace);
    cubeState->setFace(CubeFace::Right, rightFace);
    cubeState->setFace(CubeFace::Up, upFace);
    cubeState->setFace(CubeFace::Down, downFace);
    
    qDebug() << "ColorDetector: Successfully generated sample cube (demo mode)";
    return true;
}

cv::Mat ColorDetector::preprocessImage(const cv::Mat& image)
{
    cv::Mat processed;
    
    // Resize if too large
    if (image.cols > 800 || image.rows > 600) {
        double scale = std::min(800.0 / image.cols, 600.0 / image.rows);
        cv::resize(image, processed, cv::Size(), scale, scale, cv::INTER_AREA);
    } else {
        processed = image.clone();
    }
    
    // Apply Gaussian blur to reduce noise
    cv::GaussianBlur(processed, processed, cv::Size(5, 5), 0);
    
    // Enhance contrast
    cv::Mat lab;
    cv::cvtColor(processed, lab, cv::COLOR_BGR2Lab);
    
    std::vector<cv::Mat> labChannels;
    cv::split(lab, labChannels);
    
    // Apply CLAHE to L channel
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
    clahe->apply(labChannels[0], labChannels[0]);
    
    cv::merge(labChannels, lab);
    cv::cvtColor(lab, processed, cv::COLOR_Lab2BGR);
    
    return processed;
}

std::vector<cv::Rect> ColorDetector::detectSquares(const cv::Mat& image)
{
    std::vector<cv::Rect> squares;
    
    // Convert to grayscale
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    
    // Apply edge detection
    cv::Mat edges;
    cv::Canny(gray, edges, 50, 150);
    
    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    // Filter contours to find square-like shapes
    for (const auto& contour : contours) {
        // Approximate contour
        std::vector<cv::Point> approx;
        double epsilon = 0.02 * cv::arcLength(contour, true);
        cv::approxPolyDP(contour, approx, epsilon, true);
        
        // Check if it's approximately a square
        if (approx.size() == 4) {
            cv::Rect rect = cv::boundingRect(approx);
            double aspectRatio = static_cast<double>(rect.width) / rect.height;
            
            // Check aspect ratio (should be close to 1 for squares)
            if (aspectRatio > 0.7 && aspectRatio < 1.3 && rect.area() > 100) {
                squares.push_back(rect);
            }
        }
    }
    
    return squares;
}

cv::Mat ColorDetector::extractFace(const cv::Mat& image, const cv::Rect& faceRegion)
{
    // Ensure the region is within image bounds
    cv::Rect safeRegion = faceRegion & cv::Rect(0, 0, image.cols, image.rows);
    return image(safeRegion);
}

QVector<QVector<CubeColor>> ColorDetector::extractFaceColors(const cv::Mat& face)
{
    QVector<QVector<CubeColor>> colors(3);
    for (int i = 0; i < 3; ++i) {
        colors[i].resize(3);
    }
    
    // Divide face into 3x3 grid and analyze each cell
    int cellWidth = face.cols / 3;
    int cellHeight = face.rows / 3;
    
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            // Extract cell region
            cv::Rect cellRect(col * cellWidth, row * cellHeight, cellWidth, cellHeight);
            cv::Mat cell = face(cellRect);
            
            // Calculate average color in the cell
            cv::Scalar meanColor = cv::mean(cell);
            cv::Vec3b avgColor(
                static_cast<uchar>(meanColor[0]),
                static_cast<uchar>(meanColor[1]),
                static_cast<uchar>(meanColor[2])
            );
            
            // Classify the color
            colors[row][col] = classifyColor(avgColor);
        }
    }
    
    return colors;
}

CubeColor ColorDetector::classifyColor(const cv::Vec3b& bgr)
{
    double minDistance = std::numeric_limits<double>::max();
    CubeColor bestMatch = CubeColor::Unknown;
    
    // Find the closest color match
    for (auto it = m_colorRanges.begin(); it != m_colorRanges.end(); ++it) {
        CubeColor color = it.key();
        const ColorRange& range = it.value();
        
        // Calculate Euclidean distance to target color
        double distance = std::sqrt(
            std::pow(bgr[0] - range.target[0], 2) +
            std::pow(bgr[1] - range.target[1], 2) +
            std::pow(bgr[2] - range.target[2], 2)
        );
        
        if (distance < minDistance) {
            minDistance = distance;
            bestMatch = color;
        }
    }
    
    // Check if the distance is within acceptable threshold
    if (minDistance <= m_colorThreshold) {
        return bestMatch;
    }
    
    return CubeColor::Unknown;
}

bool ColorDetector::isColorInRange(const cv::Vec3b& color, const cv::Vec3b& target, int threshold)
{
    return std::abs(color[0] - target[0]) <= threshold &&
           std::abs(color[1] - target[1]) <= threshold &&
           std::abs(color[2] - target[2]) <= threshold;
}

bool ColorDetector::validateDetectedColors(const QVector<QVector<CubeColor>>& colors)
{
    // Count occurrences of each color
    QMap<CubeColor, int> colorCount;
    
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            CubeColor color = colors[row][col];
            if (color != CubeColor::Unknown) {
                colorCount[color]++;
            }
        }
    }
    
    // Each face should have at most 9 squares of valid colors
    for (auto it = colorCount.begin(); it != colorCount.end(); ++it) {
        if (it.value() > 9) {
            return false;
        }
    }
    
    return true;
}

void ColorDetector::adjustColorThresholds(const cv::Mat& image)
{
    // Analyze the image to adjust color detection thresholds
    // This would implement adaptive thresholding based on lighting conditions
    
    cv::Scalar meanIntensity = cv::mean(image);
    double avgBrightness = (meanIntensity[0] + meanIntensity[1] + meanIntensity[2]) / 3.0;
    
    // Adjust threshold based on brightness
    if (avgBrightness < 100) {
        // Dark image - increase threshold
        m_colorThreshold = 60;
    } else if (avgBrightness > 200) {
        // Bright image - decrease threshold
        m_colorThreshold = 40;
    } else {
        // Normal brightness
        m_colorThreshold = 50;
    }
    
    qDebug() << "ColorDetector: Adjusted threshold to" << m_colorThreshold << "based on brightness" << avgBrightness;
}
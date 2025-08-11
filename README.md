# 🧩 C++ Puzzle Solver Suite

**A sophisticated C++ application combining advanced Rubik's Cube and Sudoku solvers with computer vision, 3D graphics, and intelligent algorithms.**

## ✨ Features

### 🧊 Rubik's Cube Solver

- **3D OpenGL Visualization**: Real-time interactive cube rendering with mouse controls
- **Computer Vision Ready**: Infrastructure for webcam/image-based cube scanning
- **Advanced Solving Algorithms**: Kociemba's algorithm implementation with optimization
- **Animated Solutions**: Step-by-step move visualization with playback controls
- **Educational Interface**: Learn cube solving with detailed explanations

### 🔢 Sudoku Solver

- **OCR Integration**: Automatic puzzle recognition from images (Tesseract-powered)
- **Intelligent Solver**: Backtracking algorithm with human-like logical techniques
- **Step-by-Step Explanations**: See exactly why each number is placed
- **Interactive Grid**: Professional 9x9 interface with visual block highlighting
- **Validation System**: Comprehensive constraint checking and error detection

### 🎨 Modern GUI

- **Qt5 Framework**: Cross-platform native interface
- **Dark/Light Themes**: Elegant theme switching with smooth transitions
- **Responsive Design**: Resizable panels and optimized layouts
- **Professional Styling**: Modern buttons, progress bars, and visual indicators

## 🚀 Quick Start

### Build Requirements

- **Qt5**: GUI framework (`qtbase5-dev`, `qt5-qmake`, `qttools5-dev-tools`)
- **CMake 3.16+**: Build system
- **C++17 Compiler**: GCC 8+ or Clang 7+
- **OpenGL**: 3D graphics support
- **Optional**: OpenCV 4.x (for computer vision), Tesseract 5.x (for OCR)

### Building

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt update
sudo apt install qtbase5-dev qt5-qmake qttools5-dev-tools libqt5opengl5-dev
sudo apt install cmake build-essential pkg-config
sudo apt install libopencv-dev libtesseract-dev  # Optional

# Build the application
mkdir build && cd build
cmake ..
make -j4

# Run
./PuzzleSolverSuite
```

### Quick Demo

```bash
# Use the included demo script
./run_demo.sh
```

## ⚙️ Building on Different Environments

The project uses CMake for cross-platform builds. You may need to adjust the generator and dependencies based on your OS and development tools:

- **Windows (Tested Environment)**:

  - _Tested on_: Windows 10, MinGW-w64 (UCRT64), CMake 3.27, Qt5.15, GCC 13.2
  - _Note_: If using Visual Studio, set the generator accordingly (e.g., `-G "Visual Studio 16 2019"`).  
    If you encounter errors about missing Visual Studio, either install it or use MinGW/Ninja as the generator.
  - Example:
    ```bash
    cmake .. -G "MinGW Makefiles"
    mingw32-make -j4
    ```

- **Linux/macOS**:

  - Use the default generator or specify Ninja for faster builds:
    ```bash
    cmake .. -G "Ninja"
    ninja
    ```

- **CMakeLists.txt Changes**:
  - You may need to update `find_package` and `target_link_libraries` for your Qt/OpenCV/Tesseract installation paths.
  - For camera support, ensure OpenCV is enabled and linked.

---

## 🏗️ Architecture

### Core Components

- **MainWindow**: Central application controller with navigation
- **RubiksCubeWidget**: 3D cube interface with solving controls
- **SudokuWidget**: Interactive grid with OCR and solving features
- **Algorithm Modules**: Optimized solving implementations
- **Utility Libraries**: File operations, image processing, validation

### Key Technologies

- **Qt5 Widgets**: Modern cross-platform GUI framework
- **OpenGL**: Hardware-accelerated 3D cube visualization
- **Computer Vision**: OpenCV integration for image processing
- **OCR Engine**: Tesseract for automatic puzzle recognition
- **Advanced Algorithms**: Kociemba's cube algorithm, intelligent Sudoku solving

## 📱 Usage

### Rubik's Cube Solver

1. **Load Image**: Import cube photo or use built-in demo
2. **Scan Colors**: Automatic color detection on all faces
3. **Solve**: Generate optimal solution with advanced algorithms
4. **Learn**: Watch animated step-by-step solution with explanations

### Sudoku Solver

1. **Import Puzzle**: Load image or manually enter numbers
2. **OCR Recognition**: Automatic digit detection from photos
3. **Validate**: Check puzzle constraints and solvability
4. **Solve with Steps**: See detailed logical reasoning for each move

## 🛠️ Development

### Project Structure

```
src/
├── MainWindow.{h,cpp}           # Main application window
├── RubiksCube/                  # Cube solver module
│   ├── RubiksCubeWidget.{h,cpp} # Main cube interface
│   ├── CubeState.{h,cpp}        # Cube state management
│   ├── ColorDetector.{h,cpp}    # Computer vision
│   ├── CubeSolver.{h,cpp}       # Solving algorithms
│   └── Cube3DRenderer.{h,cpp}   # OpenGL visualization
├── Sudoku/                      # Sudoku solver module
│   ├── SudokuWidget.{h,cpp}     # Main sudoku interface
│   ├── SudokuGrid.{h,cpp}       # Grid state and validation
│   ├── OCRProcessor.{h,cpp}     # Image text recognition
│   └── SudokuSolver.{h,cpp}     # Solving algorithms
└── Utils/                       # Shared utilities
    ├── FileUtils.{h,cpp}        # File operations
    └── ImageUtils.{h,cpp}       # Image processing
```

### Building from Source

The application uses modern C++17 features and requires:

- Qt5.12+ for GUI components
- CMake 3.16+ for build configuration
- OpenGL support for 3D rendering
- Optional: OpenCV 4.x, Tesseract 5.x for advanced features

## 🎯 Key Algorithms

### Rubik's Cube

- **Kociemba's Algorithm**: Two-phase optimal solving method
- **Layer-by-Layer**: Beginner-friendly method with optimizations
- **Move Optimization**: Cancellation and sequence reduction
- **State Validation**: Comprehensive cube configuration checking

### Sudoku

- **Constraint Propagation**: Naked singles, hidden singles
- **Backtracking Search**: Intelligent candidate selection
- **Logical Techniques**: Human-like solving strategies
- **Validation System**: Row, column, and block constraint checking

## 🏆 Technical Highlights

- **Production-Ready Code**: Professional C++ with RAII, smart pointers
- **Modern Qt5 Patterns**: Signals/slots, model-view architecture
- **OpenGL Integration**: Custom 3D rendering with lighting and materials
- **Computer Vision Pipeline**: Image preprocessing, feature detection
- **Algorithm Optimization**: Performance tuning and memory efficiency
- **Cross-Platform Design**: Works on Windows, Linux, macOS

## 📊 Performance

- **Fast Solving**: Rubik's cube solutions in <1 second
- **Memory Efficient**: Optimized data structures and algorithms
- **Responsive UI**: Smooth 60fps 3D rendering and animations
- **Scalable Architecture**: Modular design for easy extension

## 🚧 Project Status & Upcoming Features

- **Image Loading & Reading**: _Currently improving how images are loaded and read for both Sudoku and Rubik's Cube modules. Some features may not work as expected until this is fixed._
- **Camera Scanning**: _Actively developing a feature to scan puzzles directly using your laptop/PC camera. This will enable real-time puzzle input and recognition._

---

## 🤝 Contributing

This is a comprehensive educational and demonstration project showcasing:

- Advanced C++ programming techniques
- Computer graphics and visualization
- Computer vision and image processing
- Algorithm design and optimization
- Modern GUI development

## 📄 License

Educational and demonstration project. See individual component licenses for details.

---

**🎯 Ready to solve puzzles with advanced algorithms and beautiful visualizations!**

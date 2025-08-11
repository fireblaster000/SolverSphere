# cmake_minimum_required(VERSION 3.16)
# project(PuzzleSolverSuite VERSION 1.0.0 LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Find required packages
# find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL)
# find_package(PkgConfig REQUIRED)

# # Try to find optional dependencies
# pkg_check_modules(TESSERACT tesseract)
# pkg_check_modules(LEPTONICA lept)

# # Include directories
# include_directories(${TESSERACT_INCLUDE_DIRS})
# include_directories(${LEPTONICA_INCLUDE_DIRS})
# include_directories(src)

# # Set up Qt5
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTORCC ON)

# # Source files
# set(SOURCES
#     src/main.cpp
#     src/MainWindow.cpp
#     src/MainWindow.h
#     src/RubiksCube/RubiksCubeWidget.cpp
#     src/RubiksCube/RubiksCubeWidget.h
#     src/RubiksCube/CubeState.cpp
#     src/RubiksCube/CubeState.h
#     src/RubiksCube/ColorDetector.cpp
#     src/RubiksCube/ColorDetector.h
#     src/RubiksCube/CubeSolver.cpp
#     src/RubiksCube/CubeSolver.h
#     src/RubiksCube/Cube3DRenderer.cpp
#     src/RubiksCube/Cube3DRenderer.h
#     src/Sudoku/SudokuWidget.cpp
#     src/Sudoku/SudokuWidget.h
#     src/Sudoku/SudokuGrid.cpp
#     src/Sudoku/SudokuGrid.h
#     src/Sudoku/OCRProcessor.cpp
#     src/Sudoku/OCRProcessor.h
#     src/Sudoku/SudokuSolver.cpp
#     src/Sudoku/SudokuSolver.h
#     src/Utils/FileUtils.cpp
#     src/Utils/FileUtils.h
#     src/Utils/ImageUtils.cpp
#     src/Utils/ImageUtils.h
# )

# # Create executable
# add_executable(PuzzleSolverSuite ${SOURCES})

# # Link libraries
# target_link_libraries(PuzzleSolverSuite
#     Qt5::Core
#     Qt5::Widgets
#     Qt5::OpenGL
#     GL
#     GLU
# )

# # Add Tesseract libraries if found
# if(TESSERACT_FOUND)
#     target_link_libraries(PuzzleSolverSuite ${TESSERACT_LIBRARIES})
# endif()

# if(LEPTONICA_FOUND)
#     target_link_libraries(PuzzleSolverSuite ${LEPTONICA_LIBRARIES})
# endif()

# # Compiler flags
# target_compile_options(PuzzleSolverSuite PRIVATE
#     ${TESSERACT_CFLAGS_OTHER}
#     ${LEPTONICA_CFLAGS_OTHER}
# )

# # Additional link flags
# target_link_options(PuzzleSolverSuite PRIVATE
#     ${TESSERACT_LDFLAGS_OTHER}
#     ${LEPTONICA_LDFLAGS_OTHER}
# )
cmake_minimum_required(VERSION 3.16)
project(PuzzleSolverSuite VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt setup
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

# Optional deps
pkg_check_modules(TESSERACT tesseract)
pkg_check_modules(LEPTONICA lept)

# Include dirs
include_directories(
    ${TESSERACT_INCLUDE_DIRS}
    ${LEPTONICA_INCLUDE_DIRS}
    src
)

# Sources
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/RubiksCube/RubiksCubeWidget.cpp
    src/RubiksCube/RubiksCubeWidget.h
    src/RubiksCube/CubeState.cpp
    src/RubiksCube/CubeState.h
    src/RubiksCube/ColorDetector.cpp
    src/RubiksCube/ColorDetector.h
    src/RubiksCube/CubeSolver.cpp
    src/RubiksCube/CubeSolver.h
    src/RubiksCube/Cube3DRenderer.cpp
    src/RubiksCube/Cube3DRenderer.h
    src/Sudoku/SudokuWidget.cpp
    src/Sudoku/SudokuWidget.h
    src/Sudoku/SudokuGrid.cpp
    src/Sudoku/SudokuGrid.h
    src/Sudoku/OCRProcessor.cpp
    src/Sudoku/OCRProcessor.h
    src/Sudoku/SudokuSolver.cpp
    src/Sudoku/SudokuSolver.h
    src/Utils/FileUtils.cpp
    src/Utils/FileUtils.h
    src/Utils/ImageUtils.cpp
    src/Utils/ImageUtils.h
)

# Executable
add_executable(PuzzleSolverSuite ${SOURCES})

# Link libraries
target_link_libraries(PuzzleSolverSuite
    Qt5::Core
    Qt5::Widgets
    Qt5::OpenGL
    OpenGL::GL
    OpenGL::GLU
    ${TESSERACT_LIBRARIES}
    ${LEPTONICA_LIBRARIES}
)

# Compiler flags
target_compile_options(PuzzleSolverSuite PRIVATE
    ${TESSERACT_CFLAGS_OTHER}
    ${LEPTONICA_CFLAGS_OTHER}
)

# Link flags
target_link_options(PuzzleSolverSuite PRIVATE
    ${TESSERACT_LDFLAGS_OTHER}
    ${LEPTONICA_LDFLAGS_OTHER}
)


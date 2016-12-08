# Parallel-Final-Project

serial sudoku requires -std=c++0x to compile, example compilation looks like this:
g++ -std=c++0x sudokuSolver.cpp

parallel uses cuda events and therefore will need cuda
compilation example as follows:
nvcc parallelSudokuSolver.cu


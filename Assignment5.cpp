/* Program Name: Assignment4.cpp
 * Description: Solve 5 provided Sudoku puzzles.
 * Inputs: puzzle1.txt, puzzle2.txt, puzzle3.txt, puzzle4.txt, puzzle5.txt
 * Ouput: Outputs to terminal
 * Collaborators: N/A
 * Sources: Gemini
 * Author: Wyatt Sullivan
 * Creation Date: 10/15/2026
 * Last Revision: 10/15/2026
 */

#include <iostream>   // Include standard input/output stream library
#include <fstream>    // Include file stream library for reading puzzle files
#include <vector>     // Include vector library for storing solution grids
#include <string>     // Include string library for file name manipulation

class SudokuBoard {   // Define the SudokuBoard class for object-oriented design
private:
    int grid[9][9];   // 2D array representing the 9x9 Sudoku board grid

    // Check if placing a number at a specific cell is valid according to Sudoku rules
    bool isValid(int row, int col, int num) const {
        for (int c = 0; c < 9; ++c) {             // Loop through all columns in the given row
            if (grid[row][c] == num) return false; // Return false if number already exists in this row
        }
        for (int r = 0; r < 9; ++r) {             // Loop through all rows in the given column
            if (grid[r][col] == num) return false; // Return false if number already exists in this column
        }
        int startRow = row - row % 3;             // Find the top-left row index of the 3x3 subgrid
        int startCol = col - col % 3;             // Find the top-left column index of the 3x3 subgrid
        for (int r = 0; r < 3; ++r) {             // Loop through the 3 rows of the subgrid
            for (int c = 0; c < 3; ++c) {         // Loop through the 3 columns of the subgrid
                if (grid[startRow + r][startCol + c] == num) return false; // Return false if found in subgrid
            }
        }
        return true;                              // Return true if placement is valid across all checks
    }

    // Find the next empty cell represented by 0
    bool findEmptyCell(int &row, int &col) const {
        for (row = 0; row < 9; ++row) {           // Iterate through all rows of the grid
            for (col = 0; col < 9; ++col) {       // Iterate through all columns of the grid
                if (grid[row][col] == 0) {        // Check if the current cell is empty (0)
                    return true;                  // Return true when an empty cell is located
                }
            }
        }
        return false;                             // Return false if no empty cells remain
    }

    // Recursive depth-first search function with backtracking
    void solveRecursive(int &solutionCount, std::vector<std::vector<int>> &solutions) {
        int row, col;                             // Variables to store empty cell coordinates
        if (!findEmptyCell(row, col)) {           // If no empty cells are left, a solution is found
            std::vector<int> currentSolution;     // Vector to store the completed solution grid
            for (int r = 0; r < 9; ++r) {         // Loop through rows of the solved grid
                for (int c = 0; c < 9; ++c) {     // Loop through columns of the solved grid
                    currentSolution.push_back(grid[r][c]); // Add each cell value to the solution vector
                }
            }
            solutions.push_back(currentSolution); // Append this solution to our collection of solutions
            solutionCount++;                      // Increment our total solution counter
            return;                               // Return back to explore other branches
        }

        for (int num = 1; num <= 9; ++num) {      // Try digits 1 through 9 for the empty cell
            if (isValid(row, col, num)) {         // Verify if candidate digit is valid
                grid[row][col] = num;             // Place candidate digit into the empty cell
                solveRecursive(solutionCount, solutions); // Recursively attempt to fill the next cell
                grid[row][col] = 0;               // Reset cell to 0 (backtrack) to try next candidate
            }
        }
    }

public:
    SudokuBoard() {                               // Constructor for SudokuBoard class
        for (int i = 0; i < 9; ++i)               // Loop through all 9 rows
            for (int j = 0; j < 9; ++j)           // Loop through all 9 columns
                grid[i][j] = 0;                   // Initialize every cell to 0 (blank)
    }

    // Load puzzle configuration from an input text file
    bool loadFromFile(const std::string &filename) {
        std::ifstream file(filename);             // Create input file stream object
        if (!file.is_open()) return false;        // Return false if file cannot be opened
        
        for (int r = 0; r < 9; ++r) {             // Loop through each row of the file data
            for (int c = 0; c < 9; ++c) {         // Loop through each column of the file data
                char ch;                          // Variable to hold character read from file
                if (!(file >> ch)) return false;  // Return false if file read fails prematurely
                if (ch == '_') {                  // Check if character indicates an empty space
                    grid[r][c] = 0;               // Store 0 internally for empty squares
                } else if (ch >= '1' && ch <= '9') { // Check if character is a valid digit
                    grid[r][c] = ch - '0';        // Convert character digit to integer and store
                } else {
                    return false;                 // Return false if an invalid character is encountered
                }
            }
        }
        return true;                              // Return true upon successful file loading
    }

    // Print the puzzle grid to standard output using underscores for blank cells
    void print() const {
        for (int r = 0; r < 9; ++r) {             // Loop through each row of the grid
            for (int c = 0; c < 9; ++c) {         // Loop through each column of the grid
                if (grid[r][c] == 0) {            // Check if cell is blank
                    std::cout << "_ ";            // Print underscore and space for blank cell
                } else {
                    std::cout << grid[r][c] << (c == 8 ? "" : " "); // Print digit with appropriate spacing
                }
            }
            std::cout << "\n";                    // Print a newline character at the end of each row
        }
    }

    // Solve the puzzle and print results matching the exact required layout
    void solve(const std::string &filename) {
        std::cout << filename << "\n";            // Print the puzzle file name first
        std::cout << "Puzzle:\n";                 // Print the 'Puzzle:' header label
        print();                                  // Print the original puzzle loaded from file
        std::cout << "\n";                        // Print blank line separating puzzle and solution
        
        int solutionCount = 0;                    // Initialize solution counter to zero
        std::vector<std::vector<int>> solutions;  // Vector collection to store all valid solutions
        solveRecursive(solutionCount, solutions); // Invoke the recursive backtracking solver

        if (solutionCount == 0) {                 // Check if the solution count is zero
            std::cout << "No solution found\n";   // Print failure message if no solution exists
        } else {
            for (int i = 0; i < solutionCount; ++i) { // Loop through each discovered solution
                int idx = 0;                      // Index tracker for the flattened solution vector
                for (int r = 0; r < 9; ++r) {     // Loop through solution rows
                    for (int c = 0; c < 9; ++c) { // Loop through solution columns
                        std::cout << solutions[i][idx++] << (c == 8 ? "" : " "); // Print solved grid cell value
                    }
                    std::cout << "\n";            // Print newline at end of row
                }
                // If there are multiple solutions, add a blank space between each solution
                if (i < solutionCount - 1) {
                    std::cout << "\n";
                }
            }
        }
        std::cout << "\nSolutions found: " << solutionCount << "\n\n"; // Print summary line of solutions found
    }
};

int main() {                                      // Main program execution entry point
    for (int i = 1; i <= 5; ++i) {                // Loop through puzzle files from puzzle1.txt to puzzle5.txt
        std::string filename = "puzzle" + std::to_string(i) + ".txt"; // Construct current file name string
        SudokuBoard board;                        // Instantiate a new SudokuBoard object for this puzzle
        if (board.loadFromFile(filename)) {       // Verify that the puzzle file loads successfully
            board.solve(filename);                // Execute solve routine and output formatted results
        } else {
            std::cout << filename << "\nCould not open or read file.\n\n"; // Handle file read errors gracefully
        }
    }
    return 0;                                     // Return 0 to signal successful program termination
}

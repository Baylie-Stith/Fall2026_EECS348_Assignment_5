/* Program Name: Assignment4.cpp
 * Description: Solve 5 provided Sudoku puzzles.
 * Inputs: puzzle1.txt, puzzle2.txt, puzzle3.txt, puzzle4.txt, puzzle5.txt
 * Output: Outputs to terminal and solution files
 * Collaborators: N/A
 * Sources: Gemini, Claude
 * Author: Wyatt Sullivan
 * Creation Date: 10/15/2026
 * Last Revision: 10/27/2026
 * Revised by: Baylie Stith
 */

#include <iostream>   // gives us std::cout / std::cerr for printing to the screen
#include <fstream>    // gives us ifstream / ofstream for reading and writing files
#include <vector>     // gives us std::vector to hold the list of solutions
#include <string>     // gives us std::string for building file names
#include <sstream>    // Baylie Stith gives us ostringstream to build text in memory

class SudokuBoard {   // one object of this class holds and solves one puzzle
private:
    // Baylie Stith + Claude: Named constants so the numbers 9 and 3 have a meaning.
    static constexpr int SIZE = 9;       // the board is SIZE x SIZE cells
    static constexpr int BOX_SIZE = 3;   // each inner box is BOX_SIZE x BOX_SIZE cells

    int grid[SIZE][SIZE];   // (Original) the board itself; a 0 means the cell is blank

    // Revised: Baylie Stith + Claude Original: Wyatt Sullivan + Gemini
    // Decides whether digit num may legally go at (row, col) by checking that the
    // digit is not already in that row, that column, or that 3x3 box.
    bool isValid(int row, int col, int num) const {
        for (int c = 0; c < SIZE; ++c) {        // walk across every column of this row
            if (grid[row][c] == num)
                return false;                   // digit already sits in the row, so it is illegal
        }

        for (int r = 0; r < SIZE; ++r) {        // walk down every row of this column
            if (grid[r][col] == num)
                return false;                   // digit already sits in the column, so it is illegal
        }

        int startRow = row - row % BOX_SIZE;    // first row of the box that contains this cell
        int startCol = col - col % BOX_SIZE;    // first column of the box that contains this cell

        for (int r = 0; r < BOX_SIZE; ++r) {            // go through the box's rows
            for (int c = 0; c < BOX_SIZE; ++c) {        // go through the box's columns
                if (grid[startRow + r][startCol + c] == num)
                    return false;               // digit already sits in the box, so it is illegal
            }
        }

        return true;                            // the digit passed the row, column and box checks
    }

    // Revised: Baylie Stith + Claude Original: Wyatt Sullivan + Gemini
    // Picks the next blank cell to fill. The inherited version took the first blank
    // cell it found; this one takes the blank cell with the fewest legal digits, which
    // lets the search discover dead ends sooner. Returns false when no blanks remain.
    bool findEmptyCell(int &row, int &col) const {
        int fewestCandidates = SIZE + 1;        // best (lowest) count so far; starts higher than any real count
        bool foundEmpty = false;                // becomes true once we see at least one blank cell

        for (int r = 0; r < SIZE; ++r) {        // look at every row
            for (int c = 0; c < SIZE; ++c) {    // look at every column in that row
                if (grid[r][c] != 0)
                    continue;                   // this cell already has a digit, so skip it

                int candidateCount = 0;         // how many digits could legally go in this blank

                for (int num = 1; num <= SIZE; ++num) {   // test each digit 1 through 9
                    if (isValid(r, c, num)) {   // if the digit is legal here...
                        candidateCount++;       // ...count it as a candidate
                    }
                }

                // A blank with zero candidates means the current attempt cannot succeed.
                if (candidateCount == 0) {
                    row = r;                    // hand this cell back to the caller
                    col = c;
                    return true;                // the caller will find no legal digit and back up
                }

                if (candidateCount < fewestCandidates) {  // is this the most constrained blank so far?
                    fewestCandidates = candidateCount;    // remember its candidate count
                    row = r;                    // remember its row
                    col = c;                    // remember its column
                    foundEmpty = true;          // note that a blank cell exists
                }

                // One candidate is the lowest possible useful count, so stop searching early.
                if (fewestCandidates == 1)
                    return true;
            }
        }

        return foundEmpty;                      // false means the board is completely filled
    }

    // New: Baylie Stith + Claude
    // Confirms the clues in the puzzle file do not contradict each other (for example,
    // two 5s in one row). Without this, a broken puzzle could be "solved" incorrectly.
    bool givensValid() {
        for (int r = 0; r < SIZE; ++r) {        // check every row
            for (int c = 0; c < SIZE; ++c) {    // check every column
                if (grid[r][c] != 0) {          // only clues need checking, not blanks
                    int val = grid[r][c];       // save the clue's value

                    grid[r][c] = 0;             // erase it so it doesn't conflict with itself
                    bool ok = isValid(r, c, val);   // ask whether it clashes with any other clue
                    grid[r][c] = val;           // put the clue back

                    if (!ok)
                        return false;           // two clues clash, so the puzzle is unsolvable
                }
            }
        }

        return true;                            // no clashes found
    }

    // Original: Wyatt Sullivan + Gemini
    // Backtracking search: fill one blank, recurse to fill the rest, then undo the move
    // and try the next digit. Every complete board it reaches is saved as a solution.
    void solveRecursive(
        int &solutionCount,
        std::vector<std::vector<int>> &solutions) {

        int row, col;                           // will hold the position of the blank we work on

        if (!findEmptyCell(row, col)) {         // no blanks left means the board is complete
            std::vector<int> currentSolution;   // flat list that will store this finished board

            for (int r = 0; r < SIZE; ++r) {            // go through each row of the finished board
                for (int c = 0; c < SIZE; ++c) {        // go through each column of that row
                    currentSolution.push_back(grid[r][c]);  // copy the cell into the flat list
                }
            }

            solutions.push_back(currentSolution);   // store the finished board with the others
            solutionCount++;                        // add one to the number of solutions found
            return;                                 // go back up to look for more solutions
        }

        // Try each digit in the chosen blank.
        for (int num = 1; num <= SIZE; ++num) {
            if (isValid(row, col, num)) {       // only continue with digits that break no rule
                grid[row][col] = num;           // place the digit

                solveRecursive(solutionCount, solutions);   // try to complete the rest of the board

                // Undo the placement so the next digit (or another branch) starts clean.
                grid[row][col] = 0;
            }
        }
    }

public:
    // Original: Wyatt Sullivan + Gemini
    SudokuBoard() {                             // constructor: runs when a SudokuBoard is created
        for (int r = 0; r < SIZE; ++r) {        // go through every row
            for (int c = 0; c < SIZE; ++c) {    // go through every column
                grid[r][c] = 0;                 // start with every cell blank
            }
        }
    }

    // Revised: Baylie Stith + Claude Original: Wyatt Sullivan + Gemini
    // Reads one puzzle file into grid. Returns false if the file is missing or malformed.
    bool loadFromFile(const std::string &filename) {
        std::ifstream file(filename);           // open the file for reading

        if (!file.is_open())
            return false;                       // could not open it, so report failure

        for (int r = 0; r < SIZE; ++r) {        // read 9 rows
            for (int c = 0; c < SIZE; ++c) {    // with 9 cells each
                char ch;                        // holds one character from the file

                if (!(file >> ch))              // >> skips whitespace; failure means file ended early
                    return false;

                if (ch == '_') {                // an underscore marks a blank cell
                    grid[r][c] = 0;             // store blanks as 0
                }
                else if (ch >= '1' && ch <= '9') {  // a digit character is a clue
                    grid[r][c] = ch - '0';      // convert from character to its numeric value
                }
                else {
                    return false;               // any other character makes the file invalid
                }
            }
        }

        // New: Baylie Stith + Claude After 81 cells, anything left over is an error.
        char extra;                             // holds a possible leftover character
        if (file >> extra)                      // true only if a non-whitespace character remains
            return false;

        return true;                            // the file contained exactly one valid puzzle
    }

    // Revised: Baylie Stith + Claude Original: Wyatt Sullivan + Gemini
    // Prints the board with blanks as underscores and no trailing space on a row.
    void print() const {
        for (int r = 0; r < SIZE; ++r) {        // one pass per row
            for (int c = 0; c < SIZE; ++c) {    // one pass per cell in the row
                if (grid[r][c] == 0) {          // is this cell blank?
                    std::cout << "_";           // show a blank as an underscore
                }
                else {
                    std::cout << grid[r][c];    // otherwise show the digit
                }

                // Put a space between cells but not after the last one in the row.
                if (c < SIZE - 1)
                    std::cout << " ";
            }

            std::cout << "\n";                  // finish the row
        }
    }

    // Revised: Baylie Stith + Claude Original: Wyatt Sullivan + Gemini
    // Prints the puzzle, solves it, and sends the result both to the terminal and to
    // solutionN.txt. The result text is built once so both outputs are identical.
    void solve(const std::string &filename) {
        std::cout << filename << "\n";          // show which puzzle file this is
        std::cout << "Puzzle:\n";               // label for the unsolved board
        print();                                // show the unsolved board
        std::cout << "\n";                      // blank line between puzzle and answer

        int solutionCount = 0;                  // running total of solutions found
        std::vector<std::vector<int>> solutions;    // every solution, each stored as 81 numbers

        // New: Baylie Stith + Claude Skip the search if the clues already contradict.
        if (givensValid()) {
            solveRecursive(solutionCount, solutions);   // search for all solutions
        }

        // New: Baylie Stith + Claude A string stream collects the output text in memory.
        std::ostringstream result;

        if (solutionCount == 0) {               // nothing was found
            result << "No solution found.\n";   // wording (with period) required by the assignment
        }
        else {
            for (int i = 0; i < solutionCount; ++i) {   // go through each stored solution
                int idx = 0;                    // position inside this solution's flat list

                for (int r = 0; r < SIZE; ++r) {        // write 9 rows
                    for (int c = 0; c < SIZE; ++c) {    // of 9 cells each
                        result << solutions[i][idx++];  // write the cell, then advance in the list

                        if (c < SIZE - 1)       // add a space between cells, not after the last
                            result << " ";
                    }

                    result << "\n";             // end the row
                }

                // Put a blank line between solutions, but not after the final one.
                if (i < solutionCount - 1)
                    result << "\n";
            }

            result << "\nSolutions found: "     // summary line required by the assignment
                   << solutionCount << "\n";    // followed by how many there were
        }

        std::cout << result.str() << "\n";      // [New] show the result text on the terminal

        // New: Baylie Stith + Claude Save the same text to solutionN.txt.
        std::string outName =
            "solution" + filename.substr(6);    // drop the 6 letters of "puzzle" and keep "N.txt"

        std::ofstream outFile(outName);         // open (or create) the output file

        if (!outFile.is_open()) {               // could not create the file
            std::cerr << "Error: Could not create "
                      << outName << "\n";       // tell the user which file failed
            return;                             // nothing more can be saved
        }

        outFile << result.str();                // write the result text into the file

        if (!outFile) {                         // check whether the write itself failed
            std::cerr << "Error: Could not write to "
                      << outName << "\n";       // tell the user which file failed
        }
    }
};

// Original: Wyatt Sullivan + Gemini
// Runs the solver on puzzle1.txt through puzzle5.txt, one after another.
int main() {                                    // the program starts here
    for (int i = 1; i <= 5; ++i) {              // i is the puzzle number, 1 to 5
        std::string filename =
            "puzzle" + std::to_string(i) + ".txt";  // build the name, e.g. "puzzle3.txt"

        SudokuBoard board;                      // make a fresh, empty board for this puzzle

        if (board.loadFromFile(filename)) {     // try to read the puzzle in
            board.solve(filename);              // it loaded, so solve and report it
        }
        else {
            std::cout << filename
                      << "\nCould not open or read file.\n\n";  // it did not load, so say so
        }
    }

    return 0;                                   // tell the operating system everything finished normally
}
Nash Game (Hex) Implementation Documentation
1. Overview
This document provides a comprehensive explanation of our Nash Game (Hex) implementation for the Data Structures and Algorithms assignment. The implementation fulfills all required tasks using efficient algorithms and appropriate data structures.
2. Program Structure
The program is organized into the following key classes:
2.1 Board Class
Manages the game state and provides core functionality for the game board.
2.2 Player Hierarchy
•	Player: Abstract base class defining the interface
•	HumanPlayer: Handles human input
•	RandomPlayer: Implements random move generation (Task 2)
•	SmartPlayer: Implements strategic move selection (Task 6)
•	MonteCarloPlayer: Implements Monte Carlo simulation approach (Task 7)
2.3 NashGame Class
Controls game flow, player interactions, and win detection.
3. Implementation Details
3.1 Game Board Representation
Despite Hex being played on a rhombus-shaped board with hexagonal cells, we represent it using a conventional 2D array (vector<vector<int>>):
•	1 represents White player's stones
•	-1 represents Black player's stones
•	0 represents empty cells
The hexagonal connectivity is simulated through our neighbor calculation logic.
// Direction arrays for hexagonal grid
const int DX[6] = {1, 1, 0, 0, -1, -1};
const int DY[6] = {-1, 1, -1, 1, 0, 1};
These arrays define the six possible directions from any cell in a hexagonal grid:
•	Down-left: (1, -1)
•	Down-right: (1, 1)
•	Left: (0, -1)
•	Right: (0, 1)
•	Up-left: (-1, 0)
•	Up-right: (-1, 1)
3.2 Task 1: Board Full Check
We implemented a method to check if the board is full by iterating through all cells and checking if any are still empty (0):
bool Board::isFull() const {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == 0) {
                return false;
            }
        }
    }
    return true;
}
Time Complexity: O(n²) where n is the board size Space Complexity: O(1)
3.3 Task 2: Random Player Implementation
The RandomPlayer class generates valid random moves. We optimized the implementation by collecting all valid moves first and then selecting one randomly, rather than repeatedly generating random coordinates until finding a valid one.
std::pair<int, int> RandomPlayer::getMove() override {
    // Efficient approach: Create list of all empty cells, then choose randomly
    std::vector<std::pair<int, int>> emptyCells;
    int size = board.getSize();
    
    // Find all empty cells
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board.isValidMove(i, j)) {
                emptyCells.push_back({i, j});
            }
        }
    }
    
    // Select a random cell from available empty cells
    if (!emptyCells.empty()) {
        int index = std::rand() % emptyCells.size();
        return emptyCells[index];
    }
    
    // Fallback (should not happen with proper game termination)
    return {-1, -1};
}
Time Complexity:
•	O(n²) to collect all empty cells
•	O(1) to select a random move
Space Complexity: O(n²) in the worst case when most cells are empty
This approach is more efficient than repeatedly generating random coordinates because:
1.	It requires only one call to rand() instead of potentially many failed attempts
2.	It guarantees a valid move will be found if one exists
3.	It provides uniform distribution over all possible moves
3.4 Task 3: Complete Straight Line Check
We implemented a function to check if a player has created a complete straight line:
bool Board::hasCompleteLineWin(int playerType) const {
    // Check rows (for White player)
    if (playerType == 1) {
        for (int row = 0; row < size; row++) {
            bool completeRow = true;
            for (int col = 0; col < size; col++) {
                if (board[row][col] != playerType) {
                    completeRow = false;
                    break;
                }
            }
            if (completeRow) return true;
        }
    }
    // Check columns (for Black player)
    else if (playerType == -1) {
        for (int col = 0; col < size; col++) {
            bool completeCol = true;
            for (int row = 0; row < size; row++) {
                if (board[row][col] != playerType) {
                    completeCol = false;
                    break;
                }
            }
            if (completeCol) return true;
        }
    }
    return false;
}
Time Complexity: O(n²) where n is the board size Space Complexity: O(1)
3.5 Task 4: Neighbor Finding
The getNeighbours function finds all adjacent cells that contain the same player's stones:
std::stack<std::pair<int, int>> Board::getNeighbours(int playerType, int x, int y) {
    std::stack<std::pair<int, int>> neighbors;
    
    for (int i = 0; i < 6; i++) {
        int nx = x + DX[i];
        int ny = y + DY[i];
        
        if (nx >= 0 && nx < size && ny >= 0 && ny < size && board[nx][ny] == playerType) {
            neighbors.push({nx, ny});
        }
    }
    
    return neighbors;
}
Time Complexity: O(1) - always checks exactly 6 neighboring cells Space Complexity: O(1) - at most 6 neighbors can be stored
We use a stack data structure because:
1.	It's efficient for the subsequent DFS algorithm used for path finding
2.	The LIFO (Last In, First Out) property helps with backtracking
3.	It has O(1) push and pop operations
3.6 Task 5: Win Detection Algorithm
The win condition in Hex is when a player creates a connected path between their assigned sides:
•	White player: connect left to right sides
•	Black player: connect top to bottom sides
We implemented this using Depth-First Search (DFS) algorithm:
bool Board::hasWon(int playerType) {
    // For White player (1): check path from left to right
    if (playerType == 1) {
        // Mark all stones in the leftmost column as starting points
        std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));
        for (int i = 0; i < size; i++) {
            if (board[i][0] == playerType) {
                // Use DFS to find a path to the rightmost column
                if (dfsPathFinding(playerType, i, 0, visited)) {
                    return true;
                }
            }
        }
    }
    // For Black player (-1): check path from top to bottom
    else if (playerType == -1) {
        // Mark all stones in the top row as starting points
        std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));
        for (int j = 0; j < size; j++) {
            if (board[0][j] == playerType) {
                // Use DFS to find a path to the bottom row
                if (dfsPathFinding(playerType, 0, j, visited)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Board::dfsPathFinding(int playerType, int x, int y, std::vector<std::vector<bool>>& visited) {
    // Mark current cell as visited
    visited[x][y] = true;
    
    // If White player reaches rightmost column or Black player reaches bottom row
    if ((playerType == 1 && y == size - 1) || (playerType == -1 && x == size - 1)) {
        return true;
    }
    
    // Get all neighboring cells of the same player
    std::stack<std::pair<int, int>> neighbors = getNeighbours(playerType, x, y);
    
    // Explore all unvisited neighbors
    while (!neighbors.empty()) {
        auto [nx, ny] = neighbors.top();
        neighbors.pop();
        
        if (!visited[nx][ny]) {
            if (dfsPathFinding(playerType, nx, ny, visited)) {
                return true;
            }
        }
    }
    
    return false;
}
Time Complexity: O(n²) where n is the board size Space Complexity: O(n²) for the visited matrix
The DFS algorithm works by:
1.	Starting from each stone on the player's starting side
2.	Recursively exploring all connected stones of the same player
3.	Returning true if we reach the opposite side
4.	Using a visited matrix to avoid cycles and redundant exploration
3.7 Task 6: Smart Player Implementation
The SmartPlayer uses a heuristic evaluation function to score potential moves:
std::pair<int, int> SmartPlayer::getMove() override {
    int size = board.getSize();
    std::pair<int, int> bestMove = {-1, -1};
    int bestScore = -999999;
    
    // Try each possible move and evaluate
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board.isValidMove(i, j)) {
                // Make temporary move
                board.makeMove(i, j, playerType);
                
                // Evaluate the position after this move
                int score = evaluateMove(i, j);
                
                // Undo the move
                board.makeMove(i, j, 0);
                
                // Update best move if needed
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = {i, j};
                }
            }
        }
    }
    
    return bestMove;
}

int SmartPlayer::evaluateMove(int x, int y) {
    int score = 0;
    
    // Check if this move would create a win
    if (board.hasWon(playerType)) {
        return 10000;  // Very high score for winning move
    }
    
    // Get connected neighbors with the same stone
    std::stack<std::pair<int, int>> neighbors = board.getNeighbours(playerType, x, y);
    score += neighbors.size() * 10;  // More neighbors is good
    
    // Position-based evaluation
    if (playerType == 1) {  // White player (left to right)
        score += y * 5;  // Prefer moves closer to right side
    } else {  // Black player (top to bottom)
        score += x * 5;  // Prefer moves closer to bottom
    }
    
    // Use the board's evaluation function
    score += board.evaluatePosition(playerType);
    
    return score;
}
Our evaluation function considers:
1.	Winning moves (highest priority)
2.	Number of connected neighbors (connectivity)
3.	Progress toward the goal (position-based scoring)
4.	Overall board evaluation
Time Complexity: O(n⁴) where n is the board size
•	O(n²) for trying each possible move
•	O(n²) for evaluating each move (includes win check)
Space Complexity: O(n²) for the evaluation process
3.8 Task 7: Monte Carlo Player Implementation
The MonteCarloPlayer uses a simulation-based approach:
std::pair<int, int> MonteCarloPlayer::getMove() override {
    int size = board.getSize();
    std::pair<int, int> bestMove = {-1, -1};
    int bestWins = -1;
    
    // Try each possible move
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board.isValidMove(i, j)) {
                // Make temporary move
                board.makeMove(i, j, playerType);
                
                // Run Monte Carlo simulations
                int wins = runSimulations();
                
                // Undo the move
                board.makeMove(i, j, 0);
                
                // Update best move if needed
                if (wins > bestWins) {
                    bestWins = wins;
                    bestMove = {i, j};
                }
            }
        }
    }
    
    return bestMove;
}

int MonteCarloPlayer::runSimulations() {
    int wins = 0;
    
    for (int sim = 0; sim < SIMULATIONS; sim++) {
        // Create a copy of the current board for simulations
        Board tempBoard = board;
        
        // Current player (alternating)
        int currentPlayer = -playerType;  // Start with opponent
        
        // Play random moves until someone wins or board is full
        while (true) {
            // Check if current player has won
            if (tempBoard.hasWon(currentPlayer)) {
                if (currentPlayer == playerType) {
                    wins++;
                }
                break;
            }
            
            // Check if board is full
            if (tempBoard.isFull()) {
                break;
            }
            
            // Make a random move
            // [code for random move generation]
            
            // Switch player
            currentPlayer = -currentPlayer;
        }
    }
    
    return wins;
}
The Monte Carlo approach works by:
1.	For each possible move, make the move temporarily
2.	Run many random simulations to completion from that position
3.	Count how many simulations result in a win
4.	Choose the move with the highest win rate
Time Complexity: O(n² × s × g) where:
•	n is the board size
•	s is the number of simulations (constant: 100)
•	g is the average game length (approximately n²/2)
This gives a worst-case complexity of O(n⁴)
Space Complexity: O(n²) for board copies during simulation
4. Design Decisions and Optimizations
4.1 Board Class Design
The Board class was designed to encapsulate all board-related operations, including:
•	Game state representation
•	Move validation
•	Win detection
•	Board evaluation
We implemented explicit copy constructor and assignment operator to facilitate board copying during simulations:
// Copy constructor
Board(const Board& other) : size(other.size), board(other.board) {}

// Assignment operator
Board& operator=(const Board& other) {
    if (this != &other) {
        size = other.size;
        board = other.board;
    }
    return *this;
}
4.2 Direction Array Optimization
We moved the direction arrays outside the Board class to avoid issues with const member variables:
// Define direction arrays globally instead of inside the Board class
const int DX[6] = {1, 1, 0, 0, -1, -1};
const int DY[6] = {-1, 1, -1, 1, 0, 1};
This prevents compilation errors related to non-static const member arrays and assignment operators.
4.3 Player Hierarchy
We used polymorphism to implement different player types with a common interface:
class Player {
public:
    virtual std::pair<int, int> getMove() = 0;
    virtual ~Player() {}
};
This design allows us to:
1.	Easily swap different player types
2.	Add new player types without changing the game logic
3.	Run comparisons between different AI implementations
4.4 Random Player Optimization
Instead of repeatedly generating random coordinates until finding a valid one, we collect all valid moves first:
std::vector<std::pair<int, int>> emptyCells;
// [collect all empty cells]
int index = std::rand() % emptyCells.size();
return emptyCells[index];
This optimization:
1.	Reduces the number of random number generations
2.	Ensures uniform distribution of random moves
3.	Guarantees valid move selection
4.5 Smart Player Heuristics
Our SmartPlayer uses several heuristics:
1.	Connectivity (number of neighboring stones)
2.	Position advancement (progress toward goal)
3.	Board evaluation (overall position assessment)
These heuristics create a simple but effective strategy that beats random players consistently.
4.6 Multi-Game Evaluation
For AI comparison, we implemented a method to run multiple games automatically:
void NashGame::runMultipleGames(int numGames) {
    // [runs multiple games and counts wins]
}
This allows for statistical comparison of different AI approaches.
5. Computational Complexity Analysis
5.1 Random Move Generation
•	Time Complexity: O(n²) to scan the board once + O(1) for random selection
•	Space Complexity: O(n²) in worst case for storing empty cells
5.2 Adding a New Move
•	Time Complexity: O(1) for direct array access
•	Space Complexity: O(1) for constant additional space
5.3 Win Detection Algorithm
•	Time Complexity: O(n²) where each cell is visited at most once
•	Space Complexity: O(n²) for the visited matrix
5.4 Smart Player Move Selection
•	Time Complexity: O(n⁴) considering all possible moves and evaluations
•	Space Complexity: O(n²) for evaluation data structures
5.5 Monte Carlo Player Move Selection
•	Time Complexity: O(n⁴) considering simulations
•	Space Complexity: O(n²) for board copies
6. Potential Improvements
1.	Minimax with Alpha-Beta Pruning: Implement a more advanced AI using the minimax algorithm with alpha-beta pruning for deeper look-ahead.
2.	Union-Find Data Structure: Optimize win detection using a Union-Find data structure to track connected components more efficiently.
3.	Parallelization: Parallelize Monte Carlo simulations to increase the number of simulations without increasing runtime.
4.	Opening Book: Implement an opening book for common starting positions to improve early game play.
5.	Progressive Deepening: Use iterative deepening techniques to allocate more computation time to promising moves.
7. Conclusion
Our Nash Game implementation fulfills all assignment requirements using efficient algorithms and data structures. The key components include:
1.	A 2D array representation of the hexagonal board
2.	DFS-based win detection algorithm
3.	Multiple AI player types with different strategies
4.	Comprehensive testing through automatic multi-game evaluation
The implementation balances algorithmic efficiency with code readability and maintainability, making it both educational and functional.


 
 

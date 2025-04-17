// Nash Game (Hex) Implementation
// Data Structures and Algorithms Assignment

#include <iostream>
#include <vector>
#include <stack>
#include <random>
#include <ctime>
#include <utility>
#include <algorithm>

// Forward declarations
class Board;
class Player;
class HumanPlayer;
class RandomPlayer;
class SmartPlayer;
class MonteCarloPlayer;

// Define direction arrays globally instead of inside the Board class
// This avoids issues with the default assignment operator
const int DX[6] = {1, 1, 0, 0, -1, -1};
const int DY[6] = {-1, 1, -1, 1, 0, 1};

// Abstract Player class
class Player {
public:
    virtual std::pair<int, int> getMove() = 0;
    virtual ~Player() {}
};

// Board class to represent the Nash game board
class Board {
private:
    int size;                      // Size of the board (n x n)
    std::vector<std::vector<int>> board;  // 2D array to store the board state

public:
    // Constructor
    Board(int n) : size(n) {
        // Initialize the board with all cells empty (0)
        board.resize(n, std::vector<int>(n, 0));
    }
    
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
    
    // Get the size of the board
    int getSize() const {
        return size;
    }
    
    // Get the value at a specific position
    int getValue(int x, int y) const {
        if (x >= 0 && x < size && y >= 0 && y < size) {
            return board[x][y];
        }
        return -999; // Invalid position
    }
    
    // Check if a move is valid
    bool isValidMove(int x, int y) const {
        return x >= 0 && x < size && y >= 0 && y < size && board[x][y] == 0;
    }
    
    // Make a move on the board
    bool makeMove(int x, int y, int playerType) {
        if (isValidMove(x, y)) {
            board[x][y] = playerType;
            return true;
        }
        return false;
    }
    
    // Display the board
    void display() const {
        // Display column indices
        std::cout << " ";
        for (int j = 0; j < size; j++) {
            std::cout << " " << j + 1 << " ";
        }
        std::cout << std::endl;
        
        // Display horizontal separator
        std::cout << " ";
        for (int j = 0; j < size; j++) {
            std::cout << "---";
        }
        std::cout << std::endl;
        
        // Display board with row indices
        for (int i = 0; i < size; i++) {
            std::cout << i + 1 << "|";
            for (int j = 0; j < size; j++) {
                if (board[i][j] == 0) {
                    std::cout << " ";
                } else if (board[i][j] == 1) {
                    std::cout << "W";
                } else {
                    std::cout << "B";
                }
                std::cout << " |";
            }
            std::cout << std::endl;
            
            // Display horizontal separator
            std::cout << " ";
            for (int j = 0; j < size; j++) {
                std::cout << "---";
            }
            std::cout << std::endl;
        }
    }
    
    // Task 1: Check if the board is full
    bool isFull() const {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (board[i][j] == 0) {
                    return false;
                }
            }
        }
        return true;
    }
    
    // Task 3: Check if a player has a complete straight line
    bool hasCompleteLineWin(int playerType) const {
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
    
    // Task 4: Get neighbors of a cell with the same player's stones
    std::stack<std::pair<int, int>> getNeighbours(int playerType, int x, int y) {
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
    
    // Print all neighbors of a cell with the same player's stones
    void printNeighbours(int x, int y) {
        int playerType = board[x][y];
        if (playerType == 0) {
            std::cout << "Cell (" << x + 1 << "," << y + 1 << ") is empty." << std::endl;
            return;
        }
        
        std::stack<std::pair<int, int>> neighbors = getNeighbours(playerType, x, y);
        std::cout << "Neighbors of (" << x + 1 << "," << y + 1 << ") with " 
                  << (playerType == 1 ? "White" : "Black") << " stones:" << std::endl;
        
        if (neighbors.empty()) {
            std::cout << "No neighbors with the same stone." << std::endl;
            return;
        }
        
        while (!neighbors.empty()) {
            std::pair<int, int> pos = neighbors.top();
            neighbors.pop();
            std::cout << "(" << pos.first + 1 << "," << pos.second + 1 << ") ";
        }
        std::cout << std::endl;
    }
    
    // Task 5: Check if a player has won using DFS
    bool hasWon(int playerType) {
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
    
    // DFS algorithm to find a winning path
    bool dfsPathFinding(int playerType, int x, int y, std::vector<std::vector<bool>>& visited) {
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

    // For Smart Player: Evaluate board position
    int evaluatePosition(int playerType) {
        // Simple evaluation function:
        // Count stones and give more weight to stones that are advanced toward the goal
        int score = 0;
        if (playerType == 1) {  // White player (left to right)
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    if (board[i][j] == playerType) {
                        score += j + 1;  // More weight to stones closer to right side
                    } else if (board[i][j] == -playerType) {
                        score -= j + 1;  // Subtract opponent's progress
                    }
                }
            }
        } else {  // Black player (top to bottom)
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    if (board[i][j] == playerType) {
                        score += i + 1;  // More weight to stones closer to bottom
                    } else if (board[i][j] == -playerType) {
                        score -= i + 1;  // Subtract opponent's progress
                    }
                }
            }
        }
        return score;
    }
};

// Human Player class
class HumanPlayer : public Player {
private:
    Board& board;
    int playerType;
    
public:
    HumanPlayer(Board& b, int type) : board(b), playerType(type) {}
    
    std::pair<int, int> getMove() override {
        int x, y;
        std::cout << "Enter your move (row column): ";
        std::cin >> x >> y;
        
        // Convert to 0-indexed
        x--;
        y--;
        
        // Validate the move
        while (!board.isValidMove(x, y)) {
            std::cout << "Invalid move. Try again: ";
            std::cin >> x >> y;
            x--;
            y--;
        }
        
        return {x, y};
    }
};

// Task 2: Random Player implementation
class RandomPlayer : public Player {
private:
    Board& board;
    int playerType;
    
public:
    RandomPlayer(Board& b, int type) : board(b), playerType(type) {
        // Seed the random number generator
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }
    
    std::pair<int, int> getMove() override {
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
};

// Task 6: Smart Player implementation
class SmartPlayer : public Player {
private:
    Board& board;
    int playerType;
    
public:
    SmartPlayer(Board& b, int type) : board(b), playerType(type) {}
    
    std::pair<int, int> getMove() override {
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
    
    // Evaluate a move based on various factors
    int evaluateMove(int x, int y) {
        int score = 0;
        int size = board.getSize();
        
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
};

// Task 7: Monte Carlo Player implementation
class MonteCarloPlayer : public Player {
private:
    Board& board;
    int playerType;
    const int SIMULATIONS = 100;  // Number of simulations per move
    
public:
    MonteCarloPlayer(Board& b, int type) : board(b), playerType(type) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }
    
    std::pair<int, int> getMove() override {
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
    
    // Run simulations and return the number of wins
    int runSimulations() {
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
                std::vector<std::pair<int, int>> emptyCells;
                int size = tempBoard.getSize();
                for (int i = 0; i < size; i++) {
                    for (int j = 0; j < size; j++) {
                        if (tempBoard.isValidMove(i, j)) {
                            emptyCells.push_back({i, j});
                        }
                    }
                }
                
                if (!emptyCells.empty()) {
                    int index = std::rand() % emptyCells.size();
                    auto [x, y] = emptyCells[index];
                    tempBoard.makeMove(x, y, currentPlayer);
                }
                
                // Switch player
                currentPlayer = -currentPlayer;
            }
        }
        
        return wins;
    }
};

// Nash Game class to manage the game
class NashGame {
private:
    Board board;
    Player* player1;  // White player
    Player* player2;  // Black player
    int currentPlayer;
    
public:
    NashGame(int size, int p1Type, int p2Type) : board(size), currentPlayer(1) {
        // Create players based on types
        // 0: Human, 1: Random, 2: Smart, 3: Monte Carlo
        createPlayers(p1Type, p2Type);
    }
    
    ~NashGame() {
        delete player1;
        delete player2;
    }
    
    void createPlayers(int p1Type, int p2Type) {
        // Create player 1 (White)
        switch (p1Type) {
            case 0:
                player1 = new HumanPlayer(board, 1);
                break;
            case 1:
                player1 = new RandomPlayer(board, 1);
                break;
            case 2:
                player1 = new SmartPlayer(board, 1);
                break;
            case 3:
                player1 = new MonteCarloPlayer(board, 1);
                break;
            default:
                player1 = new HumanPlayer(board, 1);
        }
        
        // Create player 2 (Black)
        switch (p2Type) {
            case 0:
                player2 = new HumanPlayer(board, -1);
                break;
            case 1:
                player2 = new RandomPlayer(board, -1);
                break;
            case 2:
                player2 = new SmartPlayer(board, -1);
                break;
            case 3:
                player2 = new MonteCarloPlayer(board, -1);
                break;
            default:
                player2 = new HumanPlayer(board, -1);
        }
    }
    
    void play() {
        bool gameOver = false;
        
        while (!gameOver) {
            // Display the board
            board.display();
            
            // Get the current player
            Player* currentPlayerObj = (currentPlayer == 1) ? player1 : player2;
            std::string playerName = (currentPlayer == 1) ? "White" : "Black";
            
            std::cout << playerName << "'s turn." << std::endl;
            
            // Get the move from the current player
            std::pair<int, int> move = currentPlayerObj->getMove();
            int x = move.first;
            int y = move.second;
            
            // Make the move
            if (board.makeMove(x, y, currentPlayer)) {
                std::cout << playerName << " places at (" << x + 1 << "," << y + 1 << ")" << std::endl;
                
                // Show neighbors for demonstration (Task 4)
                board.printNeighbours(x, y);
                
                // Check if the current player has won with a straight line (Task 3)
                if (board.hasCompleteLineWin(currentPlayer)) {
                    board.display();
                    std::cout << playerName << " wins with a straight line!" << std::endl;
                    gameOver = true;
                    continue;
                }
                
                // Check if the current player has won (Task 5)
                if (board.hasWon(currentPlayer)) {
                    board.display();
                    std::cout << playerName << " wins!" << std::endl;
                    gameOver = true;
                    continue;
                }
                
                // Check if the board is full (Task 1)
                if (board.isFull()) {
                    board.display();
                    std::cout << "Game over! The board is full." << std::endl;
                    gameOver = true;
                    continue;
                }
                
                // Switch players
                currentPlayer = -currentPlayer;
            } else {
                std::cout << "Invalid move. Try again." << std::endl;
            }
        }
    }
    
    // For Task 7: Run multiple games between computer players
    void runMultipleGames(int numGames) {
        int player1Wins = 0;
        int player2Wins = 0;
        
        for (int game = 0; game < numGames; game++) {
            // Create a new board for each game
            Board newBoard(board.getSize());
            
            // Reset game state
            int gameCurrentPlayer = 1;
            bool gameOver = false;
            
            std::cout << "Game " << game + 1 << " of " << numGames << std::endl;
            
            while (!gameOver) {
                // Get the current player
                Player* currentPlayerObj = (gameCurrentPlayer == 1) ? player1 : player2;
                
                // Get the move from the current player
                std::pair<int, int> move = currentPlayerObj->getMove();
                int x = move.first;
                int y = move.second;
                
                // Make the move
                if (newBoard.makeMove(x, y, gameCurrentPlayer)) {
                    // Check if the current player has won
                    if (newBoard.hasWon(gameCurrentPlayer)) {
                        if (gameCurrentPlayer == 1) {
                            player1Wins++;
                        } else {
                            player2Wins++;
                        }
                        gameOver = true;
                        continue;
                    }
                    
                    // Check if the board is full
                    if (newBoard.isFull()) {
                        gameOver = true;
                        continue;
                    }
                    
                    // Switch players
                    gameCurrentPlayer = -gameCurrentPlayer;
                }
            }
            
            std::cout << "Game " << game + 1 << " complete." << std::endl;
        }
        
        std::cout << "Results after " << numGames << " games:" << std::endl;
        std::cout << "Player 1 (White) wins: " << player1Wins << std::endl;
        std::cout << "Player 2 (Black) wins: " << player2Wins << std::endl;
    }
};

int main() {
    // Print welcome message
    std::cout << "Welcome to Nash (Hex) Game!" << std::endl;
    std::cout << "=============================" << std::endl;
    
    // Get board size
    int size;
    std::cout << "Enter board size (3-15 recommended): ";
    std::cin >> size;
    
    while (size < 3) {
        std::cout << "Board size must be at least 3. Try again: ";
        std::cin >> size;
    }
    
    // Get player types
    int p1Type, p2Type;
    std::cout << "Select player 1 (White) type:" << std::endl;
    std::cout << "0: Human, 1: Random, 2: Smart, 3: Monte Carlo: ";
    std::cin >> p1Type;
    
    std::cout << "Select player 2 (Black) type:" << std::endl;
    std::cout << "0: Human, 1: Random, 2: Smart, 3: Monte Carlo: ";
    std::cin >> p2Type;
    
    // Create and play the game
    NashGame game(size, p1Type, p2Type);
    
    // If both players are computer-controlled, ask if multiple games should be run
    if (p1Type > 0 && p2Type > 0) {
        char runMultiple;
        std::cout << "Run multiple games to compare performance? (y/n): ";
        std::cin >> runMultiple;
        
        if (runMultiple == 'y' || runMultiple == 'Y') {
            int numGames;
            std::cout << "Enter number of games to run: ";
            std::cin >> numGames;
            game.runMultipleGames(numGames);
            return 0;
        }
    }
    
    // Play a single game
    game.play();
    
    return 0;
}
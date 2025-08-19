#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "board.hpp"

class ChessUI {
private:
    game chess_game;
    bool running;

    void printWelcome() {
        std::cout << "\n╔══════════════════════════════════════╗\n";
        std::cout << "║            ♟️  Mate++ Chess          ║\n";
        std::cout << "║     Modern C++ Chess Engine         ║\n";
        std::cout << "╚══════════════════════════════════════╝\n\n";
    }

    void printHelp() {
        std::cout << "\n📖 HELP - Available Commands:\n";
        std::cout << "═══════════════════════════════════════\n";
        std::cout << "🔹 move <from><to>  - Make a move (e.g., 'move e2e4', 'e2e4')\n";
        std::cout << "🔹 show <square>    - Show possible moves (e.g., 'show e2', 'e2')\n";
        std::cout << "🔹 promote <piece>  - Promote pawn (Q/R/B/N)\n";
        std::cout << "🔹 flip            - Flip board perspective\n";
        std::cout << "🔹 board           - Display current board\n";
        std::cout << "🔹 help            - Show this help menu\n";
        std::cout << "🔹 quit/exit       - Exit the game\n\n";
        std::cout << "💡 Quick Tips:\n";
        std::cout << "   • You can type moves directly: 'e2e4' or 'move e2e4'\n";
        std::cout << "   • Click squares to see moves: 'e2' or 'show e2'\n";
        std::cout << "   • Squares: a1-h8 (letters a-h, numbers 1-8)\n\n";
    }

    void printStatus() {
        std::cout << "🎮 Turn: " << (chess_game.isWhitesTurn ? "⚪ White" : "⚫ Black");
        std::cout << " | View: " << (chess_game.isWhite ? "⚪ White" : "⚫ Black");
        std::cout << " | Type 'help' for commands\n";
        std::cout << "─────────────────────────────────────────────────────────\n";
    }

    std::string toLowerCase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    bool isValidSquare(const std::string& square) {
        if (square.length() != 2) return false;
        char file = square[0];
        char rank = square[1];
        return (file >= 'a' && file <= 'h') && (rank >= '1' && rank <= '8');
    }

    bool isValidMove(const std::string& move) {
        if (move.length() != 4) return false;
        return isValidSquare(move.substr(0, 2)) && isValidSquare(move.substr(2, 2));
    }

    void processMove(const std::string& from, const std::string& to) {
        if (!isValidSquare(from) || !isValidSquare(to)) {
            std::cout << "❌ Invalid square format! Use notation like 'e2' or 'a1'\n";
            return;
        }

        std::cout << "Attempting move: " << from << " → " << to << "\n";
        chess_game.makeMove(from, to);
    }

    void processShow(const std::string& square) {
        if (!isValidSquare(square)) {
            std::cout << "❌ Invalid square format! Use notation like 'e2'\n";
            return;
        }

        std::cout << "Showing moves for: " << square << "\n";
        chess_game.showMoves(square);
    }

    void processPromotion(const std::string& piece) {
        if (piece.length() != 1) {
            std::cout << "❌ Invalid piece! Use: Q (Queen), R (Rook), B (Bishop), N (Knight)\n";
            return;
        }

        char p = std::toupper(piece[0]);
        if (p != 'Q' && p != 'R' && p != 'B' && p != 'N') {
            std::cout << "❌ Invalid piece! Use: Q (Queen), R (Rook), B (Bishop), N (Knight)\n";
            return;
        }

        std::cout << "👑 Promoting to: " << p << "\n";
        chess_game.promote(std::string(1, p));
    }

    void processCommand(const std::string& input) {
        if (input.empty()) return;

        std::string command = toLowerCase(input);
        std::istringstream iss(command);
        std::string first_word;
        iss >> first_word;

        // Handle direct move input (e.g., "e2e4")
        if (isValidMove(first_word)) {
            processMove(first_word.substr(0, 2), first_word.substr(2, 2));
            return;
        }

        // Handle direct square input (e.g., "e2")
        if (isValidSquare(first_word)) {
            processShow(first_word);
            return;
        }

        // Handle single character promotion
        if (first_word.length() == 1) {
            char c = std::toupper(first_word[0]);
            if (c == 'Q' || c == 'R' || c == 'B' || c == 'N') {
                processPromotion(first_word);
                return;
            }
        }

        // Handle explicit commands
        if (first_word == "move") {
            std::string move_str;
            iss >> move_str;
            if (isValidMove(move_str)) {
                processMove(move_str.substr(0, 2), move_str.substr(2, 2));
            } else {
                std::cout << "❌ Invalid move format! Use: move e2e4\n";
            }
        }
        else if (first_word == "show") {
            std::string square;
            iss >> square;
            processShow(square);
        }
        else if (first_word == "promote") {
            std::string piece;
            iss >> piece;
            processPromotion(piece);
        }
        else if (first_word == "flip" || first_word == "rotate") {
            std::cout << "🔄 Flipping board perspective...\n";
            chess_game.changeColor();
        }
        else if (first_word == "board" || first_word == "display" || first_word == "show") {
            std::cout << "📋 Current board position:\n";
            // Board will be displayed in main loop
        }
        else if (first_word == "help" || first_word == "h" || first_word == "?") {
            printHelp();
        }
        else if (first_word == "quit" || first_word == "exit" || first_word == "q") {
            std::cout << "👋 Thanks for playing Mate++!\n";
            running = false;
        }
        else {
            std::cout << "❓ Unknown command: '" << first_word << "'\n";
            std::cout << "💡 Type 'help' to see available commands\n";
        }
    }

public:
    ChessUI() : running(true) {}

    void run() {
        printWelcome();
        printHelp();

        std::string input;
        while (running) {
            std::cout << "\n";
            chess_game.readBoard(false);
            std::cout << "\n";
            printStatus();
            std::cout << "♟️  > ";

            if (!std::getline(std::cin, input)) {
                break; // EOF or error
            }

            // Trim whitespace
            input.erase(0, input.find_first_not_of(" \t"));
            input.erase(input.find_last_not_of(" \t") + 1);

            if (!input.empty()) {
                processCommand(input);
            }
        }
    }
};

int main() {
    try {
        ChessUI ui;
        ui.run();
    } catch (const std::exception& e) {
        std::cerr << "💥 Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

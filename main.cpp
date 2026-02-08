#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <map>

using namespace std;

class Piece
{
public:
    char symbol;
    bool isWhite;

    Piece(char s = '.', bool white = true)
    {
        symbol = s;
        isWhite = white;
    }

    bool isEmpty() const
    {
        return symbol == '.';
    }
};

class ChessBoard
{
private:
    vector<vector<Piece>> board;

public:
    ChessBoard()
    {
        board = vector<vector<Piece>>(8, vector<Piece>(8));
        setupBoard();
    }

    void setupBoard()
    {
        // Clear board
        for (int r = 0; r < 8; r++)
        {
            for (int c = 0; c < 8; c++)
            {
                board[r][c] = Piece('.', true);
            }
        }

        // Pawns
        for (int c = 0; c < 8; c++)
        {
            board[1][c] = Piece('p', false);
            board[6][c] = Piece('P', true);
        }

        // Rooks
        board[0][0] = Piece('r', false);
        board[0][7] = Piece('r', false);
        board[7][0] = Piece('R', true);
        board[7][7] = Piece('R', true);

        // Knights
        board[0][1] = Piece('n', false);
        board[0][6] = Piece('n', false);
        board[7][1] = Piece('N', true);
        board[7][6] = Piece('N', true);

        // Bishops
        board[0][2] = Piece('b', false);
        board[0][5] = Piece('b', false);
        board[7][2] = Piece('B', true);
        board[7][5] = Piece('B', true);

        // Queens
        board[0][3] = Piece('q', false);
        board[7][3] = Piece('Q', true);

        // Kings
        board[0][4] = Piece('k', false);
        board[7][4] = Piece('K', true);
    }

    void display() const
    {
        cout << "\n    a b c d e f g h\n";
        cout << "   -----------------\n";

        for (int r = 0; r < 8; r++)
        {
            cout << 8 - r << " | ";
            for (int c = 0; c < 8; c++)
            {
                cout << board[r][c].symbol << " ";
            }
            cout << "| " << 8 - r << endl;
        }

        cout << "   -----------------\n";
        cout << "    a b c d e f g h\n";
    }

    bool isValidPos(int r, int c) const
    {
        return r >= 0 && r < 8 && c >= 0 && c < 8;
    }

    Piece getPiece(int r, int c) const
    {
        return board[r][c];
    }

    void setPiece(int r, int c, Piece p)
    {
        board[r][c] = p;
    }

    bool isPathClear(int fromR, int fromC, int toR, int toC) const
    {
        int rStep = (toR - fromR) == 0 ? 0 : (toR - fromR) / abs(toR - fromR);
        int cStep = (toC - fromC) == 0 ? 0 : (toC - fromC) / abs(toC - fromC);

        int r = fromR + rStep;
        int c = fromC + cStep;

        while (r != toR || c != toC)
        {
            if (!board[r][c].isEmpty())
            {
                return false;
            }
            r += rStep;
            c += cStep;
        }

        return true;
    }

    bool movePiece(int fromR, int fromC, int toR, int toC)
    {
        Piece fromPiece = board[fromR][fromC];
        board[toR][toC] = fromPiece;
        board[fromR][fromC] = Piece('.', true);
        return true;
    }

    void saveToFile(const string &filename)
    {
        ofstream file(filename);
        if (!file)
        {
            cout << "Error saving file.\n";
            return;
        }

        for (int r = 0; r < 8; r++)
        {
            for (int c = 0; c < 8; c++)
            {
                file << board[r][c].symbol;
            }
            file << endl;
        }

        file.close();
        cout << "Game saved to " << filename << endl;
    }

    void loadFromFile(const string &filename)
    {
        ifstream file(filename);
        if (!file)
        {
            cout << "Error loading file.\n";
            return;
        }

        string line;
        for (int r = 0; r < 8; r++)
        {
            getline(file, line);
            for (int c = 0; c < 8; c++)
            {
                char sym = line[c];
                bool white = isupper(sym);
                board[r][c] = Piece(sym, white);
            }
        }

        file.close();
        cout << "Game loaded from " << filename << endl;
    }
};

class ChessGame
{
private:
    ChessBoard board;
    bool whiteTurn;

    map<char, string> pieceNames;

public:
    ChessGame()
    {
        whiteTurn = true;

        pieceNames['P'] = "Pawn";
        pieceNames['R'] = "Rook";
        pieceNames['N'] = "Knight";
        pieceNames['B'] = "Bishop";
        pieceNames['Q'] = "Queen";
        pieceNames['K'] = "King";

        pieceNames['p'] = "Pawn";
        pieceNames['r'] = "Rook";
        pieceNames['n'] = "Knight";
        pieceNames['b'] = "Bishop";
        pieceNames['q'] = "Queen";
        pieceNames['k'] = "King";
    }

    bool isWhitePiece(const Piece &p)
    {
        return !p.isEmpty() && isupper(p.symbol);
    }

    bool isBlackPiece(const Piece &p)
    {
        return !p.isEmpty() && islower(p.symbol);
    }

    bool isCurrentPlayerPiece(const Piece &p)
    {
        if (whiteTurn)
            return isWhitePiece(p);
        return isBlackPiece(p);
    }

    bool isEnemyPiece(const Piece &p)
    {
        if (p.isEmpty())
            return false;
        if (whiteTurn)
            return isBlackPiece(p);
        return isWhitePiece(p);
    }

    bool parseMove(const string &input, int &fromR, int &fromC, int &toR, int &toC)
    {
        if (input.length() != 5 || input[2] != ' ')
            return false;

        char fromFile = input[0];
        char fromRank = input[1];
        char toFile = input[3];
        char toRank = input[4];

        if (fromFile < 'a' || fromFile > 'h')
            return false;
        if (toFile < 'a' || toFile > 'h')
            return false;
        if (fromRank < '1' || fromRank > '8')
            return false;
        if (toRank < '1' || toRank > '8')
            return false;

        fromC = fromFile - 'a';
        toC = toFile - 'a';

        fromR = 8 - (fromRank - '0');
        toR = 8 - (toRank - '0');

        return true;
    }

    bool validatePawnMove(int fromR, int fromC, int toR, int toC, bool isWhite)
    {
        int direction = isWhite ? -1 : 1;
        Piece target = board.getPiece(toR, toC);

        // Forward move
        if (fromC == toC && target.isEmpty())
        {
            if (toR == fromR + direction)
                return true;

            // First move can go 2 squares
            if (isWhite && fromR == 6 && toR == 4 && board.getPiece(5, fromC).isEmpty())
                return true;
            if (!isWhite && fromR == 1 && toR == 3 && board.getPiece(2, fromC).isEmpty())
                return true;
        }

        // Capture diagonally
        if (abs(toC - fromC) == 1 && toR == fromR + direction)
        {
            if (!target.isEmpty() && isEnemyPiece(target))
                return true;
        }

        return false;
    }

    bool validateRookMove(int fromR, int fromC, int toR, int toC)
    {
        if (fromR != toR && fromC != toC)
            return false;
        return board.isPathClear(fromR, fromC, toR, toC);
    }

    bool validateBishopMove(int fromR, int fromC, int toR, int toC)
    {
        if (abs(toR - fromR) != abs(toC - fromC))
            return false;
        return board.isPathClear(fromR, fromC, toR, toC);
    }

    bool validateQueenMove(int fromR, int fromC, int toR, int toC)
    {
        bool rookLike = validateRookMove(fromR, fromC, toR, toC);
        bool bishopLike = validateBishopMove(fromR, fromC, toR, toC);
        return rookLike || bishopLike;
    }

    bool validateKnightMove(int fromR, int fromC, int toR, int toC)
    {
        int rDiff = abs(toR - fromR);
        int cDiff = abs(toC - fromC);
        return (rDiff == 2 && cDiff == 1) || (rDiff == 1 && cDiff == 2);
    }

    bool validateKingMove(int fromR, int fromC, int toR, int toC)
    {
        int rDiff = abs(toR - fromR);
        int cDiff = abs(toC - fromC);
        return rDiff <= 1 && cDiff <= 1;
    }

    bool validateMove(int fromR, int fromC, int toR, int toC)
    {
        if (!board.isValidPos(fromR, fromC) || !board.isValidPos(toR, toC))
            return false;

        Piece piece = board.getPiece(fromR, fromC);
        Piece target = board.getPiece(toR, toC);

        if (piece.isEmpty())
        {
            cout << "No piece in that position.\n";
            return false;
        }

        if (!isCurrentPlayerPiece(piece))
        {
            cout << "That is not your piece.\n";
            return false;
        }

        if (!target.isEmpty() && isCurrentPlayerPiece(target))
        {
            cout << "You cannot capture your own piece.\n";
            return false;
        }

        char sym = toupper(piece.symbol);

        switch (sym)
        {
        case 'P':
            return validatePawnMove(fromR, fromC, toR, toC, isWhitePiece(piece));
        case 'R':
            return validateRookMove(fromR, fromC, toR, toC);
        case 'B':
            return validateBishopMove(fromR, fromC, toR, toC);
        case 'Q':
            return validateQueenMove(fromR, fromC, toR, toC);
        case 'N':
            return validateKnightMove(fromR, fromC, toR, toC);
        case 'K':
            return validateKingMove(fromR, fromC, toR, toC);
        default:
            return false;
        }
    }

    bool checkKingCaptured()
    {
        bool whiteKing = false;
        bool blackKing = false;

        for (int r = 0; r < 8; r++)
        {
            for (int c = 0; c < 8; c++)
            {
                Piece p = board.getPiece(r, c);
                if (p.symbol == 'K')
                    whiteKing = true;
                if (p.symbol == 'k')
                    blackKing = true;
            }
        }

        if (!whiteKing)
        {
            cout << "\nBlack wins! White king captured.\n";
            return true;
        }
        if (!blackKing)
        {
            cout << "\nWhite wins! Black king captured.\n";
            return true;
        }

        return false;
    }

    void printInstructions()
    {
        cout << "\n=== Simplified Chess Game (C++) ===\n";
        cout << "Enter moves like: e2 e4\n\n";

        cout << "Commands:\n";
        cout << "  save   -> save game to file\n";
        cout << "  load   -> load game from file\n";
        cout << "  quit   -> exit game\n\n";

        cout << "Piece Symbols:\n";
        cout << "  White pieces are UPPERCASE\n";
        cout << "  Black pieces are lowercase\n\n";

        cout << "  P/p = Pawn\n";
        cout << "  R/r = Rook\n";
        cout << "  N/n = Knight\n";
        cout << "  B/b = Bishop\n";
        cout << "  Q/q = Queen\n";
        cout << "  K/k = King\n\n";

        cout << "Goal: capture the enemy king.\n\n";
    }

    void play()
    {
        printInstructions();

        while (true)
        {
            board.display();

            cout << (whiteTurn ? "\nWhite's turn: " : "\nBlack's turn: ");

            string input;
            getline(cin, input);

            if (input == "")
                continue;

            if (input == "quit")
            {
                cout << "Game ended.\n";
                break;
            }

            if (input == "save")
            {
                board.saveToFile("chess_save.txt");
                continue;
            }

            if (input == "load")
            {
                board.loadFromFile("chess_save.txt");
                continue;
            }

            int fromR, fromC, toR, toC;

            if (!parseMove(input, fromR, fromC, toR, toC))
            {
                cout << "Invalid format. Use: e2 e4\n";
                continue;
            }

            if (!validateMove(fromR, fromC, toR, toC))
            {
                cout << "Invalid move.\n";
                continue;
            }

            Piece target = board.getPiece(toR, toC);

            if (!target.isEmpty() && isEnemyPiece(target))
            {
                cout << "Captured " << pieceNames[target.symbol] << "!\n";
            }

            board.movePiece(fromR, fromC, toR, toC);

            if (checkKingCaptured())
            {
                board.display();
                break;
            }

            whiteTurn = !whiteTurn;
        }
    }
};

int main()
{
    ChessGame game;
    game.play();
    return 0;
}

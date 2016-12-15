#ifndef CHESS_HPP
#define CHESS_HPP

#include <vector>

/**
 * Helper class to store coordinates in a readable way
 */
struct Coord {
	int x,y; //both in the range [0,7]

	/**
	 * Default constructor. Initializes (x,y) to (0,0)
	 */
	Coord() { x = 0; y = 0; }

	/**
	 * Initialization constructor
	 *
	 * \param _x The x coordinate
	 * \param y The y coordinate
	 */
	Coord(int _x, int _y) { x = _x; _y = y; }
};

/**
 * Basic enum for representing each type of piece
 */
enum Piece {
	Empty,
	Pawn,
	Rook,
	Knight,
	Bishop,
	Queen,
	King
};

/**
 * Enum for representing the color of the pieces
 */
enum Color {
	White = 0,
	Black = 1,
	None = -1
};

/**
 * Container class for storing pieces. This is kept separate from Game so temporary positions can be created to help determine if moves are legal
 */
struct Board {
	Piece pieces[8][8];

	/**
	 * Default constructor. Sets the board to the starting position
	 */
	Board();

	/**
	 * Copy constructor. Copies the contents of the given board into this one
	 *
	 * \param b The board to copy
	 */
	Board(const Board& b);
};

/**
 * Board class for storing the position and determining legal moves
 */
class Game {
	Board pieces[2]; //pieces[White] are the white pieces, pieces[Black] are the black pieces. Each side's array will have their pieces at the bottom, the reason for this is so the functions that determine moves don't have to be rewritten for each color
	Coord enPassant[2]; //if a pawn is double pushed, set these 2 positions to the squares next to the pawn. They will be used the next turn to determine if en passant is legal. First index is color
    bool rookMoved[2][2]; //use these for castling. The first index is the color, the second index is which rook (A = 0, H = 1)
    bool kingMoved[2]; //also use this for castling. Index is color

    /**
     * Determines if the given coordinate is on the board
     *
     * \param pos The position to check
     * \return True if the square is on the board, false otherwise
     */
	bool onBoard(Coord pos);

	/**
	 * Determines if a square is occupied. This function is a helper function for getControlledSquares
	 *
	 * \param white The white pieces
	 * \param black The black pieces
	 * \param pos The square to check for pieces
	 * \return The color of the piece on the square, or None if empty or if the position is outside of the board
	 */
	Color squareOccupied(Board& white, Board& black, Coord pos);

	/**
	 * Determines all of the squares that the given piece controls. This includes empty squares as well as enemy occupied squares that they can capture on. Note, not all moves may be legal, this function does not take into account pins or check
	 *
	 * \param white The white pieces
	 * \param black The black pieces
	 * \param color The color of the piece to use
	 * \param pos The position of the piece. The type of piece will be determined by what is on that square
	 * \return A vector containing all squares that the piece controls
	 */
    std::vector<Coord> getControlledSquares(Board& white, Board& black, Color color, Coord pos);

    /**
     * Determines if the given side is in check or not. This function takes a Board argument so that temporary positions can be created to easily check for pins
     *
     * \param white The white pieces
     * \param black The black pieces
     * \param color The color to test for being in check
     * \return True if the side is in check, false otherwise
     */
	bool inCheck(Board& white, Board& black, Color color);

public:
	/**
	 * Default constructor. Initializes the en passant squares and the castling variables
	 */
	Game();

	/**
	 * Determines if a move is legal or not. Handles all special cases like castling, en passant, double/single pawn push, check, etc
	 *
	 * \param color The color of the piece to check for
	 * \param oPos The current position of the piece
	 * \param nPos The position to move the piece to
	 * \return True if the move is legal, false otherwise
	 */
	bool moveIsLegal(Color color, Coord oPos, Coord nPos);

	/**
	 * Makes a move on the board if it is legal
	 *
	 * \param color The color of the piece to move
	 * \param oPos The current square of the piece
	 * \param nPos The square to move to
	 * \param promotion If the move is a pawn making it to the back rank it will become this piece
	 * \return True if the piece was moved, false otherwise
	 */
	bool makeMove(Color color, Coord oPos, Coord nPos, Piece promotion = Empty);
};

#endif // CHESS_HPP

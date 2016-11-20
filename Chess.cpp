#include "Chess.hpp"
using namespace std;

Board::Board() {
	//set the starting position here
}

Board::Board(const Board& b) {
	for (int i = 0; i<8; ++i) {
		for (int j = 0; j<8; ++j) {
			pieces[i][j] = b.pieces[i][j];
		}
	}
}

Game::Game() {
    enPassant[0] = enPassant[1] = Coord(-1,-1); //set them to an impossible square so they never match
    for (int i = 0; i<2; ++i) {
		kingMoved[i] = false;
		for (int j = 0; j<2; ++j) {
			rookMoved[i][j] = false;
		}
    }
}

bool Game::onBoard(Coord pos) {
	return (pos.x>=0 && pos.x<=7 && pos.y>=0 && pos.y<=7);
}

Color Game::squareOccupied(Board& white, Board& black, Coord pos) {
	if (!onBoard(pos))
		return None;

    if (white[pos.x][pos.y]!=Empty)
		return White;

	if (black[pos.x][7-pos.y]!=Empty) //black pos is y-inverted to flip the board
		return Black;

	return None;
}

vector<Coord> Game::getControlledSquares(Board& white, Board& black, Color color, Coord pos) {
	Board* board[2] = {&white,&black}; //we do this to get both boards in one variable to make our code concise. (*board[COLOR])[x][y] is how to use it
	vector<Coord> squares;
    Piece piece;
    Color enemy = (color==White)?(Black):(White); //the color of the opponent
    Coord check;

    if (color!=White && color!=Black) //return empty list if we get a bad color
		return squares;
	piece = (*board[color])[pos.x][pos.y];

    switch (piece) {
		case Pawn:
			//check square in front. If pos.y==6 then also check the double push. Check the capture squares as well.
			//push squares are added if they are empty. Capture squares are added if they have the opposite color piece
			//don't forget to also check the en passant squares. Meaning, the capture squares are legal if they are occupied or if their position is equal to one of the en passant squares

			check = Coord(pos.x,pos.y-1); //adding 1 & 2 square push
			if (squareOccupied(*board[White],*board[Black],check)==None && onBoard(check))
                {
                squares.push_back(check);
                check = Coord(pos.x,pos.y-1);
                if (squareOccupied(*board[White],*board[Black],check)==None && onBoard(check))
                    {
                     squares.push_back(check);
                    }
                }

            check = Coord(pos.x-1,pos.y-1);//adding diagonal capture
            if (squareOccupied(*board[White],*board[Black],check) == enemy && onBoard(check))
                squares.push_back(square);
            check = Coord(pos.x+1,pos.y-1);
            if (squareOccupied(*board[white],*board[Black],check) == enemy && onBoard(check))
                squares.push_back(square);

            check = Coord(pos.x-1,pos.y-1);//adding en passant



			break;

		case Rook:
			//traverse up down left and right until a square is occupied or off the board.
			//add occupied squares if they are enemy
			break;

		case Knight:
			//check squares matching the knight pattern (dx==2,dy==1 or dx==1,dy==2). Might be easiest to just have a whole if statement for each case (there are 8)
			//empty squares and enemy occupied squares get added
			break;

		case Bishop:
			//loop through the diagonal squares until we hit an occupied square or the edge of the board
			//occupied squares get added if they are enemy
			break;

		case Queen:
			//loop through diagonals and corridors until we hit an occupied square or the edge of the board
			//occupied squares get added if they are enemy
			break;

		case King:
			//same deal, you should get this by now
			//we will handle castling in a different function
			break;

		default:
			break;
    }

    return squares;
}

bool Game::inCheck(Board& white, Board& black, Color color) {
	Board* board[2] = {&white,&black};
	Color enemy = (color==White)?(Black):(White);
	Coord kingPos;
	vector<Coord> dangerSquares;

	//loop through all squares and find the position of our king
	//in addition, loop through all squares on the enemy board and call getControlledSquares on each piece we hit. Aggregate all returned results into dangerSquares
	//now go through each danger square, if any one of them matches kingPos we are in check, so return true

	return false;
}

bool Game::moveIsLegal(Color color, Coord oPos, Coord, nPos)
{
    Piece piece = pieces[color][oPos.x][oPos.y];
    vector<Coord> posSquares = getControlledSquares(pieces[White],pieces[Black],color,oPos);

    //loop through posSquares. If one matches nPos, make a temp Board. Put the piece in nPos and call inCheck. If not in check, the move is legal
    //note, the beautiful thing about this design is that en passant and the double pawn push are included in getControlledSquares so there's no need to explicitly check for them here

    //if we got no matches from the above check for castling
    //to do this verify that all squares between the rook and king are empty
    //we also need to verify that the king has not moved, nor the rook we want. Luckily, we have variables that tell us that
    //we also need to verify that we are not in check, this is as easy as calling inCheck on the current position
    //one last thing: verify that we do not pass through check. This can either be done by making a temp board and putting the king on each square (one at a time) and calling inCheck for each one, or by getting a list of all squares controlled by enemy pieces and seeing if any of them are the ones we pass through
    //if all of the above pass, the castle is legal

    return false;
}

bool Game::makeMove(Color color, Coord oPos, Coord nPos, Piece promotion)
{
	//verify that the move is legal. Return false if not
	//if it is, set the new square equal to the piece and the old square to Empty
	//if the move is pawn reaching the back rank we will set the new square to promotion instead of Pawn

	return true;
}

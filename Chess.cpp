#include "Chess.hpp"
using namespace std;

Board::Board()
{
    for (int i = 0; i<8; ++i)
        for (int j = 0; j<8; ++j)
            pieces[i][j] = Empty;
    for (int i = 0; i<8; ++i)
        pieces[i][1] = Pawn;

    pieces[0][0] = Rook;
    pieces[7][0] = Rook;
    pieces[1][0] = Knight;
    pieces[6][0] = Knight;
    pieces[2][0] = Bishop;
    pieces[5][0] = Bishop;
    pieces[3][0] = Queen;
    pieces[4][0] = King;
}

Board::Board(const Board& b) {
	for (int i = 0; i<8; ++i) {
		for (int j = 0; j<8; ++j) {
			pieces[i][j] = b.pieces[i][j];
		}
	}
}

Game::Game() {
    for (int i = 0; i<2; ++i) {
		kingMoved[i] = false;
		enPassant[i] = Coord(-1,-1); //set them to an impossible square so they never match
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

    if (white.pieces[pos.x][pos.y]!=Empty)
		return White;

	if (black.pieces[pos.x][7-pos.y]!=Empty) //black pos is y-inverted to flip the board
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
	piece = (*board[color]).pieces[pos.x][pos.y];

    switch (piece) {
		case Pawn:

			check = Coord(pos.x,pos.y-1); //adding 1 & 2 square push
			if (squareOccupied(*board[White],*board[Black],check)==None && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x,pos.y-2);
            if (squareOccupied(*board[White],*board[Black],check)==None && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x-1,pos.y-1);//capture diagonal & EnPassant capture
            if (squareOccupied(*board[White],*board[Black],check) == enemy && onBoard(check))
                squares.push_back(check);
            if (check==enPassant[color])
                squares.push_back(check);

            check = Coord(pos.x+1,pos.y-1);
            if (squareOccupied(*board[White],*board[Black],check) == enemy && onBoard(check))
                squares.push_back(check);
            if (check==enPassant[color])
                squares.push_back(check);

			break;

		case Knight:
			check = Coord(pos.x+1,pos.y-2);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x-1,pos.y-2);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x+1,pos.y+2);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x-1,pos.y+2);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x-2,pos.y-1);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x+2,pos.y-1);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x-2,pos.y+1);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x+2,pos.y+1);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);
			break;

		case Queen:
			//fall through into rook and bishop

        case Rook:
			for (check = pos; onBoard(check); ++check.x)
                {
                if (squareOccupied(*board[White],*board[Black],check)==None)
                    squares.push_back(check);
                if (squareOccupied(*board[White],*board[Black],check)==color)
                    break;
                if (squareOccupied(*board[White],*board[Black],check)==enemy)
                    squares.push_back(check);
                    break;
                }

			for (check = pos; onBoard(check); --check.x)
                {
                if (squareOccupied(*board[White],*board[Black],check)==None)
                    squares.push_back(check);
                if (squareOccupied(*board[White],*board[Black],check)==color)
                    break;
                if (squareOccupied(*board[White],*board[Black],check)==enemy)
                    squares.push_back(check);
                    break;
                }
			for (check = pos; onBoard(check); --check.y)
                {
                if (squareOccupied(*board[White],*board[Black],check)==None)
                    squares.push_back(check);
                if (squareOccupied(*board[White],*board[Black],check)==color)
                    break;
                if (squareOccupied(*board[White],*board[Black],check)==enemy)
                    squares.push_back(check);
                    break;
                }
            for (check = pos; onBoard(check); ++check.y)
                {
                if (squareOccupied(*board[White],*board[Black],check)==None)
                    squares.push_back(check);
                if (squareOccupied(*board[White],*board[Black],check)==color)
                    break;
                if (squareOccupied(*board[White],*board[Black],check)==enemy)
                    squares.push_back(check);
                    break;
                }
			if (piece==Rook)
                break;

        case Bishop:

            check = Coord(pos.x,pos.y);
            while (onBoard(check))
            {
                if (squareOccupied(*board[White],*board[Black],check)==None)
                    squares.push_back(check);
                if (squareOccupied(*board[White],*board[Black],check)==color)
                    break;
                if (squareOccupied(*board[White],*board[Black],check)==enemy)
                    squares.push_back(check);
                    break;
                check.x++;
                check.y++;
            }
			break;

            check = Coord(pos.x,pos.y);
            while (onBoard(check))
            {
                if (squareOccupied(*board[White],*board[Black],check)==None)
                    squares.push_back(check);
                if (squareOccupied(*board[White],*board[Black],check)==color)
                    break;
                if (squareOccupied(*board[White],*board[Black],check)==enemy)
                    squares.push_back(check);
                    break;
                check.x--;
                check.y++;
            }
			break;

            check = Coord(pos.x,pos.y);
            while (onBoard(check))
            {
                if (squareOccupied(*board[White],*board[Black],check)==None)
                    squares.push_back(check);
                if (squareOccupied(*board[White],*board[Black],check)==color)
                    break;
                if (squareOccupied(*board[White],*board[Black],check)==enemy)
                    squares.push_back(check);
                    break;
                check.x--;
                check.y--;
            }
			break;

            check = Coord(pos.x,pos.y);
            while (onBoard(check))
            {
                if (squareOccupied(*board[White],*board[Black],check)==None)
                    squares.push_back(check);
                if (squareOccupied(*board[White],*board[Black],check)==color)
                    break;
                if (squareOccupied(*board[White],*board[Black],check)==enemy)
                    squares.push_back(check);
                    break;
                check.x--;
                check.y--;
            }
			break;

		case King:
            check = Coord(pos.x+1,pos.y);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x-1,pos.y);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x,pos.y+1);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x,pos.y-1);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x+1,pos.y+1);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x+1,pos.y-1);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x-1,pos.y+1);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

            check = Coord(pos.x-1,pos.y-1);
            if (squareOccupied(*board[White],*board[Black],check) != color && onBoard(check))
                squares.push_back(check);

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

    for (int i = 0; i<8; ++i) //Finding king's Position
    {
        for (int j = 0; j<8; ++j)
        {
            if (pieces[i][j] == King)
            {
                kingPos = Coord(i,j);
                goto exit;
            }
        }
    }
    exit;

    for (int i = 0; i<8; ++i) //Finding Opponent controlled squares
    {
        for (int j = 0; j<8; ++j)
        {
            if (pieces[i][j] != Empty )
            {
                 vector<Coord> temp = getControlledSquares(white,black,enemy,Coord(i,j));
                 for (int k = 0; k<temp.size(); ++k)
                    dangerSquares.push_back(temp[k]);
            }
        }
     }

    for (int k = 0; k<temp.size(); ++k)
    {
        if (dangerSquares[k] == kingPos )
        {
            return true;
            break;
        }
    }
	return false;
}

bool Game::moveIsLegal(Color color, Coord oPos, Coord nPos)
{
    Piece piece = pieces[color].pieces[oPos.x][oPos.y];
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

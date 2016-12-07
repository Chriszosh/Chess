#include "Chess.hpp"
using namespace std;

///Standard: all positions passed are from white's perspective. For black's board the positions are y inverted but only for accessing those pieces, they remain in the standard form everywhere else

Board::Board() {
	for (int i = 0; i<8; ++i)
		for (int j = 0; j<6; ++j)
			pieces[i][j] = Empty;
	for (int i = 0; i<8; ++i)
		pieces[i][6] = Pawn;
	pieces[0][7] = pieces [7][7] = Rook;
	pieces[1][7] = pieces[6][7] = Knight;
	pieces[2][7] = pieces[5][7] = Bishop;
	pieces[3][7] = Queen;
	pieces[4][7] = King;
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
		enPassant[i] = Coord(-1,-1);
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
    Color enemy = (color==White)?(Black):(White); //the color of the opponent
    int upDir = (color==White)?(-1):(1); //add this to y to move 'up' on that board

    if (color!=White && color!=Black) //return empty list if we get a bad color
		return squares;
	Piece piece = (color==White)?((*board[color]).pieces[pos.x][pos.y]):((*board[color]).pieces[pos.x][7-pos.y]); //reverse black to actually get piece

	Coord check, dif;
	Color tCol;
	int i,j,k;
    switch (piece) {
		case Pawn:
			//push 1
			check = Coord(pos.x,pos.y+upDir);
			if (squareOccupied(*board[White],*board[Black],check)==None && onBoard(check))
                squares.push_back(check);

			//push 2
            check = Coord(pos.x,pos.y+2*upDir);
            if (squareOccupied(*board[White],*board[Black],check)==None && onBoard(check) && ((color==White && pos.y==6) || (color==Black && pos.y==1)))
                squares.push_back(check);

			//capture left or en passant
            check = Coord(pos.x-1,pos.y+upDir);
            if (squareOccupied(*board[White],*board[Black],check)==enemy && onBoard(check))
                squares.push_back(check);
			else if (check==enPassant[enemy] && onBoard(check) && squareOccupied(*board[White],*board[Black],check)==None)
				squares.push_back(check);

			//capture right or en passant
            check = Coord(pos.x+1,pos.y+upDir);
            if (squareOccupied(*board[White],*board[Black],check)==enemy && onBoard(check))
                squares.push_back(check);
			else if (check==enPassant[enemy] && onBoard(check) && squareOccupied(*board[White],*board[Black],check)==None)
				squares.push_back(check);

			break;

		case Knight:
			for (i = -1; i<=1; i+=2) {
				for (j = -1; j<=1; j+=2) {
					dif = Coord(1*i,2*j);
					for (k = 0; k<2; ++k) {
						check = pos+dif;
						if (onBoard(check) && squareOccupied(white,black,check)!=color)
							squares.push_back(check);
						swap(dif.x,dif.y);
					}
				}
			}
			break;

		case Queen:
			//fall through into bishop and rook

		case Bishop:
			//up left
			for (i = pos.x-1; i>=0; --i) {
				for (j = pos.y-1; j>=0; --j) {
                    tCol = squareOccupied(white,black,Coord(i,j));
                    if (tCol==color)
						goto doneUR;
					squares.push_back(Coord(i,j));
					if (tCol==enemy)
						goto doneUL;
				}
			}
			doneUL:

			//up right
			for (i = pos.x+1; i<8; ++i) {
				for (j = pos.y-1; j>=0; --j) {
                    tCol = squareOccupied(white,black,Coord(i,j));
                    if (tCol==color)
						goto doneUR;
					squares.push_back(Coord(i,j));
					if (tCol==enemy)
						goto doneUR;
				}
			}
			doneUR:

			//down left
			for (i = pos.x-1; i>=0; --i) {
				for (j = pos.y+1; j<8; ++j) {
                    tCol = squareOccupied(white,black,Coord(i,j));
                    if (tCol==color)
						goto doneUR;
					squares.push_back(Coord(i,j));
					if (tCol==enemy)
						goto doneDL;
				}
			}
			doneDL:

			//down right
			for (i = pos.x+1; i<8; ++i) {
				for (j = pos.y+1; j<8; ++j) {
                    tCol = squareOccupied(white,black,Coord(i,j));
                    if (tCol==color)
						goto doneUR;
					squares.push_back(Coord(i,j));
					if (tCol==enemy)
						goto doneDR;
				}
			}
			doneDR:

			if (piece==Bishop) //not a queen
				break;

		case Rook:
			//go left
			for (i = pos.x-1; i>=0; --i) {
				tCol = squareOccupied(white,black,Coord(i,pos.y));
				if (tCol==color)
					break;
				squares.push_back(Coord(i,pos.y));
				if (tCol==enemy)
					break;
			}

			//go right
			for (i = pos.x+1; i<8; ++i) {
				tCol = squareOccupied(white,black,Coord(i,pos.y));
				if (tCol==color)
					break;
				squares.push_back(Coord(i,pos.y));
				if (tCol==enemy)
					break;
			}

			//go up
			for (i = pos.y-1; i>=0; --i) {
				tCol = squareOccupied(white,black,Coord(i,pos.y));
				if (tCol==color)
					break;
				squares.push_back(Coord(i,pos.y));
				if (tCol==enemy)
					break;
			}

			//go down
			for (i = pos.y+1; i<8; ++i) {
				tCol = squareOccupied(white,black,Coord(i,pos.y));
				if (tCol==color)
					break;
				squares.push_back(Coord(i,pos.y));
				if (tCol==enemy)
					break;
			}
			break;

		case King:
			for (i = -1; i<=1; ++i) {
				for (j = -1; j<=1; ++j) {
                    if (onBoard(Coord(i,j))) {
						if (squareOccupied(white,black,Coord(i,j))!=color)
							squares.push_back(Coord(i,j));
                    }
				}
			}
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

	//find king, convert position to white perspective if black
	for (int i = 0; i<8; ++i) {
		for (int j = 0; j<8; ++j) {
			if ((*board[color]).pieces[i][j]==King) {
				kingPos = Coord(i,(color==White)?(j):(7-j));
				goto kingFound;
			}
		}
	}
	kingFound:

	//go through enemy controlled squares and determine if any are our king
	for (int i = 0; i<8; ++i) {
        for (int j = 0; j<8; ++j) {
			Coord pos(i,j);
			if (squareOccupied(white,black,pos)==enemy) {
				dangerSquares = getControlledSquares(white,black,enemy,pos);
				for (unsigned int k = 0; k<dangerSquares.size(); ++k) {
					if (dangerSquares[i]==kingPos)
						return true;
				}
			}
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

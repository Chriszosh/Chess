#include <cmath>
#include <iostream>
#include "Chess.hpp"
using namespace std;

///Standard: all positions passed are from white's perspective. For black's board the positions are y inverted but only for accessing those pieces, they remain in the standard form everywhere else

const int Chess::weights[7] = {0,1,5,3,3,9,50};

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

Chess::Chess() {
    for (int i = 0; i<2; ++i) {
		kingMoved[i] = false;
		enPassant[i] = Coord(-1,-1);
		for (int j = 0; j<2; ++j) {
			rookMoved[i][j] = false;
		}
    }
}

bool Chess::onBoard(Coord pos) {
	return (pos.x>=0 && pos.x<=7 && pos.y>=0 && pos.y<=7);
}

Color Chess::squareOccupied(Board& white, Board& black, Coord pos) {
	if (!onBoard(pos))
		return None;

    if (white.pieces[pos.x][pos.y]!=Empty)
		return White;

	if (black.pieces[pos.x][7-pos.y]!=Empty) //black pos is y-inverted to flip the board
		return Black;

	return None;
}

vector<Coord> Chess::getControlledSquares(Board& white, Board& black, Color color, Coord pos) {
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
			if (squareOccupied(*board[White],*board[Black],check)==None && onBoard(check)) {
                squares.push_back(check);
                //push 2
				check = Coord(pos.x,pos.y+2*upDir);
				if (squareOccupied(*board[White],*board[Black],check)==None && onBoard(check) && ((color==White && pos.y==6) || (color==Black && pos.y==1)))
					squares.push_back(check);
			}

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
			for (i = -1; i<=1; i+=2) {
				for (int j = -1; j<=1; j+=2) {
					dif = Coord(i,j);
					check = pos+dif;
                    while (onBoard(check)) {
						tCol = squareOccupied(white,black,check);
						if (tCol==color)
							break;
                        squares.push_back(check);
                        if (tCol==enemy)
							break;
						check = check + dif;
                    }
				}
			}

			if (piece==Bishop) //not a queen
				break;

		case Rook:
			for (i = -1; i<=1; i+=2) {
				dif = Coord(0,i);
				for (j = 0; j<2; ++j) {
					check = pos + dif;
                    while (onBoard(check)) {
						tCol = squareOccupied(white,black,check);
						if (tCol==color)
							break;
						squares.push_back(check);
						if (tCol==enemy)
							break;
						check = check + dif;
                    }
                    swap(dif.x,dif.y);
				}
			}

			break;

		case King:
			for (i = -1; i<=1; ++i) {
				for (j = -1; j<=1; ++j) {
					check = pos+Coord(i,j);
                    if (onBoard(check)) {
						if (squareOccupied(white,black,check)!=color)
							squares.push_back(check);
                    }
				}
			}
			break;

		default:
			break;
    }

    return squares;
}

bool Chess::inCheck(Board& white, Board& black, Color color) {
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
					if (dangerSquares[k]==kingPos)
						return true;
				}
			}
        }
	}

	return false;
}

bool Chess::moveIsLegal(Color color, Coord oPos, Coord nPos)
{
	Color enemy = (color==White)?(Black):(White);
    Piece piece = pieces[color].pieces[oPos.x][(color==White)?(oPos.y):(7-oPos.y)];
    vector<Coord> posSquares = getControlledSquares(pieces[White],pieces[Black],color,oPos);

    //see if a piece is moving to a valid square and that the new position isn't check
    for (unsigned int i = 0; i<posSquares.size(); ++i) {
		if (posSquares[i]==nPos) {
			Board tB = pieces[color];
			Board tE = pieces[enemy];
			tB.pieces[oPos.x][(color==White)?(oPos.y):(7-oPos.y)] = Empty;
			tE.pieces[nPos.x][(enemy==White)?(nPos.y):(7-nPos.y)] = Empty;
			tB.pieces[nPos.x][(color==White)?(nPos.y):(7-nPos.y)] = piece;
			if (piece==Pawn && nPos==enPassant[enemy])
                tE.pieces[nPos.x][4] = Empty;
			Board* boards[2] = {&tB,&tE};
			if (color==Black)
				swap(boards[0],boards[1]);
			return !inCheck(*boards[White],*boards[Black],color);
		}
    }

    //castling
    if (((color==White && oPos==Coord(4,7)) || (color==Black && oPos==Coord(4,0))) && piece==King) {
    	//verify actually castling and not just moving
    	if ((nPos.x!=2 && nPos.x!=6) || nPos.y!=oPos.y)
			return false;

		//check not in check
		if (inCheck(pieces[White],pieces[Black],color))
			return false;

		//check king moved
		if (kingMoved[color])
			return false;

		//queenside
        if (nPos.x==2) {
			//check rook moved
			if (rookMoved[color][0])
				return false;

			//check all squares free between king/rook
			for (int i = 1; i<4; ++i) {
				if (squareOccupied(pieces[White],pieces[Black],Coord(i,oPos.y))!=None)
					return false;
			}

			//check not moving through check
			for (int i = 3; i>=2; --i) {
				Board tB = pieces[color];
				Board* boards[2] = {&tB,&pieces[enemy]};
				if (color==Black)
					swap(boards[0],boards[1]);
				tB.pieces[oPos.x][(color==White)?(oPos.y):(7-oPos.y)] = Empty;
				tB.pieces[i][(color==White)?(oPos.y):(7-oPos.y)] = King;
				if (inCheck(*boards[White],*boards[Black],color))
					return false;
			}

			//we made it
			return true;
        }

        //kingside
        else if (nPos.x==6) {
			//check rook moved
			if (rookMoved[color][1])
				return false;

			//check all squares free between king/rook
			for (int i = 5; i<6; ++i) {
				if (squareOccupied(pieces[White],pieces[Black],Coord(i,oPos.y))!=None)
					return false;
			}

			//check not moving through check
			for (int i = 5; i<=7; ++i) {
				Board tB = pieces[color];
				Board* boards[2] = {&tB,&pieces[enemy]};
				if (color==Black)
					swap(boards[0],boards[1]);
				tB.pieces[oPos.x][(color==White)?(oPos.y):(7-oPos.y)] = Empty;
				tB.pieces[i][(color==White)?(oPos.y):(7-oPos.y)] = King;
				if (inCheck(*boards[White],*boards[Black],color))
					return false;
			}

			//we made it
			return true;
        }
    }

    return false;
}

bool Chess::makeMove(Color color, Coord oPos, Coord nPos, Piece promotion)
{
	Color enemy = (color==White)?(Black):(White);
	Piece piece = pieces[color].pieces[oPos.x][(color==White)?(oPos.y):(7-oPos.y)];

	//make sure it's legal
	if (!moveIsLegal(color,oPos,nPos))
		return false;

	//update kingMoved
	if (piece==King) {
		kingMoved[color] = true;
		if (abs(oPos.x-nPos.x)==2) {
			if (nPos.x==6) {
				pieces[color].pieces[7][7] = Empty;
				pieces[color].pieces[5][7] = Rook;
			}
			else {
				pieces[color].pieces[3][7] = Rook;
				pieces[color].pieces[0][7] = Empty;
			}
		}
	}

	//update rookMoved
	if (piece==Rook) {
		if ((oPos==Coord(0,0) && color==Black) || (oPos==Coord(0,7) && color==White))
			rookMoved[color][0] = true;
		if ((oPos==Coord(7,0) && color==Black) || (oPos==Coord(7,7) && color==White))
			rookMoved[color][1] = true;
	}

	//set en passant square if applicable and reset enemy en passant square
	if (piece==Pawn && nPos==enPassant[enemy])
		pieces[enemy].pieces[nPos.x][4] = Empty;
	if (piece==Pawn && abs(oPos.y-nPos.y)==2)
		enPassant[color] = Coord(nPos.x,(color==White)?(nPos.y+1):(nPos.y-1));
	enPassant[enemy] = Coord(-1,-1);

	//update boards
	pieces[color].pieces[oPos.x][(color==White)?(oPos.y):(7-oPos.y)] = Empty;
	pieces[color].pieces[nPos.x][(color==White)?(nPos.y):(7-nPos.y)] = piece;
	pieces[enemy].pieces[nPos.x][(enemy==White)?(nPos.y):(7-nPos.y)] = Empty;

	//if promotion, promote
	if (piece==Pawn) {
		if ((color==White && nPos.y==0) || (color==Black && nPos.y==7))
			pieces[color].pieces[nPos.x][(color==White)?(nPos.y):(7-nPos.y)] = promotion;
	}

	return true;
}

Board Chess::getPieces(Color color)
{
	if (color==White)
		return pieces[White];

	Board ret = pieces[Black];
	for (int i = 0; i<4; ++i)
		for (int j = 0; j<8; ++j)
			swap(ret.pieces[j][i],ret.pieces[j][7-i]);
	return ret;
}

Color Chess::squareOccupied(Coord pos)
{
	return squareOccupied(pieces[White],pieces[Black],pos);
}

Piece Chess::getPiece(Coord pos, Color color)
{
	return pieces[color].pieces[pos.x][(color==White)?(pos.y):(7-pos.y)];
}

vector<Coord> Chess::getLegalMoves(Coord pos)
{
	Color color = squareOccupied(pos);
	Piece piece = pieces[color].pieces[pos.x][(color==White)?(pos.y):(7-pos.y)];
	vector<Coord> squares = getControlledSquares(pieces[White],pieces[Black],color,pos);

	for (unsigned int i = 0; i<squares.size(); ++i) {
		if (!moveIsLegal(color,pos,squares[i])) {
			squares.erase(squares.begin()+i);
			--i;
		}
	}
    if (piece==King) {
		if (moveIsLegal(color,pos,Coord(pos.x-2,pos.y)))
			squares.push_back(Coord(pos.x-2,pos.y));
		if (moveIsLegal(color,pos,Coord(pos.x+2,pos.y)))
			squares.push_back(Coord(pos.x+2,pos.y));
    }

    return squares;
}

vector<pair<Coord,Coord> > Chess::getAllMoves(Color col)
{
	vector<pair<Coord,Coord> > ret;
	vector<Coord> t;

	for (int x = 0; x<8; ++x) {
		for (int y = 0; y<8; ++y) {
			Coord c(x,y);
            if (squareOccupied(pieces[White],pieces[Black],c)==col) {
				t = getLegalMoves(c);
				for (unsigned int i = 0; i<t.size(); ++i)
					ret.push_back(make_pair(c,t[i]));
            }
		}
	}

	return ret;
}

bool Chess::inCheckmate(Color color)
{
	if (!inCheck(pieces[White],pieces[Black],color))
		return false;

    //check for any legal moves
    for (int x = 0; x<8; ++x) {
		for (int y = 0; y<8; ++y) {
			Coord t(x,y);
			if (squareOccupied(t)==color) {
				if (getLegalMoves(t).size()>0)
					return false;
			}
		}
    }

    return true;
}

bool Chess::inStalemate(Color color)
{
	if (inCheck(pieces[White],pieces[Black],color))
		return false;

    //check for any legal moves
    for (int x = 0; x<8; ++x) {
		for (int y = 0; y<8; ++y) {
			Coord t(x,y);
			if (squareOccupied(t)==color) {
				if (getLegalMoves(t).size()>0)
					return false;
			}
		}
    }

    return true;
}

int Chess::getScore()
{
	int r = 0;
	for (int x = 0; x<8; ++x)
		for (int y = 0; y<8; ++y)
			r += weights[pieces[White].pieces[x][y]] - weights[pieces[Black].pieces[x][7-y]];
	return r;
}

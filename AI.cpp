#include <cstdlib>
#include <iostream>
#include "AI.hpp"
using namespace std;

AI::AI(Game& b, Color m) : board(b)
{
	me = m;
	enemy = (me==White)?(Black):(White);
}

int AI::sumBoard(Board b)
{
	int s = 0;
	for (int x = 0; x<8; ++x) {
		for (int y = 0; y<8; ++y) {
			s += weights[b.pieces[x][y]+1];
		}
	}
	return s;
}

int AI::scoreMove(Game& b, Coord oPos, Coord nPos, int dt)
{
	Color moved = b.squareOccupied(oPos);
	Color toMove = (moved==White)?(Black):(White);
	int score = 0;
	int off = 0;

	b.makeMove(moved,oPos,nPos,Queen);
	score = (sumBoard(b.getPieces(me))-sumBoard(b.getPieces(enemy)));
	off = score;

	//do more moves if not at depth
	Coord c;
	if (dt>=0){
		for (int x = 0; x<8; ++x) {
			for (int y = 0; y<8; ++y) {
				c = Coord(x,y);
				if (b.squareOccupied(c)==toMove) {
					vector<Coord> moves = b.getLegalMoves(c);
					for (unsigned int i = 0; i<moves.size(); ++i) {
                        Game t = b;
                        int ts = scoreMove(t,c,moves[i],dt-1);
                        score += ts-off;
					}
				}
			}
		}
	}

    return score;
}

pair<Coord,Coord> AI::getMove()
{
	pair<Coord,Coord> topMove;
	int topScore = -110;
	cout << "current score: " << (sumBoard(board.getPieces(me))-sumBoard(board.getPieces(enemy))) << endl;

	for (int x = 0; x<8; ++x) {
		for (int y = 0; y<8; ++y) {
			Coord c(x,y);
			if (board.squareOccupied(c)==me) {
				Game t = board;
				vector<Coord> moves = board.getLegalMoves(c);
                for (unsigned int i = 0; i<moves.size(); ++i) {
					int s = scoreMove(t,c,moves[i],0);
					if (s>topScore || topScore==-110) {
						cout << "New topmove is (" << c.x << "," << c.y << ") to (" << moves[i].x << "," << moves[i].y << ") with score " << s << endl;
						topScore = s;
						topMove.first = c;
						topMove.second = moves[i];
					}
                }
			}
		}
	}

	cout << "move chosen\n";
	return topMove;
}

pair<Coord,Coord> AI::getRandomMove()
{
	vector<pair<Coord,vector<Coord> > > moves;
	for (int x = 0; x<8; ++x) {
		for (int y = 0; y<8; ++y) {
            if (board.squareOccupied(Coord(x,y))==me) {
				vector<Coord> t = board.getLegalMoves(Coord(x,y));
				if (t.size()>0)
					moves.push_back(make_pair(Coord(x,y),t));
            }
		}
	}
	int i = rand()%moves.size();
	int j = rand()%moves[i].second.size();
	return make_pair(moves[i].first,moves[i].second[j]);
}

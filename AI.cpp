#include <cstdlib>
#include "AI.hpp"
using namespace std;

AI::AI(Game& b, Color m) : board(b)
{
	me = m;
}

pair<Coord,Coord> AI::getMove()
{
	return make_pair(Coord(),Coord());
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

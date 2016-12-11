#include "Tree.hpp"
using namespace std;

MoveTreeNode::MoveTreeNode(Chess pos, Color toMove)
{
	data.position = pos;
	data.score = pos.getScore();
	data.toMove = toMove;
	scoreValid = true;
	isLeaf = true;
}

MoveTreeNode::~MoveTreeNode()
{
	for (unsigned int i = 0; i<moves.size(); ++i) {
		delete children[calcIndex(moves[i])];
	}
}

int MoveTreeNode::calcIndex(pair<Coord,Coord> m)
{
	return (m.first.x<<9) | (m.first.y<<6) | (m.second.x<<3) | m.second.y; //equivalent to: m.first.x*512 + m.first.y*64 + m.second.x*8 + m.second.y assuming all in range [0,8)
}

void MoveTreeNode::spawnChildren(int depth)
{
	if (depth>=0) {
		if (isLeaf) {
			isLeaf = false;
			moves = data.position.getAllMoves(data.toMove);
            for (unsigned int i = 0; i<moves.size(); ++i) {
				int h = calcIndex(moves[i]);
				Chess t = data.position;
				t.makeMove(data.toMove,moves[i].first,moves[i].second,Queen);
                children[h] = new MoveTreeNode(t,(data.toMove==White)?(Black):(White));
                children[h]->spawnChildren(depth-1);
            }
		}
		else {
			for (unsigned int i = 0; i<moves.size(); ++i) {
				children[calcIndex(moves[i])]->spawnChildren(depth-1);
			}
		}
	}
}

void MoveTreeNode::updateScore()
{
	int avg = 0;
	if (!isLeaf) {
		//update children and get average
		for (unsigned int i = 0; i<moves.size(); ++i) {
			int h = calcIndex(moves[i]);
			children[h]->updateScore();
			avg += children[h]->data.score;
		}
		avg /= moves.size();
	}
	data.score = data.position.getScore()+avg;
}

NodeData& MoveTreeNode::getData()
{
	return data;
}
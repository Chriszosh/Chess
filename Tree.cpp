#include <cmath>
#include <thread>
#include <iostream>
#include "Tree.hpp"
using namespace std;
using namespace sf;

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
		scoreValid = false;
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
	if (!isLeaf && !scoreValid) {
		//update children and get average
		for (unsigned int i = 0; i<moves.size(); ++i) {
			int h = calcIndex(moves[i]);
			children[h]->updateScore();
			avg += children[h]->data.score;
		}
		if (moves.size()>0)
			avg /= sqrt(moves.size());
		else
			avg = ((data.toMove==White)?(-1):(1))*10000;
	}
	else if (!scoreValid)
		data.score = data.position.getScore()+avg;
}

NodeData& MoveTreeNode::getData()
{
	return data;
}

MoveTree::MoveTree(Color me, int d, int nt)
{
	mult = (me==White)?(1):(-1);
	depth = d;
	root = new MoveTreeNode(Chess(),White);
	root->spawnChildren(depth);
	root->updateScore();

	numThreads = nt;
	threadIdCounter = 0;
	threads = new Thread*[nt];
	threadRanges = new int[nt*2];
	for (int i = 0; i<nt; ++i) {
		threads[i] = new Thread(&MoveTree::calcTree,this);
	}
	update();
}

MoveTree::~MoveTree()
{
	delete root;
}

void MoveTree::calcTree()
{
	lock.lock();
	int i = threadIdCounter;
	cout << "Thread " << threadIdCounter << " spawned\n";
	threadIdCounter++;
	lock.unlock();


	int mn = threadRanges[i*2];
	int mx = threadRanges[i*2+1];
	for (int i = mn; i<mx; ++i) {
		int h = root->calcIndex(root->moves[i]);
		root->children[h]->spawnChildren(depth-1);
		root->children[h]->updateScore();
	}
}

void MoveTree::update()
{
	double rng = double(root->moves.size())/double(numThreads);
	cout << "Range is: " << rng << endl;
	threadIdCounter = 0;
	for (int i = 0; i<numThreads; ++i) {
		threadRanges[i*2] = i*rng;
		threadRanges[i*2+1] = i*rng+rng;
		threads[i]->launch();
	}
	for (int i = 0; i<numThreads; ++i)
		threads[i]->wait();
	root->updateScore();
}

pair<Coord,Coord> MoveTree::getBestMove()
{
	int mI = -1, mx = -10000000;
	for (unsigned int i = 0; i<root->moves.size(); ++i) {
		if (root->children[root->calcIndex(root->moves[i])]->getData().score*mult>=mx) {
			mx = root->children[root->calcIndex(root->moves[i])]->getData().score*mult;
			mI = i;
		}
	}
	return root->moves[mI];
}

void MoveTree::makeMove(pair<Coord, Coord> m)
{
	int nr = root->calcIndex(m);
	for (unsigned int i = 0; i<root->moves.size(); ++i) {
		int j = root->calcIndex(root->moves[i]);
		if (j!=nr)
			delete root->children[j];
	}
	root = root->children[nr];
	update();
}

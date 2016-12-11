#ifndef TREE_HPP
#define TREE_HPP

#include "Chess.hpp"

/**
 * Basic structure containing a position and a score, used in the Tree
 */
struct NodeData
{
	Chess position;
	Color toMove;
	int score;
};

/**
 * Tree node class. Used to create move trees
 */
class MoveTreeNode
{
	NodeData data;
	std::vector<std::pair<Coord,Coord> > moves;
    MoveTreeNode* children[4096];
    bool isLeaf, scoreValid;

    /**
     * Special hash that maps moves onto our array
     */
	int calcIndex(std::pair<Coord,Coord> m);

public:
	/**
	 * Creates the node from the given position
	 *
	 * \param pos The position to create the node with
	 * \param toMove The color who's turn it is
	 */
	MoveTreeNode(Chess pos, Color toMove);

	/**
	 * Frees all memory
	 */
    ~MoveTreeNode();

	/**
	 * If not done already, creates children nodes based on legal moves
	 */
	void spawnChildren(int depth);

	/**
	 * Redetermines score based on current position and children positions
	 */
	void updateScore();

	/**
	 * Returns the data for this node
	 */
	NodeData& getData();
};

#endif // TREE_HPP

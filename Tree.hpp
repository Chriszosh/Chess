#ifndef TREE_HPP
#define TREE_HPP

#include <SFML/System.hpp>
#include "Chess.hpp"

class MoveTree;

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

    friend class MoveTree;

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

/**
 * Tree class for storing possible continuations
 */
class MoveTree
{
    MoveTreeNode* root;
    int mult; //always select highest score. Set this to -1 to get lowest score
    int depth;

    sf::Thread** threads;
    int threadIdCounter, numThreads;
    int* threadRanges;
    sf::Mutex lock;

    /**
     * Updates the tree for the root moves given
     *
     * \param mn The inclusive minimum to start at
     * \param mx The exclusive mx to end at
     */
    void calcTree();

    /**
     * Updates the tree by ensuring that it is the proper depth
     */
	void update();

public:
	/**
	 * Initializes the internal board and the tree to the right depth
	 *
	 * \param me The color to play as
	 * \param d The depth to calculate to
	 * \param nt The number of threads to use
	 */
	MoveTree(Color me, int d = 3, int nt = 8);

	/**
	 * Frees all memory
	 */
	~MoveTree();

	/**
	 * Returns the best move but makes no changes
	 */
	std::pair<Coord,Coord> getBestMove();

	/**
	 * Makes the move on the internal board, resets the tree roots and calculates another level of moves
	 *
	 * \param mover The color moving
	 * \param m The move to make
	 */
    void makeMove(std::pair<Coord,Coord> m);
};

#endif // TREE_HPP

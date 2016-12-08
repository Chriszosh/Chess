#ifndef AI_HPP
#define AI_HPP

#include "Chess.hpp"

/**
 * For now this essentially wraps a function. Later maybe it can store trees and optimize by only
 * needing to search down one level more as moves are made
 */
class AI
{
    static const int depth = 2;
	const int weights[7] = {0,1,5,3,3,9,50};

    Game& board;
    Color me, enemy;

    /**
     * Generates a score for the passed move based on material and material in sub-moves
     *
     * \param b The game board to use
     * \param oPos The position of the piece to move
     * \param nPos The position to move to
     * \param dt The current depth left. Will not recurse if less than 0
     * \return A score for the move
     */
    int scoreMove(Game& b, Coord oPos, Coord nPos, int dt);

    /**
     * Sums all of the material on a board
     * \param b The board to count on
     * \return A count of all the material on the board
     */
	int sumBoard(Board b);

public:
	/**
     * Default constructor
     *
     * \param b A reference to the board of play
     * \param m The color to play as
     */
	AI(Game& b, Color m);

	/**
	 * Evaluates the position based on material and determines a move
	 *
	 * \return A pair containing the Coord of the piece to move and another Coord to move it to
	 */
	std::pair<Coord,Coord> getMove();

	/**
	 * Returns a random legal move
	 *
	 * \return A pair containing the Coord of the piece to move and another Coord to move it to
	 */
	std::pair<Coord,Coord> getRandomMove();
};

#endif // AI_HPP

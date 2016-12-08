#ifndef AI_HPP
#define AI_HPP

#include "Chess.hpp"

/**
 * For now this essentially wraps a function. Later maybe it can store trees and optimize by only
 * needing to search down one level more as moves are made
 */
class AI
{
    static const int depth = 3;

    Game& board;
    Color me;

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

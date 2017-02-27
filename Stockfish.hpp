#ifndef STOCKFISH_HPP
#define STOCKFISH_HPP

/**
 * Class to interface with Stockfish
 */
class Stockfish {
	//pipe data

public:
	/**
	 * Launches Stockfish and creates the pipes to communicate with it
	 *
	 * \param exepath The executable of Stockfish
	 */
	Stockfish(std::string exepath);

	/**
	 * Sets the position in the engine via a fen string
	 *
	 * \param fen The fen string
	 */
	void setPosition(std::string fen);

	/**
	 * Runs the engine and returns the current advantage in centipawns
	 *
	 * \return The current advantage in centipawns
	 */
	int run();
};

#endif // STOCKFISH_HPP

#ifndef PGN_HPP
#define PGN_HPP

#include <string>
#include <vector>

/**
 * Structure to store games
 */
struct Game {
	std::vector<std::string> moves;
	bool isWhite;
	enum Outcome {
		Win,
		Loss,
		Draw
	}result;
	int age; //in months
	int rating, opponentRating;
};

/**
 * Class for loading PGN files
 */
class PGN {
	std::string file;
	std::string playerName;
	int curYear, curMonth;
    std::vector<Game> games;

public:
	/**
	 * Constructs the object and prepares to load the file
	 *
	 * \param file The file to load from
	 */
	PGN(std::string file);

	/**
	 * Sets the name of the player whose games we care about
	 *
	 * \param name The name of the player
	 */
	void setPlayer(std::string nm);

	/**
	 * Actually loads the file and filters the games
	 */
	void parse();

	/**
	 * Returns all the parsed and filtered games
	 *
	 * \return A vector containing all the games
	 */
    std::vector<Game> getGames();
};

#endif // PGN_HPP

#ifndef PGN_HPP
#define PGN_HPP

#include <string>
#include <vector>

/**
 * Structure to store games
 */
struct Game {
	std::vector<std::string> moves;
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
    std::vector<int> allowedYears;
    int minMonth;

    std::vector<Game> games;

    /**
     * Helper function to determine if a game should be kept based on age
     *
     * \param y The year of the game
     * \param m The month of the game
     * \return True if the game should be kept, false otherwise
     */
    bool shouldKeep(int y, int m);

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
	 * Adds a year to get games from
	 *
	 * \param y The year to get games from
	 */
	void addAllowedYear(int y);

	/**
	 * Sets the earliest month to get games from in the lowest allowed year
	 *
	 * \param m The month to start getting games at (Jan = 1)
	 */
	void setMinimumMonth(int m);

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

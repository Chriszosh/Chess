#ifndef STOCKFISH_HPP
#define STOCKFISH_HPP

#include <windows.h>
#include <string>

/**
 * Class to interface with Stockfish
 */
class Stockfish {
	static const DWORD BUFFER_SIZE = 8192;
    char buffer[ BUFFER_SIZE ] ;

    HANDLE hPipeRead, hPipeWrite;
    HANDLE hConOut;
    STARTUPINFO startInfo;
    PROCESS_INFORMATION procInfo;
    DWORD nRead;

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

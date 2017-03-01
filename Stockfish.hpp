#ifndef STOCKFISH_HPP
#define STOCKFISH_HPP

#include <windows.h>
#include <string>
#include <vector>

/**
 * Class to interface with Stockfish
 */
class Stockfish {
	static const DWORD BUFFER_SIZE = 8192;
    char buffer[ BUFFER_SIZE ] ;

    SECURITY_ATTRIBUTES saAttr;
    HANDLE g_hChildStd_OUT_Rd;
	HANDLE g_hChildStd_OUT_Wr;
	HANDLE g_hChildStd_IN_Rd;
	HANDLE g_hChildStd_IN_Wr;

	void WriteToPipe(std::string msg);
	std::vector<std::string> ReadFromPipe();

public:
	/**
	 * Launches Stockfish and creates the pipes to communicate with it
	 */
	Stockfish();

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

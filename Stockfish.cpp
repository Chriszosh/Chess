#include <iostream>
#include <SFML/System.hpp>
#include "Stockfish.hpp"
using namespace std;

extern int stringToInt(string s);

void Stockfish::writeToPipe(string msg) {
    WriteFile(g_hChildStd_IN_Wr, msg.c_str(), msg.size(), NULL, NULL);
}

vector<string> Stockfish::readFromPipe() {
    DWORD dwRead, dwAvailableBytes, dwBytesLeft;
    CHAR chBuf[BUFFER_SIZE];
    BOOL bSuccess = FALSE;

    vector<string> ret;
    string temp;

	bSuccess = PeekNamedPipe(g_hChildStd_OUT_Rd, chBuf, BUFFER_SIZE, &dwRead, &dwAvailableBytes, &dwBytesLeft);
	if (bSuccess && dwAvailableBytes>0)
		ReadFile(g_hChildStd_OUT_Rd, chBuf, dwAvailableBytes, &dwRead, NULL);

	for (unsigned int i = 0; i<dwRead; ++i) {
		if (chBuf[i]=='\n') {
			ret.push_back(temp);
			temp.clear();
		}
		else if (chBuf[i]!='\r')
			temp.push_back(chBuf[i]);
	}

    return ret;
}

Stockfish::Stockfish() {
    g_hChildStd_OUT_Rd = NULL;
    g_hChildStd_IN_Wr = NULL;
    g_hChildStd_OUT_Rd = NULL;
    g_hChildStd_OUT_Wr = NULL;

    /*
     * CREATE PIPES
    */
    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create pipes for the child process's STDOUT and STDIN,
    // ensures both handle are not inherited
    CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0);
    SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);

    CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0);
    SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

    /*
     * CREATE CHILD PROCESS
    */

    TCHAR szCmdline[]=TEXT("Stockfish/Windows/stockfish_8_x64.exe");

    // Set up members of the PROCESS_INFORMATION structure.
    ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

    // Set up members of the STARTUPINFO structure.
    // This structure specifies the STDIN and STDOUT handles for redirection.
    ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError  = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput  = g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    CreateProcess(NULL, szCmdline, NULL, NULL, TRUE, 0,
                  NULL,  NULL, &siStartInfo, &piProcInfo);

    // Close handles to the child process and its primary thread.
    // Some applications might keep these handles to monitor the status
    // of the child process, for example.
    CloseHandle(g_hChildStd_OUT_Wr);
    CloseHandle(g_hChildStd_IN_Rd);
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    /*
     * ACTUAL APPLICATION
    */

    readFromPipe();
    writeToPipe("uci\n");
    writeToPipe("setoption name Minimum Thinking Time 5000\n");
    vector<string> ms;
    while (true) {
		ms = readFromPipe();
		for (unsigned int i = 0; i<ms.size(); ++i) {
			if (ms[i]=="uciok")
				goto done;
		}
    }
    done:;
}

Stockfish::~Stockfish() {
	DWORD dwDesiredAccess = PROCESS_TERMINATE;
    BOOL  bInheritHandle  = FALSE;
    HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, piProcInfo.dwProcessId);
	TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);
}

void Stockfish::setPosition(string fen) {
	writeToPipe("position fen "+fen+"\n");
	writeToPipe("go\n"); //start processing so that the data is more likely to be available by the time the user requests it
}

int Stockfish::run() {
	vector<string> messages;
	string temp;
	int score = 0;

	enum {
		ReadingLines,
		WaitingScore,
		ReadingScore
	}state = ReadingLines;

	while (true) {
		messages = readFromPipe();
		for (unsigned int i = 0; i<messages.size(); ++i) {
			temp.clear();
			for (unsigned int j = 0; j<messages[i].size(); ++j) {
				if (messages[i][j]==' ') {
					switch (state) {
						case ReadingLines:
							if (temp=="info")
								state = WaitingScore;
							else if (temp=="bestmove")
								goto done;
							break;

						case WaitingScore:
							if (temp=="cp")
								state = ReadingScore;
							break;

						case ReadingScore:
							score = stringToInt(temp);
							state = ReadingLines;
							goto nextLine;

						default:
							break;
					}
					temp.clear();
				}
				else
					temp.push_back(messages[i][j]);
			}
			nextLine:;
		}
	}
	done:
	return score;
}

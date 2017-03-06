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

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0);
    SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
    CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0);
    SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);
    TCHAR szCmdline[]=TEXT("Stockfish/Windows/stockfish_8_x64.exe");
    ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
    ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError  = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput  = g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
    CreateProcess(NULL, szCmdline, NULL, NULL, TRUE, 0, NULL,  NULL, &siStartInfo, &piProcInfo);
    CloseHandle(g_hChildStd_OUT_Wr);
    CloseHandle(g_hChildStd_IN_Rd);
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    readFromPipe();
    writeToPipe("uci\n");
    writeToPipe("isready\n");
    writeToPipe("setoption name Minimum Thinking Time value 3000\n");
    writeToPipe("setoption name Threads value 8\n");
    vector<string> ms;
    bool uciok = false, ready = false;
    while (!uciok || !ready) {
		ms = readFromPipe();
		for (unsigned int i = 0; i<ms.size(); ++i) {
			if (ms[i]=="uciok")
				uciok = true;
			if (ms[i]=="readyok")
				ready = true;
		}
    }
    writeToPipe("ucinewgame\n");
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
							if (temp=="score")
								state = WaitingScore;
							else if (temp=="bestmove")
								goto done;
							break;

						case WaitingScore:
							if (temp=="cp")
								state = ReadingScore;
							if (temp=="mate")
								return 999999999;
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

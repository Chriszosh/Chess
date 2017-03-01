#include <iostream>
#include <SFML/System.hpp>
#include "Stockfish.hpp"
using namespace std;

void Stockfish::WriteToPipe(string msg) {
    WriteFile(g_hChildStd_IN_Wr, msg.c_str(), msg.size(), NULL, NULL);
}

vector<string> Stockfish::ReadFromPipe() {
    DWORD dwRead, dwAvailableBytes, dwBytesLeft;
    CHAR chBuf[BUFFER_SIZE];
    BOOL bSuccess = FALSE;

    vector<string> ret;
    string temp;

    cout << "Trying to read from pipe...\n";
    WriteToPipe("jdhufdhudf");
    while (!bSuccess || dwRead==0) {
        //bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFFER_SIZE, &dwRead, NULL);
        bSuccess = PeekNamedPipe(g_hChildStd_OUT_Rd, chBuf, BUFFER_SIZE, &dwRead, &dwAvailableBytes, &dwBytesLeft);
        if (bSuccess && dwAvailableBytes>0) {
			cout << "Success! " << dwAvailableBytes << " bytes available. " << dwBytesLeft << " bytes left\n";
			ReadFile(g_hChildStd_OUT_Rd, chBuf, dwAvailableBytes, &dwRead, NULL);
        }
       // if (!bSuccess || dwRead==0)
		//	break;
        for (unsigned int i = 0; i<dwRead; ++i) {
            if (chBuf[i]=='\n') {
				ret.push_back(temp);
				temp.clear();
            }
            else
				temp.push_back(chBuf[i]);
        }
    }

    cout << "Read: ";
    for (unsigned int i = 0; i<ret.size(); ++i) {
        cout << i+1 << ": " << ret[i] << endl;
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
    STARTUPINFO siStartInfo;
    PROCESS_INFORMATION piProcInfo;

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

    WriteToPipe("uci");
    //CloseHandle(g_hChildStd_IN_Wr);

    sf::sleep(sf::milliseconds(1000));
    ReadFromPipe();
    ReadFromPipe();

    WriteToPipe("<56>\n"); // I can't, as I have released the handle already
}


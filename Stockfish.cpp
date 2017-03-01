#include <iostream>
#include "Stockfish.hpp"
using namespace std;

Stockfish::Stockfish(string exepath) {
    CreatePipe( &hPipeRead, &hPipeWrite, 0, 0 );
    hConOut = GetStdHandle( STD_OUTPUT_HANDLE );
    SetStdHandle( STD_OUTPUT_HANDLE, hPipeWrite );

    startInfo = { sizeof(STARTUPINFO) };
    CreateProcess( exepath.c_str(), "", 0, 0, false, 0, 0, 0, &startInfo, &procInfo );

    SetStdHandle( STD_OUTPUT_HANDLE, hConOut );

    nRead = 0;
    ReadFile( hPipeRead, buffer, BUFFER_SIZE - 1, &nRead, 0 );
    buffer[ nRead ] = 0;
    std::cout << "\n=====\n" << buffer << "\n=====\n";
    WriteFile(hPipeWrite,"uci",3,&nRead,NULL);
    ReadFile( hPipeRead, buffer, BUFFER_SIZE - 1, &nRead, 0 );
    buffer[ nRead ] = 0;
    std::cout << "\n=====\n" << buffer << "\n=====\n";
}


#include <iostream>
#include <windows.h>
/*
int main() {
   const DWORD BUFFER_SIZE = 2000;
   char buffer[ BUFFER_SIZE ] ;

   HANDLE hPipeRead, hPipeWrite;
   CreatePipe( &hPipeRead, &hPipeWrite, 0, 0 );
   HANDLE hConOut = GetStdHandle( STD_OUTPUT_HANDLE );
   SetStdHandle( STD_OUTPUT_HANDLE, hPipeWrite );

   STARTUPINFO         startInfo = { sizeof(STARTUPINFO) };
   PROCESS_INFORMATION procInfo;
   CreateProcess( "c:/windows/system32/ipconfig.exe", "",
                  0, 0, false, 0, 0, 0,
                  &startInfo, &procInfo );

   SetStdHandle( STD_OUTPUT_HANDLE, hConOut );

   DWORD nRead = 0;
   ReadFile( hPipeRead, buffer, BUFFER_SIZE - 1, &nRead, 0 );
   buffer[ nRead ] = 0;
   std::cout << "\n=====\n" << buffer << "\n=====\n";
}
*/

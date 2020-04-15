// Author: TangAnke
// Date: 2020-Apr

/* ref/file/file.tm */
// example
// int bitAnd P(( int, int));

// :Begin:
// :Function: bitAnd
// :Pattern: bitAnd[x_Integer, y_Integer]
// :Arguments: {x, y}
// :ArgumentTypes: {Integer, Integer}
// :ReturnType: Integer
// :End:

//:Evaluate: bitAnd::usage = "bitAnd[x, y] gives the bitwise conjunction of two integers x and y."

// extern int DatabaseConnect P((char*ip,char *user,char*pass,char*db,int port))
:Evaluate: BeginPackage["TDengineLink`"]

:Begin:
:Function: TDengineConnect
:Pattern: TDengineConnect[ip_String,user_String,pass_String,db_String,port_Integer]
:Arguments: {ip,user,pass,db,port}
:ArgumentTypes: {String,String,String,String,Integer32}
:ReturnType: Manual
:End:
:Evaluate: TDengineConnect::usage = "TDengineConnect[ip,user,pass,db,port] connect a TDengine database."

:Begin:
:Function: TDengineClose
:Pattern: TDengineClose[TDengineDescriptor[id_Integer]]
:Arguments: {id}
:ArgumentTypes: {Integer32}
:ReturnType: Manual
:End:
:Evaluate: TDengineClose::usage = "TDengineClose[TDengineDescriptor[id]] close a TDengine database connection."

:Begin:
:Function: TDengineQuery
:Pattern: TDengineQuery[TDengineDescriptor[id_Integer],sqlstr_String]
:Arguments: {id,sqlstr}
:ArgumentTypes: {Integer32,String}
:ReturnType: Manual
:End:
:Evaluate: TDengineQuery::usage = "TDengineQuery[TDengineDescriptor[id],sqlstr] Execute a SQL statement sqlstr on the TDengine database connection refered to by TDengineDescriptor[id]"

:Begin:
:Function: TDengineServerInfo
:Pattern: TDengineServerInfo[TDengineDescriptor[id_Integer]]
:Arguments: {id}
:ArgumentTypes: {Integer32}
:ReturnType: Manual
:End:
:Evaluate: TDengineServerInfo::usage = "TDengineServerInfo[TDengineDescriptor[id]] returns information of the server."

:Begin:
:Function: TDengineClientInfo
:Pattern: TDengineClientInfo[]
:Arguments: {}
:ArgumentTypes: {}
:ReturnType: Manual
:End:
:Evaluate: TDengineServerInfo::usage = "TDengineClientInfo[]] returns information of the client."

:Evaluate: EndPackage[]

#include <mathlink.h>

#if WINDOWS_MATHLINK

#include <windows.h>
extern HWND MLInitializeIcon( HINSTANCE hinstCurrent, int nCmdShow);

int PASCAL WinMain( HINSTANCE hinstCurrent, HINSTANCE hinstPrevious, LPSTR lpszCmdLine, int nCmdShow)
{
        char buff[512];
        char * buff_start = buff;
        char * argv[32];
        char ** argv_end = argv + 32;

        hinstPrevious = hinstPrevious; /* suppress warning */

        if( !MLInitializeIcon( hinstCurrent, nCmdShow)) return 1;
        MLScanString( argv, &argv_end, &lpszCmdLine, &buff_start);
        return MLMain( (int)(argv_end - argv), argv);
}

#else

int main(int argc, char* argv[])
{
    return MLMain(argc, argv);
}

#endif

#include "pin.H"
#include <iostream>

using namespace std;

UINT64 funcCount = 0;   // total # of functions executed

/*
 * analysis function (called whenever a function is executed)
 */
VOID incrementFuncCount ()
{
    funcCount++;
}

/*
 * instrumentation procedure (called when a new function is executed for the
 * first time)
 */
VOID instrumentFunction (RTN func, VOID *v)
{
    RTN_Open(func);
    RTN_InsertCall(func, IPOINT_BEFORE, (AFUNPTR)incrementFuncCount, IARG_END);
    RTN_Close(func);
}

/*
 * output procedure (called when program exits)
 */
VOID printResults (INT32 code, VOID *v)
{
    std::cout << "Total function execution count: " << funcCount << std::endl;
    std::cout << "Exit code: " << code << std::endl;
}

int main(int argc, char *argv[])
{
    // initialize Pin
    if (PIN_Init(argc,argv)) {
        std::cout << "Usage: pin -t <tool name> -- <exefile>" << std::endl;
        return -1;
    }

    // install Pin callbacks
    RTN_AddInstrumentFunction(instrumentFunction, 0);
    PIN_AddFiniFunction(printResults, 0);

    // start the program (never returns!)
    PIN_StartProgram();

    return 0;
}


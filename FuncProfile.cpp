#include "pin.H"
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

UINT32 numFuncs = 0;

vector<string> name;
vector<UINT64> fcount;

/*
 * analysis function (called whenever a function is executed)
 */
VOID incrementFuncCount (UINT32 id)
{
    fcount[id]++;
}

/*
 * instrumentation procedure (called when a new function is executed for the
 * first time)
 */
VOID instrumentFunction (RTN func, VOID *v)
{
    UINT32 newID = numFuncs++;
    name.push_back(PIN_UndecorateSymbolName(RTN_Name(func),
                                            UNDECORATION_NAME_ONLY));
    fcount.push_back(0);

    RTN_Open(func);
    RTN_InsertCall(func, IPOINT_BEFORE, (AFUNPTR)incrementFuncCount,
            IARG_UINT32, newID, IARG_END);
    RTN_Close(func);
}

/*
 * output procedure (called when program exits)
 */
VOID printResults (INT32 code, VOID *v)
{
    UINT64 totalExecs = 0;
    for (UINT32 i = 0; i < numFuncs; i++) {
        if (fcount[i] > 0) {
            cout << std::setw(10) << fcount[i] << "  " << name[i] << endl;
            totalExecs += fcount[i];
        }
    }
    cout << "Total function execution count: " << totalExecs << endl;
}

int main(int argc, char *argv[])
{
    // initialize Pin
    if (PIN_Init(argc,argv)) {
        cout << "Usage: pin -t <tool name> -- <exefile>" << endl;
        return -1;
    }
    PIN_InitSymbols();

    // install Pin callbacks
    RTN_AddInstrumentFunction(instrumentFunction, 0);
    PIN_AddFiniFunction(printResults, 0);

    // start the program (never returns!)
    PIN_StartProgram();

    return 0;
}


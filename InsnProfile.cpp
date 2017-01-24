#include "pin.H"
#include <iostream>
#include <string>
#include <cstring>
#include <iomanip>
#include <vector>
#include <sstream>

UINT32 numInsns = 0;

vector<ADDRINT> addr;
vector<string>  disas;
vector<string>  fname;
vector<string>  srcloc;
vector<UINT64>  icount;

/*
 * utility method: strip the path out of a filename
 */
inline const char* stripPath (const char *fn)
{
    const char *name = strrchr(fn, '/');
    if (name) {
        return name+1;
    } else {
        return fn;
    }
}

/*
 * analysis function (called whenever an instruction is executed)
 */
VOID incrementInsnCount (UINT32 id)
{
    icount[id]++;
}

/*
 * instrumentation procedure (called when a new instruction is executed for the
 * first time)
 */
VOID instrumentInstruction (INS insn, VOID *v)
{
    RTN func = INS_Rtn(insn);
    if (RTN_Valid(func)) {

        // save instruction address, disassembly, and function name
        UINT32 newID = numInsns++;
        addr.push_back(INS_Address(insn));
        disas.push_back(INS_Disassemble(insn));
        fname.push_back(PIN_UndecorateSymbolName(RTN_Name(INS_Rtn(insn)),
                                                 UNDECORATION_NAME_ONLY));

        // save instruction source file and lineno (requires debug symbols)
        INT32 line, col;
        string fn;
        PIN_GetSourceLocation(INS_Address(insn), &col, &line, &fn);
        stringstream ss("");
        ss << "[" << stripPath(fn.c_str()) << ":" << line << "]";
        srcloc.push_back(ss.str());

        // add instrumentation
        icount.push_back(0);
        INS_InsertCall(insn, IPOINT_BEFORE, (AFUNPTR)incrementInsnCount,
                IARG_UINT32, newID, IARG_END);
    }
}

/*
 * output procedure (called when program exits)
 */
VOID printResults (INT32 code, VOID *v)
{
    UINT64 totalExecs = 0;
    for (UINT32 i = 0; i < numInsns; i++) {
        if (icount[i] > 0) {
            cout << setw(10) << icount[i] << "  " << fname[i] << ": "
                << disas[i] << "  src=" << srcloc[i] <<endl;
            totalExecs += icount[i];
        }
    }
    cout << "Total instruction execution count: " << totalExecs << endl;
}

int main(int argc, char *argv[])
{
    // initialize Pin
    if (PIN_Init(argc,argv)) {
        cout << "Usage: pin -t <tool name> -- <exefile>" << endl;
        return -1;
    }
    PIN_InitSymbols();
    PIN_SetSyntaxATT();

    // install Pin callbacks
    INS_AddInstrumentFunction(instrumentInstruction, 0);
    PIN_AddFiniFunction(printResults, 0);

    // start the program (never returns!)
    PIN_StartProgram();

    return 0;
}


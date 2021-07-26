#include "pin.H"
#include "pin_isa.H"
extern "C" {
#include "xed-interface.h"
}

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <sstream>

using namespace std;

const UINT8 OE = 0x08;      // overflow
const UINT8 UE = 0x10;      // underflow
const UINT8 PE = 0x20;      // precision

UINT8  emask  = PE;
UINT32 icount = 0;

vector<ADDRINT> addr;      // address
vector<string>  disas;     // ATT disassembly
vector<string>  fname;     // function name
vector<string>  srcloc;    // source location ("filename:lineno")
vector<UINT64>  ecount;    // instruction execution count
vector<UINT64>  xcount;    // exception count

/*
 * utility method: strip the path out of a filename
 */
inline const char* stripPath (const char *fn)
{
    const char *name = strrchr(fn, '/');
    return (name ? name+1 : fn);
}

/*
 * utility method: is the instruction part of the SSE/AVX instruction set?
 */
bool isSSE(INS ins)
{
    // extract instruction info
    INT32 icategory = INS_Category(ins);

    // check for floating-point instructions
    return (icategory == XED_CATEGORY_SSE ||
            icategory == XED_CATEGORY_AVX ||
            icategory == XED_CATEGORY_AVX2);
}

/*
 * analysis function (called whenever an instruction is executed)
 */

// ATTEMPT #1: pass register value
//VOID incrementInsnCounts (UINT32 id, ADDRINT reg)

// ATTEMPT #2: pass register reference
//VOID incrementInsnCounts (UINT32 id, const PIN_REGISTER *reg)

// ATTEMPT #3: use an asm directive
VOID incrementInsnCounts (UINT32 id)
{
    // ATTEMPT #1:
    //int mxcsr = (int)reg;

    // ATTEMPT #2:
    //int mxcsr = reg->dword[0];

    // ATTEMPT #3:
    int mxcsr = 0;
    asm volatile ("stmxcsr %0" : "=m" (mxcsr));

    // check for exception
    if ((UINT8)mxcsr & emask) {
        xcount[id]++;
    }

    // overall instruction count
    ecount[id]++;
}

/*
 * instrumentation procedure (called when a new instruction is executed for the
 * first time)
 */
VOID instrumentInstruction (INS insn, VOID *v)
{
    RTN func = INS_Rtn(insn);
    if (RTN_Valid(func) && isSSE(insn)
            && INS_HasFallThrough(insn)) {  // required for IPOINT_AFTER

        // save instruction address, disassembly, and function name
        UINT32 newID = icount++;
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
        xcount.push_back(0);
        ecount.push_back(0);
        INS_InsertCall(insn, IPOINT_AFTER, (AFUNPTR)incrementInsnCounts,
                IARG_UINT32, newID,
                //IARG_REG_VALUE, REG_MXCSR,              // ATTEMPT #1
                //IARG_REG_CONST_REFERENCE, REG_MXCSR,    // ATTEMPT #2
                IARG_END);
    }
}

/*
 * output procedure (called when program exits)
 */
VOID printResults (INT32 code, VOID *v)
{
    UINT64 totalExecs = 0, totalExceps = 0;
    UINT64 ihistogram[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    UINT64 ehistogram[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (UINT32 i = 0; i < icount; i++) {
        if (ecount[i] > 0) {    // don't divide by zero

            // calculate pct of instructions that caused an exception
            float pct = (float)xcount[i] / (float)ecount[i];

            // only print "interesting" instructions
            if (ecount[i] > 100 && (pct > .01 && pct < .99)) {
                std::cout << setw(15) << right << xcount[i] << " of "
                    << setw(15) << left << ecount[i]
                    << " (" << setprecision(3) << (pct*100.0) << "\%)"
                    << " in " << fname[i]
                    << ": " << disas[i]
                    //<< "  addr=0x" << addr[i]     // ugly
                    << "  src=" << srcloc[i]
                    << std::endl;
            }

            // update histograms
            int bin = -1;
            if (pct == 0.0) {
                bin = 0;
            } else if (pct == 1.0) {
                bin = 11;
            } else {
                bin = (int)(pct*10.0) + 1;
            }
            ihistogram[bin]++;
            ehistogram[bin] += ecount[i];
            totalExecs  += ecount[i];
            totalExceps += xcount[i];
        }
    }
    std::cout << "Instruction histogram: " << std::endl;
    std::cout << "  =  0\%: " << ihistogram[0] << std::endl;
    for (int i = 1; i < 11; i++) {
        std::cout << "  <" << setw(3) << (i*10) << "\%: " << ihistogram[i] << std::endl;
    }
    std::cout << "  =100\%: " << ihistogram[11] << std::endl;
    std::cout << "Execution histogram: " << std::endl;
    std::cout << "  =  0\%: " << ehistogram[0] << std::endl;
    for (int i = 1; i < 11; i++) {
        std::cout << "  <" << setw(3) << (i*10) << "\%: " << ehistogram[i] << std::endl;
    }
    std::cout << "  =100\%: " << ehistogram[11] << std::endl;
    std::cout << "Total executions: " << setw(15) << right << totalExecs << std::endl;
    std::cout << "Total exceptions: " << setw(15) << right << totalExceps << " ("
        << setprecision(3) << ((float)totalExceps / (float)totalExecs * 100.0)
        << "\%)" << std::endl;
}

int main(int argc, char *argv[])
{
    // initialize Pin
    if (PIN_Init(argc,argv)) {
        std::cout << "Usage: pin -t <tool name> -- <exefile>" << std::endl;
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


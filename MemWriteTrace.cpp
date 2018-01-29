/*
 * Example from original Pin paper (PLDI'05):
 * https://doi.org/10.1145/1065010.1065034
 */

#include "pin.H"
#include <stdio.h>

FILE *trace;

// Print a memory write record
VOID RecordMemWrite(VOID *ip, VOID *addr, UINT32 size)
{
    fprintf(trace, "%p: W %p %d\n", ip, addr, size);
}

// Called for every instruction
VOID Instruction(INS ins, VOID *v)
{
    if (INS_IsMemoryWrite(ins)) {

        // instruments writes using a predicated call, i.e. the call happens
        // iff the store is actually executed
        INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
                IARG_INST_PTR,
                IARG_MEMORYWRITE_EA,
                IARG_MEMORYWRITE_SIZE,
                IARG_END);
    }
}

int main(int argc, char *argv[])
{
    PIN_Init(argc, argv);
    trace = fopen("atrace.out", "w");
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_StartProgram();     // Never returns
    return 0;
}


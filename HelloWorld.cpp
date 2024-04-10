#include "pin.H"
#include <iostream>

using std::cout;
using std::endl;

VOID sayGoodbye (INT32 code, VOID *v)
{
    cout << "Goodbye! (exit code = " << code << ")" << endl;
}

int main(int argc, char *argv[])
{
    // initialize Pin
    if (PIN_Init(argc,argv)) {
        cout << "Usage: pin -t <tool name> -- <exefile>" << endl;
        return -1;
    }

    // install Pin callbacks
    PIN_AddFiniFunction(sayGoodbye, 0);

    // say hello
    cout << "Hello, world!" << endl;

    // start the program (never returns!)
    PIN_StartProgram();

    return 0;
}


# uug-pin

Pintool demonstration for JMU Unix Users Group


## Installing Pin

To install Pin, you can use the provided script:

    ./install_pin.sh

It will download and extract Pin into your `$HOME/opt` folder (creating it
first if necessary). If you run into problems, you can always grab the most
recent distribution from the [official
website](https://software.intel.com/en-us/articles/pintool-downloads) and
extract it somewhere yourself.


## Compiling Pintools

First, you must make sure that the `PIN_ROOT` environment variable is set to
the installation directory. If you use Bash and the provided installation
script worked for you, you can use the provided environment setup script:

    source ./setup_env.sh

If you installed Pin to a different location, you will need to modify the
script or just set `PIN_ROOT` manually.

After this, you should be able to compile the provided Pintools with  `make`.


## Running Pintools

Normally, you run Pintools with a command similar to the following:

    $PIN_ROOT/pin -t path/to/ToolName.so -- </path/to/app> <app-options>

However, for this demonstration the only important parts are the name of the
particular tool that is being run (`ToolName`) and the application-specific
part (everything after the `--`). Therefore, you can use the provided run
script to make things cleaner on the command line:

    ./run.sh <ToolName> </path/to/app> <app-options>

For instance, the following command will run the function profiler on a
traceroute:

    ./run.sh FuncProfile traceroute jmu.edu

Note that (as with compilation) this will fail if you don't have `$PIN_ROOT`
set to the Pin installation folder.

Included Pintools:

  * **HelloWorld** - Prints a hello message before the program begins, and a
    goodbye message (with exit code) after the program ends.

  * **MemWriteTrace** - Creates a file called `atrace.out` with a list of all
    memory writes, with the instruction address, the effective address of the
    access, and the number of bytes written.

  * **FuncCount** - Prints a single count of the total number of functions
    called at the end of execution.

  * **FuncProfile** - Prints a list of all functions called during execution
    alongside a count of how many times that function was run.

  * **InsnProfile** - Similar to FuncProfile, but instruments at the instruction
    level instead of the function level.
  
## Writing Pintools

The easiest way to create your own Pintool is just to copy one of the existing
`.cpp` files and modify it. You will need to add your new tool to the
`TEST_TOOL_ROOTS` variable in `Makefile.rules`.

For more information, see the [Building Your Own Tool](https://software.intel.com/sites/landingpage/pintool/docs/98830/Pin/doc/html/index.html#BUILDINGTOOLS)
section of the [User Manual](https://software.intel.com/sites/landingpage/pintool/docs/98830/Pin/doc/html/index.html)
as well as the [API Reference](https://software.intel.com/sites/landingpage/pintool/docs/98830/Pin/doc/html/group__API__REF.html).


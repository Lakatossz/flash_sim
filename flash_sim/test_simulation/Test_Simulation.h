#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>
#include "../flash_sim/include/flash/flash.h"
#include "test/main_test.cpp"

class Test_Simulation {

private:

    void Write_Status(ostream *output, const u_char status);

    void Write_Head(ostream *output);

    Flash_Memory * Specify_Memory(int argc, char **argv, ostream *output);

    int Handle_Life_Cycle(ostream *output, istream *input, Flash_Memory *flashMemory);

public:

    int Run_Demostration(int argc, char **argv);
};


#include "flash_sim/test_simulation/Test_Simulation.h"

using namespace std;

int main(int argc, char **argv) {

    auto simluation_test = new Test_Simulation();

    simluation_test->Run_Demostration(argc, argv);
}

#include "test_simulation/Test_Simulation.h"
#include "test_simulation/test/test.hpp"

using namespace std;

int main(int argc, char **argv) {

    cout << "===========================tests-begin===========================" << endl;

    run_tests();

    cout << "===========================tests-finish===========================" << endl;

    auto simluation_test = new Test_Simulation();

    simluation_test->Run_Demostration(argc, argv);
}

#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

struct {
    u_int32_t data[];
} block;

struct {

} sector;

int main(int argc, char **argv) {

    std::fstream input_file, output_file;

    if (argc > 1) {
        for (int i = 0; i < (argc - 1); ++i) {
            if (::strcmp(argv[i], "-input_file") == 0) {
                input_file.open(argv[i + 1], std::fstream::in);
            } else if (::strcmp(argv[i], "-output_file") == 0) {
                output_file.open(argv[i + 1], std::fstream::out);
            }
        }
    }

    while (1) {

    }

    input_file.close();
    output_file.close();
}

#include <iostream>
#include <cstring>
#include "../include/flash/flash.h"

#define COM_STOP "STOP"
#define COM_UNKNOWN "Neznamy prikaz!\n"

#define PARAM_FILE_IN "-input_file"
#define PARAM_FILE_OUT "-output_file"
#define PARAM_BLOCK_SIZE "-block_size"
#define PARAM_PAGE_SIZE "-page_size"
#define PARAM_READ_PAGE_TIME "-read_page_time"
#define PARAM_READ_PAGE_PROG "-read_page_prog"
#define PARAM_ERASE_TIME "-erase_time"

using namespace std;

int main(int argc, char **argv) {

    fstream input_file, output_file;
    string command;

    Flash_Memory *flashMemory = new Flash_Memory();

    if (argc > 1) {
        for (int i = 0; i < (argc - 1); ++i) {
            if (strcmp(argv[i], PARAM_FILE_IN) == 0) {
                input_file.open(argv[i + 1], ios::in);
                if (input_file.is_open()) {
                    cout << "Otevrel jsem input" << endl;
                }
            } else if (strcmp(argv[i], PARAM_FILE_OUT) == 0) {
                output_file.open(argv[i + 1], ios::out);
                if (output_file.is_open()) {
                    cout << "Otevrel jsem output" << endl;
                }
            } else if (strcmp(argv[i], PARAM_BLOCK_SIZE) == 0) {

            } else if (strcmp(argv[i], PARAM_PAGE_SIZE) == 0) {

            } else if (strcmp(argv[i], PARAM_READ_PAGE_TIME) == 0) {

            } else if (strcmp(argv[i], PARAM_READ_PAGE_PROG) == 0) {

            } else if (strcmp(argv[i], PARAM_ERASE_TIME) == 0) {

            }
        }
    } else {
        ((output_file.is_open()) ? output_file : cout) << "Vlozili jste malo parametru - program nemuze spravne bezet" << endl;
    }

    flashMemory->Init();

    while (true) {
        ((input_file.is_open()) ? input_file : cin) >> command;

        cout << command << endl;

        if (command == string(COM_STOP)) {
            break;
        } else if (command == string(COM_STOP)) {
        } else {
            ((output_file.is_open()) ? output_file : cout) << COM_UNKNOWN;
        }
    }

    ((output_file.is_open()) ? output_file : cout) << "Koncim\n";

    if (input_file.is_open()) {
        cout << "Zaviram input" << endl;
        input_file.close();
    }

    if (output_file.is_open()) {
        cout << "Zaviram output" << endl;
        output_file.close();
    }
}

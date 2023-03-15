#include <iostream>
#include <cstring>
#include "../include/flash/flash.h"

/**
 * Definice parametrů vložených při spuštění programu.
 */
#define PARAM_FILE_IN "-input_file"
#define PARAM_FILE_OUT "-output_file"
#define PARAM_BLOCK_SIZE "-block_size"
#define PARAM_PAGE_SIZE "-page_size"
#define PARAM_READ_PAGE_TIME "-read_page_time"
#define PARAM_READ_PAGE_PROG "-read_page_prog"
#define PARAM_ERASE_TIME "-erase_time"

#define COM_STOP "STOP"
#define COM_UNKNOWN "Neznamy prikaz!\n"

#define COM_READ_PAGE "READ_PAGE"
#define COM_READ_CACHE "READ_CACHE"
#define COM_READ_STATUS "READ_STATUS"
#define COM_READ_ID "READ_ID"
#define COM_PROGRAM_PAGE "PROGRAM_PAGE"
#define COM_PROGRAM_PAGE_CACHE "PROGRAM_PAGE_CACHE"
#define COM_PROGRAM_DATA_MOVE "PROGRAM_DATA_MOVE"
#define COM_BLOCK_ERASE "BLOCK_ERASE"
#define COM_RESET "RESET"
#define COM_RANDOM_DATA_READ "RANDOM_DATA_READ"
#define COM_RANDOM_DATA_INPUT "RANDOM_DATA_INPUT"

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
        } else if (command == string(COM_READ_PAGE)) {
            ((output_file.is_open()) ? output_file : cout) << "Obsah stránky byl načten do cache.\n";
        } else if (command == string(COM_READ_CACHE)) {
            ((output_file.is_open()) ? output_file : cout) << "Obsah cache byl přečten.\n";
        } else if (command == string(COM_READ_STATUS)) {
            ((output_file.is_open()) ? output_file : cout) << "Status paměti byl přečten.\n";
        } else if (command == string(COM_READ_ID)) {
            ((output_file.is_open()) ? output_file : cout) << "ID zařízení bylo přečteno.\n";
        } else if (command == string(COM_PROGRAM_PAGE)) {
            ((output_file.is_open()) ? output_file : cout) << "Stránka byla naprogramována.\n";
        } else if (command == string(COM_PROGRAM_PAGE_CACHE)) {
            ((output_file.is_open()) ? output_file : cout) << "Cache byla naprogramována.\n";
        } else if (command == string(COM_PROGRAM_DATA_MOVE)) {
            ((output_file.is_open()) ? output_file : cout) << "Obsah stránky by naprogramován na jiné místo.\n";
        } else if (command == string(COM_BLOCK_ERASE)) {
            ((output_file.is_open()) ? output_file : cout) << "Obsah bloku byl vymazán.\n";
        } else if (command == string(COM_RESET)) {
            ((output_file.is_open()) ? output_file : cout) << "Paměť byla vyresetována.\n";
        } else if (command == string(COM_RANDOM_DATA_READ)) {
            ((output_file.is_open()) ? output_file : cout) << "Obsah náhodné stránky byl načten do cache.\n";
        } else if (command == string(COM_RANDOM_DATA_INPUT)) {
            ((output_file.is_open()) ? output_file : cout) << "Stránka byla naprogramována náhodným obsahem.\n";
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

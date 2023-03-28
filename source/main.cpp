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
#define COM_READ_STATUS "READ_STATUS"
#define COM_READ_ID "READ_ID"
#define COM_PROGRAM_PAGE "PROGRAM_PAGE"
#define COM_PROGRAM_DATA_MOVE "PROGRAM_DATA_MOVE"
#define COM_BLOCK_ERASE "BLOCK_ERASE"
#define COM_RESET "RESET"
#define COM_RANDOM_DATA_READ "RANDOM_DATA_READ"
#define COM_RANDOM_DATA_INPUT "RANDOM_DATA_INPUT"

using namespace std;

int main(int argc, char **argv) {

    istream *input = &cin;
    ostream *output = &cout;
    ifstream input_file;
    ofstream output_file;
    string command;

    int_16 page_size = DEFAULT_PAGE_SIZE;

    auto *flashMemory = new Flash_Memory();

    if (argc > 1) {
        for (int i = 0; i < (argc - 1); ++i) {
            if (strcmp(argv[i], PARAM_FILE_IN) == 0) {
                input_file = ifstream(argv[i + 1]);
                if (input_file) {
                    input = &input_file;
                    cout << "Otevrel jsem input soubor." << endl;
                } else
                    cout << "Nepodarilo se otevrit soubor: " << argv[i + 1] << endl;
//                input_file.open(argv[i + 1], ios::in);
            } else if (strcmp(argv[i], PARAM_FILE_OUT) == 0) {
                output_file = ofstream(argv[i + 1]);
                if (output_file) {
                    output = &output_file;
                    cout << "Otevrel jsem output soubor." << endl;
                } else
                    cout << "Nepodarilo se otevrit soubor: " << argv[i + 1] << endl;
//                output_file.open(argv[i + 1], ios::out);
            } else if (strcmp(argv[i], PARAM_BLOCK_SIZE) == 0) {

            } else if (strcmp(argv[i], PARAM_PAGE_SIZE) == 0) {


            } else if (strcmp(argv[i], PARAM_READ_PAGE_TIME) == 0) {

            } else if (strcmp(argv[i], PARAM_READ_PAGE_PROG) == 0) {

            } else if (strcmp(argv[i], PARAM_ERASE_TIME) == 0) {

            }
        }
    } else {
        *output << "Vlozili jste malo parametru - program nemuze spravne bezet" << endl;
    }

    flashMemory->Init();


    while (true) {
        *input >> command;

        cout << command << endl;

        if (command == string(COM_STOP)) {
            break;
        } else if (command == string(COM_READ_PAGE)) {

            int_16 address;
            *input >> address;
            u_char *data = flashMemory->Read_Page(address);
            if (!data) {
                *output << "Nepodařilo se načíst stránku.\n";
            }

            *output << "Obsah stránky byl načten.\n";
        } else if (command == string(COM_READ_STATUS)) {
            u_char *data = flashMemory->Read_Status();
            if (!data) {
                *output << "Nepodařilo se načíst status.\n";
            }
            *output << "Status paměti byl přečten.\n";
        } else if (command == string(COM_READ_ID)) {
            flashMemory->Read_ID();
            *output << "ID zařízení bylo přečteno.\n";
        } else if (command == string(COM_PROGRAM_PAGE)) {
            int_16 address;
            *input >> address;
            auto *data = (u_char *) malloc(sizeof(u_char) * page_size);
            if (!data) {
                *output << "Není dostatek paměti.\n";
            }
            *input >> data;
            flashMemory->Program_Page(address, data);
            *output << "Stránka byla naprogramována.\n";
        } else if (command == string(COM_PROGRAM_DATA_MOVE)) {
            *output << "Obsah stránky by naprogramován na jiné místo.\n";
        } else if (command == string(COM_BLOCK_ERASE)) {
            int_16 address;
            *input >> address;
            flashMemory->Block_Erase(address);
            *output << "Obsah bloku byl vymazán.\n";
        } else if (command == string(COM_RESET)) {
            flashMemory->Reset();
            *output << "Paměť byla vyresetována.\n";
        } else if (command == string(COM_RANDOM_DATA_READ)) {
            u_char *data = flashMemory->Random_Data_Read();
            if (!data) {
                *output << "Nepodařilo se načíst stránku.\n";
            }
            *output << "Obsah náhodné stránky byl načten do cache.\n";
        } else if (command == string(COM_RANDOM_DATA_INPUT)) {
            auto *data = (u_char *) malloc(sizeof(u_char) * page_size);
            if (!data) {
                *output << "Není dostatek paměti.\n";
            }
            *input >> data;
            flashMemory->Random_Data_Input( data);
            *output << "Stránka byla naprogramována náhodným obsahem.\n";
        } else {
            *output << COM_UNKNOWN;
        }
    }

    *output << "Koncim\n";

    if (input_file && input_file.is_open()) {
        cout << "Zaviram input" << endl;
        input_file.close();
    }

    if (output_file && output_file.is_open()) {
        cout << "Zaviram output" << endl;
        output_file.close();
    }
}

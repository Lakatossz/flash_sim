#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include "../include/flash/flash.h"

/**
 * Definice parametrů vložených při spuštění programu.
 */
#define PARAM_FILE_IN "-input_file"
#define PARAM_FILE_OUT "-output_file"
#define PARAM_BLOCK_SIZE "-block_size"
#define PARAM_PAGE_SIZE "-page_size"
#define PARAM_READ_PAGE_TIME "-read_page_time"
#define PARAM_PAGE_PROG_TIME "-page_prog_time"
#define PARAM_ERASE_TIME "-erase_time"
#define PARAM_MEM_TYPE "-mem_type"

#define MAX_BLOCK_SIZE 10000
#define MAX_PAGE_SIZE 1024
#define MAX_READ_PAGE_TIME 1000
#define MAX_PAGE_PROG_TIME 1000
#define MAX_ERASE_TIME 1000

#define MIN_BLOCK_SIZE 1
#define MIN_PAGE_SIZE 1
#define MIN_READ_PAGE_TIME 1
#define MIN_PAGE_PROG_TIME 1
#define MIN_ERASE_TIME 1

#define COM_UNKNOWN "Neznamy prikaz!\n"

#define COM_READ_PAGE "0x03"
#define COM_READ_STATUS "0x04"
#define COM_READ_ID "0x05"
#define COM_PROGRAM_PAGE "0x06"
#define COM_PROGRAM_DATA_MOVE "0x07"
#define COM_BLOCK_ERASE "0x08"
#define COM_RESET "0x09"
#define COM_RANDOM_DATA_READ "0x0a"
#define COM_RANDOM_DATA_INPUT "0x0b"
#define COM_STOP "STOP"

using namespace std;

Flash_Memory * specifyMemory(int argc, char **argv, ostream *output) {

    int_32 page_size = DEFAULT_PAGE_SIZE;
    int_32 block_size = DEFAULT_BLOCK_SIZE;
    int_32 read_page_time = DEFAULT_READ_PAGE_TIME;
    int_32 page_prog_time = DEFAULT_PAGE_PROG_TIME;
    int_32 erase_time = DEFAULT_ERASE_TIME;
    mem_type type = DEFAULT_MEM_TYPE;
    string type_s = "slc";

    for (int i = 0; i < (argc - 1); i++) {
        if (strcmp(argv[i], PARAM_BLOCK_SIZE) == 0) {
            try {
                block_size = stoi(argv[i + 1]);
                if (block_size > MAX_BLOCK_SIZE || block_size < MIN_BLOCK_SIZE) {
                    cout << "Parametr musi byt cislo mezi " << MIN_BLOCK_SIZE << " a " << MAX_BLOCK_SIZE << "!\n";
                    return nullptr;
                }
            } catch (const std::invalid_argument & e) {
                cout << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_PAGE_SIZE) == 0) {
            try {
                page_size = stoi(argv[i + 1]);
                if (page_size > MAX_PAGE_SIZE || page_size < MIN_PAGE_SIZE) {
                    cout << "Parametr musi byt cislo mezi " << MIN_PAGE_SIZE << " a " << MAX_PAGE_SIZE << "!\n";
                    return nullptr;
                }
            } catch (const std::invalid_argument & e) {
                cout << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_READ_PAGE_TIME) == 0) {
            try {
                read_page_time = stoi(argv[i + 1]);
                if (read_page_time > MAX_READ_PAGE_TIME || read_page_time < MIN_READ_PAGE_TIME) {
                    cout << "Parametr musi byt cislo mezi " << MIN_READ_PAGE_TIME <<
                         " a " << MAX_READ_PAGE_TIME << "!\n";
                    return nullptr;
                }
            } catch (const std::invalid_argument & e) {
                cout << "Parametr musi byt cislo!\n";
                return nullptr;
            }

        } else if (strcmp(argv[i], PARAM_PAGE_PROG_TIME) == 0) {
            try {
                page_prog_time = stoi(argv[i + 1]);
                if (page_prog_time > MAX_PAGE_PROG_TIME || page_prog_time < MIN_PAGE_PROG_TIME) {
                    cout << "Parametr musi byt cislo mezi " << MIN_PAGE_PROG_TIME <<
                         " a " << MAX_PAGE_PROG_TIME << "!\n";
                    return nullptr;
                }
            } catch (const std::invalid_argument & e) {
                cout << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_ERASE_TIME) == 0) {
            try {
                erase_time = stoi(argv[i + 1]);
                if (erase_time > MAX_ERASE_TIME || erase_time < MIN_ERASE_TIME) {
                    cout << "Parametr musi byt cislo mezi " << MIN_ERASE_TIME << " a " << MAX_ERASE_TIME << "!\n";
                    return nullptr;
                }
            } catch (const std::invalid_argument & e) {
                cout << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_MEM_TYPE) == 0) {
            type_s = string(argv[i + 1]);
            if (strcmp(argv[i + 1], MEM_TYPE_SLC) == 0) {
                type = SLC;
            } else if (strcmp(argv[i + 1], MEM_TYPE_MLC) == 0) {
                type = MLC;
            } else if (strcmp(argv[i + 1], MEM_TYPE_TLC) == 0) {
                type = TLC;
            } else if (strcmp(argv[i + 1], MEM_TYPE_QLC) == 0) {
                type = QLC;
            } else {
                cout << "Spatny parametr typu pameti. Povolene jsou (slc, mlc, tlc, qlc)!\n";
                return nullptr;
            }
        }
    }

    cout << "Vami nastavene parametry:\n";
    cout << "Velikost stranky: " << page_size << " byte" << endl;
    cout << "Velikost bloku: " << block_size << " byte" << endl;
    cout << "Typ pameti: " << type_s << endl;
    cout << "Cas cteni stranky: " << read_page_time << " μs" << endl;
    cout << "Cas naprogramovani stranky: " << page_prog_time << " μs" << endl;
    cout << "Cas vymazani bloku: " << erase_time << " μs" << endl;

    cout << "=================================================================" << endl;

    return new Flash_Memory(page_size, block_size,type, read_page_time, page_prog_time, erase_time);
}

int main(int argc, char **argv) {

    string command, line;
    string args[5] = {};

    istream *input = &cin;
    ostream *output = &cout;

    int changed_input = 0;

    auto *flashMemory = new Flash_Memory();

    cout << "=================================================================" << endl;

    cout << " ______   _                 _          _____   _             " << endl;
    cout << "|  ____| | |               | |        / ____| (_)            " << endl;
    cout << "| |__    | |   __ _   ___  | |__     | (___    _   _ __ ___  " << endl;
    cout << R"(|  __|   | |  / _` | / __| | '_ \     \___ \  | | | '_ ` _ \ )" << endl;
    cout << "| |      | | | (_| | \\__ \\ | | | |    ____) | | | | | | | | |" << endl;
    cout << "|_|      |_|  \\__,_| |___/ |_| |_|   |_____/  |_| |_| |_| |_|" << endl;

    cout << "=================================================================" << endl;

    if (argc > 1) {
        ifstream input_file;
        ofstream output_file;

        for (int i = 0; i < (argc - 1); i++) {
            if (strcmp(argv[i], PARAM_FILE_IN) == 0) {
                cout << "1\n";
                changed_input++;
                input_file = ifstream(argv[i + 1]);
                if (input_file) {
                    input = &input_file;
                    cout << "Otevrel jsem input.txt soubor." << endl;
                } else
                    cout << "Nepodarilo se otevrit soubor: " << argv[i + 1] << endl;
            } else if (strcmp(argv[i], PARAM_FILE_OUT) == 0) {
                changed_input++;
                output_file = ofstream(argv[i + 1]);
                if (output_file) {
                    output = &output_file;
                    cout << "Otevrel jsem output soubor." << endl;
                } else
                    cout << "Nepodarilo se otevrit soubor: " << argv[i + 1] << endl;
            }
        }

        flashMemory = specifyMemory(argc - changed_input, argv, output);
        if (!flashMemory) {
            *output << "Koncim" << endl;
            return EXIT_FAILURE;
        }

        flashMemory->Init();

        *output << "Pamet byla incializovana a je pripravena k pouziti!" << endl;

        cout << "=================================================================" << endl;

        while (true) {
            *input >> command;

            int i = 0;

            cout << "command: " << command << endl;

            if (command == string(COM_STOP)) {
                break;
            } else if (command == string(COM_READ_PAGE)) {
                try {
                    *input >> line;
                    u_char *data = flashMemory->Read_Page((int16_t) stoi(line));
                    if (!data) {
                        *output << "Nepodařilo se načíst stránku.\n";
                        return EXIT_FAILURE;
                    }

                    *output << "Stranka byla nactena.\n";
                } catch (const std::invalid_argument & e) {
                    cout << "Parametr musi byt cislo!\n";
                    return EXIT_FAILURE;
                }

                *output << "Obsah stránky byl načten.\n";
            } else if (command == string(COM_READ_STATUS)) {
                u_char data = flashMemory->Read_Status();
                *output << "Status paměti byl přečten.\n";
            } else if (command == string(COM_READ_ID)) {
                flashMemory->Read_ID();
                *output << "ID zařízení bylo přečteno.\n";
            } else if (command == string(COM_PROGRAM_PAGE)) {
                try {
                    *input >> line;
                    *output << "adresa: " << line << endl;
                    *input >> line;
                    *output << "data: " << line << endl;
                    flashMemory->Program_Page((int16_t) stoi(line), line);
                    *output << "Stránka byla naprogramována.\n";
                } catch (const std::invalid_argument & e) {
                    cout << "Parametr musi byt cislo!\n";
                    return EXIT_FAILURE;
                }
            } else if (command == string(COM_PROGRAM_DATA_MOVE)) {
                *output << "Obsah stránky by naprogramován na jiné místo.\n";
            } else if (command == string(COM_BLOCK_ERASE)) {
                try {
                    *input >> line;
                    flashMemory->Block_Erase((int16_t) stoi(line));
                    *output << "Obsah bloku byl vymazán.\n";
                } catch (const std::invalid_argument & e) {
                    cout << "Parametr musi byt cislo!\n";
                    return EXIT_FAILURE;
                }
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
                flashMemory->Random_Data_Input();
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
}

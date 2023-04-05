#include <iostream>
#include <cstring>
#include <cstdlib>
#include "../include/flash/flash.h"

using namespace std;

void writeHead(ostream *output) {
    *output << "=================================================================" << endl;

    *output << " ______   _                 _          _____   _             " << endl;
    *output << "|  ____| | |               | |        / ____| (_)            " << endl;
    *output << "| |__    | |   __ _   ___  | |__     | (___    _   _ __ ___  " << endl;
    *output << R"(|  __|   | |  / _` | / __| | '_ \     \___ \  | | | '_ ` _ \ )" << endl;
    *output << "| |      | | | (_| | \\__ \\ | | | |    ____) | | | | | | | | |" << endl;
    *output << "|_|      |_|  \\__,_| |___/ |_| |_|   |_____/  |_| |_| |_| |_|" << endl;

    *output << "=================================================================" << endl;
}

/**
 * Funkce pro načtení parametrů paměti, přidaných při spuštění programu.
 * @return Instance paměti s změněnými parametry.
 */
Flash_Memory * specifyMemory(int argc, char **argv, ostream *output) {

    /**
     * Všechny možné parametry - mají přiřazeny defaultní hodnoty. Na konci se zavolá konstruktor
     * se všemi parametry - Změna se projeví jen u předefinovaných.
     */
    int_32 page_size = DEFAULT_PAGE_SIZE;
    int_32 block_size = DEFAULT_BLOCK_SIZE;
    int_32 num_of_blocks = DEFAULT_NUM_OF_BLOCKS;
    float read_page_time = DEFAULT_READ_PAGE_TIME;
    float page_prog_time = DEFAULT_PAGE_PROG_TIME;
    float erase_time = DEFAULT_ERASE_TIME;
    Mem_Type type = DEFAULT_MEM_TYPE;
    string type_s = "slc";

    /**
     * Zkontrolují se všechny všechny řetězce, hledají se označení parametrů a porovnají se s
     * definovanými hodnotami. Pokud je nalezen, přiřadí se hodnota parametru.
     */
    for (int i = 0; i < (argc - 1); i++) {
        if (strcmp(argv[i], PARAM_BLOCK_SIZE) == 0) {
            try {
                block_size = stoi(argv[i + 1]);
                if (block_size > MAX_BLOCK_SIZE || block_size < MIN_BLOCK_SIZE) {
                    *output << "Parametr musi byt cislo mezi " << MIN_BLOCK_SIZE << " a " << MAX_BLOCK_SIZE << "!\n";
                    return nullptr;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_PAGE_SIZE) == 0) {
            try {
                page_size = stoi(argv[i + 1]);
                if (page_size > MAX_PAGE_SIZE || page_size < MIN_PAGE_SIZE) {
                    *output << "Parametr musi byt cislo mezi " << MIN_PAGE_SIZE << " a " << MAX_PAGE_SIZE << "!\n";
                    return nullptr;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_NUM_OF_BLOCK) == 0) {
            try {
                num_of_blocks = stoi(argv[i + 1]);
                if (num_of_blocks > MAX_NUM_OF_BLOCK || num_of_blocks < MIN_NUM_OF_BLOCK) {
                    *output << "Parametr musi byt cislo mezi " << MIN_NUM_OF_BLOCK << " a " << MAX_NUM_OF_BLOCK << "!\n";
                    return nullptr;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_READ_PAGE_TIME) == 0) {
            try {
                read_page_time = stof(argv[i + 1]);
                if (read_page_time > MAX_READ_PAGE_TIME || read_page_time < MIN_READ_PAGE_TIME) {
                    *output << "Parametr musi byt cislo mezi " << MIN_READ_PAGE_TIME <<
                         " a " << MAX_READ_PAGE_TIME << "!\n";
                    return nullptr;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return nullptr;
            }

        } else if (strcmp(argv[i], PARAM_PAGE_PROG_TIME) == 0) {
            try {
                page_prog_time = stof(argv[i + 1]);
                if (page_prog_time > MAX_PAGE_PROG_TIME || page_prog_time < MIN_PAGE_PROG_TIME) {
                    *output << "Parametr musi byt cislo mezi " << MIN_PAGE_PROG_TIME <<
                         " a " << MAX_PAGE_PROG_TIME << "!\n";
                    return nullptr;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_ERASE_TIME) == 0) {
            try {
                erase_time = stof(argv[i + 1]);
                if (erase_time > MAX_ERASE_TIME || erase_time < MIN_ERASE_TIME) {
                    *output << "Parametr musi byt cislo mezi " << MIN_ERASE_TIME << " a " << MAX_ERASE_TIME << "!\n";
                    return nullptr;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
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
                *output << "Spatny parametr typu pameti. Povolene jsou (slc, mlc, tlc, qlc)!\n";
                return nullptr;
            }
        }
    }

    /**
     * Výpis konečných parametrů paměti.
     */

    *output << "Vámi nastavené parametry:\n";
    *output << "Velikost stránky: " << page_size << " bytů" << endl;
    *output << "Velikost bloku: " << block_size << " bytů" << endl;
    *output << "Počet bloků: " << num_of_blocks << endl;
    *output << "Typ paměti: " << type_s << endl;
    *output << "Čas čtení stránky: " << read_page_time << " μs" << endl;
    *output << "Čas naprogramování stránky: " << page_prog_time << " μs" << endl;
    *output << "Čas vymazání bloku: " << erase_time << " μs" << endl;

    *output << "=================================================================" << endl;

    /** Nakonec se vytvoří nová instance paměti. */
    return new Flash_Memory(page_size, block_size, num_of_blocks, type, read_page_time, page_prog_time, erase_time);
}

int handleLifeCycle(ostream *output, istream *input, Flash_Memory *flashMemory) {

    string command;
    string args[5] = {};

    while (true) {
        *input >> command;
        *output << "command: " << command << endl;

        if (command == string(COM_STOP)) {
            return EXIT_SUCCESS;
        } else if (command == string(COM_READ_PAGE)) {
            try {
                *input >> args[0];
                u_char *data = flashMemory->Read_Page((int16_t) stoi(args[0]));
                if (!data) {
                    *output << "Nepodařilo se načíst stránku.\n";
                    return EXIT_FAILURE;
                }
                *output << "Nactena data: " << data << endl;
//                    *output << "Stranka byla nactena.\n";
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_READ_STATUS)) {
            u_char data = flashMemory->Read_Status();
            *output << "Status paměti byl přečten.\n";
        } else if (command == string(COM_READ_ID)) {
            flashMemory->Read_ID();
            *output << "ID zařízení bylo přečteno.\n";
        } else if (command == string(COM_PROGRAM_PAGE)) {
            try {
                *input >> args[0];
                *output << "adresa: " << args[0] << endl;
                *input >> args[1];
                *output << "data: " << args[1] << endl;
                flashMemory->Program_Page((int16_t) stoi(args[0]), args[1]);
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo! " << "\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_PROGRAM_DATA_MOVE)) {
            try {
                *input >> args[0];
                *output << "puvodni adresa: " << args[0] << endl;
                *input >> args[1];
                *output << "nova adresa: " << args[1] << endl;
                flashMemory->Program_Data_Move((int16_t) stoi(args[0]), (int16_t) stoi(args[1]));
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo! " << "\n";
                return EXIT_FAILURE;
            }
//                *output << "Obsah stránky by naprogramován na jiné místo.\n";
        } else if (command == string(COM_BLOCK_ERASE)) {
            try {
                *input >> args[0];
                *output << "adresa: " << args[0] << endl;
                flashMemory->Block_Erase((int16_t) stoi(args[0]));
//                    *output << "Obsah bloku byl vymazán.\n";
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_RESET)) {
            flashMemory->Reset();
//                *output << "Paměť byla vyresetována.\n";
        } else if (command == string(COM_RANDOM_DATA_READ)) {
            u_char *data = flashMemory->Random_Data_Read();
            if (!data) {
                *output << "Nepodařilo se načíst stránku.\n";
            }
//                *output << "Obsah náhodné stránky byl načten do cache.\n";
        } else if (command == string(COM_RANDOM_DATA_INPUT)) {
            flashMemory->Random_Data_Input();
//                *output << "Stránka byla naprogramována náhodným obsahem.\n";
        } else {
            *output << COM_UNKNOWN;
        }

        *output << "Prikaz byl dokoncen\n";
    }
}

int main(int argc, char **argv) {

    istream *input = &cin;
    ostream *output = &cout;
    ifstream input_file;
    ofstream output_file;

    int changed_input = 0;

    auto *flashMemory = new Flash_Memory();

    writeHead(output);

    if (argc > 1) {
        for (int i = 0; i < (argc - 1); i++) {
            if (strcmp(argv[i], PARAM_FILE_IN) == 0) {
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
    }

    flashMemory->Init();
    *output << "Pamet byla incializovana a je pripravena k pouziti!" << endl;

    *output << "=================================================================" << endl;

    *output << "Program konci.\n";

    if (handleLifeCycle(output, input, flashMemory) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    if (input_file && input_file.is_open()) {
        cout << "Zavira se vstupni soubor.\n";
        input_file.close();
    }

    if (output_file && output_file.is_open()) {
        cout << "Zavira se vystupni soubor.\n";
        output_file.close();
    }
}

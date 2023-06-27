#include "Test_Simulation.h"

void Test_Simulation::Write_Status(ostream *output, const u_char status) {
    *output << "WP: " << ((status >> 0) & 1) << endl;
    *output << "BP: " << ((status >> 1) & 1) << endl;
    *output << "EPE: " << ((status >> 2) & 1) << endl;
    *output << "RB: " << ((status >> 3) & 1) << endl;
    *output << "ES: " << ((status >> 4) & 1) << endl;
    *output << "PT: " << ((status >> 5) & 1) << endl;
    *output << "ECC: " << ((status >> 6) & 1) << endl;
    *output << "ERR: " << ((status >> 7) & 1) << endl;
}

void Test_Simulation::Write_Head(ostream *output) {
    *output << "=================================================================" << endl;

    *output << " ______   _                 _          _____   _             " << endl;
    *output << "|  ____| | |               | |        / ____| (_)            " << endl;
    *output << "| |__    | |   __ _   ___  | |__     | (___    _   _ __ ___  " << endl;
    *output << R"(|  __|   | |  / _` | / __| | '_ \     \___ \  | | | '_ ` _ \ )" << endl;
    *output << "| |      | | | (_| | \\__ \\ | | | |    ____) | | | | | | | | |" << endl;
    *output << "|_|      |_|  \\__,_| |___/ |_| |_|   |_____/  |_| |_| |_| |_|" << endl;

    *output << "=================================================================" << endl;
}

Flash_Memory * Test_Simulation::Specify_Memory(int argc, char **argv, ostream *output) {

    /**
     * Všechny možné parametry - mají přiřazeny defaultní hodnoty. Na konci se zavolá konstruktor
     * se všemi parametry - Změna se projeví jen u předefinovaných.
     */
    size_t page_size = DEFAULT_PAGE_SIZE;
    size_t block_size = DEFAULT_BLOCK_SIZE;
    size_t num_of_blocks = DEFAULT_NUM_OF_BLOCKS;
    float read_page_time = DEFAULT_READ_PAGE_TIME;
    float page_prog_time = DEFAULT_PAGE_PROG_TIME;
    float erase_time = DEFAULT_ERASE_TIME;
    NMem_Type type = DEFAULT_MEM_TYPE;
    string type_s = "slc";
    size_t bad_blocks_fact = DEFAULT_BAD_BLOCKS_FACTORY;
    float max_read_page_time = MAX_READ_PAGE_TIME;
    float max_page_prog_time = MAX_PAGE_PROG_TIME;
    float max_erase_time = MAX_ERASE_TIME;
    float min_read_page_time = MIN_READ_PAGE_TIME;
    float min_page_prog_time = MIN_PAGE_PROG_TIME;
    float min_erase_time = MIN_ERASE_TIME;
    float com_time = DEFAULT_COM_TIME;
    size_t max_erase_num = MAX_ERASE_NUMBER;


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
                type = NMem_Type::SLC;
            } else if (strcmp(argv[i + 1], MEM_TYPE_MLC) == 0) {
                type = NMem_Type::MLC;
            } else if (strcmp(argv[i + 1], MEM_TYPE_TLC) == 0) {
                type = NMem_Type::TLC;
            } else if (strcmp(argv[i + 1], MEM_TYPE_QLC) == 0) {
                type = NMem_Type::QLC;
            } else {
                *output << "Spatny parametr typu pameti. Povolene jsou (slc, mlc, tlc, qlc)!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_BAD_BLOCKS_FACT) == 0) {
            try {
                bad_blocks_fact = stoi(argv[i + 1]);
//                if (erase_time > MAX_ERASE_TIME || erase_time < MIN_ERASE_TIME) {
//                    *output << "Parametr musi byt cislo mezi " << MIN_ERASE_TIME << " a " << MAX_ERASE_TIME << "!\n";
//                    return nullptr;
//                }

            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_MAX_READ_PAGE_TIME) == 0) {
            try {
                max_read_page_time = stof(argv[i + 1]);
//                if (erase_time > MAX_ERASE_TIME || erase_time < MIN_ERASE_TIME) {
//                    *output << "Parametr musi byt cislo mezi " << MIN_ERASE_TIME << " a " << MAX_ERASE_TIME << "!\n";
//                    return nullptr;
//                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_MAX_PAGE_PROG_TIME) == 0) {
            try {
                max_page_prog_time = stof(argv[i + 1]);
//                if (erase_time > MAX_ERASE_TIME || erase_time < MIN_ERASE_TIME) {
//                    *output << "Parametr musi byt cislo mezi " << MIN_ERASE_TIME << " a " << MAX_ERASE_TIME << "!\n";
//                    return nullptr;
//                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_MAX_ERASE_TIME) == 0) {
            try {
                max_erase_time = stof(argv[i + 1]);
//                if (erase_time > MAX_ERASE_TIME || erase_time < MIN_ERASE_TIME) {
//                    *output << "Parametr musi byt cislo mezi " << MIN_ERASE_TIME << " a " << MAX_ERASE_TIME << "!\n";
//                    return nullptr;
//                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_MIN_READ_PAGE_TIME) == 0) {
            try {
                min_read_page_time = stof(argv[i + 1]);
//                if (erase_time > MAX_ERASE_TIME || erase_time < MIN_ERASE_TIME) {
//                    *output << "Parametr musi byt cislo mezi " << MIN_ERASE_TIME << " a " << MAX_ERASE_TIME << "!\n";
//                    return nullptr;
//                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_MIN_PAGE_PROG_TIME) == 0) {
            try {
                min_page_prog_time = stof(argv[i + 1]);
//                if (erase_time > MAX_ERASE_TIME || erase_time < MIN_ERASE_TIME) {
//                    *output << "Parametr musi byt cislo mezi " << MIN_ERASE_TIME << " a " << MAX_ERASE_TIME << "!\n";
//                    return nullptr;
//                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_MIN_ERASE_TIME) == 0) {
            try {
                min_erase_time = stof(argv[i + 1]);
//                if (erase_time > MAX_ERASE_TIME || erase_time < MIN_ERASE_TIME) {
//                    *output << "Parametr musi byt cislo mezi " << MIN_ERASE_TIME << " a " << MAX_ERASE_TIME << "!\n";
//                    return nullptr;
//                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        } else if (strcmp(argv[i], PARAM_COM_TIME) == 0) {
            try {
                com_time = stof(argv[i + 1]);
//                if (erase_time > MAX_ERASE_TIME || erase_time < MIN_ERASE_TIME) {
//                    *output << "Parametr musi byt cislo mezi " << MIN_ERASE_TIME << " a " << MAX_ERASE_TIME << "!\n";
//                    return nullptr;
//                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return nullptr;
            }
        }
        else if (strcmp(argv[i], PARAM_MAX_ERASE_NUM) == 0) {
            try {
                max_erase_num = stoi(argv[i + 1]);
//                if (erase_time > MAX_ERASE_TIME || erase_time < MIN_ERASE_TIME) {
//                    *output << "Parametr musi byt cislo mezi " << MIN_ERASE_TIME << " a " << MAX_ERASE_TIME << "!\n";
//                    return nullptr;
//                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
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
    return new Flash_Memory(page_size, block_size, num_of_blocks, type,
                            read_page_time,page_prog_time, erase_time);
}

int Test_Simulation::Handle_Life_Cycle(ostream *output, istream *input, Flash_Memory *flashMemory) {

    string command;
    string args[5] = {};
    int number = 0;
    float value;
    bool bool_value;

    while (true) {
        *input >> command;
        *output << "command: " << command << endl;

        if (command == string(COM_STOP)) {
            return EXIT_SUCCESS;
        } else if (command == string(COM_READ_PAGE)) {
            try {
                *input >> args[0];
                if (flashMemory->Read_Page((int16_t) stoi(args[0], nullptr, 16))) {
                    *output << "Nepodarilo se provest operaci cteni stranky.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_READ_SECTOR)) {
            try {
                *input >> args[0];
                if (flashMemory->Read_Sector((int16_t) stoi(args[0], nullptr, 16))) {
                    *output << "Nepodarilo se provest operaci cteni sektoru.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_READ_CACHE)) {
            try {
                u_char *data = flashMemory->Read_Cache();
                if (!data) {
                    *output << "Nepodarilo se provest operaci cteni cache.\n";
                    return EXIT_FAILURE;
                }
                *output << "Nactena data: " << data << endl;
                free(data);
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_READ_STATUS)) {
            u_char data = flashMemory->Read_Status();
            *output << "Status pameti je: " << (int) data << endl;
            Write_Status(output, data);
        } else if (command == string(COM_READ_ID)) {
            flashMemory->Read_ID();
            *output << "ID zařízení bylo přečteno.\n";
        } else if (command == string(COM_PROGRAM_PAGE)) {
            try {
                *input >> args[0];
                *output << "adresa: " << (int16_t) stoi(args[0], nullptr, 16) << endl;
                if (flashMemory->Program_Page((int16_t) stoi(args[0], nullptr, 16))) {
                    *output << "Nepodarilo se provest operaci zapisu.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo! " << "\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_PROGRAM_SECTOR)) {
            try {
                *input >> args[0];
                *output << "adresa: " << args[0] << endl;
                if (flashMemory->Program_Sector((int16_t) stoi(args[0], nullptr, 16))) {
                    *output << "Nepodarilo se provest operaci zapsani sektoru.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo! " << "\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_WRITE_CACHE)) {
            *input >> args[1];
            *output << "data: " << args[1] << endl;
            if (flashMemory->Write_Cache(args[1])) {
                *output << "Nepodarilo se provest operaci.\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_PROGRAM_DATA_MOVE)) {
            try {
                *input >> args[0];
                *output << "puvodni adresa: " << args[0] << endl;
                *input >> args[1];
                *output << "nova adresa: " << args[1] << endl;
                if (flashMemory->
                        Program_Data_Move((int16_t) stoi(args[0], nullptr, 16),
                                          (int16_t) stoi(args[1], nullptr, 16))) {
                    *output << "Nepodarilo se provest operaci presunuti dat.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo! " << "\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_BLOCK_ERASE)) {
            try {
                *input >> args[0];
                if (flashMemory->Block_Erase((int16_t) stoi(args[0], nullptr, 16))) {
                    *output << "Nepodarilo se provest operaci vymazani bloku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_NUM_OF_WRITES_PAGE)) {
            try {
                *input >> args[0];
                number = flashMemory->Num_Of_Writes_Page((int16_t) stoi(args[0], nullptr, 16));
                if (number >= 0) {
                    *output << "Na stranku na adrese " << args[0] << " bylo zapsano " << number << "krat.\n";
                } else {
                    *output << "Nepodarilo se provest operaci pocet zapisu stranky.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_NUM_OF_READS_PAGE)) {
            try {
                *input >> args[0];
                number = flashMemory->Num_Of_Reads_Page((int16_t) stoi(args[0], nullptr, 16));
                if (number >= 0) {
                    *output << "Stranka na adrese " << args[0] << " byla prectena " << number << "krat.\n";
                } else {
                    *output << "Nepodarilo se provest operaci pocet cteni stranky.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_ECC_INFO)) {
            try {
                *input >> args[0];
                u_char *data = flashMemory->ECC_Info((int16_t) stoi(args[0], nullptr, 16));
                if(!data) {
                    *output << "Nepodarilo se precist ECC info na adrese " << args[0] << ".\n";
                    return EXIT_FAILURE;
                } else {
                    *output << "Obsah ecc na adrese " << args[0] << ": " << data << ".\n";
                    free(data);
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_READ_TIME_LAST)) {
            try {
                *input >> args[0];
                value = flashMemory->Read_Time_Last((int16_t) stoi(args[0], nullptr, 16));
                if (value >= 0) {
                    *output << "Stranka na adrese " << args[0] << " byla prectena za " << value << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci cas posledniho cteni.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_PROG_TIME_LAST)) {
            try {
                *input >> args[0];
                value = flashMemory->Program_Time_Last((int16_t) stoi(args[0], nullptr, 16));
                if (value >= 0) {
                    *output << "Stranka na adrese " << args[0] << " byla naprogramovana za " << value << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci cas posledniho zapisu.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_READ_TIME_TOTAL)) {
            try {
                *input >> args[0];
                value = flashMemory->Read_Time_Total((int16_t) stoi(args[0], nullptr, 16));
                if (value >= 0) {
                    *output << "Stranka na adrese " << args[0] << " byla prectena za " << value << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci celkovy cas cteni.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_PROG_TIME_TOTAL)) {
            try {
                *input >> args[0];
                value = flashMemory->Program_Time_Total((int16_t) stoi(args[0], nullptr, 16));
                if (value >= 0) {
                    *output << "Stranka na adrese " << args[0] << " byla dohromady naprogramovana za " << value << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci celkovy cas zapsani.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_COM_TOTAL_TIME)) {
            try {
                *input >> args[0];
                value = flashMemory->Com_Total_Time((int16_t) stoi(args[0], nullptr, 16));
                if (value >= 0) {
                    *output << "Stranka na adrese " << args[0] << " byla dohromady com za " << value << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci celkovy cas komunikace.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_NUM_OF_ERASES_PAGE)) {
            try {
                *input >> args[0];
                number = flashMemory->Num_Of_Erases_Page((int16_t) stoi(args[0], nullptr, 16));
                if (number >= 0) {
                    *output << "Stranka na adrese " << args[0] << " byla vymazana " << number << "krat.\n";
                } else {
                    *output << "Nepodarilo se provest operaci pocet mazani pro stranku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_SECTOR_STATUS_PAGE)) {
            try {
                *input >> args[0];
                size_t *data = flashMemory->Sector_Status_Page((int16_t) stoi(args[0], nullptr, 16));
                if (!data) {
                    *output << "Status sectoru stranky je - zapis: " << data[0] << " error: " << data[1] << endl;
                    delete(data);
                } else {
                    *output << "Nepodarilo se provest operaci status sektoru v bloku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_NUM_OF_ERASES_BLOCK)) {
            try {
                *input >> args[0];
                number = flashMemory->Num_Of_Erases_Block((int16_t) stoi(args[0], nullptr, 16));
                if (number >= 0) {
                    *output << "Blok na adrese " << args[0] << " byl smazan " << number << "krat.\n";
                } else {
                    *output << "Nepodarilo se provest operaci pocet mazani bloku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_ERASE_TIME_TOTAL)) {
            try {
                *input >> args[0];
                value = flashMemory->Erase_Time_Total((int16_t) stoi(args[0], nullptr, 16));
                if (value >= 0) {
                    *output << "Blok na adrese " << args[0] << " byl dohormady smazan za " << value << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci celkovy cas mazani.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_ERASE_TIME_LAST)) {
            try {
                *input >> args[0];
                value = flashMemory->Erase_Time_Last((int16_t) stoi(args[0], nullptr, 16));
                if (value >= 0) {
                    *output << "Stranka na adrese " << args[0] << " byl smazan za " << number << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci cas posledniho mazani.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_IS_BAD_BLOCK)) {
            try {
                *input >> args[0];
                bool_value = flashMemory->Is_Bad_Block((u_int8_t) stoi(args[0], nullptr, 16));
                if (bool_value) {
                    *output << "Blok na adrese " << args[0] << " je spatny: " << bool_value << endl;
                } else {
                    *output << "Nepodarilo se provest operaci poskozeni bloku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_NUM_OF_BAD_PAGES)) {
            try {
                *input >> args[0];
                number = flashMemory->Num_Of_Bad_Pages((int16_t) stoi(args[0], nullptr, 16));
                if (number >= 0) {
                    *output << "Blok na adrese " << args[0] << " ma " << number << "poskozenych stranek.\n";
                } else {
                    *output << "Nepodarilo se provest operaci pocet spatnych stranek v bloku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_ECC_HISTOGRAM_BLOCK)) {
            try {
                *input >> args[0];
                size_t *data = flashMemory->ECC_Histogram((int16_t) stoi(args[0], nullptr, 16));
                if(data) {
                    *output << "Nepodarilo se precist ECC histogram bloku na adrese " << args[0] << ".\n";
                    return EXIT_FAILURE;
                } else {
                    *output << "Obsah ecc na adrese " << args[0];
                    for (int i = 0; i < MEMORY_ECC_SIZE; i++) {
                        *output << " " << data[i];
                    }
                    *output << endl;
                    delete(data);
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_NUM_OF_WRITES_BLOCK)) {
            try {
                *input >> args[0];
                number = flashMemory->Num_Of_Writes((int16_t) stoi(args[0], nullptr, 16));
                if (number >= 0) {
                    *output << "Do bloku na adrese " << args[0] << " bylo zapsano " << number << "krat.\n";
                } else {
                    *output << "Nepodarilo se provest operaci pocet zapsanych stranke v bloku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_NUM_OF_READS_BLOCK)) {
            try {
                *input >> args[0];
                number = flashMemory->Num_Of_Reads((int16_t) stoi(args[0], nullptr, 16));
                if (number >= 0) {
                    *output << "Z bloku na adrese " << args[0] << " bylo precteno " << number << "krat.\n";
                } else {
                    *output << "Nepodarilo se provest operaci pocet prectenych stranek v bloku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_SECTOR_STATUS_BLOCK)) {
            try {
                *input >> args[0];
                size_t *data = flashMemory->Sector_Status_Block((int16_t) stoi(args[0],
                                                                               nullptr, 16));
                if (!data) {
                    *output << "Status sectoru bloku je - mazani: " << data[0] << " zapis: " << data[1] << " error: " << data[2] << endl;
                    delete(data);
                } else {
                    *output << "Nepodarilo se provest operaci status sektoru v bloku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_NUM_OF_BAD_BLOCKS)) {
            try {
                number = flashMemory->Num_Of_Bad_Blocks();
                if (number >= 0) {
                    *output << "V pameti je " << number << "poskozenych bloku.\n";
                } else {
                    *output << "Nepodarilo se provest operaci pocet poskozenych stranek v bloku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_NUM_OF_BAD_PAGES_MEM)) {
            try {
                number = flashMemory->Num_Of_Bad_Pages();
                if (number >= 0) {
                    *output << "V pameti je " << number << "poskozenych stranek.\n";
                } else {
                    *output << "Nepodarilo se provest operaci pocet spatnych stranek v pameti.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_ECC_HISTOGRAM_MEM)) {
            try {
                *input >> args[0];
                size_t *data = flashMemory->ECC_Histogram();
                if(!data) {
                    *output << "Nepodarilo se precist ECC histogram pameti " << args[0] << ".\n";
                    return EXIT_FAILURE;
                } else {
                    *output << "Obsah ecc na adrese " << args[0];
                    for (int i = 0; i < MEMORY_ECC_SIZE; i++) {
                        *output << " " << data[i];
                    }
                    *output << endl;
                    delete(data);
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_MEM_STATUS_PAGE)) {
            try {
                size_t *data = flashMemory->Sector_Status_Memory();
                if (!data) {
                    *output << "Status sectoru pameti - zapis:" << data[0] << " error: " << data[1]  << endl;
                    delete(data);
                } else {
                    *output << "Nepodarilo se provest operaci status sektoru v bloku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_NUM_OF_WRITES_MEM)) {
            try {
                number = flashMemory->Num_Of_Writes();
                if (number >= 0) {
                    *output << "Do pameti bylo zapsano " << number << "krat.\n";
                } else {
                    *output << "Nepodarilo se provest operaci pocet zapisu v pameti.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_NUM_OF_READS_MEM)) {
            try {
                number = flashMemory->Num_Of_Reads();
                if (number >= 0) {
                    *output << "Z pameti bylo cteno " << number << "krat.\n";
                } else {
                    *output << "Nepodarilo se provest operaci pocet cteni v pameti.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_CHANGE_PROG_TIME_PAGE)) {
            try {
                *input >> args[0];
                *input >> args[1];
                if (stof(args[1]) > MAX_PAGE_PROG_TIME || stof(args[1]) < MIN_PAGE_PROG_TIME) {
                    *output << "Parametr musi byt cislo mezi " << MIN_PAGE_PROG_TIME <<
                            " a " << MAX_PAGE_PROG_TIME << "!\n";
                    return EXIT_FAILURE;
                }
                if (flashMemory->Set_Prog_Time_Page(stoi(args[0], nullptr, 16),
                                                    stof(args[1]))) {
                    *output << "Podarilo se zmenit cas naprogramovani stranky na adrese " << args[0]
                            << " na " << args[1] << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci zmena casu zapisu stranky.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_CHANGE_PROG_TIME_BLOCK)) {
            try {
                *input >> args[0];
                *input >> args[1];
                if (stof(args[1]) > MAX_PAGE_PROG_TIME || stof(args[1]) < MIN_PAGE_PROG_TIME) {
                    *output << "Parametr musi byt cislo mezi " << MIN_PAGE_PROG_TIME <<
                            " a " << MAX_PAGE_PROG_TIME << "!\n";
                    return EXIT_FAILURE;
                }
                if (flashMemory->Set_Prog_Time_Block(stoi(args[0], nullptr, 16),
                                                     stof(args[1]))) {
                    *output << "Podarilo se zmenit cas naprogramovani bloku na adrese " << args[0]
                            << " na " << args[1] << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci zmena casu zapisu v bloku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_CHANGE_PROG_TIME_MEM)) {
            try {
                *input >> args[0];
                if (stof(args[0]) > MAX_PAGE_PROG_TIME || stof(args[0]) < MIN_PAGE_PROG_TIME) {
                    *output << "Parametr musi byt cislo mezi " << MIN_PAGE_PROG_TIME <<
                            " a " << MAX_PAGE_PROG_TIME << "!\n";
                    return EXIT_FAILURE;
                }
                if (flashMemory->Set_Prog_Time_Mem(stof(args[0]))) {
                    *output << "Podarilo se zmenit cas naprogramovani cele pamti na " << args[1] << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci zmena casu zapisu v pameti.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_CHANGE_READ_TIME_PAGE)) {
            try {
                *input >> args[0];
                *input >> args[1];
                if (stof(args[1]) > MAX_READ_PAGE_TIME || stof(args[1]) < MIN_READ_PAGE_TIME) {
                    *output << "Parametr musi byt cislo mezi " << MIN_READ_PAGE_TIME <<
                            " a " << MAX_READ_PAGE_TIME << "!\n";
                    return EXIT_FAILURE;
                }
                if (flashMemory->Set_Read_Time_Page(stoi(args[0], nullptr, 16),
                                                    stof(args[1]))) {
                    *output << "Podarilo se zmenit cas cteni stranky na adrese " << args[0]
                            << " na " << args[1] << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci zmena casu cteni stranky.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_CHANGE_READ_TIME_BLOCK)) {
            try {
                *input >> args[0];
                *input >> args[1];
                if (stof(args[1]) > MAX_READ_PAGE_TIME || stof(args[1]) < MIN_READ_PAGE_TIME) {
                    *output << "Parametr musi byt cislo mezi " << MIN_READ_PAGE_TIME <<
                            " a " << MAX_READ_PAGE_TIME << "!\n";
                    return EXIT_FAILURE;
                }
                if (flashMemory->Set_Read_Time_Block(stoi(args[0], nullptr, 16),
                                                     stof(args[1]))) {
                    *output << "Podarilo se zmenit cas cteni bloku na adrese " << args[0]
                            << " na " << args[1] << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci zmena casu cteni bloku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_CHANGE_READ_TIME_MEM)) {
            try {
                *input >> args[0];
                if (stof(args[0]) > MAX_READ_PAGE_TIME || stof(args[0]) < MIN_READ_PAGE_TIME) {
                    *output << "Parametr musi byt cislo mezi " << MIN_READ_PAGE_TIME <<
                            " a " << MAX_READ_PAGE_TIME << "!\n";
                    return EXIT_FAILURE;
                }
                if (flashMemory->Set_Read_Time_Mem(stof(args[0]))) {
                    *output << "Podarilo se zmenit cas cteni cele pameti na " << args[0] << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci zmena casu cteni v pameti.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_CHANGE_ERASE_TIME_BLOCK)) {
            try {
                *input >> args[0];
                *input >> args[1];
                if (stof(args[1]) > MAX_ERASE_TIME || stof(args[1]) < MIN_ERASE_TIME) {
                    *output << "Parametr musi byt cislo mezi " << MIN_ERASE_TIME <<
                            " a " << MAX_ERASE_TIME << "!\n";
                    return EXIT_FAILURE;
                }
                if (flashMemory->Set_Erase_Time_Block(stoi(args[0], nullptr, 16),
                                                      stof(args[1]))) {
                    *output << "Podarilo se zmenit cas mazani bloku na adrese " << args[0]
                            << " na " << args[1] << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci zmena casu mazani v bloku.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_CHANGE_ERASE_TIME_MEM)) {
            try {
                *input >> args[0];
                if (stof(args[0]) > MAX_ERASE_TIME || stof(args[0]) < MIN_ERASE_TIME) {
                    *output << "Parametr musi byt cislo mezi " << MIN_ERASE_TIME <<
                            " a " << MAX_ERASE_TIME << "!\n";
                    return EXIT_FAILURE;
                }
                if (flashMemory->Set_Erase_Time_Mem(stof(args[0]))) {
                    *output << "Podarilo se zmenit cas naprogramovani cele pameti na " << args[0] << "μs.\n";
                } else {
                    *output << "Nepodarilo se provest operaci zmena casu mazani v pameti.\n";
                    return EXIT_FAILURE;
                }
            } catch (const std::invalid_argument & e) {
                *output << "Parametr musi byt cislo!\n";
                return EXIT_FAILURE;
            }
        } else if (command == string(COM_RESET)) {
            if (flashMemory->Reset()) {
                *output << "Pamet byla vyresetovana.\n";
            }
        } else if (command == string(COM_SAVE_MEM)) {
            *input >> args[0];
            if (flashMemory->Save_Memory(args[0])) {
                *output << "Obsah pameti byl ulozen.\n";
            }
        } else if (command == string(COM_SAVE_STATE)) {
            *input >> args[0];
            if (flashMemory->Save_State(args[0])) {
                *output << "Stav pameti byl ulozen.\n";
            }
        } else if (command == string(COM_LOAD_MEM)) {
            *input >> args[0];
            if (flashMemory->Load_Memory(args[0])) {
                *output << "Obsah pameti byl nacten.\n";
            }
        } else if (command == string(COM_LOAD_STATE)) {
            *input >> args[0];
            flashMemory = flashMemory->Load_State(args[0]);
            if (flashMemory) {
                *output << "Stav pameti byl nacten.\n";
            } else {
                *output << "Nepodarilo se nacist novy stav pameti.\n";
            }
        } else {
            *output << COM_UNKNOWN;
        }

        *output << "Prikaz byl dokoncen\n";
    }
}

int Test_Simulation::Run_Demostration(int argc, char **argv) {

    istream *input = &cin;
    ostream *output = &cout;
    ifstream input_file;
    ofstream output_file;

    int changed_input = 0;

    auto *flashMemory = new Flash_Memory();

    Write_Head(output);

    if (argc > 1) {
        for (int i = 0; i < (argc - 1); i++) {
            if (strcmp(argv[i], PARAM_FILE_IN) == 0) {
                changed_input++;
                input_file = ifstream(argv[i + 1]);
                if (input_file) {
                    input = &input_file;
                    cout << "Otevrel jsem input.txt soubor." << endl;
                } else
                    cout << "Nepodarilo se vstupni otevrit soubor: " << argv[i + 1] << endl;
            } else if (strcmp(argv[i], PARAM_FILE_OUT) == 0) {
                changed_input++;
                output_file = ofstream(argv[i + 1]);
                if (output_file) {
                    output = &output_file;
                    cout << "Otevrel jsem output soubor." << endl;
                } else
                    cout << "Nepodarilo se vystupni otevrit soubor: " << argv[i + 1] << endl;
            }
        }

        flashMemory = Specify_Memory(argc - changed_input, argv, output);
        if (!flashMemory) {
            *output << "Koncim" << endl;
            return EXIT_FAILURE;
        }
    }

    *output << "===========================tests-begin===========================" << endl;

//    run_tests();

    *output << "===========================tests-finish===========================" << endl;

    flashMemory->Flash_Init();
    flashMemory->Cache_Init();
    *output << "Pamet byla incializovana a je pripravena k pouziti!" << endl;

    *output << "=================================================================" << endl;

    if (Handle_Life_Cycle(output, input, flashMemory) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    *output << "Program konci.\n";

    if (input_file && input_file.is_open()) {
        cout << "Zavira se vstupni soubor.\n";
        input_file.close();
    }

    if (output_file && output_file.is_open()) {
        cout << "Zavira se vystupni soubor.\n";
        output_file.close();
    }

    return EXIT_SUCCESS;
}

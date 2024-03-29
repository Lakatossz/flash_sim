#pragma once

#include <iostream>
#include <cstring>
#include <fstream>
#include <memory>
#include <cstdlib>
#include <iterator>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "statistics/Page_Stats.h"
#include "statistics/Block_Stats.h"
#include "statistics/Memory_Stats.h"

using namespace std;

using json = nlohmann::json;

/**
 * Definice parametrů vložených při spuštění programu.
 */
#define PARAM_FILE_IN "-input_file"
#define PARAM_FILE_OUT "-output_file"
#define PARAM_BLOCK_SIZE "-block_size"
#define PARAM_PAGE_SIZE "-page_size"
#define PARAM_NUM_OF_BLOCK "-num_of_block"
#define PARAM_MEM_TYPE "-mem_type"
#define PARAM_READ_PAGE_TIME "-read_page_time"
#define PARAM_PAGE_PROG_TIME "-page_prog_time"
#define PARAM_ERASE_TIME "-erase_time"
#define PARAM_COM_TIME "-com_time"
#define PARAM_MAX_ERASE_NUM "-max_erase_unm"

/**
 * Maximální možné hodnoty parametrů.
 */
#define MAX_BLOCK_SIZE 10000000
#define MAX_PAGE_SIZE 10000000
#define MAX_SECTOR_SIZE MAX_PAGE_SIZE
#define MAX_NUM_OF_BLOCK 10000000
#define MAX_READ_PAGE_TIME 1000000
#define MAX_PAGE_PROG_TIME 1000000
#define MAX_ERASE_TIME 100000
#define MAX_ERASE_NUMBER 100000000

/**
 * Minimální možné hodnoty parametrů.
 * TODO Potřeba doupřesnit.
 */
#define MIN_BLOCK_SIZE 1
#define MIN_PAGE_SIZE 1
#define MIN_SECTOR_SIZE 1
#define MIN_NUM_OF_BLOCK 1
#define MIN_READ_PAGE_TIME 1
#define MIN_PAGE_PROG_TIME 1
#define MIN_ERASE_TIME 1

/**
 * Definové kódy příkazů.
 */
#define COM_READ_PAGE "read_page"
#define COM_READ_SECTOR "read_sector"
#define COM_READ_STATUS "read_status"
#define COM_READ_ID "read_id"
#define COM_PROGRAM_PAGE "program_page"
#define COM_PROGRAM_SECTOR "program_sector"
#define COM_PROGRAM_DATA_MOVE "program_data_move"
#define COM_BLOCK_ERASE "block_erase"
#define COM_RESET "reset"
#define COM_READ_CACHE "read_cache"
#define COM_WRITE_CACHE "write_cache"
#define COM_NUM_OF_WRITES_PAGE "number_of_writes_page"
#define COM_NUM_OF_READS_PAGE "number_of_reads_page"
#define COM_ECC_INFO "ecc_info"
#define COM_READ_TIME_LAST "read_last_time"
#define COM_PROG_TIME_LAST "program_last_time"
#define COM_READ_TIME_TOTAL "read_total_time"
#define COM_PROG_TIME_TOTAL "program_total_time"
#define COM_COM_TOTAL_TIME "comm_total_time"
#define COM_NUM_OF_ERASES_PAGE "number_of_erases_page"
#define COM_SECTOR_STATUS_BLOCK "sector_status_block"
#define COM_NUM_OF_ERASES_BLOCK "number_of_erases_block"
#define COM_ERASE_TIME_TOTAL "erase_total_time"
#define COM_ERASE_TIME_LAST "erase_last_time"
#define COM_IS_BAD_BLOCK "is_bad_block"
#define COM_NUM_OF_BAD_PAGES "number_of_bad_pages"
#define COM_ECC_HISTOGRAM_BLOCK "ecc_histogram_block"
#define COM_NUM_OF_WRITES_BLOCK "number_of_writes_block"
#define COM_NUM_OF_READS_BLOCK "number_of_reads_block"
#define COM_SECTOR_STATUS_PAGE "sector_status_page"
#define COM_NUM_OF_BAD_BLOCKS "number_of_bad_blocks"
#define COM_NUM_OF_BAD_PAGES_MEM "number_of_bad_pages_memory"
#define COM_ECC_HISTOGRAM_MEM "ecc_histogram_memory"
#define COM_MEM_STATUS_PAGE "sector_status_memory"
#define COM_NUM_OF_WRITES_MEM "number_of_writes_memory"
#define COM_NUM_OF_READS_MEM "number_of_reads_memory"
#define COM_SAVE_MEM "save_memory"
#define COM_SAVE_STATE "save_state"
#define COM_LOAD_MEM "load_memory"
#define COM_LOAD_STATE "load_state"

/**
 * Definice příkazů vložených při běhu.
 * TODO - Jak budu řešit změnu počtu zápisů při změně času?
 */
#define COM_CHANGE_PROG_TIME_PAGE "change_program_time_page"
#define COM_CHANGE_PROG_TIME_BLOCK "change_program_time_block"
#define COM_CHANGE_PROG_TIME_MEM "change_program_time_memory"
#define COM_CHANGE_READ_TIME_PAGE "change_read_time_page"
#define COM_CHANGE_READ_TIME_BLOCK "change_read_time_block"
#define COM_CHANGE_READ_TIME_MEM "change_read_time_memory"
#define COM_CHANGE_ERASE_TIME_BLOCK "change_erase_time_block"
#define COM_CHANGE_ERASE_TIME_MEM "change_erase_time_memory"

/** Příkaz ukončující běh programu. */
#define COM_STOP "STOP"

/** Odpověď na neznámý příkaz. */
#define COM_UNKNOWN "Neznamy prikaz!\n"

/**
 * Definice řetězců pro určení typu paměti jako vstupní parametr.
 */
#define MEM_TYPE_SLC "slc"
#define MEM_TYPE_MLC "mlc"
#define MEM_TYPE_TLC "tlc"
#define MEM_TYPE_QLC "qlc"

/**
 * Defaultní hodnoty parametrů.
 */
 /* Sector - minimální jednotka dat */
#define DEFAULT_SECTOR_SIZE 512
#define DEFAULT_PAGE_SIZE 1024
#define DEFAULT_BLOCK_SIZE 8192
#define DEFAULT_NUM_OF_BLOCKS 16
//#define DEFAULT_READ_PAGE_TIME 10
//#define DEFAULT_PAGE_PROG_TIME 15
//#define DEFAULT_ERASE_TIME 30
#define DEFAULT_COM_TIME 15
#define DEFAULT_BAD_BLOCKS_FACTORY 12
#define DEFAULT_ID 666

#define SECTOR_SPARE_WRITTEN 0
#define SECTOR_SPARE_ERROR 1

#define PAGE_SPARE_BAD 0
#define PAGE_SPARE_VALID 1
#define PAGE_SPARE_ERROR 2
#define PAGE_SPARE_LOCKED 3
#define PAGE_SPARE_SECURITY 4

#define BLOCK_SPARE_BAD 0
#define BLOCK_SPARE_VALID 1
#define BLOCK_SPARE_ERASED 2
#define BLOCK_SPARE_RESERVED 3
#define BLOCK_SPARE_DIRTY 4
#define BLOCK_SPARE_LOCKED 5
#define BLOCK_SPARE_WORE_OUT 6
#define BLOCK_SPARE_ERROR 7
#define BLOCK_SPARE_PROG_SUSPEND 8
#define BLOCK_SPARE_ERASE_SUSPEND 9

#define STATUS_FLAG_WP 0
#define STATUS_FLAG_BP 1
#define STATUS_FLAG_EPE 2
#define STATUS_FLAG_RB 3
#define STATUS_FLAG_ES 4
#define STATUS_FLAG_PT 5
#define STATUS_FLAG_ECC 6
#define STATUS_FLAG_ERR 7

#define READ_PAGE_ERROR_RATE 10000
#define PAGE_PROG_ERROR_RATE 100000

/**
 * Definice struktury představující konstanty typu paměti.
 * Konstanty slouží k výpočtu automatických fcí.
 */
typedef struct mem_type_values_struct {
    float error_const;
    float read_time_const;
    float prog_time_const;
    float erase_time_const;
} mem_type_values;

/**
 * Definice struktury představující typy buněk paměti.
 */
#define SLC (mem_type_values){0.1, 1, 0.1, 0.1}
#define MLC (mem_type_values){0.2, 1, 0.2, 0.2}
#define TLC (mem_type_values){0.4, 1, 0.4, 0.4}
#define QLC (mem_type_values){0.8, 1, 0.8, 0.8}

#define DEFAULT_MEM_TYPE SLC

/**
 * Definice výčtového typu pro algortimy výpočtu času.
 */
enum NInstruction_Type {
    READ_PAGE = 1,
    PAGE_PROG = 2,
    ERASE = 3,
};

typedef struct nand_metadata_struct {
    // Statické parametry paměti.
    size_t id{}; /** Identifikátor paměti. */
    size_t sector_size = DEFAULT_SECTOR_SIZE; /** Velikost sektoru stránky. */
    size_t num_of_sectors = DEFAULT_PAGE_SIZE / DEFAULT_SECTOR_SIZE; /** Počet sektorů. */
    size_t page_size = DEFAULT_PAGE_SIZE; /** Velikost stránky. */
    size_t num_of_pages = DEFAULT_BLOCK_SIZE / DEFAULT_PAGE_SIZE; /** Počet stránek. */
    size_t block_size = DEFAULT_BLOCK_SIZE; /** Velikost bloku. */
    size_t num_of_blocks = DEFAULT_NUM_OF_BLOCKS; /** Počet bloků. */
    size_t mem_size = 0; /** Velikost paměti. */
    u_int32_t true_mem_size = 0; /** Počet bytů pameti. */
    u_int32_t ecc_size = 16; /** Počet bitů ECC kódu. */
    size_t block_wear_size = 32; /** Velikost bitů pro pole počítadla wear-levelingu. */
    size_t md_s_size = 1; /** Celkový počet metdat jednoho sektoru. */
    size_t md_p_size = 3; /** Celkový počet metadat jedné stránky 16b pro ECC + 8b navic napr pro valid bit. */
    size_t md_b_size = 5 + block_wear_size; /** Celkový počet metadat jednoho bloku. */
    u_char status = 0; /** Status registr. */
    size_t status_bits = 8; /** Počet bitů status registru. */
    mem_type_values mem_type = SLC; /** Typ paměti - určuje velikost buňky. */

    // Dynamické parametry paměti.
    float mem_time = 0; /** Doba běhu paměti v μs. */
    Memory_Stats *mem_stats = new Memory_Stats();
    size_t num_of_bad_blocks = 0; /** Počet poškozených bloků v paměti. */
    size_t num_of_bad_pages = 0; /** Počet poškozených stránek v paměti. */
    Block_Stats *blocks_stats = nullptr; /** Pole statistik bloků. */
    Page_Stats *pages_stats = nullptr; /** Pole statistik stránek. */
} nand_metadata;

/**
 * Definice struktury paměti.
 */
typedef struct nand_memory_struct {
    u_char *data = nullptr; /** Obsah paměti. */
    nand_metadata md = { 0 }; /** Metadata paměti. */
    u_char *mem_cache = nullptr; /** Cache paměti. */
} nand_memory;

/**
 * Definice třídy představijící flash paměť.
 */
class Flash_Memory {

public:

    nand_memory m = { nullptr };

public:

    /**
     * Vytvoří defaultní paměť.
     */
    Flash_Memory();

    /**
     * Vytvoří paměť s parametry danými vstupem programu.
     * Tato metoda slouží pro prvotní definici simulátoru
     * spolu se vstupnimi parametry.
     */
    Flash_Memory(size_t page_size,
                 size_t block_size,
                 size_t number_of_blocks,
                 mem_type_values memory_type,
                 float read_page_time,
                 float page_prog_time,
                 float erase_time);

    /**
     * Vytvoří paměť s parametry danými z načtení ze souboru.
     * Metoda slouží k předefinování parametrů paměti
     * - např. načtení paměti ze souboru.
     */
    Flash_Memory(size_t page_size,
                 size_t block_size,
                 size_t number_of_blocks,
                 mem_type_values memory_type);

    /**
     * Uvolní všechnu alokovanou paměť.
     */
    ~Flash_Memory();

    /**
     * Inicializuje paměť a připraví ji k použití.
     * Slouží k prvotní inicializaci paměti.
     */
    int Flash_Init();

    /**
     * Připraví blok cache k použití.
     */
    int Cache_Init();

    /**
     * Vrátí data obsažené ve zvolené stránce do cache.
     */
    int Read_Page(u_int32_t addr);

    /**
     * Přečte sector na dané adrese do cache.
     */
    int Read_Sector(u_int32_t addr);

    /**
     * Přečte sector z cache.
     */
    int Read_Sector_From_Cache(u_int32_t addr);

    /**
     * Přečte obsahe cache paměti.
     */
    u_char* Read_Cache() const;

    /**
     * Vrátí základní informace o statusu paměti a operaci.
     */
    u_char Read_Status() const;

    /**
     * Vráti size_t id zařízení.
     */
    size_t Read_ID();

    /**
     * Nastaví data do stránky dané adresou obsahem cache.
     */
    int Program_Page(u_int32_t addr);

    /**
     * Zapíše data do sectoru o velikosti @DEFAULT_SECTOR_SIZE obsahem cache.
     */
    int Program_Sector(u_int32_t addr);

    /**
     * Zapíše data do cache.
     */
    int Write_Cache(const string& data) const;

    /**
     * Přesune data uvnitř paměti.
     */
    int Program_Data_Move(u_int32_t old_addr, u_int32_t new_addr);

    /**
     * Vymaže blok na dane adrese.
     * Adresa ve formatu 16bit čísla: ||-|-|-|-|-Blok-|-|-|-|-|-|-|-Adresa-|-|-|-|-||
     */
    int Block_Erase(u_int32_t block_address);

    /**
     * Uvede paměť do původního stavu - paměť je prázdná.
     * Všechna data uvnitř paměti budou ztracena.
     */
    int Reset();

    /**
     * Vrátí počet zápisu do stránky na dané adrese.
     */
    size_t Num_Of_Writes(u_int32_t addr) const;

    /**
     * Vrátí počet čtení ze stránky na dané adrese.
     */
    size_t Num_Of_Reads(u_int32_t addr) const;

    /**
     * Vrátí ECC infromace stránky na dané adrese.
     */
    u_char * ECC_Info(u_int32_t addr) const;

    /**
     * Vrátí čas posledního čtení ze stránky na dané adrese.
     */
    float Read_Time_Last(u_int32_t addr) const;

    /**
     * Vrátí čas posledního zápisu do stránky na dané adrese.
     */
    float Program_Time_Last(u_int32_t addr) const;

    /**
     * Vrátí celkový čas čtení ze stránky na dané adrese.
     */
    float Read_Time_Total(u_int32_t addr) const;

    /**
     * Vrátí celkový čas zápisu do stránky na dané adrese.
     */
    float Program_Time_Total(u_int32_t addr) const;

    /**
     * Vrátí čas komunikace se stránkou na dané adrese.
     */
    float Com_Total_Time(u_int32_t addr) const;

    /**
     * Vrátí počet mazání stránky na dané adrese.
     */
    size_t Num_Of_Erases_Page(u_int32_t addr) const;

    /**
     * Vrátí sector status bloku na dané adrese.
     */
    size_t * Sector_Status_Block(u_int32_t addr) const;

    /**
     * Vrátí počet mazání bloku na dané adrese.
     */
    size_t Num_Of_Erases_Block(u_int32_t addr) const;

    /**
     * Vrátí celkový čas mazání bloku na dané adrese.
     */
    float Erase_Time_Total(u_int32_t addr) const;

    /**
     * Vrátí čas posledního mazání bloku na dané adrese.
     */
    float Erase_Time_Last(u_int32_t addr) const;

    /**
     * Může být použit pro zjištění, zda-li je blok použitelný.
     */
    bool Is_Bad_Block(u_int32_t addr) const;

    /**
     * Vrátí počet poškozených stránek v bloku na dané adrese.
     */
    size_t Num_Of_Bad_Pages(u_int32_t addr) const;

    /**
     * Histogram pro ECC všech stránek v bloku na dané adrese.
     */
    size_t * ECC_Histogram(u_int32_t addr) const;

    /**
     * Vrátí počet zápisů do stránek v bloku na dané adrese.
     */
    size_t Num_Of_Writes_Page(u_int32_t addr) const;

    /**
     * Vrátí počet čtení stránek v bloku na dané adrese.
     */
    size_t Num_Of_Reads_Page(u_int32_t addr) const;

    /**
     * Vrátí sector statusy v bloku na dané adrese.
     */
    size_t * Sector_Status_Page(u_int32_t addr) const;

    /**
     * Vrátí počet poškozených bloků v paměti.
     */
    size_t Num_Of_Bad_Blocks() const;

    /**
     * Vrátí počet poškozených stránek v paměti.
     */
    size_t Num_Of_Bad_Pages() const;

    /**
     * Vrátí ECC histrogram ze všech stránek v paměti.
     */
    size_t * ECC_Histogram();

    /**
     * Vrátí sector statusy celé paměti.
     */
    size_t * Sector_Status_Memory() const;

    /**
     * Vrátí počet zápisů do paměti.
     */
    size_t Num_Of_Writes() const;

    /**
     * Vrátí počet čtení z paměti.
     */
    size_t Num_Of_Reads() const;

    /**
     * Nastaví čas zápisu do stránky na dané adrese.
     */
    int Set_Prog_Time_Page(u_int32_t addr, float time) const;

    /**
     * Nastaví čas zápisu v celém bloku na dané adrese.
     */
    int Set_Prog_Time_Block(u_int32_t addr, float time) const;

    /**
     * Nastaví čas zápisu v paměti.
     */
    int Set_Prog_Time_Mem(float time) const;

    /**
     * Nastaví čas čtení do stránky na dané adrese.
     */
    int Set_Read_Time_Page(u_int32_t addr, float time) const;

    /**
     * Nastaví čas čtení v celém bloku na dané adrese.
     */
    int Set_Read_Time_Block(u_int32_t addr, float time) const;

    /**
     * Nastaví čas čtení v paměti.
     */
    int Set_Read_Time_Mem(float time) const;

    /**
     * Nastaví čas mazání v celém bloku na dané adrese.
     */
    int Set_Erase_Time_Block(u_int32_t addr, float time) const;

    /**
     * Nastaví čas mazání v paměti.
     */
    int Set_Erase_Time_Mem(float time) const;

    /**
     * Uloží data paměti do souboru.
     */
    int Save_Memory(const string& file_name) const;

    /**
     * Uloží stav paměti do souboru.
     */
    int Save_State(const string& file_name);

    /**
     * Načte data paměti ze souboru.
     */
    int Load_Memory(const string& file_name) const;

    /**
     * Načte stav paměti ze souboru.
     */
    Flash_Memory * Load_State(const string& file_name) const;

    void Set_Id(size_t id);

    size_t Get_True_Mem_Size() const;

private:

    void Simulate_Error(u_int32_t addr, NInstruction_Type type);

    void Increase_Time(u_int32_t addr, NInstruction_Type type);
};

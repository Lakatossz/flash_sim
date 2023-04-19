#pragma once

#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <uuid/uuid.h>
#include "../include/flash/statistics/Page_Stats.h"
#include "../include/flash/statistics/Block_Stats.h"
#include "../include/flash/statistics/Memory_Stats.h"

typedef u_int32_t int_32;
typedef u_int8_t int_8;
typedef u_int16_t int_16;
using namespace std;

/**
 * Definice parametrů vložených při spuštění programu.
 */
#define PARAM_FILE_IN "-input_file"
#define PARAM_FILE_OUT "-output_file"
#define PARAM_BLOCK_SIZE "-block_size"
#define PARAM_PAGE_SIZE "-page_size"
#define PARAM_NUM_OF_BLOCK "-num_of_block"
#define PARAM_MEM_TYPE "-mem_type"

/**
 * Definice parametrů vložených při spuštění.
 */
#define PARAM_READ_PAGE_TIME "-read_page_time"
#define PARAM_PAGE_PROG_TIME "-page_prog_time"
#define PARAM_ERASE_TIME "-erase_time"

/**
 * Maximální možné hodnoty parametrů.
 * TODO Potřeba doupřesnit.
 */
#define MAX_BLOCK_SIZE 10000
#define MAX_PAGE_SIZE 1024
#define MAX_NUM_OF_BLOCK 128
#define MAX_READ_PAGE_TIME 1000
#define MAX_PAGE_PROG_TIME 1000
#define MAX_ERASE_TIME 1000
#define MAX_ERASE_NUMBER 100000

/**
 * Minimální možné hodnoty parametrů.
 * TODO Potřeba doupřesnit.
 */
#define MIN_BLOCK_SIZE 1
#define MIN_PAGE_SIZE 1
#define MIN_NUM_OF_BLOCK 1
#define MIN_READ_PAGE_TIME 1
#define MIN_PAGE_PROG_TIME 1
#define MIN_ERASE_TIME 1

/**
 * Definové kódy příkazů.
 */
#define COM_READ_PAGE "0x01"
#define COM_READ_SECTOR "0x02"
#define COM_READ_STATUS "0x03"
#define COM_READ_ID "0x04"
#define COM_PROGRAM_PAGE "0x05"
#define COM_PROGRAM_SECTOR "0x06"
#define COM_PROGRAM_DATA_MOVE "0x07"
#define COM_BLOCK_ERASE "0x08"
#define COM_RESET "0x09"
#define COM_READ_CACHE "0x0a"
#define COM_WRITE_CACHE "0x0b"
#define COM_NUM_OF_WRITES "0x0c"
#define COM_NUM_OF_READS "0x0d"
#define COM_ECC_INFO "0x0e"
#define COM_READ_TIMES_LAST "0x0f"
#define COM_PROG_TIME_LAST "0x10"
#define COM_READ_TIME_TOTAL "0x11"
#define COM_PROG_TIME_TOTAL "0x12"
#define COM_COM_TOTAL_TIME "0x13"
#define COM_NUM_OF_ERASES_PAGE "0x14"
#define COM_SECTOR_STATUS_BLOCK "0x15"
#define COM_NUM_OF_ERASES_BLOCK "0x16"
#define COM_ERASE_TIME_TOTAL "0x17"
#define COM_ERASE_TIME_LAST "0x18"
#define COM_IS_BAD_BLOCK "0x19"
#define COM_NUM_OF_BAD_PAGES "0x1a"
#define COM_ECC_HISTOGRAM "0x1b"
#define COM_NUM_OF_WRITES_PAGE "0x1c"
#define COM_NUM_OF_READS_PAGE "0x1d"
#define COM_SECTOR_STATUS_PAGE "0x1e"
#define COM_NUM_OF_BAD_BLOCKS "0x1f"
#define COM_NUM_OF_BAD_PAGES_MEM "0x20"
#define COM_ECC_HISTOGRAM_MEM "0x21"
#define COM_NUM_OF_WRITES_MEM "0x22"
#define COM_NUM_OF_READS_MEM "0x23"

/**
 * Definice příkazů vložených při běhu.
 * TODO - Jak budu řešit změnu počtu zápisů při změně času?
 */
#define COM_CHANGE_PROG_TIME_PAGE "0x24"
#define COM_CHANGE_PROG_TIME_BLOCK "0x25"
#define COM_CHANGE_PROG_TIME_MEM "0x26"
#define COM_CHANGE_READ_TIME_PAGE "0x27"
#define COM_CHANGE_READ_TIME_BLOCK "0x28"
#define COM_CHANGE_READ_TIME_MEM "0x29"
#define COM_CHANGE_ERASE_TIME_BLOCK "0x2a"
#define COM_CHANGE_ERASE_TIME_MEM "0x2b"

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
#define DEFAULT_PAGE_SIZE 512
#define DEFAULT_BLOCK_SIZE 8192
#define DEFAULT_NUM_OF_BLOCKS 16
//#define DEFAULT_READ_PAGE_TIME 10
//#define DEFAULT_PAGE_PROG_TIME 15
//#define DEFAULT_ERASE_TIME 30
#define DEFAULT_MEM_TYPE NMem_Type::SLC

/**
 * Definice výčtového typu představující typ buňky paměti.
 * Do budoucna by mohl určovat poruchovost, chybovost, životnost.
 */
enum class NMem_Type {
    SLC = 1,
    MLC = 2,
    TLC = 3,
    QLC = 4,
};

/**
 * Definice výčtového typu pro algortimy výpočtu času.
 */
enum NTime_Type {
    READ_PAGE_TIME = 1,
    PAGE_PROG_TIME = 2,
    ERASE_TIME = 3,
};

/*typedef enum Block_Metadata_Struct { // 8 + 32 + 16?
    BLOCK_VALID = 0,
    BLOCK_BAD = 1,
    BLOCK_WEAR = 2, // 32
    BLOCK_BAD_PAGES = 16, // 16
} Block_Metadata;

typedef enum Page_Metadata_Enum { // 8 + 16
    PAGE_VALID = 0,
    PAGE_ECC = 1, // 16 bitů
    PAGE_WRITE_NUM = 16, //
} Page_Metadata;*/

typedef struct nand_metadata_struct {
    // Statické parametry paměti.
    uuid_t id{}; /** Identifikátor paměti. */
    size_t page_size = DEFAULT_PAGE_SIZE; /** Velikost stránky. */
    int_32 num_of_pages = 0; /** Počet stránek. */
    size_t block_size = DEFAULT_BLOCK_SIZE; /** Velikost bloku. */
    int_32 num_of_blocks = DEFAULT_NUM_OF_BLOCKS; /** Počet bloků. */
    size_t mem_size = 0; /** Velikost paměti. */
    int_32 true_mem_size = 0; /** Počet bytů pameti. */
    int_32 ecc_size = 16; /** Počet bitů ECC kódu. */
    size_t md_p_size = 3; /** Celkový počet metadat jedné stránky. */
    size_t md_b_size = 5; /** Celkový počet metadat jednoho bloku. */
    size_t block_wear_size = 32; /** Velikost bitů pro pole počítadla wear-levelingu. */
    u_char status = 0; /** 0. Device busy | 1. WEL | 5. EPE | 6. EPS | 7. ETM */
    NMem_Type memory_type = NMem_Type::SLC; /** Typ paměti - určuje velikost buňky. */

    // Dynamické parametry paměti.
    int_32 mem_time = 0; /** Doba běhu paměti v μs. */
    Memory_Stats *memory_stats = new Memory_Stats();
    int num_of_bad_blocks = 0; /** Počet poškozených bloků v paměti. */
    int num_of_bad_pages = 0; /** Počet poškozených stránek v paměti. */
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
     */
    Flash_Memory(int_32 page_size,
                 int_32 block_size,
                 int_32 number_of_blocks,
                 NMem_Type memory_type,
                 float read_page_time,
                 float page_prog_time,
                 float erase_time);

    /**
     * Uvolní všechnu alokovanou paměť.
     */
    ~Flash_Memory();

    /**
     * Inicializuje paměť a připraví ji k použití. Vrátí ID chipu.
     */
    int Flash_Init();

    /**
     * Připraví blok cache k použití.
     */
    int Cache_Init();

    /**
     * Vrátí data obsažené ve zvolené stránce do cache.
     */
    int Read_Page(int_16 addr);

    /**
     * Přečte sector o velikosti 512B na dané adrese do cache.
     */
    int Read_Sector(int_16 addr);

    /**
     * Přečte obsahe cache paměti.
     */
    u_char* Read_Cache() const;

    /**
     * Vrátí základní informace o statusu paměti a operaci.
     */
    u_char Read_Status() const;

    /**
     * Vráti UUID (128b) zařízení.
     */
    uuid_t* Read_ID();

    /**
     * Nastaví data do stránky dané adresou obsahem cache.
     */
    int Program_Page(int_16 addr);

    /**
     * Zapíše data do sectoru o velikosti 512B obsahem cache.
     */
    int Program_Sector(int_16 addr);

    /**
     * Zapíše data do cache.
     */
    int Write_Cache(const string& data) const;

    /**
     * Přesune data uvnitř paměti.
     */
    int Program_Data_Move(int_16 old_addr, int_16 new_addr);

    /**
     * Vymaže blok na dane adrese.
     * Adresa ve formatu 16bit čísla: ||-|-|-|-|-Blok-|-|-|-|-|-|-|-Adresa-|-|-|-|-||
     */
    int Block_Erase(int_8 block_address);

    /**
     * Uvede paměť do původního stavu - paměť je prázdná.
     * Všechna data uvnitř paměti budou ztracena.
     */
    int Reset();

    /**
     * Vrátí počet zápisu do stránky na dané adrese.
     */
    int Num_Of_Writes(int_16 addr) const;

    /**
     * Vrátí počet čtení ze stránky na dané adrese.
     */
    int Num_Of_Reads(int_16 addr) const;

    /**
     * Vrátí ECC infromace stránky na dané adrese.
     */
    u_char * ECC_Info(int_16 addr) const;

    /**
     * Vrátí čas posledního čtení ze stránky na dané adrese.
     */
    float Read_Time_Last(int_16 addr) const;

    /**
     * Vrátí čas posledního zápisu do stránky na dané adrese.
     */
    float Program_Time_Last(int_16 addr) const;

    /**
     * Vrátí celkový čas čtení ze stránky na dané adrese.
     */
    float Read_Time_Total(int_16 addr) const;

    /**
     * Vrátí celkový čas zápisu do stránky na dané adrese.
     */
    float Program_Time_Total(int_16 addr) const;

    /**
     * Vrátí čas komunikace se stránkou na dané adrese.
     */
    float Com_Total_Time(int_16 addr) const;

    /**
     * Vrátí počet mazání stránky na dané adrese.
     */
    int Num_Of_Erases_Page(int_16 addr) const;

    /**
     * Vrátí sector status bloku na dané adrese. TODO
     */
    u_char * Sector_Status_Block(int_16 addr) const;

    /**
     * Vrátí počet mazání bloku na dané adrese.
     */
    int Num_Of_Erases_Block(int_16 addr) const;

    /**
     * Vrátí celkový čas mazání bloku na dané adrese.
     */
    float Erase_Time_Total(int_16 addr) const;

    /**
     * Vrátí čas posledního mazání bloku na dané adrese.
     */
    float Erase_Time_Last(int_16 addr) const;

    /**
     * Může být použit pro zjištění, zda-li je blok použitelný.
     */
    bool Is_Bad_Block(int_16 addr) const;

    /**
     * Vrátí počet poškozených stránek v bloku na dané adrese.
     */
    int Num_Of_Bad_Pages(int_16 addr) const;

    /**
     * Histogram pro ECC všech stránek v bloku na dané adrese.
     */
    string ECC_Histogram(int_16 addr) const;

    /**
     * Vrátí počet zápisů do stránek v bloku na dané adrese.
     */
    int Num_Of_Writes_Page(int_16 addr) const;

    /**
     * Vrátí počet čtení stránek v bloku na dané adrese.
     */
    int Num_Of_Reads_Page(int_16 addr) const;

    /**
     * Vrátí sector statusy stránek v bloku na dané adrese. TODO
     */
    u_char * Sector_Status_Page(int_16 addr) const;

    /**
     * Vrátí počet poškozených bloků v paměti.
     */
    int Num_Of_Bad_Blocks() const;

    /**
     * Vrátí počet poškozených stránek v paměti.
     */
    int Num_Of_Bad_Pages() const;

    /**
     * Vrátí ECC histrogram ze všech stránek v paměti.
     */
    string ECC_Histogram();

    /**
     * Vrátí počet zápisů do paměti.
     */
    int Num_Of_Writes() const;

    /**
     * Vrátí počet čtení z paměti.
     */
    int Num_Of_Reads() const;

    /**
     * Nastaví čas zápisu do stránky na dané adrese.
     */
    int Set_Prog_Time_Page(int_16 addr, float time) const;

    /**
     * Nastaví čas zápisu v celém bloku na dané adrese.
     */
    int Set_Prog_Time_Block(int_16 addr, float time) const;

    /**
     * Nastaví čas zápisu v paměti.
     */
    int Set_Prog_Time_Mem(float time) const;

    /**
     * Nastaví čas čtení do stránky na dané adrese.
     */
    int Set_Read_Time_Page(int_16 addr, float time) const;

    /**
     * Nastaví čas čtení v celém bloku na dané adrese.
     */
    int Set_Read_Time_Block(int_16 addr, float time) const;

    /**
     * Nastaví čas čtení v paměti.
     */
    int Set_Read_Time_Mem(float time) const;

    /**
     * Nastaví čas mazání v celém bloku na dané adrese.
     */
    int Set_Erase_Time_Block(int_16 addr, float time) const;

    /**
     * Nastaví čas mazání v paměti.
     */
    int Set_Erase_Time_Mem(float time) const;
};

#pragma once

#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <uuid/uuid.h>

typedef u_int32_t int_32;
typedef u_int8_t int_8;
typedef u_int16_t int_16;
using namespace std;

/**
 * Definice parametrů, vložených při spuštění programu.
 */
#define PARAM_FILE_IN "-input_file"
#define PARAM_FILE_OUT "-output_file"
#define PARAM_BLOCK_SIZE "-block_size"
#define PARAM_PAGE_SIZE "-page_size"
#define PARAM_NUM_OF_BLOCK "-num_of_block"
#define PARAM_MEM_TYPE "-mem_type"

/**
 * Definice parametrů, vložených buď při spuštění nebo při běhu.
 * TODO
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
#define COM_READ_PAGE "0x03"
#define COM_READ_STATUS "0x04"
#define COM_READ_ID "0x05"
#define COM_PROGRAM_PAGE "0x06"
#define COM_PROGRAM_DATA_MOVE "0x07"
#define COM_BLOCK_ERASE "0x08"
#define COM_RESET "0x09"
#define COM_RANDOM_DATA_READ "0x0a"
#define COM_RANDOM_DATA_INPUT "0x0b"

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
#define DEFAULT_PAGE_SIZE 512
#define DEFAULT_BLOCK_SIZE 8192
#define DEFAULT_NUM_OF_BLOCKS 16
#define DEFAULT_READ_PAGE_TIME 10
#define DEFAULT_PAGE_PROG_TIME 15
#define DEFAULT_ERASE_TIME 30
#define DEFAULT_MEM_TYPE SLC

/**
 * Definice výštového typu představující typ buňky paměti.
 * Do budoucna by mohl určovat poruchovost, chybovost, životnost.
 */
typedef enum Mem_Type_Enum {
    SLC = 1,
    MLC = 2,
    TLC = 3,
    QLC = 4,
} Mem_Type;

typedef enum Block_Metadata_Enum {
    BLOCK_VALID = 0,
    BLOCK_BAD = 1,
    BLOCK_WEAR = 2,
    BLOCK_BAD_PAGES = 16,
} Block_Metadata;

typedef enum Page_Metadata_Enum {
    PAGE_VALID = 0,
    PAGE_ECC = 1,
    PAGE_WRITE_NUM = 16,
    PAGE_READ_NUM = 32,
} Page_Metadata;

/**
 *
 */
typedef enum Time_Type {
    READ_PAGE_TIME,
    PAGE_PROG_TIME,
    ERASE_TIME,
} Time_Type;

typedef struct page_statistics_struct {
    float read_page_time = DEFAULT_READ_PAGE_TIME; /** Čas čtení stránky. */
    float page_prog_time = DEFAULT_PAGE_PROG_TIME; /** Čas naprogramování stránky. */
    float last_read_page_time = 0; /** Poslední čas čtení stránky. */
    float last_page_prog_time = 0; /** Poslední čas naprogramování stránky. */
    float total_read_page_time = 0; /** Celkový čas čtení stránky. */
    float total_page_prog_time = 0; /** Celkový čas naprogramování stránky. */
    float com_time;
} page_statistics;

typedef struct block_statistics_struct {
    float erase_time = DEFAULT_ERASE_TIME; /** Čas smazání bloku. */
    float last_erase_time = 0; /** Poslední čas smazání bloku. */
    float total_erase_time = 0; /** Celkový čas smazání bloku. */
} block_statistics;

typedef struct nand_metadata_struct {
    uuid_t id{}; /** Identifikátor paměti. */
    block_statistics *blocks_stats = nullptr;
    page_statistics *pages_stats = nullptr;
    int_32 page_size = DEFAULT_PAGE_SIZE; /** Velikost stránky. */
    int_32 num_of_pages = 0; /** Počet stránek. */
    int_32 block_size = DEFAULT_BLOCK_SIZE; /** Velikost bloku. */
    int_32 num_of_blocks = DEFAULT_NUM_OF_BLOCKS; /** Počet bloků. */
    int_32 mem_size = 0; /** Velikost paměti. */
    int_32 true_mem_size = 0; /** Počet bytů pameti. */
    int_32 ecc_size = 0; /** Počet bitů ECC kódu. */
    int_32 md_p_size = 0; /** Celkový počet metadat jedné stránky. */
    int_32 md_b_size = 0; /** Celkový počet metadat jednoho bloku. */
    int_32 block_wear_size = 0;
    u_char status = 0; /** 0. Device busy | 1. WEL | 5. EPE | 6. EPS | 7. ETM */
    Mem_Type memory_type = DEFAULT_MEM_TYPE; /** Typ paměti - určuje velikost buňky. */
    int_32 mem_time = 0; /** Doba běhu paměti v μs. */
} nand_metadata;

/**
 * Definice struktury paměti.
 */
typedef struct nand_memory_struct {
    u_char *data = nullptr; /** Obsah paměti. */
    nand_metadata md = { 0 }; /** Metadata paměti. */
    u_char *mem_cache = nullptr;
} nand_memory;


class Flash_Memory {

public:

    nand_memory m = { nullptr };

public:

    Flash_Memory();

    Flash_Memory(int_32 page_size, int_32 block_size, int_32 number_of_blocks, Mem_Type memory_type,
                 float read_page_time, float page_prog_time, float erase_time);

    ~Flash_Memory();

    bool Init();

    /**
     * Vrátí data obsazené ve zvolené stránce do cache.
     */
    u_char* Read_Page(int_16 addr);

    /**
     * Vrátí základní informace o statusu paměti a operaci.
     */
    u_char Read_Status() const;

    /**
     * Vráti UUID (128b) zařízení.
     */
    uuid_t* Read_ID();

    /**
     * Nastaví data do stránky dané adresou.
     */
    void Program_Page(int_16 addr, const string& data);

    /**
     * Přesune data uvnitř paměti.
     */
    void Program_Data_Move(int_16 old_addr, int_16 new_addr);

    /**
     * Vymaže blok na dane adrese.
     * @param block_address Adresa bloku (Měla by být uvedena v LUN).
     * Adresa ve formatu 16bit čísla: ||-|-|-|-|-Blok-|-|-|-|-|-|-|-Adresa-|-|-|-|-||
     */
    void Block_Erase(int_8 block_address);

    /**
     * Uvede paměť do původního stavu - paměť je prázdná.
     */
    void Reset();

    /**
     * Vrátí data obsazené ve náhodné stránce.
     */
    u_char* Random_Data_Read();

    /**
     * Nastaví náhodná data až o velikosti stránky do page registru
     */
    void Random_Data_Input();

    int_32 Num_Of_Writes(int_16 addr);

    int_32 Num_Of_Reads(int_16 addr);

    u_char * ECC_Info(int_16 addr);

    float Read_Time_Last(int_16 addr);

    float Program_Time_Last(int_16 addr);

    float Read_Time_Total(int_16 addr);

    float Program_Time_Total(int_16 addr);

    float Com_Total_Time(int_16 addr);

    int_32 Num_Of_Erases_Page(int_16 addr);

    u_char * Sector_Status_Block(int_16 addr);

    int_32 Num_Of_Erases_Block(int_16 addr);

    float Erase_Time_Total(int_16 addr);

    float Erase_Time_Last(int_16 addr);

    bool Is_Bad_Block(int_16 addr);

    int_32 Num_Of_Bad_Pages(int_16 addr);

    u_char * ECC_Histogram(int_16 addr);

    int_32 Num_Of_Writes_Page(int_16 addr);

    int_32 Num_Of_Reads_Page(int_16 addr);

    u_char * Sector_Status_Page(int_16 addr);

    int_32 Num_Of_Bad_Blocks();

    int_32 Num_Of_Bad_Pages();

    u_char * ECC_Histogram();

    int_32 Num_Of_Writes();

    int_32 Num_Of_Reads();
};

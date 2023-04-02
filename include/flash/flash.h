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

#define MEM_TYPE_SLC "slc"
#define MEM_TYPE_MLC "mlc"
#define MEM_TYPE_TLC "tlc"
#define MEM_TYPE_QLC "qlc"

#define DEFAULT_PAGE_SIZE 512
#define DEFAULT_BLOCK_SIZE 16
#define DEFAULT_READ_PAGE_TIME 10
#define DEFAULT_PAGE_PROG_TIME 15
#define DEFAULT_ERASE_TIME 30
#define DEFAULT_MEM_TYPE SLC

#define MAX_ERASE_NUMBER 100000

/**
 * Definice výštového typu představující typ buňky paměti.
 * Do budoucna by mohl určovat poruchovost, chybovost, životnost.
 */
typedef enum mem_type_enum {
    SLC = 1,
    MLC = 2,
    TLC = 3,
    QLC = 4,
} mem_type;

typedef enum Page_Metadata {
    VALID = 0,
    ECC = 1,
    WEAR = 16,
} Page_Metadata;

typedef enum Time_Type {
    READ_PAGE_TIME,
    PAGE_PROG_TIME,
    ERASE_TIME,
} Time_Type;

typedef struct nand_metadata_struct {
    uuid_t id{}; /** Identifikátor paměti. */
    int_32 read_page_time = DEFAULT_READ_PAGE_TIME;
    int_32 page_prog_time = DEFAULT_PAGE_PROG_TIME;
    int_32 erase_time = DEFAULT_ERASE_TIME;
    int_32 page_size = 0; /** Velikost stránky. */
    int_32 num_of_pages = 0; /** Pocet stránek. */
    int_32 block_size = 0; /** Velikost bloku. */
    int_32 num_of_blocks = 0; /** Počet bloků. */
    int_32 mem_size = 0; /** Veliskot paměti. */
    int_32 true_mem_size = 0; /** Pocet bitu pameti. */
    int_32 ecc_size = 0; /** Počet bitů ECC kódu. */
    int_32 md_p_size = 0; /** Celkovy pocet metadat jedne stranky. */
    int_32 md_b_size = 0; /** Celkovy pocet metadat jednoho bloku. */
    u_char status = 0; /** 0. Device busy | 1. WEL | 5. EPE | 6. EPS | 7. ETM */
    mem_type memory_type = DEFAULT_MEM_TYPE; /** Typ paměti - určuje velikost buňky. */
    int_32 mem_time = 0; /** Cas behu pameti. */
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

private:

    nand_memory m = { nullptr };

public:

    Flash_Memory();

    Flash_Memory(int_32 page_size, int_32 block_size, mem_type memory_type,
                 int_32 read_page_time, int_32 page_prog_time, int_32 erase_time);

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
    void Program_Page(int_16 addr, string data);

    /**
     * Přesune data uvnitř paměti.
     */
    void Program_Data_Move(int_32 old_row_addr, int_32 new_row_addr);

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
};

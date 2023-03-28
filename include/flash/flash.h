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

#define DEFAULT_PAGE_SIZE 512
#define DEFAULT_BLOCK_SIZE 16
#define DEFAULT_MEMORY_ID 0

#define MAX_ERASE_NUMBER 100000

/**
 * Definice výčtového typu možných typů velikosti stránek.
 */
typedef enum page_size_struct {
    size_256 = 256,
    size_512 = 512,
    size_2048 = 2048
} page_size;

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

typedef enum metadata {
    VALID = 0,
    ECC = 1,
    WEAR = 16,
}metadata;

/**
 * Definice struktury buňky paměti.
 */
typedef struct cell_struct {
    int_32 id = 0; /** Identifikátor buňky. */
    bool *value = nullptr; /** Hodnota uložená v buňce */
} cell;

/**
 * Definice struktury metadat stránky.
 */
typedef struct page_metadata_struct {
    int_32 id = 0; /** Identifikátor stránky. */
    int_32 erase_number = 0; /** Počet smazání stránky. */
    int_32 data_age = 0; /** Stáří dat. */
    bool valid = false; /** Příznak validních dat. */
    bool erased = false; /** Příznak čistoty stránky. */
    bool bad = false; /** Příznak použitelnosti. */
    int_32 wear_level = 0; /** Použitelnost. */
} page_metadata;

typedef struct page_spare_struct {
    page_metadata metadata = { 0 }; /** Metadata stránky.  */
    u_char *ecc = nullptr; /** Kontrolni kód stránky. */
} page_spare;

/**
 * Definice struktury stránky.
 */
typedef struct page_struct {
    cell_struct *data = nullptr; /** Obsah stránky. */
    page_spare spare = { 0 };
} page;

/**
 * Definice struktury metadat bloku.
 */
typedef struct block_metadata_struct {
    int_32 id = 0; /** Identifikátor bloku. */
    int_32 erase_number = 0; /** Pocet smazani bloku. */
    bool valid = false; /** Příznak validních dat. */
    bool erased = false; /** Příznak čistoty bloku. */
    bool bad = false; /** Příznak použitelnosti. */
    int_32 wear_level = 0; /** Použitelnost. */
} block_metadata;

/**
 * Definice struktury bloku.
 */
typedef struct block_struct {
    page_struct *pages = nullptr; /** Obsah bloku. */
    block_metadata metadata = { 0 }; /** Metadata bloku. */
} block;

typedef struct nand_metadata_struct {
    uuid_t id; /** Identifikátor paměti. */
    int_32 page_size = 0; /** Velikost stránky. */
    int_8 num_of_pages = 0; /** Pocet stránek. */
    int_32 block_size = 0; /** Velikost bloku. */
    int_8 num_of_blocks = 0; /** Počet bloků. */
    int_32 mem_size = 0; /** Veliskot paměti. */
    int_32 true_mem_size = 0; /** Pocet bitu pameti. */
    int_8 ecc_size = 0; /** Počet bitů ECC kódu. */
    int_8 md_p_size = 0; /** Celkovy pocet metadat jedne stranky. */
    int_8 md_b_size = 0; /** Celkovy pocet metadat jednoho bloku. */
    u_char status = 0; /** 0. Device busy | 1. WEL | 5. EPE | 6. EPS | 7. ETM */
    mem_type memory_type = SLC; /** Typ paměti - určuje velikost buňky. */
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

    nand_memory m = { 0 };

public:

    Flash_Memory();

    Flash_Memory(int_8 page_size, int_8 block_size, mem_type memory_type);

    ~Flash_Memory();

    bool Init();

    /**
     * Vrátí data obsazené ve zvolené stránce do cache.
     */
    u_char* Read_Page(int_16 addr);

    /**
     * Vrátí základní informace o statusu paměti a operaci.
     */
    u_char Read_Status();

    /**
     * Vráti UUID (128b) zařízení.
     */
    uuid_t* Read_ID();

    /**
     * Nastaví data do stránky dané adresou.
     */
    void Program_Page(int_16 addr, u_char *data);

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
    void Random_Data_Input(u_char *data);
};

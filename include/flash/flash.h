#pragma once

#include <iostream>
#include <cstring>
#include <fstream>
#include <stdlib.h>

typedef u_int32_t int_32;
typedef u_int8_t int_8;
using namespace std;

#define DEFAULT_PAGE_SIZE 512
#define DEFAULT_BLOCK_SIZE 16
#define DEFAULT_MEMORY_ID 0

/**
 * Definice výčtového typu možných typů velikosti stránek.
 */
enum page_size {
    size_256 = 256,
    size_512 = 512,
    size_2048 = 2048
};

/**
 * Definice výštového typu představující typ buňky paměti.
 * Do budoucna by mohl určovat poruchovost, chybovost, životnost.
 */
enum mem_type {
    SLC,
    MLC,
    TLC,
    QLC
};

/**
 * Definice struktury stránky.
 */
typedef struct page_struct {
    int_32 page_id = 0; /** Identifikátor stránky. */
    u_char *page_data = nullptr; /** Obsah stránky. */
    u_char *ecc = nullptr; /** Kontrolni kód stránky. */
} page_struct;

/**
 * Definice struktury bloku.
 */
typedef struct block_struct {
    int_32 block_id = 0; /** Identifikátor bloku. */
    page_struct *block_pages = nullptr; /** Obsah bloku. */
} block_struct;

/**
 * Definice struktury paměti.
 */
typedef struct nand_memory_struct {
    int_32 mem_id = 0; /** Identifikátor paměti. */
    int_8 page_size = 0; /** Velikost stránky. */
    int_8 block_size = 0; /** Velikost bloku. */
    int_32 mem_size = 0; /** Veliskot paměti. */
    block_struct *mem_blocks = nullptr; /** Obsah paměti. */
} nand_memory_struct;


class Flash_Memory {

private:

    nand_memory_struct memory = { 0 };

public:

    Flash_Memory();

    Flash_Memory(int_8 page_size, int_8 block_size);

    ~Flash_Memory();

    bool Init();

    /**
     * Vrati ID ONFI - ‘O’ = 4Fh, ‘N’ = 4Eh, ‘F’ = 46h, and ‘I’ = 49h
     */
    string Read_ID();

    /**
     * Vrati datovou strukuturu pro ONFI.
     */
    string Read_Parameter_Page_Definition();

    /**
     * Vrati UID (16B) zarizeni.
     */
    string Read_Uniquie_ID();

    /**
     * Vymaze blok na dane adrese.
     * @param block_address Adresa bloku (Mela by byt uvedena v LUN).
     * Navratova hodnota by mela byt v SR[0] == 0.
     */
    void Block_Erase(int_32 block_address);

    /**
     * Vrati zakladni informace o statusu pameti a operaci.
     * starsi command
     */
    string Read_Status();


    /**
     * Vrati detailni informace o statusu pameti a operaci.
     * novejsi command (mene podporovany?)
     */
    string Read_Status_Enhanced();

    /**
     * Vrati hodnotu registru SR.
     */
    string Status_Read();

    /**
     * Vrati data obsazene eve zvolene strance.
     */
    string Read(int_32 row_addr);

    /**
     * Umozni pouzit cache buffer pri cteni dat.
     */
    void Read_Cache();

    /**
     * Nastavi data az o velikosti stranky do page registru.
     */
    void Page_Program(int_32 col_addr);

    /**
     * Povoli zapis dat az o velikosti stranky do page registru.
     */
    void Page_Cache_Program();

    /**
     * Nakopiruje data z jednoho mista na jine ve stejne LUN.
     */
    void CopyBack(int_32 old_row_addr, int_32 new_row_addr);

    /**
     * Umozni praci s daty mensimi, nez je velikost stranky.
     */
    void Small_Data_Move();

    /**
     * Zmeni hodnotu adresy sloupecku, ze ktereho jsou data ctena.
     */
    void Change_Read_Column(int_32 col_addr);

    /**
     * Zmeni LUN adresu, plane adresu a adresu sloupecku, ze ktereho jsou data ctena.
     */
    void Change_Read_Column_Enhanced(int_32 lun_addr, int_32 plane_addr, int_32 col_addr);

    /**
     * Zmeni adresu sloupecku, do ktereho je zapisovano.
     */
    void Change_Write_Column(int_32 row_addr);

    /**
     * Zmeni adresu sloupecku a radky, do ktereho je zapisovano.
     */
    void Change_Row_Address(int_32 row_addr, int_32 col_addr);


    /**
     * Nevim, jestli musi byt.
     */
    void ODT_Enable();
    void ODT_Disable();

    /**
     * Provede inicializaci pri zapinani nebo resetu.
     */
    void Calibration_Long();

    /**
     * Provede periodickou kalibraci.
     */
    void Calibration_Short();

    /**
     * Nastavi nektere vlastnosti pameti, napr. power-on.
     */
    void Set_Features(int_8 feature);

    /**
     * Vrati pouzite nastaveni pro zvolenou vlastnost.
     */
    string Get_Features(int_8 feature);

    /**
     * Nastavuje parametry?
     */
    void Feature_Parameter(int_8 param);
};

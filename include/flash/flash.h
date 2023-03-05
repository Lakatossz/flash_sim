#pragma once

#include <iostream>
#include <cstring>
#include <fstream>

typedef u_int32_t my_int;
using namespace std;

class Flash_Commands {

public:

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
    void Block_Erase(u_int32_t block_address);

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
    string Read(u_int32_t row_addr);

    /**
     * Umozni pouzit cache buffer pri cteni dat.
     */
    void Read_Cache();

    /**
     * Nastavi data az o velikosti stranky do page registru.
     */
    void Page_Program(u_int32_t col_addr);

    /**
     * Povoli zapis dat az o velikosti stranky do page registru.
     */
    void Page_Cache_Program();

    /**
     * Nakopiruje data z jednoho mista na jine ve stejne LUN.
     */
    void CopyBack(u_int32_t old_row_addr, u_int32_t new_row_addr);

    /**
     * Umozni praci s daty mensimi, nez je velikost stranky.
     */
    void Small_Data_Move();

    /**
     * Zmeni hodnotu adresy sloupecku, ze ktereho jsou data ctena.
     */
    void Change_Read_Column(u_int32_t col_addr);

    /**
     * Zmeni LUN adresu, plane adresu a adresu sloupecku, ze ktereho jsou data ctena.
     */
    void Change_Read_Column_Enhanced(u_int32_t lun_addr, u_int32_t plane_addr, u_int32_t col_addr);

    /**
     * Zmeni adresu sloupecku, do ktereho je zapisovano.
     */
    void Change_Write_Column(u_int32_t row_addr);

    /**
     * Zmeni adresu sloupecku a radky, do ktereho je zapisovano.
     */
    void Change_Row_Address(u_int32_t row_addr, u_int32_t col_addr);


    /**
     * Nevim, jestli musi byt.
     */
    void ODT_Enable();
    void ODT_Disable();

    /**
     * Provede inicializaci pri zapinani nebo resetu.
     */
    void ZQ_Calibration_Long();

    /**
     * Provede periodickou kalibraci.
     */
    void ZQ_Calibration_Short();

    /**
     * Nastavi nektere vlastnosti pameti, napr. power-on.
     */
    void Set_Features(u_int8_t feature);

    /**
     * Vrati pouzite nastaveni pro zvolenou vlastnost.
     */
    string Get_Features(u_int8_t feature);

    /**
     * Nastavuje parametry?
     */
    void Feature_Parameter(u_int8_t param);
};

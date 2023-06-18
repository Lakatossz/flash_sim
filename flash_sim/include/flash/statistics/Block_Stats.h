#pragma once

#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <uuid/uuid.h>

#define DEFAULT_READ_PAGE_TIME 10
#define DEFAULT_PAGE_PROG_TIME 15
#define DEFAULT_ERASE_TIME 30

#define BLOCK_ECC_SIZE 20

/**
 * Definice třídy reprezentující statistiky bloku.
 */
class Block_Stats {
private:
    /** Čas smazání bloku. */
    float Erase_Time;
    /** Poslední čas smazání bloku. */
    float Last_Erase_Time = 0;
    /** Celkový čas smazání bloku. */
    float Total_Erase_Time = 0;
    /** Počet přečtených stránek v bloku. */
    size_t Num_Of_Reads = 0;
    /** Počet zapsaných stránek v bloku. */
    size_t Num_Of_Writes = 0;
    /** Počet smazání bloku. */
    size_t Num_Of_Erases = 0;
    /** Počet poškozených stránek. */
    size_t Num_Of_Bad_Pages = 0;
    /** Počet chyb stránky. */
    size_t Num_Of_Errors;
    /** Histogram ECC oprav v bloku. */
    size_t ECC_Histogram[BLOCK_ECC_SIZE];

public:

    /**
     * Vytvoří nové statistiky pro blok.
     */
    Block_Stats();

    float getEraseTime() const;

    void setEraseTime(float eraseTime);

    void addEraseTime(float eraseTime);

    float getLastEraseTime() const;

    void setLastEraseTime(float lastEraseTime);

    void addLastEraseTime(float eraseTime);

    float getTotalEraseTime() const;

    void setTotalEraseTime(float totalEraseTime);

    void addTotalEraseTime(float eraseTime);

    size_t getNumOfReads() const;

    void setNumOfReads(size_t numOfReads);

    void addNumOfReads(size_t numOfReads);

    size_t getNumOfWrites() const;

    void setNumOfWrites(size_t numOfWrites);

    void addNumOfWrites(size_t numOfWrites);

    size_t getNumOfErases() const;

    void setNumOfErases(size_t numOfErases);

    void addNumOfErases(size_t numOfErases);

    size_t getNumOfBadPages() const;

    void setNumOfBadPages(size_t numOfBadPages);

    size_t* getHistogram();

    size_t getNumOfErrors() const;

    void setNumOfErrors(size_t numOfErrors);
};

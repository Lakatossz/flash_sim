#pragma once

#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <uuid/uuid.h>

#define DEFAULT_READ_PAGE_TIME 10
#define DEFAULT_PAGE_PROG_TIME 15
#define DEFAULT_ERASE_TIME 30

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
    int Num_Of_Reads = 0;
    /** Počet zapsaných stránek v bloku. */
    int Num_Of_Writes = 0;
    /** Počet smazání bloku. */
    int Num_Of_Erases = 0;

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

    int getNumOfReads() const;

    void setNumOfReads(int numOfReads);

    void addNumOfReads(int numOfReads);

    int getNumOfWrites() const;

    void setNumOfWrites(int numOfWrites);

    void addNumOfWrites(int numOfWrites);

    int getNumOfErases() const;

    void setNumOfErases(int numOfErases);

    void addNumOfErases(int numOfErases);
};

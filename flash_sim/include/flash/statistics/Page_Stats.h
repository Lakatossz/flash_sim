#pragma once

#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <uuid/uuid.h>

#define DEFAULT_READ_PAGE_TIME 10
#define DEFAULT_PAGE_PROG_TIME 15

/**
 * Definice třídy reprezentující statistiky stránky.
 */
class Page_Stats {
private:
    /** Čas čtení stránky. */
    float Read_Page_Time;
    /** Čas naprogramování stránky. */
    float Page_Prog_Time;
    /** Poslední čas čtení stránky. */
    float Last_Read_Page_Time;
    /** Poslední čas naprogramování stránky. */
    float Last_Page_Prog_Time;
    /** Celkový čas čtení stránky. */
    float Total_Read_Page_Time;
    /** Celkový čas naprogramování stránky. */
    float Total_Page_Prog_Time;
    /** Čas pro komunikaci se stránkou paměti - spoje, atd. */
    float Com_Time;
    /** Počet čtení stránky. */
    int Num_Of_Reads;
    /** Počet zápisů stránky. */
    int Num_Of_Writes;

public:

    Page_Stats();

    float getReadPageTime() const;

    void setReadPageTime(float readPageTime);

    void addReadPageTime(float readPageTime);

    void setPageProgTime(float pageProgTime);

    void addPageProgTime(float pageProgTime);

    void setLastReadPageTime(float readPageTime);

    void addLastReadPageTime(float readPageTime);

    void setLastPageProgTime(float pageProgTime);

    void addLastPageProgTime(float pageProgTime);

    void setTotalReadPageTime(float readPageTime);

    void addTotalReadPageTime(float readPageTime);

    void setTotalPageProgTime(float pageProgTime);

    void addTotalPageProgTime(float pageProgTime);

    void setComTime(float comTime);

    void addComTime(float comTime);

    void setNumOfReads(int numOfReads);

    void addNumOfReads(int numOfReads);

    void setNumOfWrites(int numOfWrites);

    void addNumOfWrites(int numOfWrites);

    float getPageProgTime() const;

    float getLastReadPageTime() const;

    float getLastPageProgTime() const;

    float getTotalReadPageTime() const;

    float getTotalPageProgTime() const;

    float getComTime() const;

    int getNumOfReads() const;

    int getNumOfWrites() const;
};
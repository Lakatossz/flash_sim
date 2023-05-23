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
    size_t Num_Of_Reads;
    /** Počet zápisů stránky. */
    size_t Num_Of_Writes;

public:

    Page_Stats();

    Page_Stats(float read_page_time, float page_prog_time);

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

    void setNumOfReads(size_t numOfReads);

    void addNumOfReads(size_t numOfReads);

    void setNumOfWrites(size_t numOfWrites);

    void addNumOfWrites(size_t numOfWrites);

    float getPageProgTime() const;

    float getLastReadPageTime() const;

    float getLastPageProgTime() const;

    float getTotalReadPageTime() const;

    float getTotalPageProgTime() const;

    float getComTime() const;

    size_t getNumOfReads() const;

    size_t getNumOfWrites() const;
};
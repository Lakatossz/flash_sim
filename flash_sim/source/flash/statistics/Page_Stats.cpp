#include "../../../include/flash/statistics/Page_Stats.h"

Page_Stats::Page_Stats() {
    Read_Page_Time = DEFAULT_READ_PAGE_TIME;
    Page_Prog_Time = DEFAULT_PAGE_PROG_TIME;
    Last_Read_Page_Time = 0;
    Last_Page_Prog_Time = 0;
    Total_Read_Page_Time = 0;
    Total_Page_Prog_Time = 0;
    Com_Time = 0.0;
    Num_Of_Reads = 0;
    Num_Of_Writes = 0;
}

Page_Stats::Page_Stats(float read_page_time, float page_prog_time) {
    Read_Page_Time = DEFAULT_READ_PAGE_TIME;
    Page_Prog_Time = DEFAULT_PAGE_PROG_TIME;
}

float Page_Stats::getReadPageTime() const {
    return Read_Page_Time;
}

float Page_Stats::getPageProgTime() const {
    return Page_Prog_Time;
}

float Page_Stats::getLastReadPageTime() const {
    return Last_Read_Page_Time;
}

float Page_Stats::getLastPageProgTime() const {
    return Last_Page_Prog_Time;
}

float Page_Stats::getTotalReadPageTime() const {
    return Total_Read_Page_Time;
}

float Page_Stats::getTotalPageProgTime() const {
    return Total_Page_Prog_Time;
}

float Page_Stats::getComTime() const {
    return Com_Time;
}

size_t Page_Stats::getNumOfReads() const {
    return Num_Of_Reads;
}

size_t Page_Stats::getNumOfWrites() const {
    return Num_Of_Writes;
}

void Page_Stats::setReadPageTime(float readPageTime) {
    Read_Page_Time = readPageTime;
}

void Page_Stats::setPageProgTime(float pageProgTime) {
    Page_Prog_Time = pageProgTime;
}

void Page_Stats::setLastReadPageTime(float lastReadPageTime) {
    Last_Read_Page_Time = lastReadPageTime;
}

void Page_Stats::setLastPageProgTime(float lastPageProgTime) {
    Last_Page_Prog_Time = lastPageProgTime;
}

void Page_Stats::setTotalReadPageTime(float totalReadPageTime) {
    Total_Read_Page_Time = totalReadPageTime;
}

void Page_Stats::setTotalPageProgTime(float totalPageProgTime) {
    Total_Page_Prog_Time = totalPageProgTime;
}

void Page_Stats::setComTime(float comTime) {
    Com_Time = comTime;
}

void Page_Stats::setNumOfReads(size_t numOfReads) {
    Num_Of_Reads = numOfReads;
}

void Page_Stats::setNumOfWrites(size_t numOfWrites) {
    Num_Of_Writes = numOfWrites;
}

void Page_Stats::addReadPageTime(float pageTime) {
    Read_Page_Time += pageTime;
}

void Page_Stats::addPageProgTime(float progTime) {
    Page_Prog_Time += progTime;
}

void Page_Stats::addLastReadPageTime(float readPageTime) {
    Last_Read_Page_Time += readPageTime;
}

void Page_Stats::addLastPageProgTime(float pageProgTime) {
    Last_Page_Prog_Time += pageProgTime;
}

void Page_Stats::addTotalReadPageTime(float readPageTime) {
    Total_Read_Page_Time += readPageTime;
}

void Page_Stats::addTotalPageProgTime(float totalPageProgTime) {
    Total_Page_Prog_Time += totalPageProgTime;
}

void Page_Stats::addComTime(float comTime) {
    Com_Time += comTime;
}

void Page_Stats::addNumOfReads(size_t numOfReads) {
    Num_Of_Reads += numOfReads;
}

void Page_Stats::addNumOfWrites(size_t numOfWrites) {
    Num_Of_Writes += numOfWrites;
}

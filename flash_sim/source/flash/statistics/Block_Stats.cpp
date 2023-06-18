#include "../../../include/flash/statistics/Block_Stats.h"

Block_Stats::Block_Stats() {
    Erase_Time = DEFAULT_ERASE_TIME;
    Last_Erase_Time = 0;
    Total_Erase_Time = 0;
    Num_Of_Reads = 0;
    Num_Of_Writes = 0;
    Num_Of_Erases = 0;
}

float Block_Stats::getEraseTime() const {
    return Erase_Time;
}

void Block_Stats::setEraseTime(float eraseTime) {
    Erase_Time = eraseTime;
}

float Block_Stats::getLastEraseTime() const {
    return Last_Erase_Time;
}

void Block_Stats::setLastEraseTime(float lastEraseTime) {
    Last_Erase_Time = lastEraseTime;
}

float Block_Stats::getTotalEraseTime() const {
    return Total_Erase_Time;
}

void Block_Stats::setTotalEraseTime(float totalEraseTime) {
    Total_Erase_Time = totalEraseTime;
}

size_t Block_Stats::getNumOfReads() const {
    return Num_Of_Reads;
}

void Block_Stats::setNumOfReads(size_t numOfReads) {
    Num_Of_Reads = numOfReads;
}

size_t Block_Stats::getNumOfWrites() const {
    return Num_Of_Writes;
}

void Block_Stats::setNumOfWrites(size_t numOfWrites) {
    Num_Of_Writes = numOfWrites;
}

size_t Block_Stats::getNumOfErases() const {
    return Num_Of_Erases;
}

void Block_Stats::setNumOfErases(size_t numOfErases) {
    Num_Of_Erases = numOfErases;
}

void Block_Stats::addEraseTime(float eraseTime) {
    eraseTime += eraseTime;
}

void Block_Stats::addLastEraseTime(float eraseTime) {
    Last_Erase_Time += eraseTime;
}

void Block_Stats::addTotalEraseTime(float eraseTime) {
    Total_Erase_Time += eraseTime;
}

void Block_Stats::addNumOfReads(size_t numOfReads) {
    Num_Of_Reads += numOfReads;
}

void Block_Stats::addNumOfWrites(size_t numOfWrites) {
    Num_Of_Writes += numOfWrites;
}

void Block_Stats::addNumOfErases(size_t numOfErases) {
    Num_Of_Erases += numOfErases;
}

size_t Block_Stats::getNumOfBadPages() const {
    return Num_Of_Bad_Pages;
}

void Block_Stats::setNumOfBadPages(size_t numOfBadPages) {
    Num_Of_Bad_Pages = numOfBadPages;
}

size_t* Block_Stats::getHistogram() {
    return this->ECC_Histogram;
}

size_t Block_Stats::getNumOfErrors() const {
    return Num_Of_Errors;
}

void Block_Stats::setNumOfErrors(size_t numOfErrors) {
    Num_Of_Errors = numOfErrors;
}

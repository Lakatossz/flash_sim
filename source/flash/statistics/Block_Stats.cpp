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

int Block_Stats::getNumOfReads() const {
    return Num_Of_Reads;
}

void Block_Stats::setNumOfReads(int numOfReads) {
    Num_Of_Reads = numOfReads;
}

int Block_Stats::getNumOfWrites() const {
    return Num_Of_Writes;
}

void Block_Stats::setNumOfWrites(int numOfWrites) {
    Num_Of_Writes = numOfWrites;
}

int Block_Stats::getNumOfErases() const {
    return Num_Of_Erases;
}

void Block_Stats::setNumOfErases(int numOfErases) {
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

void Block_Stats::addNumOfReads(int numOfReads) {
    Num_Of_Reads += numOfReads;
}

void Block_Stats::addNumOfWrites(int numOfWrites) {
    Num_Of_Writes += numOfWrites;
}

void Block_Stats::addNumOfErases(int numOfErases) {
    Num_Of_Erases += numOfErases;
}

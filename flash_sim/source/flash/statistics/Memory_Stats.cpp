#include "../../../include/flash/statistics/Memory_Stats.h"

int Memory_Stats::getNumOfReads() const {
    return Num_Of_Reads;
}

void Memory_Stats::setNumOfReads(int numOfReads) {
    Num_Of_Reads = numOfReads;
}

int Memory_Stats::getNumOfWrites() const {
    return Num_Of_Writes;
}

void Memory_Stats::setNumOfWrites(int numOfWrites) {
    Num_Of_Writes = numOfWrites;
}

void Memory_Stats::addNumOfReads(int numOfReads) {
    Num_Of_Reads += numOfReads;
}

void Memory_Stats::addNumOfWrites(int numOfWrites) {
    Num_Of_Writes += Num_Of_Writes;
}

#include "../../../include/flash/statistics/Memory_Stats.h"

size_t Memory_Stats::getNumOfReads() const {
    return Num_Of_Reads;
}

void Memory_Stats::setNumOfReads(size_t numOfReads) {
    Num_Of_Reads = numOfReads;
}

size_t Memory_Stats::getNumOfWrites() const {
    return Num_Of_Writes;
}

void Memory_Stats::setNumOfWrites(size_t numOfWrites) {
    Num_Of_Writes = numOfWrites;
}

void Memory_Stats::addNumOfReads(size_t numOfReads) {
    Num_Of_Reads += numOfReads;
}

void Memory_Stats::addNumOfWrites(size_t numOfWrites) {
    Num_Of_Writes += Num_Of_Writes;
}

size_t* Memory_Stats::getHistogram() {
    return this->ECC_Histogram;
}

size_t Memory_Stats::getNumOfErrors() const {
    return Num_Of_Errors;
}

void Memory_Stats::setNumOfErrors(size_t numOfErrors) {
    Num_Of_Errors = numOfErrors;
}

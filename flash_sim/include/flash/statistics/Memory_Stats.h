#pragma once

#include <cstddef>

#define MEMORY_ECC_SIZE 60

/**
 * Definice třídy reprezentující statistiky paměti.
 */
class Memory_Stats {
private:
    /** Počet čtení stránek z paměti. */
    size_t Num_Of_Reads;
    /** Počet zápisů stránek do paměti. */
    size_t Num_Of_Writes;
    /** Počet chyb stránky. */
    size_t Num_Of_Errors;
    /** Histogram ECC oprav v paměti. */
    size_t ECC_Histogram[MEMORY_ECC_SIZE];

public:
    size_t getNumOfReads() const;

    void setNumOfReads(size_t numOfReads);

    void addNumOfReads(size_t numOfReads);

    size_t getNumOfWrites() const;

    void setNumOfWrites(size_t numOfWrites);

    void addNumOfWrites(size_t numOfWrites);

    size_t* getHistogram();

    size_t getNumOfErrors() const;

    void setNumOfErrors(size_t numOfErrors);
};

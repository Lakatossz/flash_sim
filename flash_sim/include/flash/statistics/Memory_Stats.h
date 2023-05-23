#pragma once

#include <cstddef>

/**
 * Definice třídy reprezentující statistiky paměti.
 */
class Memory_Stats {
private:
    /** Počet čtení stránek z paměti. */
    size_t Num_Of_Reads;
    /** Počet zápisů stránek do paměti. */
    size_t Num_Of_Writes;

public:
    size_t getNumOfReads() const;

    void setNumOfReads(size_t numOfReads);

    void addNumOfReads(size_t numOfReads);

    size_t getNumOfWrites() const;

    void setNumOfWrites(size_t numOfWrites);

    void addNumOfWrites(size_t numOfWrites);
};

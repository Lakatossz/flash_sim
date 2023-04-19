#pragma once

/**
 * Definice třídy reprezentující statistiky paměti.
 */
class Memory_Stats {
private:
    /** Počet čtení stránek z paměti. */
    int Num_Of_Reads;
    /** Počet zápisů stránek do paměti. */
    int Num_Of_Writes;

public:
    int getNumOfReads() const;

    void setNumOfReads(int numOfReads);

    void addNumOfReads(int numOfReads);

    int getNumOfWrites() const;

    void setNumOfWrites(int numOfWrites);

    void addNumOfWrites(int numOfWrites);
};

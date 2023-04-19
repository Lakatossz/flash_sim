#include "flash/flash.h"

/**
 * Funkce pro inkrementaci pole predstavujici pocitadlo.
 * @param counter Pole u_char - pocitadlo.
 * @param size Pocet bytu pole.
 */
void increase_counter(u_char *counter, size_t size) {
    for (size_t i = size - 1; i > 0; --i) {
        if (counter[i] < (u_char) 1L) {
            counter[i]++;
            return;
        }
    }
}

int_32 counter_value(const u_char *counter, size_t size) {

    int_32 value = 0;
    size_t i = 0;
    while (counter[i] == (u_char) 1L || i < size) {
        value += (u_char) 1L;
        ++i;
    }

    return value;
}

/**
 *
 * @param time_type
 */
void increase_time(NTime_Type type) {
    if (type == NTime_Type::READ_PAGE_TIME) {
        return;
    } else if (type == NTime_Type::PAGE_PROG_TIME) {
        return;
    } else if (type == NTime_Type::ERASE_TIME) {
        return;
    }
}

int check_address(nand_memory m, int_16 addr) {
    /** Zkontroluju adresu bloku. */
    if ((addr >> 8) >= m.md.num_of_blocks) {
        cout << (addr >> 8) << endl;
        cout << "Adresa bloku je příliš velká.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
        /** Zkontroluju adresu stránky. */
    } else if ((addr & (uint8_t) ~0L) >= m.md.num_of_pages) {
        cout << "Adresa stránky je příliš velká.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

Flash_Memory::Flash_Memory()
{
    m.md.pages_stats = (Page_Stats *) malloc(sizeof(Page_Stats) * m.md.num_of_pages * m.md.num_of_blocks);
    if (!m.md.pages_stats) {
        cout << "Není dostatek paměti.\n";
        return;
    }

    for (int i = 0; i < m.md.num_of_pages; ++i) {
        m.md.pages_stats[i] = Page_Stats();
    }

    m.md.blocks_stats = (Block_Stats *) malloc(sizeof(Block_Stats) * m.md.num_of_blocks);
    if (!m.md.blocks_stats) {
        cout << "Není dostatek paměti.\n";
        return;
    }

    for (int i = 0; i < m.md.num_of_blocks; ++i) {
        m.md.blocks_stats[i] = Block_Stats();
    }
}
Flash_Memory::Flash_Memory(int_32 page_size, int_32 block_size, int_32 number_of_blocks, NMem_Type memory_type,
                           float read_page_time, float page_prog_time, float erase_time)
{
    m.md.page_size = page_size;
    m.md.block_size = block_size;
    m.md.num_of_blocks = number_of_blocks;
    m.md.memory_type = memory_type;

    free(m.md.pages_stats);
    m.md.pages_stats = (Page_Stats *) malloc(sizeof(Page_Stats) * m.md.num_of_pages * m.md.num_of_blocks);
    if (!m.md.pages_stats) {
        cout << "Není dostatek paměti.\n";
        return;
    }

    for (int i = 0; i < m.md.num_of_pages; ++i) {
        Page_Stats stats = Page_Stats();
        stats.setReadPageTime(read_page_time);
        stats.setPageProgTime(page_prog_time);
        m.md.pages_stats[i] = stats;
    }

    free(m.md.blocks_stats);
    m.md.blocks_stats = (Block_Stats *) malloc(sizeof(Block_Stats) * m.md.num_of_blocks);
    if (!m.md.blocks_stats) {
        cout << "Není dostatek paměti.\n";
        return;
    }

    for (int i = 0; i < m.md.num_of_blocks; ++i) {
        Block_Stats stats = Block_Stats();
        stats.setEraseTime(erase_time);
        m.md.blocks_stats[i] = Block_Stats();
    }
}

Flash_Memory::~Flash_Memory()
{
    free(m.mem_cache);
    free(m.md.pages_stats);
    free(m.md.blocks_stats);
}

int Flash_Memory::Flash_Init()
{
    /** Pocet stranek uvnitr bloku. */
    m.md.num_of_pages = m.md.block_size / m.md.page_size;

    /** Počet bytů, které paměť obsahuje. */
    m.md.mem_size = m.md.num_of_blocks * m.md.block_size;

    /** Celková velikost paměti = úložný prostor + metadata pro stránky + metadata pro bloky. */
    m.md.true_mem_size = m.md.mem_size + m.md.num_of_blocks * m.md.md_b_size
            + m.md.num_of_blocks * m.md.num_of_pages * m.md.md_p_size;

    m.data = (u_char *) malloc(sizeof(u_char) * m.md.true_mem_size);
    if (m.data == nullptr) {
        return EXIT_FAILURE;
    }
    memset(m.data, 0L, m.md.true_mem_size);

//    uuid_generate_random(m.md.id);

    cout << "Page size: " << m.md.page_size << endl;
    cout << "Block size: " << m.md.block_size << endl;
    cout << "Number of pages: " << m.md.num_of_pages << endl;
    cout << "Number of blocks: " << m.md.num_of_blocks << endl;
    cout << "Memory size: " << m.md.mem_size << endl;
    cout << "True memory size: " << m.md.true_mem_size << endl;

    return EXIT_SUCCESS;
}

int Flash_Memory::Cache_Init() {

    m.mem_cache = (u_char *) malloc(sizeof(u_char) * m.md.page_size);
    if (!m.mem_cache) {
        cout << "Není dostatek paměti.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Read_Page(int_16 addr)
{
    if (check_address(m, addr)) {
        cout << "Adresu nelze pouzit.\n";
        return EXIT_FAILURE;
    }

    int_32 pointer = addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

//    m.md.status = m.md.status | (1 << 5)

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[pointer + m.md.page_size] >> 0 == 1) {
        cout << "Stranka neni validni.\n";
        return EXIT_FAILURE;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if ((m.data[pointer + 1] >> 1) == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
    }

    memcpy(m.mem_cache, &m.data[pointer], m.md.page_size);

    size_t page_stats_pointer = (addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L);

    /** Aktualizace času běhu. */
    m.md.pages_stats[page_stats_pointer]
        .addTotalReadPageTime(m.md.pages_stats[page_stats_pointer].getReadPageTime());

    m.md.pages_stats[page_stats_pointer]
        .setLastReadPageTime(m.md.pages_stats[page_stats_pointer].getReadPageTime());
    increase_time(READ_PAGE_TIME);

    m.md.pages_stats[page_stats_pointer].addNumOfReads(1);
    m.md.blocks_stats[(addr >> 8)].addNumOfReads(1);
    m.md.memory_stats->addNumOfReads(1);

    return EXIT_SUCCESS;
}

int Flash_Memory::Read_Sector(int_16 addr)
{
    if (check_address(m, addr)) {
        cout << "Adresu nelze pouzit.\n";
        return EXIT_FAILURE;
    }

    int_32 pointer = addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

//    m.md.status = m.md.status | (1 << 5)

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[pointer + m.md.page_size] >> 0 == 1) {
        cout << "Stranka neni validni.\n";
        return EXIT_FAILURE;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if ((m.data[pointer + 1] >> 1) == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
    }

    memcpy(m.mem_cache, &m.data[pointer], DEFAULT_SECTOR_SIZE);

    size_t page_stats_pointer = (addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L);

    /** Aktualizace času běhu. */
    m.md.pages_stats[page_stats_pointer]
            .addTotalReadPageTime(m.md.pages_stats[page_stats_pointer].getReadPageTime());

    m.md.pages_stats[page_stats_pointer]
            .setLastReadPageTime(m.md.pages_stats[page_stats_pointer].getReadPageTime());
    increase_time(READ_PAGE_TIME);

    m.md.pages_stats[page_stats_pointer].addNumOfReads(1);
    m.md.blocks_stats[(addr >> 8)].addNumOfReads(1);
    m.md.memory_stats->addNumOfReads(1);

    return EXIT_SUCCESS;
}

u_char* Flash_Memory::Read_Cache() const
{
    auto * buf = (u_char *) malloc(sizeof(u_char) * m.md.page_size);
    if (!buf) {
        cout << "Není dostatek paměti.\n";
        return nullptr;
    }

    memcpy(buf, m.mem_cache, m.md.page_size);

    return buf;
}

u_char Flash_Memory::Read_Status() const
{
    return m.md.status;
}

uuid_t* Flash_Memory::Read_ID()
{
    return &m.md.id;
}

int Flash_Memory::Program_Page(int_16 addr)
{
    if (check_address(m, addr))
        return EXIT_FAILURE;

    int_32 pointer = addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[pointer + m.md.page_size] >> 0 == 1) {
        return EXIT_FAILURE;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[pointer + 1] >> 1 == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
    }

    memcpy(&m.data[pointer], m.mem_cache, m.md.page_size);

    size_t page_stats_pointer = (addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L);

    /** Aktualizace času běhu. */
    m.md.pages_stats[page_stats_pointer]
        .addTotalPageProgTime(m.md.pages_stats[page_stats_pointer].getPageProgTime());

    m.md.pages_stats[page_stats_pointer]
        .setLastPageProgTime(m.md.pages_stats[page_stats_pointer].getPageProgTime());

    increase_time(PAGE_PROG_TIME);

    m.md.pages_stats[page_stats_pointer].addNumOfWrites(1);
    m.md.blocks_stats[(addr >> 8)].addNumOfWrites(1);
    m.md.memory_stats->addNumOfWrites(1);

    return EXIT_SUCCESS;
}

int Flash_Memory::Program_Sector(int_16 addr)
{
    if (check_address(m, addr)) {
        return EXIT_FAILURE;
    }

    int_32 pointer = addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[pointer + m.md.page_size] >> 0 == 1) {
        return EXIT_FAILURE;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[pointer + 1] >> 1 == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
    }

    memcpy(&m.data[pointer], m.mem_cache, DEFAULT_SECTOR_SIZE);

    size_t page_stats_pointer = (addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L);

    /** Aktualizace času běhu. */
    m.md.pages_stats[page_stats_pointer]
            .addTotalPageProgTime(m.md.pages_stats[page_stats_pointer].getPageProgTime());

    m.md.pages_stats[page_stats_pointer]
            .setLastPageProgTime(m.md.pages_stats[page_stats_pointer].getPageProgTime());

    increase_time(PAGE_PROG_TIME);

    m.md.pages_stats[page_stats_pointer].addNumOfWrites(1);
    m.md.blocks_stats[(addr >> 8)].addNumOfWrites(1);
    m.md.memory_stats->addNumOfWrites(1);

    return EXIT_SUCCESS;
}

int Flash_Memory::Write_Cache(const string& data) const
{
    if (data.length() <= 0 && data.length() > m.md.page_size) {
        cout << "Data nemohou být uložena.\n";
        return EXIT_FAILURE;
    }

    memset(m.mem_cache, 0, m.md.page_size);
    memcpy(m.mem_cache, (u_char *) data.c_str(), m.md.page_size);

    return EXIT_SUCCESS;
}

int Flash_Memory::Program_Data_Move(int_16 old_addr, int_16 new_addr)
{
    if (check_address(m, old_addr) || check_address(m, new_addr))
        return EXIT_FAILURE;

    int_32 old_pointer = old_addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[old_pointer + m.md.page_size] >> 0 == 1) {
        return EXIT_FAILURE;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[old_pointer + 1] >> 1 == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
    }

    size_t old_page_stats_pointer = (old_addr >> 8) * m.md.num_of_pages + (old_addr & (uint8_t) ~0L);

    /** Aktualizace času běhu. */
    m.md.pages_stats[old_page_stats_pointer]
        .addTotalReadPageTime(m.md.pages_stats[old_page_stats_pointer].getPageProgTime());
    m.md.pages_stats[old_page_stats_pointer]
            .setLastReadPageTime(m.md.pages_stats[old_page_stats_pointer].getPageProgTime());

    increase_time(READ_PAGE_TIME);

    m.md.pages_stats[old_page_stats_pointer].addNumOfReads(1);
    m.md.blocks_stats[(old_addr >> 8)].addNumOfReads(1);
    m.md.memory_stats->addNumOfReads(1);

    int_32 new_pointer = old_addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[new_pointer + m.md.page_size] >> 0 == 1) {
        return EXIT_FAILURE;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[new_pointer + 1] >> 1 == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
    }

    memcpy(&m.data[old_pointer], &m.data[new_pointer], m.md.page_size);

    size_t new_page_stats_pointer = (new_addr >> 8) * m.md.num_of_pages + (new_addr & (uint8_t) ~0L);

    /** Aktualizace času běhu. */
    m.md.pages_stats[new_page_stats_pointer]
        .addTotalPageProgTime(m.md.pages_stats[new_page_stats_pointer].getPageProgTime());

    m.md.pages_stats[new_page_stats_pointer]
            .setLastPageProgTime(m.md.pages_stats[new_page_stats_pointer].getPageProgTime());

    increase_time(PAGE_PROG_TIME);

    m.md.pages_stats[new_page_stats_pointer].addNumOfWrites(1);
    m.md.blocks_stats[(new_addr >> 8)].addNumOfWrites(1);
    m.md.memory_stats->addNumOfWrites(1);

    return EXIT_SUCCESS;
}

//for (int i = 0; i < numBytes; i++) {
//intValue |= ((int)charArray[i] << ((numBytes - 1 - i) * 8));
//}

int Flash_Memory::Block_Erase(int_8 addr)
{
    if (check_address(m, addr)) {
        return EXIT_FAILURE;
    }

    int_32 pointer = addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[pointer + m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size] >> 1 == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
    }

    /** Smazání bloku. */
    memset(&m.data[pointer],0L,m.md.page_size);
    increase_counter(
            &m.data[pointer + m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size + 2],
            m.md.block_wear_size);

    if (counter_value(
            &m.data[pointer + m.md.block_size + m.md.num_of_pages * m.md.md_p_size] + 1,
            m.md.block_wear_size) == MAX_ERASE_NUMBER) {
        m.data[pointer + m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size]
        = m.data[pointer + m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size] | (1 << 1);
    }

    /** Aktualizace času běhu. */
    m.md.blocks_stats[(addr >> 8)].addTotalEraseTime(m.md.blocks_stats[(addr >> 8)].getEraseTime());

    m.md.blocks_stats[(addr >> 8)].setLastEraseTime(m.md.blocks_stats[(addr >> 8)].getEraseTime());

    m.md.blocks_stats[(addr >> 8)].addNumOfErases(1);

    increase_time(ERASE_TIME);

    return EXIT_SUCCESS;
}

int Flash_Memory::Reset()
{
    memset(m.data, 0L, m.md.true_mem_size);
    memset(m.mem_cache, 0, m.md.page_size);
    memset(m.md.pages_stats, 0, m.md.num_of_blocks * m.md.num_of_pages * sizeof(Page_Stats));
    memset(m.md.blocks_stats, 0, m.md.num_of_blocks * sizeof(Block_Stats));

    m.md.status = 0;
    m.md.num_of_bad_blocks = 0;
    m.md.num_of_bad_pages = 0;
    m.md.memory_stats->setNumOfReads(0);
    m.md.memory_stats->setNumOfWrites(0);
    m.md.mem_time = 0.0;

    return EXIT_SUCCESS;
}

int Flash_Memory::Num_Of_Writes(int_16 addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getNumOfWrites();
}

int Flash_Memory::Num_Of_Reads(int_16 addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getNumOfReads();
}

u_char * Flash_Memory::ECC_Info(int_16 addr) const
{
    if (check_address(m, addr)) {
        return nullptr;
    }

    // TODO - doimplementovat

    int_32 pointer = addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    auto *ecc_data = (u_char *) malloc(sizeof(u_char) * m.md.ecc_size);
    if (!ecc_data) {
        cout << "Neni dostatek pameti!\n";
        return nullptr;
    }

    memcpy(ecc_data, &m.data[pointer], m.md.ecc_size);

    return ecc_data;
}

float Flash_Memory::Read_Time_Last(int_16 addr) const
{
    if (check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getLastReadPageTime();
}

float Flash_Memory::Program_Time_Last(int_16 addr) const
{
    if (check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getLastPageProgTime();
}

float Flash_Memory::Read_Time_Total(int_16 addr) const
{
    if (check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getTotalReadPageTime();
}

float Flash_Memory::Program_Time_Total(int_16 addr) const
{
    if (check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getTotalPageProgTime();
}

float Flash_Memory::Com_Total_Time(int_16 addr) const
{
    if (check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getComTime();
}

int Flash_Memory::Num_Of_Erases_Page(int_16 addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].getNumOfErases();
}

u_char * Flash_Memory::Sector_Status_Block(int_16 addr) const
{
    if (check_address(m, addr)) {
        return nullptr;
    }

    // TODO - doimplementovat
}

int Flash_Memory::Num_Of_Erases_Block(int_16 addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].getNumOfErases();
}

float Flash_Memory::Erase_Time_Total(int_16 addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].getTotalEraseTime();
}

float Flash_Memory::Erase_Time_Last(int_16 addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].getLastEraseTime();
}

bool Flash_Memory::Is_Bad_Block(int_16 addr) const
{
    if (check_address(m, addr)) {
        return false;
    }

    // TODO - doimplementovat
}

int Flash_Memory::Num_Of_Bad_Pages(int_16 addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    // TODO - doimplementovat
}

string Flash_Memory::ECC_Histogram(int_16 addr) const
{
    if (check_address(m, addr)) {
        return nullptr;
    }

    // TODO - doimplementovat

    return string("neco");
}

int Flash_Memory::Num_Of_Writes_Page(int_16 addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].getNumOfWrites();
}

int Flash_Memory::Num_Of_Reads_Page(int_16 addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].getNumOfReads();
}

u_char * Flash_Memory::Sector_Status_Page(int_16 addr) const
{
    if (check_address(m, addr)) {
        return nullptr;
    }

    // TODO - doimplementovat
}

int Flash_Memory::Num_Of_Bad_Blocks() const
{
    return m.md.num_of_bad_blocks;
}

int Flash_Memory::Num_Of_Bad_Pages() const
{
    return m.md.num_of_bad_pages;
}

string Flash_Memory::ECC_Histogram()
{
//        sort(&array[0], &array[n]);
//        int index = 0, k = n, reduce = 1;
//        double min = array[0], max = array[n - 1];
//        if (k > 10)
//            k = 50;
//
//        auto delta = (double) ((max - min) / k);
//
//        int *counted = (int *) malloc(sizeof(int) * k);
//        if (!counted) {
//            cout << "Neni dostatek pameti!\n";
//            return;
//        }
//
//
//        if (n >= 2500) {
//            reduce = 2;
//            if (n >= 5000) {
//                reduce = 5;
//                if (n >= 10000) {
//                    reduce = 10;
//                    if (n >= 25000) {
//                        reduce = 20;
//                        if (n >= 50000) {
//                            reduce = 50;
//                            if (n >= 100000) {
//                                reduce = 100;
//                                if (n >= 250000) {
//                                    reduce = 200;
//                                    if (n >= 500000) {
//                                        reduce = 500;
//                                        if (n >= 1000000) {
//                                            reduce = 1000;
//                                        }
//                                    }
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//
//
//        for (int i = 0; i < k; ++i) {
//            while (array[index] >= (array[0] + (i * delta)) && array[index] <= (array[0] + (i + 1) * delta)) {
//                ++counted[i];
//                index++;
//            }
//        }
//
//        for (int i = 0; i < k; ++i) {
//            cout << setw(8) << setprecision(3) << (array[0] + (i * delta))<< ": ";
//            for (int j = 0; j < counted[i] / reduce; ++j) {
//                cout << "*";
//            }
//            cout << endl;
//        }
//    }
}

int Flash_Memory::Num_Of_Writes() const
{
    return m.md.memory_stats->getNumOfReads();
}

int Flash_Memory::Num_Of_Reads() const
{
    return m.md.memory_stats->getNumOfWrites();
}

int Flash_Memory::Set_Prog_Time_Page(int_16 addr, float time) const
{
    if (check_address(m, addr)) {
        return -1;
    }


    m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].setPageProgTime(time);

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Prog_Time_Block(int_16 addr, float time) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    if (check_address(m, addr)) {
        return -1;
    }

    for (size_t i = (addr >> 8); i < m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].setPageProgTime(time);
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Prog_Time_Mem(float time) const
{
    for (size_t i = 0; i < m.md.num_of_blocks * m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].setPageProgTime(time);
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Read_Time_Page(int_16 addr, float time) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].setReadPageTime(time);

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Read_Time_Block(int_16 addr, float time) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    for (size_t i = (addr >> 8); i < m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].setReadPageTime(time);
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Read_Time_Mem(float time) const
{
    for (size_t i = 0; i < m.md.num_of_blocks * m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].setReadPageTime(time);
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Erase_Time_Block(int_16 addr, float time) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    m.md.blocks_stats[addr >> 8].setEraseTime(time);

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Erase_Time_Mem(float time) const
{
    for (size_t i = 0; i < m.md.num_of_blocks; ++i) {
        m.md.blocks_stats[i].setEraseTime(time);
    }

    return EXIT_SUCCESS;
}

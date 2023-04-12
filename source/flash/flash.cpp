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
    m.mem_cache = (u_char *) malloc(sizeof(u_char) * m.md.page_size);
    if (!m.mem_cache) {
        cout << "Není dostatek paměti.\n";
        return;
    }

    m.md.pages_stats = (page_statistics *) malloc(sizeof(page_statistics) * m.md.num_of_pages * m.md.num_of_blocks);
    if (!m.md.pages_stats) {
        cout << "Není dostatek paměti.\n";
        return;
    }

    for (int i = 0; i < m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].read_page_time = DEFAULT_READ_PAGE_TIME;
        m.md.pages_stats[i].page_prog_time = DEFAULT_PAGE_PROG_TIME;
        m.md.pages_stats[i].last_read_page_time = 0;
        m.md.pages_stats[i].last_page_prog_time = 0;
        m.md.pages_stats[i].total_read_page_time = 0;
        m.md.pages_stats[i].total_page_prog_time = 0;
        m.md.pages_stats[i].com_time = 0;
        m.md.pages_stats[i].num_of_reads = 0;
    }

    m.md.blocks_stats = (block_statistics *) malloc(sizeof(block_statistics) * m.md.num_of_blocks);
    if (!m.md.blocks_stats) {
        cout << "Není dostatek paměti.\n";
        return;
    }

    for (int i = 0; i < m.md.num_of_blocks; ++i) {
        m.md.blocks_stats[i].erase_time = DEFAULT_ERASE_TIME;
        m.md.blocks_stats[i].last_erase_time = 0;
        m.md.blocks_stats[i].total_erase_time = 0;
        m.md.blocks_stats[i].num_of_reads = 0;
    }
}
Flash_Memory::Flash_Memory(int_32 page_size, int_32 block_size, int_32 number_of_blocks, NMem_Type memory_type,
                           float read_page_time, float page_prog_time, float erase_time)
{
    m.md.page_size = page_size;
    m.md.block_size = block_size;
    m.md.num_of_blocks = number_of_blocks;
    m.md.memory_type = memory_type;

    m.mem_cache = (u_char *) malloc(sizeof(u_char) * m.md.page_size);
    if (!m.mem_cache) {
        cout << "Není dostatek paměti.\n";
        return;
    }

    free(m.md.pages_stats);
    m.md.pages_stats = (page_statistics *) malloc(sizeof(page_statistics) * m.md.num_of_pages * m.md.num_of_blocks);
    if (!m.md.pages_stats) {
        cout << "Není dostatek paměti.\n";
        return;
    }

    for (int i = 0; i < m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].read_page_time = read_page_time;
        m.md.pages_stats[i].page_prog_time = page_prog_time;
        m.md.pages_stats[i].last_read_page_time = 0;
        m.md.pages_stats[i].last_page_prog_time = 0;
        m.md.pages_stats[i].total_read_page_time = 0;
        m.md.pages_stats[i].total_page_prog_time = 0;
        m.md.pages_stats[i].com_time = 0;
        m.md.pages_stats[i].num_of_reads = 0;
    }

    free(m.md.blocks_stats);
    m.md.blocks_stats = (block_statistics *) malloc(sizeof(block_statistics) * m.md.num_of_blocks);
    if (!m.md.blocks_stats) {
        cout << "Není dostatek paměti.\n";
        return;
    }

    for (int i = 0; i < m.md.num_of_blocks; ++i) {
        m.md.blocks_stats[i].erase_time = erase_time;
        m.md.blocks_stats[i].last_erase_time = 0;
        m.md.blocks_stats[i].total_erase_time = 0;
        m.md.blocks_stats[i].num_of_reads = 0;
    }
}

Flash_Memory::~Flash_Memory()
{
    free(m.mem_cache);
    free(m.md.pages_stats);
    free(m.md.blocks_stats);
}

int Flash_Memory::Init()
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

int Flash_Memory::Read_Page(int_16 addr)
{
    if (!check_address(m, addr)) {
        cout << "Adresu nelze pouzit.\n";
        return EXIT_FAILURE;
    }

    int_32 pointer = addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

//    m.md.status = m.md.status | (1 << 5)

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[pointer + m.md.page_size] >> PAGE_VALID == 1) {
        cout << "Stranka neni validni.\n";
        return EXIT_FAILURE;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if ((m.data[pointer + 1] >> BLOCK_BAD) == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
    }

    memcpy(m.mem_cache, &m.data[pointer], m.md.page_size);

    /** Aktualizace času běhu. */
    m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].total_read_page_time +=
            m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].read_page_time;

    m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].last_read_page_time =
            m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].read_page_time;
    increase_time(READ_PAGE_TIME);

    m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].num_of_reads++;
    m.md.blocks_stats[(addr >> 8)].num_of_reads++;
    m.md.num_of_reads++;

    return EXIT_SUCCESS;
}

int Flash_Memory::Read_Sector(int_16 addr)
{
    // TODO - doimplementovat
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
    if (!check_address(m, addr))
        return EXIT_FAILURE;

    int_32 pointer = addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[pointer + m.md.page_size] >> PAGE_VALID == 1) {
        return EXIT_FAILURE;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[pointer + 1] >> BLOCK_BAD == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
    }

    memcpy(&m.data[pointer], m.mem_cache, m.md.page_size);

    /** Aktualizace času běhu. */
    m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].total_page_prog_time +=
            m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].page_prog_time;

    m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].last_page_prog_time =
            m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].page_prog_time;

    increase_time(PAGE_PROG_TIME);

    m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].num_of_writes++;
    m.md.blocks_stats[(addr >> 8)].num_of_writes++;
    m.md.num_of_writes++;

    return EXIT_SUCCESS;
}

int Flash_Memory::Program_Sector(int_16 addr)
{
    // TODO - doimplementovat
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
    if (!check_address(m, old_addr) || !check_address(m, new_addr))
        return EXIT_FAILURE;

    int_32 old_pointer = old_addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[old_pointer + m.md.page_size] >> PAGE_VALID == 1) {
        return EXIT_FAILURE;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[old_pointer + 1] >> BLOCK_BAD == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
    }

    /** Aktualizace času běhu. */
    m.md.pages_stats[(old_addr >> 8) * m.md.num_of_pages + (old_addr & (uint8_t) ~0L)].total_read_page_time +=
            m.md.pages_stats[(old_addr >> 8) * m.md.num_of_pages + (old_addr & (uint8_t) ~0L)].read_page_time;

    m.md.pages_stats[(old_addr >> 8) * m.md.num_of_pages + (old_addr & (uint8_t) ~0L)].last_read_page_time =
            m.md.pages_stats[(old_addr >> 8) * m.md.num_of_pages + (old_addr & (uint8_t) ~0L)].read_page_time;

    increase_time(READ_PAGE_TIME);

    m.md.pages_stats[(old_addr >> 8) * m.md.num_of_pages + (old_addr & (uint8_t) ~0L)].num_of_reads++;
    m.md.blocks_stats[(old_addr >> 8)].num_of_reads++;
    m.md.num_of_reads++;

    int_32 new_pointer = old_addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[new_pointer + m.md.page_size] >> PAGE_VALID == 1) {
        return EXIT_FAILURE;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[new_pointer + 1] >> BLOCK_BAD == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
    }

    memcpy(&m.data[old_pointer], &m.data[new_pointer], m.md.page_size);

    /** Aktualizace času běhu. */
    m.md.pages_stats[(new_addr >> 8) * m.md.num_of_pages + (new_addr & (uint8_t) ~0L)].total_page_prog_time +=
            m.md.pages_stats[(new_addr >> 8) * m.md.num_of_pages + (new_addr & (uint8_t) ~0L)].page_prog_time;

    m.md.pages_stats[(new_addr >> 8) * m.md.num_of_pages + (new_addr & (uint8_t) ~0L)].last_page_prog_time =
            m.md.pages_stats[(new_addr >> 8) * m.md.num_of_pages + (new_addr & (uint8_t) ~0L)].page_prog_time;

    increase_time(PAGE_PROG_TIME);

    m.md.pages_stats[(new_addr >> 8) * m.md.num_of_pages + (new_addr & (uint8_t) ~0L)].num_of_writes++;
    m.md.blocks_stats[(new_addr >> 8)].num_of_writes++;
    m.md.num_of_writes++;

    return EXIT_SUCCESS;
}

//for (int i = 0; i < numBytes; i++) {
//intValue |= ((int)charArray[i] << ((numBytes - 1 - i) * 8));
//}

int Flash_Memory::Block_Erase(int_8 addr)
{
    if (!check_address(m, addr)) {
        return EXIT_FAILURE;
    }

    int_32 pointer = addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[pointer + m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size] >> BLOCK_BAD == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return EXIT_FAILURE;
    }

    /** Smazání bloku. */
    memset(&m.data[pointer],0L,m.md.page_size);
    increase_counter(
            &m.data[pointer + m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size + BLOCK_WEAR],
            m.md.block_wear_size);

    if (counter_value(
            &m.data[pointer + m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size] + BLOCK_WEAR,
            m.md.block_wear_size) == MAX_ERASE_NUMBER) {
        m.data[pointer + m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size]
        = m.data[pointer + m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size] | (1 << BLOCK_BAD);
    }

    /** Aktualizace času běhu. */
    m.md.blocks_stats[(addr >> 8)].total_erase_time +=
            m.md.blocks_stats[(addr >> 8)].erase_time;

    m.md.blocks_stats[(addr >> 8)].last_erase_time =
            m.md.blocks_stats[(addr >> 8)].erase_time;

    m.md.blocks_stats[(addr >> 8)].num_of_erases++;

    increase_time(ERASE_TIME);

    return EXIT_SUCCESS;
}

int Flash_Memory::Reset()
{
    memset(m.data, 0L, m.md.true_mem_size);
    memset(m.mem_cache, 0, m.md.page_size);
    memset(m.md.pages_stats, 0, m.md.num_of_blocks * m.md.num_of_pages * sizeof(page_statistics));
    memset(m.md.blocks_stats, 0, m.md.num_of_blocks * sizeof(block_statistics));

    m.md.status = 0;
    m.md.num_of_bad_blocks = 0;
    m.md.num_of_bad_pages = 0;
    m.md.num_of_reads = 0;
    m.md.num_of_writes = 0;
    m.md.mem_time = 0.0;

    return EXIT_SUCCESS;
}

int Flash_Memory::Num_Of_Writes(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return -1;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].num_of_writes;
}

int Flash_Memory::Num_Of_Reads(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return -1;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].num_of_reads;
}

u_char * Flash_Memory::ECC_Info(int_16 addr) const
{
    if (!check_address(m, addr)) {
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
    if (!check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].last_read_page_time;
}

float Flash_Memory::Program_Time_Last(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].last_page_prog_time;
}

float Flash_Memory::Read_Time_Total(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].total_read_page_time;
}

float Flash_Memory::Program_Time_Total(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].total_page_prog_time;
}

float Flash_Memory::Com_Total_Time(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].com_time;
}

int Flash_Memory::Num_Of_Erases_Page(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].num_of_erases;
}

u_char * Flash_Memory::Sector_Status_Block(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return nullptr;
    }

    // TODO - doimplementovat
}

int Flash_Memory::Num_Of_Erases_Block(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].num_of_erases;
}

float Flash_Memory::Erase_Time_Total(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].last_erase_time;
}

float Flash_Memory::Erase_Time_Last(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].last_erase_time;
}

bool Flash_Memory::Is_Bad_Block(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return false;
    }

    // TODO - doimplementovat
}

int Flash_Memory::Num_Of_Bad_Pages(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return -1;
    }

    // TODO - doimplementovat
}

u_char * Flash_Memory::ECC_Histogram(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return nullptr;
    }

    // TODO - doimplementovat
}

int Flash_Memory::Num_Of_Writes_Page(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].num_of_writes;
}

int Flash_Memory::Num_Of_Reads_Page(int_16 addr) const
{
    if (!check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].num_of_reads;
}

u_char * Flash_Memory::Sector_Status_Page(int_16 addr) const
{
    if (!check_address(m, addr)) {
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

u_char * Flash_Memory::ECC_Histogram()
{
    // TODO - doimplementovat
}

int Flash_Memory::Num_Of_Writes() const
{
    return m.md.num_of_writes;
}

int Flash_Memory::Num_Of_Reads() const
{
    return m.md.num_of_reads;
}

int Flash_Memory::Set_Prog_Time_Page(int_16 addr, float time) const
{
    if (!check_address(m, addr)) {
        return -1;
    }


    m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].page_prog_time = time;

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Prog_Time_Block(int_16 addr, float time) const
{
    if (!check_address(m, addr)) {
        return -1;
    }

    if (!check_address(m, addr)) {
        return -1;
    }

    for (size_t i = (addr >> 8); i < m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].page_prog_time = time;
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Prog_Time_Mem(float time) const
{
    for (size_t i = 0; i < m.md.num_of_blocks * m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].page_prog_time = time;
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Read_Time_Page(int_16 addr, float time) const
{
    if (!check_address(m, addr)) {
        return -1;
    }

    m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].read_page_time = time;

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Read_Time_Block(int_16 addr, float time) const
{
    if (!check_address(m, addr)) {
        return -1;
    }

    for (size_t i = (addr >> 8); i < m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].read_page_time = time;
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Read_Time_Mem(float time) const
{
    for (size_t i = 0; i < m.md.num_of_blocks * m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].read_page_time = time;
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Erase_Time_Block(int_16 addr, float time) const
{
    if (!check_address(m, addr)) {
        return -1;
    }

    m.md.blocks_stats[addr >> 8].erase_time = time;

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Erase_Time_Mem(float time) const
{
    for (size_t i = 0; i < m.md.num_of_blocks; ++i) {
        m.md.blocks_stats[i].erase_time = time;
    }

    return EXIT_SUCCESS;
}

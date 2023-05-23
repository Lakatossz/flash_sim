#include "flash/flash.h"

/**
 * Funkce pro inkrementaci pole představující počítadlo.
 * @param array Pole u_char - počítadlo.
 * @param size Počet bytu pole.
 */
void increase_counter(u_char *array, size_t size) {
    for (size_t i = size - 1; i > 0; --i) {
        if (array[i] < (u_char) 1L) {
            array[i]++;
            return;
        }
    }
}

/**
 * Funkce pro získání integer hodnoty pole znaků o dané velikosti.
 * @param array Ukazatel na pole znaků.
 * @param size Velikost pole.
 * @return Číselná hodnota pole.
 */
u_int32_t counter_value(const u_char *array, size_t size) {

    u_int32_t value = 0;
    size_t i = 0;
    while (array[i] == (u_char) 1L || i < size) {
        value += (u_char) 1L;
        ++i;
    }

    return value;
}

/**
 * Zavolá daný algoritmus pro typ operace.
 * @param time_type Typ operace.
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

/**
 * Zkontroluje validnost adresy.
 * @param m Struktura paměti.
 * @param addr Kontrolovaná adresa.
 * @return Adresa je validní - 0, jinak - 1.
 */
u_int32_t check_address(nand_memory m, u_int16_t addr) {
    /** Zkontroluju adresu bloku. */
    if ((addr >> 8) >= m.md.num_of_blocks) {
        cout << (addr >> 8) << endl;
        cout << "Adresa bloku je příliš velká.\n";
        return EXIT_FAILURE;
        /** Zkontroluju adresu stránky. */
    } else if ((addr & (uint8_t) ~0L) >= m.md.num_of_pages) {
        cout << "Adresa stránky je příliš velká.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool check_flags(u_char *byte, size_t num_of_bits, size_t index) {
    return byte && num_of_bits > 0 && (num_of_bits > index) &&
            (byte[(index / num_of_bits) + (index % num_of_bits)] & (1 << index));
}

u_int32_t set_flag(u_char *byte, size_t num_of_bits, size_t index, bool value) {
    if (byte && num_of_bits > 0 && index < num_of_bits) {
        byte[(index / num_of_bits) + (index % num_of_bits)] =
                byte[(index / num_of_bits) + (index % num_of_bits)] | (u_char)(value << index);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

bool check_ecc(const u_char *ecc, size_t size) {
    if (!ecc) {
        return true;
    } else {
        return false;
    }
}

Flash_Memory::Flash_Memory()
{
    /** Vytvoření pole pro ukládání statistik. */
    m.md.pages_stats = new Page_Stats[m.md.num_of_pages * m.md.num_of_blocks];
    m.md.blocks_stats = new Block_Stats[m.md.num_of_blocks];
}
Flash_Memory::Flash_Memory(size_t page_size, size_t block_size, size_t number_of_blocks, NMem_Type memory_type,
                           float read_page_time, float page_prog_time, float erase_time)
{
    m.md.page_size = page_size;
    m.md.block_size = block_size;
    m.md.num_of_blocks = number_of_blocks;
    m.md.mem_type = memory_type;

    delete(m.md.pages_stats);
    delete(m.md.blocks_stats);

    m.md.pages_stats = new Page_Stats[m.md.num_of_pages * m.md.num_of_blocks];
    m.md.blocks_stats = new Block_Stats[m.md.num_of_blocks];

    for (int i = 0; i < m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].setReadPageTime(read_page_time);
        m.md.pages_stats[i].setPageProgTime(page_prog_time);
    }

    for (int i = 0; i < m.md.num_of_blocks; ++i) {
        m.md.blocks_stats[i].setEraseTime(erase_time);
    }
}

Flash_Memory::Flash_Memory(size_t page_size, size_t block_size, size_t number_of_blocks, NMem_Type memory_type)
{
    m.md.page_size = page_size;
    m.md.block_size = block_size;
    m.md.num_of_blocks = number_of_blocks;
    m.md.mem_type = memory_type;

    delete(m.md.pages_stats);
    delete(m.md.blocks_stats);

    m.md.pages_stats = new Page_Stats[m.md.num_of_pages * m.md.num_of_blocks];
    m.md.blocks_stats = new Block_Stats[m.md.num_of_blocks];
}

Flash_Memory::~Flash_Memory()
{
    delete(m.mem_cache);
    delete(m.md.pages_stats);
    delete(m.md.blocks_stats);
}

int Flash_Memory::Flash_Init()
{
    /** Pocet stranek uvnitr bloku. */
    m.md.num_of_pages = m.md.block_size / m.md.page_size;

    /** Počet bytů, které paměť obsahuje. */
    m.md.mem_size = m.md.num_of_blocks * m.md.block_size;

    /** Celková velikost paměti = úložný prostor + metadata pro stránky + metadata pro bloky. */
    m.md.true_mem_size = m.md.mem_size
            + m.md.num_of_blocks * m.md.md_b_size
            + m.md.num_of_blocks * m.md.num_of_pages * m.md.md_p_size
            + m.md.num_of_blocks * m.md.num_of_pages * m.md.num_of_sectors * m.md.md_p_size;

    m.data = new u_char[m.md.true_mem_size];

    m.md.status = 0L;

//    uuid_generate_random(m.md.id);

    cout << "Sector size: " << m.md.sector_size << endl;
    cout << "Page size: " << m.md.page_size << endl;
    cout << "Block size: " << m.md.block_size << endl;
    cout << "Num of sectors: " << m.md.num_of_sectors << endl;
    cout << "Number of pages: " << m.md.num_of_pages << endl;
    cout << "Number of blocks: " << m.md.num_of_blocks << endl;
    cout << "Memory size: " << m.md.mem_size << endl;
    cout << "True memory size: " << m.md.true_mem_size << endl;

    return EXIT_SUCCESS;
}

int Flash_Memory::Flash_Init(u_char *data, size_t size)
{
    /** Pocet stranek uvnitr bloku. */
    m.md.num_of_pages = m.md.block_size / m.md.page_size;

    /** Počet bytů, které paměť obsahuje. */
    m.md.mem_size = m.md.num_of_blocks * m.md.block_size;

    /** Celková velikost paměti = úložný prostor + metadata pro stránky + metadata pro bloky. */
    m.md.true_mem_size = m.md.mem_size
            + m.md.num_of_blocks * m.md.md_b_size
            + m.md.num_of_blocks * m.md.num_of_pages * m.md.md_p_size
            + m.md.num_of_blocks * m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size;

    m.data = new u_char(m.md.true_mem_size);
    memcpy(m.data, data, size);

    m.md.status = 0L;

//    uuid_generate_random(m.md.id);

    cout << "Sector size: " << m.md.sector_size << endl;
    cout << "Page size: " << m.md.page_size << endl;
    cout << "Block size: " << m.md.block_size << endl;
    cout << "Num of sectors: " << m.md.num_of_sectors << endl;
    cout << "Number of pages: " << m.md.num_of_pages << endl;
    cout << "Number of blocks: " << m.md.num_of_blocks << endl;
    cout << "Memory size: " << m.md.mem_size << endl;
    cout << "True memory size: " << m.md.true_mem_size << endl;

    return EXIT_SUCCESS;
}

int Flash_Memory::Cache_Init() {
    m.mem_cache = new u_char[m.md.page_size];
    return EXIT_SUCCESS;
}

int Flash_Memory::Read_Page(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        cout << "Adresu nelze pouzit.\n";
        return EXIT_FAILURE;
    }

    u_int32_t pointer = (addr >> 8) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                     + (addr & (uint8_t) ~0L) * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    if (!check_flags((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, true);
    } else {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_ERR, true);
        return EXIT_FAILURE;
    }

    if (check_flags(&m.data[pointer + m.md.page_size + 1],
                    m.md.ecc_size / sizeof(u_char), PAGE_SPARE_BAD) ||
        !check_flags(&m.data[pointer + m.md.page_size + 1],
                    m.md.ecc_size / sizeof(u_char), PAGE_SPARE_VALID)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_ERR, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
        cout << "Stránka je poškozená.\n";
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
    m.md.mem_stats->addNumOfReads(1);

    set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
    set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, false);

    return EXIT_SUCCESS;
}

int Flash_Memory::Read_Sector(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        cout << "Adresu nelze pouzit.\n";
        return EXIT_FAILURE;
    }

    u_int32_t pointer = (addr >> 8) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
                                       + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                        + (addr & (uint8_t) ~0L) * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    if (!check_flags((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, true);
    } else {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_ERR, true);
        return EXIT_FAILURE;
    }

    /* TODO - tady to bude fungovat trochu jinak nez u stranky. */
    if (check_ecc(reinterpret_cast<const u_char *>(&m.data[pointer + m.md.page_size + 1])
            , m.md.ecc_size / sizeof(u_char))) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_ECC, true);
        set_flag((u_char *) &m.data[pointer + m.md.page_size],
                 m.md.md_p_size - m.md.ecc_size / sizeof(u_char), PAGE_SPARE_ERROR, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
        return EXIT_FAILURE;
    }

    if (check_flags(&m.data[pointer + m.md.page_size + 1],
                    m.md.ecc_size / sizeof(u_char), PAGE_SPARE_BAD) ||
        !check_flags(&m.data[pointer + m.md.page_size + 1],
                     m.md.ecc_size / sizeof(u_char), PAGE_SPARE_VALID)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_ERR, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
        cout << "Sektor je poškozený.\n";
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
    m.md.mem_stats->addNumOfReads(1);

    set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
    set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, false);

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

int Flash_Memory::Program_Page(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, true);
        return EXIT_FAILURE;
    }
//    blok: (addr >> 8) stranka: (addr & (uint8_t) ~0L)
    u_int32_t pointer = (addr >> 8) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
                                       + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                        + (addr & (uint8_t) ~0L) * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    if (!check_flags((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, true);
    } else {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, true);
        return EXIT_FAILURE;
    }

    if (check_flags(&m.data[pointer + m.md.page_size + 1],
                    m.md.ecc_size / sizeof(u_char), PAGE_SPARE_BAD) ||
        check_flags(&m.data[pointer + m.md.page_size + 1],
                     m.md.ecc_size / sizeof(u_char), PAGE_SPARE_VALID)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, PAGE_SPARE_ERROR, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
        cout << "Stránka je poškozená.\n";
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
    m.md.mem_stats->addNumOfWrites(1);

    set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
    set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, false);
    return EXIT_SUCCESS;
}

int Flash_Memory::Program_Sector(u_int32_t addr)
{
    if (check_address(m, addr)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, true);
        return EXIT_FAILURE;
    }

    u_int32_t pointer = (addr >> 8) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
                                       + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                        + (addr & (uint8_t) ~0L)
                        * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    if (!check_flags((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, true);
    } else {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, true);
        return EXIT_FAILURE;
    }

    if (check_flags(&m.data[pointer + m.md.page_size + 1],
                    m.md.ecc_size / sizeof(u_char), PAGE_SPARE_BAD) ||
        check_flags(&m.data[pointer + m.md.page_size + 1],
                    m.md.ecc_size / sizeof(u_char), PAGE_SPARE_VALID)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, PAGE_SPARE_ERROR, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
        cout << "Stránka je poškozená.\n";
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
    m.md.mem_stats->addNumOfWrites(1);

    set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
    set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, false);
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

int Flash_Memory::Program_Data_Move(u_int16_t old_addr, u_int16_t new_addr)
{
    if (check_address(m, old_addr) || check_address(m, new_addr)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, true);
        return EXIT_FAILURE;
    }

    u_int32_t old_pointer = (old_addr >> 8) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                        + (old_addr & (uint8_t) ~0L)
                        * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    if (!check_flags((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, true);
    } else {
        set_flag((u_char *) &m.data[old_pointer + m.md.page_size],
                 m.md.md_p_size - m.md.ecc_size / sizeof(u_char), PAGE_SPARE_ERROR, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, true);
        return EXIT_FAILURE;
    }

    if (check_ecc(reinterpret_cast<const u_char *>(&m.data[old_pointer + m.md.page_size + 1])
            , m.md.ecc_size / sizeof(u_char))) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_ECC, true);
        set_flag((u_char *) &m.data[old_pointer + m.md.page_size],
                 m.md.md_p_size - m.md.ecc_size / sizeof(u_char), PAGE_SPARE_ERROR, true);
    }

    if (check_flags(&m.data[old_pointer + m.md.page_size + 1],
                    m.md.ecc_size / sizeof(u_char), PAGE_SPARE_BAD) ||
        !check_flags(&m.data[old_pointer + m.md.page_size + 1],
                     m.md.ecc_size / sizeof(u_char), PAGE_SPARE_VALID)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_ERR, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
        cout << "Sektor je poškozený.\n";
        return EXIT_FAILURE;
    }

    size_t old_page_stats_pointer = (old_addr >> 8)
            * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size) + (old_addr & (uint8_t) ~0L);

    /** Aktualizace času běhu. */
    m.md.pages_stats[old_page_stats_pointer]
        .addTotalReadPageTime(m.md.pages_stats[old_page_stats_pointer].getPageProgTime());
    m.md.pages_stats[old_page_stats_pointer]
            .setLastReadPageTime(m.md.pages_stats[old_page_stats_pointer].getPageProgTime());

    increase_time(READ_PAGE_TIME);

    m.md.pages_stats[old_page_stats_pointer].addNumOfReads(1);
    m.md.blocks_stats[(old_addr >> 8)].addNumOfReads(1);
    m.md.mem_stats->addNumOfReads(1);

    u_int32_t new_pointer = (new_addr >> 8) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                    + (new_addr & (uint8_t) ~0L)
                    * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    if (check_flags(&m.data[new_pointer + m.md.page_size + 1],
                    m.md.ecc_size / sizeof(u_char), PAGE_SPARE_BAD) ||
        check_flags(&m.data[new_pointer + m.md.page_size + 1],
                    m.md.ecc_size / sizeof(u_char), PAGE_SPARE_VALID)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, PAGE_SPARE_ERROR, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
        cout << "Stránka je poškozená.\n";
        return EXIT_FAILURE;
    }

    memcpy(&m.data[old_pointer], &m.data[new_pointer], m.md.page_size);

    size_t new_page_stats_pointer = (new_addr >> 8)
            * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size) + (new_addr & (uint8_t) ~0L);

    /** Aktualizace času běhu. */
    m.md.pages_stats[new_page_stats_pointer]
        .addTotalPageProgTime(m.md.pages_stats[new_page_stats_pointer].getPageProgTime());

    m.md.pages_stats[new_page_stats_pointer]
            .setLastPageProgTime(m.md.pages_stats[new_page_stats_pointer].getPageProgTime());

    increase_time(PAGE_PROG_TIME);

    m.md.pages_stats[new_page_stats_pointer].addNumOfWrites(1);
    m.md.blocks_stats[(new_addr >> 8)].addNumOfWrites(1);
    m.md.mem_stats->addNumOfWrites(1);

    set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
    set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, false);
    return EXIT_SUCCESS;
}

int Flash_Memory::Block_Erase(u_int8_t addr)
{
    if (check_address(m, addr)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, true);
        return EXIT_FAILURE;
    }

    u_int32_t pointer = addr * (m.md.block_size + m.md.md_b_size
            + m.md.num_of_pages * m.md.md_p_size + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size);

    if (!check_flags((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, true);
    } else {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, true);
    }

    if (check_flags(&m.data[pointer + m.md.block_size + m.md.page_size * m.md.md_p_size + 1],
                    m.md.ecc_size / sizeof(u_char), BLOCK_SPARE_BAD) ||
        check_flags(&m.data[pointer + m.md.block_size + m.md.page_size * m.md.md_p_size + 1],
                     m.md.ecc_size / sizeof(u_char), BLOCK_SPARE_ERASED) ||
        check_flags(&m.data[pointer + m.md.block_size + m.md.page_size * m.md.md_p_size + 1],
                    m.md.ecc_size / sizeof(u_char), BLOCK_SPARE_ERASE_SUSPEND) ||
        check_flags(&m.data[pointer + m.md.block_size + m.md.page_size * m.md.md_p_size + 1],
                    m.md.ecc_size / sizeof(u_char), BLOCK_SPARE_LOCKED) ||
        check_flags(&m.data[pointer + m.md.block_size + m.md.page_size * m.md.md_p_size + 1],
                    m.md.ecc_size / sizeof(u_char), BLOCK_SPARE_WORE_OUT) ||
        check_flags(&m.data[pointer + m.md.block_size + m.md.page_size * m.md.md_p_size + 1],
                    m.md.ecc_size / sizeof(u_char), BLOCK_SPARE_RESERVED)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, BLOCK_SPARE_ERROR, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
        cout << "Blok je poškozený.\n";
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
    } else {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, BLOCK_SPARE_ERROR, true);
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
        cout << "Blok je poškozený - wore out.\n";
        return EXIT_FAILURE;
    }

    /** Aktualizace času běhu. */
    m.md.blocks_stats[(addr >> 8)].addTotalEraseTime(m.md.blocks_stats[(addr >> 8)].getEraseTime());

    m.md.blocks_stats[(addr >> 8)].setLastEraseTime(m.md.blocks_stats[(addr >> 8)].getEraseTime());

    m.md.blocks_stats[(addr >> 8)].addNumOfErases(1);

    increase_time(ERASE_TIME);

    set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_RB, false);
    set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE, false);
    return EXIT_SUCCESS;
}

int Flash_Memory::Reset()
{
    memset(m.data, 0L, m.md.true_mem_size);
    memset(m.mem_cache, 0, m.md.page_size);
    memset(m.md.pages_stats, 0, m.md.num_of_blocks * m.md.num_of_pages * sizeof(Page_Stats));
    memset(m.md.blocks_stats, 0, m.md.num_of_blocks * sizeof(Block_Stats));

    m.md.status = 0L;
    m.md.num_of_bad_blocks = 0;
    m.md.num_of_bad_pages = 0;
    m.md.mem_stats->setNumOfReads(0);
    m.md.mem_stats->setNumOfWrites(0);
    m.md.mem_time = 0.0;

    return EXIT_SUCCESS;
}

size_t Flash_Memory::Num_Of_Writes(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getNumOfWrites();
}

size_t Flash_Memory::Num_Of_Reads(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getNumOfReads();
}

u_char * Flash_Memory::ECC_Info(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return nullptr;
    }

    // TODO - doimplementovat

    u_int32_t pointer = (addr >> 8) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                    + (addr & (uint8_t) ~0L)
                    * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    auto *ecc_data = (u_char *) malloc(sizeof(u_char) * m.md.ecc_size);
    if (!ecc_data) {
        cout << "Neni dostatek pameti!\n";
        return nullptr;
    }

    memcpy(ecc_data, &m.data[pointer], m.md.ecc_size);

    return ecc_data;
}

float Flash_Memory::Read_Time_Last(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getLastReadPageTime();
}

float Flash_Memory::Program_Time_Last(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getLastPageProgTime();
}

float Flash_Memory::Read_Time_Total(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getTotalReadPageTime();
}

float Flash_Memory::Program_Time_Total(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getTotalPageProgTime();
}

float Flash_Memory::Com_Total_Time(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return 0;
    }

    return m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].getComTime();
}

size_t Flash_Memory::Num_Of_Erases_Page(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].getNumOfErases();
}

u_char * Flash_Memory::Sector_Status_Block(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return nullptr;
    }

    u_int32_t pointer = (addr >> 8) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
                                       + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                        + (addr & (uint8_t) ~0L)
                        * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    // TODO - doimplementovat
}

size_t Flash_Memory::Num_Of_Erases_Block(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].getNumOfErases();
}

float Flash_Memory::Erase_Time_Total(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].getTotalEraseTime();
}

float Flash_Memory::Erase_Time_Last(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].getLastEraseTime();
}

bool Flash_Memory::Is_Bad_Block(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return false;
    }

    u_int32_t pointer = (addr >> 8) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
                                       + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                        + (addr & (uint8_t) ~0L)
                        * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    // TODO - doimplementovat
}

size_t Flash_Memory::Num_Of_Bad_Pages(u_int8_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    // TODO - doimplementovat
}

string Flash_Memory::ECC_Histogram(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return nullptr;
    }

    // TODO - doimplementovat

    return string("neco");
}

size_t Flash_Memory::Num_Of_Writes_Page(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].getNumOfWrites();
}

size_t Flash_Memory::Num_Of_Reads_Page(u_int16_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(addr >> 8)].getNumOfReads();
}

u_char * Flash_Memory::Sector_Status_Page(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return nullptr;
    }

    u_int32_t pointer = (addr >> 8) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
                                       + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                        + (addr & (uint8_t) ~0L)
                          * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    // TODO - doimplementovat
}

size_t Flash_Memory::Num_Of_Bad_Blocks() const
{
    return m.md.num_of_bad_blocks;
}

size_t Flash_Memory::Num_Of_Bad_Pages() const
{
    return m.md.num_of_bad_pages;
}

string Flash_Memory::ECC_Histogram()
{

}

size_t Flash_Memory::Num_Of_Writes() const
{
    return m.md.mem_stats->getNumOfReads();
}

size_t Flash_Memory::Num_Of_Reads() const
{
    return m.md.mem_stats->getNumOfWrites();
}

int Flash_Memory::Set_Prog_Time_Page(u_int16_t addr, float time) const
{
    if (check_address(m, addr)) {
        return -1;
    }


    m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].setPageProgTime(time);

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Prog_Time_Block(u_int16_t addr, float time) const
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

int Flash_Memory::Set_Read_Time_Page(u_int16_t addr, float time) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    m.md.pages_stats[(addr >> 8) * m.md.num_of_pages + (addr & (uint8_t) ~0L)].setReadPageTime(time);

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Read_Time_Block(u_int16_t addr, float time) const
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

int Flash_Memory::Set_Erase_Time_Block(u_int16_t addr, float time) const
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

int Flash_Memory::Save_Memory(const string& file_name) const
{
    ofstream f(file_name, ios::out | ios::binary);
    if(!f) {
        cout << "Nepodarilo se otevrit soubor " << file_name << endl;
        return 1;
    }

    f.write((char *) m.data, m.md.true_mem_size);

    f.close();
    cout << "Soubor byl uzavren.\n";
    return EXIT_SUCCESS;
}

int Flash_Memory::Save_State(const string& file_name)
{
    ofstream f(file_name);
    if (!f) {
        cout << "Nepodarilo se otevrit soubor " << file_name << endl;
        return EXIT_FAILURE;
    }

    json j;
//    j["id"] = m.md.id;
    j["page_size"] = m.md.page_size;
    j["num_of_pages"] = m.md.num_of_pages;
    j["block_size"] = m.md.block_size;
    j["num_of_blocks"] = m.md.num_of_blocks;
    j["mem_size"] = m.md.mem_size;
    j["true_mem_size"] = m.md.true_mem_size;
    j["ecc_size"] = m.md.ecc_size;
    j["md_p_size"] = m.md.md_p_size;
    j["md_b_size"] = m.md.md_b_size;
    j["block_wear_size"] = m.md.block_wear_size;
    j["status"] = m.md.status;
    j["mem_type"] = m.md.mem_type;

    j["mem_time"] = m.md.mem_time;
    j["mem_stats"] = {
            {"num_of_reads", m.md.mem_stats->getNumOfReads()},
            {"num_of_writes", m.md.mem_stats->getNumOfWrites()}
    };

    j["num_of_bad_blocks"] = m.md.num_of_bad_blocks;
    j["num_of_bad_pages"] = m.md.num_of_bad_pages;
    auto jsonObjects = json::array();
    for(auto i = 0; i <= m.md.num_of_blocks; ++i) {
        jsonObjects.push_back({
            {"erase_time", m.md.blocks_stats[i].getEraseTime()},
            {"last_erase_time", m.md.blocks_stats[i].getLastEraseTime()},
            {"total_erase_time", m.md.blocks_stats[i].getTotalEraseTime()},
            {"num_of_reads", m.md.blocks_stats[i].getNumOfReads()},
            {"num_of_writes", m.md.blocks_stats[i].getNumOfWrites()},
            {"num_of_erases", m.md.blocks_stats[i].getNumOfErases()}
        });
    }

    j["block_stats"] = jsonObjects;

    jsonObjects = json::array();
    for(auto i = 0; i <= m.md.num_of_blocks * m.md.num_of_pages; ++i) {
        jsonObjects.push_back({
            {"read_page_time", m.md.pages_stats[i].getReadPageTime()},
            {"page_prog_time", m.md.pages_stats[i].getPageProgTime()},
            {"last_read_page_time", m.md.pages_stats[i].getLastReadPageTime()},
            {"last_page_prog_time", m.md.pages_stats[i].getLastPageProgTime()},
            {"total_read_page_time", m.md.pages_stats[i].getTotalReadPageTime()},
            {"total_page_prog_time", m.md.pages_stats[i].getTotalPageProgTime()},
            {"com_time", m.md.pages_stats[i].getComTime()},
            {"num_of_reads", m.md.pages_stats[i].getNumOfReads()},
            {"num_of_writes", m.md.pages_stats[i].getNumOfWrites()}
        });
    }

    j["page_stats"] = jsonObjects;

    f << j;
    f.close();
    cout << "Soubor byl uzavren.\n";
    return EXIT_SUCCESS;
}

int Flash_Memory::Load_Memory(const string& file_name) const
{
    ifstream f(file_name, ios::in | ios::binary);
    if (!f) {
        cout << "Nepodarilo se otevrit soubor " << file_name << endl;
        return EXIT_FAILURE;
    }

    string line;
    size_t size = 0;

    while(getline( f, line)) {
        size += line.length();
    }

    if (size >= m.md.true_mem_size) {
        f.clear();
        f.seekg(0);
        auto *data = new u_char(size);

        f >> data;

//        ostringstream ss;
//        ss << f.rdbuf(); // reading data
    }

}

Flash_Memory * Flash_Memory::Load_State(const string& file_name) const
{
    ifstream f(file_name);
    if (!f) {
        cout << "Nepodarilo se otevrit soubor " << file_name << endl;
        return nullptr;
    }
    json j = json::parse(f);

    auto *flashMemory = new Flash_Memory(
            j["page_size"],
            j["block_size"],
            j["num_of_blocks"],
            j["mem_type"]
            );

//    flashMemory->Set_Id(data["id"]);

    size_t size = 0;
    u_char *data = this->Get_Data(size);

    if (this->Get_True_Mem_Size() < size) {
        return nullptr;
    }

    flashMemory->Flash_Init(data, size);

    auto jsonObject = j["mem_stats"];
    flashMemory->m.md.mem_stats->setNumOfWrites(jsonObject["num_of_writes"]);
    flashMemory->m.md.mem_stats->setNumOfReads(jsonObject["num_of_reads"]);

    // num_of_bad_blocks
    // num_of_bad_pages

    jsonObject = j["block_stats"];

    for(auto i = 0; i < flashMemory->m.md.num_of_blocks; i++) {
        flashMemory->m.md.blocks_stats[i].setEraseTime(jsonObject.at(i)["erase_time"]);
        flashMemory->m.md.blocks_stats[i].setLastEraseTime(jsonObject.at(i)["last_erase_time"]);
        flashMemory->m.md.blocks_stats[i].setTotalEraseTime(jsonObject.at(i)["total_erase_time"]);
        flashMemory->m.md.blocks_stats[i].setNumOfReads(jsonObject.at(i)["num_of_reads"]);
        flashMemory->m.md.blocks_stats[i].setNumOfWrites(jsonObject.at(i)["num_of_writes"]);
        flashMemory->m.md.blocks_stats[i].setNumOfErases(jsonObject.at(i)["num_of_erases"]);
    }

    jsonObject = j["page_stats"];

    for(auto i = 0; i < flashMemory->m.md.num_of_blocks * flashMemory->m.md.num_of_pages; i++) {
        flashMemory->m.md.pages_stats[i].setReadPageTime(jsonObject.at(i)["read_page_time"]);
        flashMemory->m.md.pages_stats[i].setPageProgTime(jsonObject.at(i)["page_prog_time"]);
        flashMemory->m.md.pages_stats[i].setLastReadPageTime(jsonObject.at(i)["last_read_page_time"]);
        flashMemory->m.md.pages_stats[i].setLastPageProgTime(jsonObject.at(i)["last_page_prog_time"]);
        flashMemory->m.md.pages_stats[i].setTotalReadPageTime(jsonObject.at(i)["total_read_page_time"]);
        flashMemory->m.md.pages_stats[i].setTotalPageProgTime(jsonObject.at(i)["total_page_prog_time"]);
        flashMemory->m.md.pages_stats[i].setComTime(jsonObject.at(i)["com_time"]);
        flashMemory->m.md.pages_stats[i].setNumOfReads(jsonObject.at(i)["num_of_reads"]);
        flashMemory->m.md.pages_stats[i].setNumOfWrites(jsonObject.at(i)["num_of_writes"]);
    }

    return flashMemory;
}

void Flash_Memory::Set_Id(uuid_t id) {
    memcpy(m.md.id, id, sizeof(uuid_t));
}

u_char * Flash_Memory::Get_Data(size_t& size) const {
    auto data = new u_char(m.md.true_mem_size);
    memcpy(data, m.data, m.md.true_mem_size);
    size = m.md.true_mem_size;
    return data;
}

size_t Flash_Memory::Get_True_Mem_Size() const {
    return m.md.true_mem_size;
}

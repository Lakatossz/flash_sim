#include "flash/flash.h"

/**
 * Zkontroluje validnost adresy.
 * @param m Struktura paměti.
 * @param addr Kontrolovaná adresa.
 * @return Adresa je validní - 0, jinak - 1.
 */
u_int32_t check_address(nand_memory m, u_int32_t addr) {
    /** Zkontroluju adresu bloku. */
    if ((uint16_t)(addr >> 16) >= m.md.num_of_blocks) {
//        cout << (addr >> 16) << endl;
//        cout << "Adresa bloku je příliš velká.\n";
        return EXIT_FAILURE;
        /** Zkontroluju adresu stránky. */
    } else if ((uint8_t)(addr >> 8) >= m.md.num_of_pages) {
//        cout << (addr >> 8) << endl;
//        cout << "Adresa stránky je příliš velká.\n";
        return EXIT_FAILURE;
    } else if ((uint8_t)(addr & (uint8_t) ~0L) >= m.md.num_of_pages) {
//        cout << (addr & (uint8_t) ~0L) << endl;
//        cout << "Adresa stránky je příliš velká.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool check_flags(u_char *byte, size_t num_of_bits, size_t index) {
    return byte && num_of_bits > 0 && (num_of_bits > index) &&
            (byte[(index / num_of_bits)] & (1 << (index % num_of_bits)));
}

bool check_flags(u_char *byte, size_t index) {
    return (byte[(index / 8)] & (1 << (index % 8)));
}

void set_flag(u_char *byte, size_t index) {
    byte[(index / 8)] |= (u_char)(1UL << (index % 8));
}

void clear_flag(u_char *byte, size_t index) {
    byte[(index / 8)] &= ~(u_char)(1UL << (index % 8));
}

u_int32_t set_flag(u_char *byte, size_t num_of_bits, size_t index) {
    if (byte && num_of_bits > 0 && index < num_of_bits) {
        byte[(index / num_of_bits) + (index % num_of_bits)] |= (u_char)(1UL << index);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

u_int32_t clear_flag(u_char *byte, size_t num_of_bits, size_t index) {
    if (byte && num_of_bits > 0 && index < num_of_bits) {
        byte[(index / num_of_bits) + (index % num_of_bits)] &= ~(u_char)(1UL << index);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int count_set_bits(u_char byte) {
    int count = 0;
    while (byte > 0) {
        if (byte & 1)
            count++;
        byte >>= 1;
    }
    return count;
}

size_t check_ecc(const u_char *ecc, size_t n) {
    size_t count = 0;
    for(int i = 0; i < n; i++) {
        count += count_set_bits(ecc[i]);
    }
    return count;
}

Flash_Memory::Flash_Memory()
{
    /** Vytvoření pole pro ukládání statistik. */
    m.md.pages_stats = new Page_Stats[m.md.num_of_pages * m.md.num_of_blocks];
    m.md.blocks_stats = new Block_Stats[m.md.num_of_blocks];
}


Flash_Memory::Flash_Memory(size_t page_size, size_t block_size, size_t number_of_blocks, mem_type_values memory_type,
                           float read_page_time, float page_prog_time, float erase_time)
{
    /** Kontrola parametrů paměti. */
    if (block_size % page_size != 0 || page_size % m.md.sector_size != 0) {
        cout << "Nevalidni parametry pameti!\n";
        return;
    }

    m.md.num_of_sectors = page_size / m.md.sector_size;
    m.md.page_size = page_size;
    m.md.block_size = block_size;
    m.md.num_of_blocks = number_of_blocks;
    m.md.mem_type = memory_type;

    /** Je potreba znovu vytvorit pole pro staty. */
    delete(m.md.pages_stats);
    delete(m.md.blocks_stats);
    m.md.pages_stats = new Page_Stats[m.md.num_of_pages * m.md.num_of_blocks];
    m.md.blocks_stats = new Block_Stats[m.md.num_of_blocks];

    /** A také nastavit nadefinované časy. */
    for (int i = 0; i < m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].setReadPageTime(read_page_time);
        m.md.pages_stats[i].setPageProgTime(page_prog_time);
    }

    for (int i = 0; i < m.md.num_of_blocks; ++i) {
        m.md.blocks_stats[i].setEraseTime(erase_time);
    }
}

Flash_Memory::Flash_Memory(size_t page_size, size_t block_size, size_t number_of_blocks, mem_type_values memory_type)
{
    m.md.page_size = page_size;
    m.md.block_size = block_size;
    m.md.num_of_blocks = number_of_blocks;
    m.md.mem_type = memory_type;

    /** Je potreba znovu vytvorit pole pro staty. */
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
    /** Počet stránek uvnitř bloku. */
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

    m.md.id = DEFAULT_ID;
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

int Flash_Memory::Read_Page(u_int32_t addr) {
    if (check_address(m, addr)) {
        return EXIT_FAILURE;
    }

    if (check_flags((u_char *)&m.md.status, STATUS_FLAG_RB) == 0) {
        set_flag((u_char *)&m.md.status, STATUS_FLAG_RB);
    } else {
        set_flag((u_char *)&m.md.status, STATUS_FLAG_ERR);
        return EXIT_FAILURE;
    }

    u_int32_t pointer = (uint16_t)(addr >> 16) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                     + (uint8_t)(addr >> 8) * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    if (check_flags(&m.data[pointer + m.md.page_size + m.md.md_s_size * m.md.num_of_sectors + 1], PAGE_SPARE_BAD)) {
        set_flag((u_char *) &m.md.status, STATUS_FLAG_ERR);
        clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
        cout << "Stránka je poškozená.\n";
        return EXIT_FAILURE;
    }

    size_t page_stats_pointer = (uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8);

    /* Pro jistotu vymažeme předchozí obsah cache. */
    memset(m.mem_cache, 0L, m.md.page_size);

    for (int i = 0; i < m.md.page_size / m.md.sector_size; i++) {
        if (check_ecc(&m.data[pointer + i * m.md.sector_size + m.md.md_s_size * i + m.md.sector_size + 1], 2) > 0) {
            m.md.pages_stats[page_stats_pointer]
                    .increaseHistogramOnIndex(
                            check_ecc(&m.data[pointer + i* m.md.sector_size + m.md.md_s_size * i
                            + m.md.sector_size + 1],m.md.ecc_size / 8));

            set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_ECC);
            set_flag((u_char *) &m.data[pointer + m.md.sector_size], SECTOR_SPARE_ERROR);
        }
        memcpy(m.mem_cache + i * m.md.sector_size,
               &m.data[pointer+ i * m.md.sector_size + m.md.md_s_size * i], m.md.sector_size);
    }

    /** Aktualizace času běhu. */
    m.md.pages_stats[page_stats_pointer]
        .addTotalReadPageTime(m.md.pages_stats[page_stats_pointer].getReadPageTime());

    m.md.pages_stats[page_stats_pointer]
        .setLastReadPageTime(m.md.pages_stats[page_stats_pointer].getReadPageTime());

    this->Increase_Time(page_stats_pointer, READ_PAGE);
    for (int i = 0; i < m.md.num_of_sectors; i++) {
        this->Simulate_Error(pointer + i * m.md.sector_size + m.md.md_s_size * i + m.md.sector_size + 1, READ_PAGE);
    }

    m.md.pages_stats[page_stats_pointer].addNumOfReads(1);
    m.md.blocks_stats[(uint16_t)(addr >> 16)].addNumOfReads(1);
    m.md.mem_stats->addNumOfReads(1);

    clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
    clear_flag((u_char *) &m.md.status, STATUS_FLAG_EPE);

    return EXIT_SUCCESS;
}

int Flash_Memory::Read_Sector(u_int32_t addr) {
    if (check_address(m, addr)) {
//        cout << "Adresu nelze pouzit.\n";
        return EXIT_FAILURE;
    }

    if (check_flags((u_char *)&m.md.status, STATUS_FLAG_RB) == 0) {
        set_flag((u_char *)&m.md.status, STATUS_FLAG_RB);
    } else {
        set_flag((u_char *)&m.md.status, STATUS_FLAG_ERR);
        return EXIT_FAILURE;
    }

    u_int32_t pointer = (uint16_t)(addr >> 16) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
                                       + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
               + (uint8_t) (addr >> 8) * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size)
               + (uint8_t)(addr & (uint8_t) ~0L) * (m.md.num_of_sectors * m.md.md_s_size);

    u_int32_t page_pointer = (uint16_t)(addr >> 16) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
                                             + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                             + (uint8_t)(addr >> 8) * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    if (check_flags(&m.data[page_pointer + m.md.page_size + m.md.md_s_size * m.md.num_of_sectors + 1], PAGE_SPARE_BAD)) {
        set_flag((u_char *) &m.md.status, STATUS_FLAG_ERR);
        clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
        cout << "Stránka je poškozená.\n";
        return EXIT_FAILURE;
    }

    size_t page_stats_pointer = (uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8);

    if (check_ecc(&m.data[pointer + m.md.sector_size + 1], 2) > 0) {
        m.md.pages_stats[page_stats_pointer].increaseHistogramOnIndex(
                check_ecc(&m.data[pointer + m.md.sector_size + 1],m.md.ecc_size / 8));

        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_ECC);
        set_flag((u_char *) &m.data[pointer + m.md.sector_size], SECTOR_SPARE_ERROR);
    }
    /* TODO Spare data pro stranku? */

    /* Pro jistotu vymažeme předchozí obsah cache. */
    memcpy(m.mem_cache, &m.data[pointer], m.md.sector_size);

    /** Aktualizace času běhu. */
    m.md.pages_stats[page_stats_pointer]
            .addTotalReadPageTime(m.md.pages_stats[page_stats_pointer].getReadPageTime());

    m.md.pages_stats[page_stats_pointer]
            .setLastReadPageTime(m.md.pages_stats[page_stats_pointer].getReadPageTime());

    this->Increase_Time(page_stats_pointer, READ_PAGE);
    for (int i = 0; i < m.md.num_of_sectors; i++) {
        this->Simulate_Error(pointer + m.md.sector_size + 1, READ_PAGE);
    }

    m.md.pages_stats[page_stats_pointer].addNumOfReads(1);
    m.md.blocks_stats[(uint16_t)(addr >> 16)].addNumOfReads(1);
    m.md.mem_stats->addNumOfReads(1);

    clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
    clear_flag((u_char *) &m.md.status, STATUS_FLAG_EPE);

    return EXIT_SUCCESS;
}

int Flash_Memory::Read_Sector_From_Cache(u_int32_t addr)
{
    if (check_address(m, addr)) {
//        cout << "Adresu nelze pouzit.\n";
        return EXIT_FAILURE;
    }

    auto buffer = new u_char(m.md.sector_size);

    memcpy(buffer, m.mem_cache, m.md.sector_size);

    cout << "Obsah sektoru v cache: " << buffer << endl;
}

u_char* Flash_Memory::Read_Cache() const
{
    auto * buf = (u_char *) malloc(sizeof(u_char) * m.md.page_size);
    if (!buf) {
//        cout << "Není dostatek paměti.\n";
        return nullptr;
    }

    memcpy(buf, m.mem_cache, m.md.page_size);
    return buf;
}

u_char Flash_Memory::Read_Status() const
{
    return m.md.status;
}

size_t Flash_Memory::Read_ID()
{
    return m.md.id;
}

int Flash_Memory::Program_Page(u_int32_t addr) {
    if (check_address(m, addr)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE);
        return EXIT_FAILURE;
    }

    if (check_flags((u_char *)&m.md.status, STATUS_FLAG_RB) == 0) {
        set_flag((u_char *)&m.md.status, STATUS_FLAG_RB);
    } else {
        set_flag((u_char *)&m.md.status, STATUS_FLAG_ERR);
        return EXIT_FAILURE;
    }

    u_int32_t pointer = (uint16_t)(addr >> 16) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
            + (uint8_t)(addr >> 8) * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    if (check_flags(&m.data[pointer + m.md.page_size + m.md.md_s_size * m.md.num_of_sectors + 1], PAGE_SPARE_BAD)) {
        set_flag((u_char *) &m.md.status, STATUS_FLAG_EPE);
        set_flag((u_char *) &m.md.status, STATUS_FLAG_ERR);
        clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
        cout << "Stránka je poškozená.\n";
        return EXIT_FAILURE;
    }

    for (int i = 0; i < m.md.page_size / m.md.sector_size; i++) {
        memcpy(&m.data[pointer + i * m.md.sector_size + i * m.md.md_s_size],
               m.mem_cache + i * m.md.sector_size, m.md.sector_size);
        set_flag(&m.data[pointer + i * m.md.sector_size + i * m.md.md_s_size + m.md.sector_size], SECTOR_SPARE_WRITTEN);
    }

    size_t page_stats_pointer = (uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8);

    /** Aktualizace času běhu. */
    m.md.pages_stats[page_stats_pointer]
        .addTotalPageProgTime(m.md.pages_stats[page_stats_pointer].getPageProgTime());

    m.md.pages_stats[page_stats_pointer]
        .setLastPageProgTime(m.md.pages_stats[page_stats_pointer].getPageProgTime());

    this->Increase_Time(page_stats_pointer, PAGE_PROG);
    for (int i = 0; i < m.md.num_of_sectors; i++) {
        this->Simulate_Error(pointer + i * m.md.sector_size + m.md.md_s_size * i + m.md.sector_size + 1, PAGE_PROG);
    }

    m.md.pages_stats[page_stats_pointer].addNumOfWrites(1);
    m.md.blocks_stats[(uint16_t)(addr >> 16)].addNumOfWrites(1);
    m.md.mem_stats->addNumOfWrites(1);

    clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
    clear_flag((u_char *) &m.md.status, STATUS_FLAG_EPE);
    return EXIT_SUCCESS;
}

int Flash_Memory::Program_Sector(u_int32_t addr)
{
    if (check_address(m, addr)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE);
        return EXIT_FAILURE;
    }

    if (check_flags((u_char *)&m.md.status, STATUS_FLAG_RB) == 0) {
        set_flag((u_char *)&m.md.status, STATUS_FLAG_RB);
    } else {
        set_flag((u_char *)&m.md.status, STATUS_FLAG_ERR);
        return EXIT_FAILURE;
    }

    u_int32_t pointer = (uint16_t)(addr >> 16) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
                                                  + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                + (uint8_t) (addr >> 8) * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size)
                + (uint8_t)(addr & (uint8_t) ~0L) * (m.md.num_of_sectors * m.md.md_s_size);

    u_int32_t page_pointer = (uint16_t)(addr >> 16) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
                                             + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                             + (uint8_t)(addr >> 8) * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    if (check_flags(&m.data[page_pointer + m.md.page_size + m.md.md_s_size * m.md.num_of_sectors + 1], PAGE_SPARE_BAD)) {
        set_flag((u_char *) &m.md.status, STATUS_FLAG_ERR);
        clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
        cout << "Stránka je poškozená.\n";
        return EXIT_FAILURE;
    }

    /* Pro jistotu vymažeme předchozí obsah cache. */
    memcpy(&m.data[pointer], m.mem_cache, m.md.sector_size);
    set_flag(&m.data[pointer + m.md.sector_size], SECTOR_SPARE_WRITTEN);

    size_t page_stats_pointer = (uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8);

    /** Aktualizace času běhu. */
    m.md.pages_stats[page_stats_pointer]
            .addTotalPageProgTime(m.md.pages_stats[page_stats_pointer].getPageProgTime());

    m.md.pages_stats[page_stats_pointer]
            .setLastPageProgTime(m.md.pages_stats[page_stats_pointer].getPageProgTime());

    this->Increase_Time(page_stats_pointer, PAGE_PROG);
    for (int i = 0; i < m.md.num_of_sectors; i++) {
        this->Simulate_Error(pointer + m.md.sector_size + 1, PAGE_PROG);
    }

    m.md.pages_stats[page_stats_pointer].addNumOfWrites(1);
    m.md.blocks_stats[(uint16_t)(addr >> 16)].addNumOfWrites(1);
    m.md.mem_stats->addNumOfWrites(1);

    clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
    clear_flag((u_char *) &m.md.status, STATUS_FLAG_EPE);
    return EXIT_SUCCESS;
}

int Flash_Memory::Write_Cache(const string& data) const
{
    if (data.length() <= 0 || data.length() > m.md.page_size) {
//        cout << "Data jsou moc kratka nebo moc dlouha.\n";
        return EXIT_FAILURE;
    }

    memset(m.mem_cache, 0L, m.md.page_size);
    memcpy(m.mem_cache, (u_char *) data.c_str(), m.md.page_size);

    return EXIT_SUCCESS;
}

int Flash_Memory::Program_Data_Move(u_int32_t old_addr, u_int32_t new_addr)
{
    if (check_address(m, new_addr) || check_address(m, old_addr)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE);
        return EXIT_FAILURE;
    }

    if (check_flags((u_char *)&m.md.status, STATUS_FLAG_RB) == 0) {
        set_flag((u_char *)&m.md.status, STATUS_FLAG_RB);
    } else {
        set_flag((u_char *)&m.md.status, STATUS_FLAG_ERR);
        return EXIT_FAILURE;
    }

    u_int32_t old_pointer = (uint8_t)(old_addr >> 16) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                        + (uint8_t)(old_addr >> 8)
                        * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    if (check_flags(&m.data[old_pointer + m.md.page_size + m.md.md_s_size * m.md.num_of_sectors + 1], PAGE_SPARE_BAD)) {
        set_flag((u_char *) &m.md.status, STATUS_FLAG_ERR);
        clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
        cout << "Stránka je poškozená.\n";
        return EXIT_FAILURE;
    }

    size_t old_page_stats_pointer = (uint8_t)(old_addr >> 16)
            * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size) + (uint8_t)(old_addr >> 8);

    /** Aktualizace času běhu. */
    m.md.pages_stats[old_page_stats_pointer]
        .addTotalReadPageTime(m.md.pages_stats[old_page_stats_pointer].getPageProgTime());
    m.md.pages_stats[old_page_stats_pointer]
            .setLastReadPageTime(m.md.pages_stats[old_page_stats_pointer].getPageProgTime());

    this->Increase_Time(old_page_stats_pointer, READ_PAGE);
    for (int i = 0; i < m.md.num_of_sectors; i++) {
        this->Simulate_Error(old_pointer + i * m.md.sector_size + m.md.md_s_size * i + m.md.sector_size + 1, READ_PAGE);
    }

    m.md.pages_stats[old_page_stats_pointer].addNumOfReads(1);
    m.md.blocks_stats[(uint8_t)(old_addr >> 16)].addNumOfReads(1);
    m.md.mem_stats->addNumOfReads(1);

    u_int32_t new_pointer = (uint8_t)(new_addr >> 16) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                    + (uint8_t)(new_addr >> 8)
                    * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size);

    if (check_flags(&m.data[new_pointer + m.md.page_size + m.md.md_s_size * m.md.num_of_sectors + 1], PAGE_SPARE_BAD)) {
        set_flag((u_char *) &m.md.status,  STATUS_FLAG_EPE);
        set_flag((u_char *) &m.md.status,  STATUS_FLAG_ERR);
        clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
        cout << "Stránka je poškozená.\n";
        return EXIT_FAILURE;
    }

    for (int i = 0; i < m.md.page_size / m.md.sector_size; i++) {
        memcpy(&m.data[new_pointer + i * m.md.sector_size + i * m.md.md_s_size],
               &m.data[old_pointer + i * m.md.sector_size + i * m.md.md_s_size], m.md.sector_size);
        set_flag(&m.data[new_pointer + i * m.md.sector_size + i * m.md.md_s_size + m.md.sector_size], SECTOR_SPARE_WRITTEN);
    }

    size_t new_page_stats_pointer = (uint8_t)(new_addr >> 16)
            * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size) + (uint8_t)(new_addr >> 8);

    /** Aktualizace času běhu. */
    m.md.pages_stats[new_page_stats_pointer]
        .addTotalPageProgTime(m.md.pages_stats[new_page_stats_pointer].getPageProgTime());

    m.md.pages_stats[new_page_stats_pointer]
            .setLastPageProgTime(m.md.pages_stats[new_page_stats_pointer].getPageProgTime());

    this->Increase_Time(new_page_stats_pointer, PAGE_PROG);
    for (int i = 0; i < m.md.num_of_sectors; i++) {
        this->Simulate_Error(new_pointer + i * m.md.sector_size + m.md.md_s_size * i + m.md.sector_size + 1, PAGE_PROG);
    }

    m.md.pages_stats[new_page_stats_pointer].addNumOfWrites(1);
    m.md.blocks_stats[(uint8_t)(new_addr >> 16)].addNumOfWrites(1);
    m.md.mem_stats->addNumOfWrites(1);

    clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
    clear_flag((u_char *) &m.md.status, STATUS_FLAG_EPE);
    return EXIT_SUCCESS;
}

int Flash_Memory::Block_Erase(u_int32_t addr)
{
    if (check_address(m, addr)) {
        set_flag((u_char *) &m.md.status, m.md.status_bits, STATUS_FLAG_EPE);
        return EXIT_FAILURE;
    }

    if (check_flags((u_char *)&m.md.status, STATUS_FLAG_RB) == 0) {
        set_flag((u_char *)&m.md.status, STATUS_FLAG_RB);
    } else {
        set_flag((u_char *)&m.md.status, STATUS_FLAG_ERR);
        return EXIT_FAILURE;
    }

    u_int32_t pointer = (uint16_t)(addr >> 16) * (m.md.block_size + m.md.md_b_size
            + m.md.num_of_pages * m.md.md_p_size + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size);

    if (m.md.blocks_stats[(uint16_t)(addr >> 16)].getNumOfErases() == MAX_ERASE_NUMBER) {
        set_flag(&m.data[pointer + m.md.block_size + m.md.page_size * m.md.md_p_size
                         + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size], BLOCK_SPARE_WORE_OUT);
        set_flag((u_char *) &m.md.status, STATUS_FLAG_EPE);
        set_flag((u_char *) &m.md.status, BLOCK_SPARE_ERROR);
        clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
//        cout << "Blok je poškozený.\n";
        return EXIT_FAILURE;
    }

    if (check_flags(&m.data[pointer + m.md.block_size + m.md.page_size * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size], BLOCK_SPARE_BAD) ||
        check_flags(&m.data[pointer + m.md.block_size + m.md.page_size * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size], BLOCK_SPARE_ERASED) ||
        check_flags(&m.data[pointer + m.md.block_size + m.md.page_size * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size], BLOCK_SPARE_ERASE_SUSPEND) ||
        check_flags(&m.data[pointer + m.md.block_size + m.md.page_size * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size], BLOCK_SPARE_LOCKED) ||
        check_flags(&m.data[pointer + m.md.block_size + m.md.page_size * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size], BLOCK_SPARE_WORE_OUT) ||
        check_flags(&m.data[pointer + m.md.block_size + m.md.page_size * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size], BLOCK_SPARE_RESERVED)) {
        set_flag((u_char *) &m.md.status, STATUS_FLAG_EPE);
        set_flag((u_char *) &m.md.status, BLOCK_SPARE_ERROR);
        clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
//        cout << "Blok je poškozený.\n";
        return EXIT_FAILURE;
    }

    /** Smazání bloku. */
    memset(&m.data[pointer],0L,m.md.page_size);
//    increase_counter(
//            &m.data[pointer + m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size + 2],
//            m.md.block_wear_size);

    for (int i = 0; i < m.md.num_of_pages; i++) {
        m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].resetHistogram();
    }

    /** Aktualizace času běhu. */
    m.md.blocks_stats[(uint16_t)(addr >> 16)].addTotalEraseTime(m.md.blocks_stats[(uint16_t)(addr >> 16)].getEraseTime());
    m.md.blocks_stats[(uint16_t)(addr >> 16)].setLastEraseTime(m.md.blocks_stats[(uint16_t)(addr >> 16)].getEraseTime());
    m.md.blocks_stats[(uint16_t)(addr >> 16)].addNumOfErases(1);

    this->Increase_Time((uint16_t)(addr >> 16), ERASE);

    clear_flag((u_char *) &m.md.status, STATUS_FLAG_RB);
    clear_flag((u_char *) &m.md.status, STATUS_FLAG_EPE);
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

size_t Flash_Memory::Num_Of_Writes(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].getNumOfWrites();
}

size_t Flash_Memory::Num_Of_Reads(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].getNumOfReads();
}

u_char * Flash_Memory::ECC_Info(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return nullptr;
    }

    u_int32_t pointer = (uint16_t)(addr >> 16) * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size
            + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                    + (uint8_t)(addr >> 8) * (m.md.page_size + m.md.md_p_size + m.md.num_of_sectors * m.md.md_s_size)
                    + m.md.sector_size + 1;

    auto *ecc_data = (u_char *) malloc(sizeof(u_char) * m.md.ecc_size / 8);
    if (!ecc_data) {
//        cout << "Neni dostatek pameti!\n";
        return nullptr;
    }

    memcpy(ecc_data, &m.data[pointer], m.md.ecc_size / 8);

    return ecc_data;
}

float Flash_Memory::Read_Time_Last(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].getLastReadPageTime();
}

float Flash_Memory::Program_Time_Last(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].getLastPageProgTime();
}

float Flash_Memory::Read_Time_Total(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].getTotalReadPageTime();
}

float Flash_Memory::Program_Time_Total(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].getTotalPageProgTime();
}

float Flash_Memory::Com_Total_Time(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].getComTime();
}

size_t Flash_Memory::Num_Of_Erases_Page(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(uint16_t)(addr >> 16)].getNumOfErases();
}

size_t * Flash_Memory::Sector_Status_Block(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return nullptr;
    }

    size_t *sector_status = new size_t(3);

    sector_status[0] = m.md.blocks_stats[(uint16_t)(addr >> 16)].getNumOfErases();
    sector_status[1] = m.md.blocks_stats[(uint16_t)(addr >> 16)].getNumOfWrites();
    sector_status[2] = m.md.blocks_stats[(uint16_t)(addr >> 16)].getNumOfErrors();

    return sector_status;
}

size_t Flash_Memory::Num_Of_Erases_Block(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(uint16_t)(addr >> 16)].getNumOfErases();
}

float Flash_Memory::Erase_Time_Total(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(uint16_t)(addr >> 16)].getTotalEraseTime();
}

float Flash_Memory::Erase_Time_Last(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(uint16_t)(addr >> 16)].getLastEraseTime();
}
// TODO maybe?
bool Flash_Memory::Is_Bad_Block(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return false;
    }

    u_int32_t pointer = (uint16_t)(addr >> 16) * (m.md.block_size + m.md.md_b_size
            + m.md.num_of_pages * m.md.md_p_size + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size)
                    + m.md.block_size + m.md.num_of_pages * m.md.md_p_size
                    + m.md.num_of_pages * m.md.num_of_sectors * m.md.md_s_size;

    return check_flags(&m.data[pointer],1, BLOCK_SPARE_BAD);
}

size_t Flash_Memory::Num_Of_Bad_Pages(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(uint16_t)(addr >> 16)].getNumOfBadPages();
}

size_t * Flash_Memory::ECC_Histogram(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return nullptr;
    }

    size_t *histogram = new size_t(MEMORY_ECC_SIZE);
    for (size_t i = 0; i < MEMORY_ECC_SIZE; ++i) {
        histogram[i] = m.md.blocks_stats[(uint16_t)(addr >> 16)].getHistogram()[i];
    }

    return histogram;
}

size_t Flash_Memory::Num_Of_Writes_Page(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(uint16_t)(addr >> 16)].getNumOfWrites();
}

size_t Flash_Memory::Num_Of_Reads_Page(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return -1;
    }

    return m.md.blocks_stats[(uint16_t)(addr >> 16)].getNumOfReads();
}

size_t * Flash_Memory::Sector_Status_Page(u_int32_t addr) const
{
    if (check_address(m, addr)) {
        return nullptr;
    }

    size_t *sector_status = new size_t(2);

    sector_status[0] = m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].getNumOfWrites();
    sector_status[1] = m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].getNumOfErrors();

    return sector_status;
}

size_t Flash_Memory::Num_Of_Bad_Blocks() const
{
    return m.md.num_of_bad_blocks;
}

size_t Flash_Memory::Num_Of_Bad_Pages() const
{
    return m.md.num_of_bad_pages;
}
// TODO
size_t * Flash_Memory::ECC_Histogram()
{
    size_t *histogram = new size_t[MEMORY_ECC_SIZE];
    for (size_t i = 0; i < MEMORY_ECC_SIZE; ++i) {
        histogram[i] = m.md.mem_stats->getHistogram()[i];
    }

    return histogram;
}

size_t * Flash_Memory::Sector_Status_Memory() const
{
    size_t *sector_status = new size_t(2);

    sector_status[0] = m.md.mem_stats->getNumOfWrites();
    sector_status[1] = m.md.mem_stats->getNumOfErrors();

    return sector_status;
}

size_t Flash_Memory::Num_Of_Writes() const
{
    return m.md.mem_stats->getNumOfWrites();
}

size_t Flash_Memory::Num_Of_Reads() const
{
    return m.md.mem_stats->getNumOfReads();
}

int Flash_Memory::Set_Prog_Time_Page(u_int32_t addr, float time) const
{
    if (check_address(m, addr)) {
        return EXIT_FAILURE;
    }

    m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].setLastPageProgTime(time);
    m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].setTotalPageProgTime(time);

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Prog_Time_Block(u_int32_t addr, float time) const
{
    if (check_address(m, addr)) {
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < m.md.num_of_pages; ++i) {
        m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + i].setLastPageProgTime(time);
        m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + i].setPageProgTime(time);
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Prog_Time_Mem(float time) const
{
    for (size_t i = 0; i < m.md.num_of_blocks * m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].setLastPageProgTime(time);
        m.md.pages_stats[i].setPageProgTime(time);
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Read_Time_Page(u_int32_t addr, float time) const
{
    if (check_address(m, addr)) {
        return EXIT_FAILURE;
    }

    m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].setLastReadPageTime(time);
    m.md.pages_stats[(uint16_t)(addr >> 16) * m.md.num_of_pages + (uint8_t)(addr >> 8)].setReadPageTime(time);

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Read_Time_Block(u_int32_t addr, float time) const
{
    if (check_address(m, addr)) {
        return EXIT_FAILURE;
    }

    for (size_t i = (uint16_t)(addr >> 16); i < m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].setLastReadPageTime(time);
        m.md.pages_stats[i].setReadPageTime(time);
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Read_Time_Mem(float time) const
{
    for (size_t i = 0; i < m.md.num_of_blocks * m.md.num_of_pages; ++i) {
        m.md.pages_stats[i].setLastReadPageTime(time);
        m.md.pages_stats[i].setReadPageTime(time);
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Erase_Time_Block(u_int32_t addr, float time) const
{
    if (check_address(m, addr)) {
        return EXIT_FAILURE;
    }

    m.md.blocks_stats[(uint16_t)(addr >> 16)].setLastEraseTime(time);
    m.md.blocks_stats[(uint16_t)(addr >> 16)].setEraseTime(time);

    return EXIT_SUCCESS;
}

int Flash_Memory::Set_Erase_Time_Mem(float time) const
{
    for (size_t i = 0; i < m.md.num_of_blocks; ++i) {
        m.md.blocks_stats[i].setLastEraseTime(time);
        m.md.blocks_stats[i].setEraseTime(time);
    }

    return EXIT_SUCCESS;
}

int Flash_Memory::Save_Memory(const string& file_name) const
{
    ofstream f(file_name, ios::out | ios::binary);
    if(!f) {
//        cout << "Nepodarilo se otevrit soubor " << file_name << endl;
        return EXIT_FAILURE;
    }

    f.write((char *) m.data, m.md.true_mem_size);

    f.close();
//    cout << "Soubor byl uzavren.\n";
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
    j["id"] = m.md.id;
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
    j["mem_type"] = {
            {"error_const", m.md.mem_type.error_const},
            {"read_time_const", m.md.mem_type.read_time_const},
            {"prog_time_const", m.md.mem_type.prog_time_const},
            {"erase_time_const", m.md.mem_type.erase_time_const}
    };

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
//    cout << "Soubor byl uzavren.\n";
    return EXIT_SUCCESS;
}

int Flash_Memory::Load_Memory(const string& file_name) const
{
    ifstream f(file_name, ios::in | ios::binary);
    if (!f) {
//        cout << "Nepodarilo se otevrit soubor " << file_name << endl;
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
    return EXIT_SUCCESS;
}

Flash_Memory * Flash_Memory::Load_State(const string& file_name) const
{
    ifstream f(file_name);
    if (!f) {
        //cout << "Nepodarilo se otevrit soubor " << file_name << endl;
        return nullptr;
    }
    json j = json::parse(f);

    size_t page_size = j["page_size"];
    size_t block_size = j["block_size"];
    size_t num_of_blocks = j["num_of_blocks"];
    json mem_type = j["mem_type"];
    mem_type_values mem_t = {
            mem_type["error_const"],
            mem_type["read_time_const"],
            mem_type["prog_time_const"],
            mem_type["erase_time_const"]
    };
    json mem_stats = j["mem_stats"];
    json block_stats = j["block_stats"];
    json page_stats = j["page_stats"];

    auto *flashMemory = new Flash_Memory(page_size,block_size,num_of_blocks,mem_t);

    flashMemory->Set_Id(j["id"]);

    flashMemory->m.md.mem_stats->setNumOfWrites(mem_stats["num_of_writes"]);
    flashMemory->m.md.mem_stats->setNumOfReads(mem_stats["num_of_reads"]);

    // num_of_bad_blocks
    // num_of_bad_pages

    for(auto i = 0; i < flashMemory->m.md.num_of_blocks; i++) {
        flashMemory->m.md.blocks_stats[i].setEraseTime(block_stats.at(i)["erase_time"]);
        flashMemory->m.md.blocks_stats[i].setLastEraseTime(block_stats.at(i)["last_erase_time"]);
        flashMemory->m.md.blocks_stats[i].setTotalEraseTime(block_stats.at(i)["total_erase_time"]);
        flashMemory->m.md.blocks_stats[i].setNumOfReads(block_stats.at(i)["num_of_reads"]);
        flashMemory->m.md.blocks_stats[i].setNumOfWrites(block_stats.at(i)["num_of_writes"]);
        flashMemory->m.md.blocks_stats[i].setNumOfErases(block_stats.at(i)["num_of_erases"]);
    }

    for(auto i = 0; i < flashMemory->m.md.num_of_blocks * flashMemory->m.md.num_of_pages; i++) {
        flashMemory->m.md.pages_stats[i].setReadPageTime(page_stats.at(i)["read_page_time"]);
        flashMemory->m.md.pages_stats[i].setPageProgTime(page_stats.at(i)["page_prog_time"]);
        flashMemory->m.md.pages_stats[i].setLastReadPageTime(page_stats.at(i)["last_read_page_time"]);
        flashMemory->m.md.pages_stats[i].setLastPageProgTime(page_stats.at(i)["last_page_prog_time"]);
        flashMemory->m.md.pages_stats[i].setTotalReadPageTime(page_stats.at(i)["total_read_page_time"]);
        flashMemory->m.md.pages_stats[i].setTotalPageProgTime(page_stats.at(i)["total_page_prog_time"]);
        flashMemory->m.md.pages_stats[i].setComTime(page_stats.at(i)["com_time"]);
        flashMemory->m.md.pages_stats[i].setNumOfReads(page_stats.at(i)["num_of_reads"]);
        flashMemory->m.md.pages_stats[i].setNumOfWrites(page_stats.at(i)["num_of_writes"]);
    }

    flashMemory->Flash_Init();
    flashMemory->Cache_Init();

    return flashMemory;
}

void Flash_Memory::Set_Id(size_t id) {
    m.md.id = id;
}

size_t Flash_Memory::Get_True_Mem_Size() const {
    return m.md.true_mem_size;
}

void Flash_Memory::Simulate_Error(u_int32_t addr, NInstruction_Type type)
{
    if (type == NInstruction_Type::READ_PAGE) {
        for (int i = 0; i < m.md.ecc_size; ++i) {
            int chance = rand() % READ_PAGE_ERROR_RATE;
            if (chance == 1) {
                set_flag(&m.data[addr + i / 8], i % 8);
            }
        }
    } else if (type == NInstruction_Type::PAGE_PROG) {
        for (int i = 0; i < m.md.ecc_size; ++i) {
            int chance = rand() % PAGE_PROG_ERROR_RATE;
            if (chance == 1) {
                set_flag(&m.data[addr + i / 8], i % 8);
            }
        }
    }
}

void Flash_Memory::Increase_Time(u_int32_t addr, NInstruction_Type type)
{
    float new_time;
    if (type == NInstruction_Type::READ_PAGE) {
        new_time = m.md.mem_type.read_time_const * m.md.pages_stats[addr].getReadPageTime();
        m.md.pages_stats[addr].setReadPageTime(new_time);
    } else if (type == NInstruction_Type::PAGE_PROG) {
        new_time = m.md.pages_stats[addr].getPageProgTime() - m.md.mem_type.prog_time_const * m.md.pages_stats[addr].getNumOfWrites() / 1000;
        m.md.pages_stats[addr].setPageProgTime(new_time);
    } else if (type == NInstruction_Type::ERASE) {
        new_time = m.md.blocks_stats[addr].getEraseTime() + m.md.mem_type.erase_time_const * m.md.blocks_stats[addr].getNumOfErases();
        m.md.blocks_stats[addr].setEraseTime(new_time);
    }
}

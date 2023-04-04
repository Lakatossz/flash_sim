#include "flash/flash.h"

/**
 * Funkce pro inkrementaci pole predstavujici pocitadlo.
 * @param counter Pole u_char - pocitadlo.
 * @param size Pocet bytu pole.
 */
void increase_counter(u_char *counter, int size) {
    for (int i = size - 1; i >= 0; -i) {
        if (counter[i] < (u_char) 1L) {
            counter[i]++;
            return;
        }
    }
}

int_32 counter_value(u_char *counter, int size) {

    int_32 value = 0, i = 0;
    while (counter[i] == (u_char) 1L) {
        value += (u_char) 1L;
        ++i;
    }

    return value;
}

/**
 *
 * @param time_type
 */
void increase_time(Time_Type type) {
    if (type == READ_PAGE_TIME) {

    } else if (type == PAGE_PROG_TIME) {

    } else if (type == ERASE_TIME) {

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
    m.md.page_size = DEFAULT_PAGE_SIZE;
    m.md.block_size = DEFAULT_BLOCK_SIZE;
}
Flash_Memory::Flash_Memory(int_32 page_size, int_32 block_size, int_32 number_of_blocks, Mem_Type memory_type,
                           int_32 read_page_time, int_32 page_prog_time, int_32 erase_time)
{
    m.md.page_size = page_size;
    m.md.block_size = block_size;
    m.md.num_of_blocks = number_of_blocks;
    m.md.memory_type = memory_type;
    m.md.read_page_time = read_page_time;
    m.md.page_prog_time = page_prog_time;
    m.md.erase_time = erase_time;
    m.md.block_wear_size = 4;
    m.md.md_p_size = 8;
    m.md.md_b_size = 8;
}

Flash_Memory::~Flash_Memory() = default;

bool Flash_Memory::Init()
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
        return false;
    }
    memset(m.data, 0L, m.md.true_mem_size);

//    uuid_generate_random(m.md.id);

    cout << "Page size: " << m.md.page_size << endl;
    cout << "Block size: " << m.md.block_size << endl;
    cout << "Number of pages: " << m.md.num_of_pages << endl;
    cout << "Number of blocks: " << m.md.num_of_blocks << endl;
    cout << "Memory size: " << m.md.mem_size << endl;
    cout << "True memory size: " << m.md.true_mem_size << endl;

    return true;
}

u_char* Flash_Memory::Read_Page(int_16 addr)
{
    if (!check_address(m, addr))
        return nullptr;

    int_32 pointer = addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

//    m.md.status = m.md.status | (1 << 5)

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[pointer + m.md.page_size] >> PAGE_VALID == 1) {
        return nullptr;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[pointer + 1] >> BLOCK_BAD == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return nullptr;
    }

    auto * buf = (u_char *) malloc(sizeof(u_char) * m.md.page_size);
    if (!buf) {
        cout << "Není dostatek paměti.\n";
        return nullptr;
    }

    memcpy(buf, &m.data[pointer], m.md.page_size);

    /** Aktualizace času běhu. */
    m.md.mem_time += m.md.read_page_time;
    increase_time(READ_PAGE_TIME);

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

void Flash_Memory::Program_Page(int_16 addr, const string& data)
{
    if (!check_address(m, addr))
        return;

    if (data.length() <= 0 && data.length() > m.md.page_size) {
        cout << "Data nemohou být uložena.\n";
        return;
    }

    int_32 pointer = addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[pointer + m.md.page_size] >> PAGE_VALID == 1) {
        return;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[pointer + 1] >> BLOCK_BAD == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return;
    }

    memcpy(&m.data[pointer], (u_char *) data.c_str(), m.md.page_size);

    /** Aktualizace času běhu. */
    m.md.mem_time += m.md.page_prog_time;
    increase_time(PAGE_PROG_TIME);
}

void Flash_Memory::Program_Data_Move(int_16 old_addr, int_16 new_addr)
{
    if (!check_address(m, old_addr) || !check_address(m, new_addr))
        return;

    int_32 old_pointer = old_addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[old_pointer + m.md.page_size] >> PAGE_VALID == 1) {
        return;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[old_pointer + 1] >> BLOCK_BAD == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return;
    }

    /** Aktualizace času běhu. */
    m.md.mem_time += m.md.page_prog_time;
    increase_time(PAGE_PROG_TIME);

    int_32 new_pointer = old_addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    /** Kontrola validace stránky - jestli je použitelná. */
    if (m.data[new_pointer + m.md.page_size] >> PAGE_VALID == 1) {
        return;
    }

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[new_pointer + 1] >> BLOCK_BAD == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return;
    }

    memcpy(&m.data[old_pointer], &m.data[new_pointer], m.md.page_size);

    /** Aktualizace času běhu. */
    m.md.mem_time += m.md.read_page_time;
    increase_time(READ_PAGE_TIME);
}

//for (int i = 0; i < numBytes; i++) {
//intValue |= ((int)charArray[i] << ((numBytes - 1 - i) * 8));
//}

void Flash_Memory::Block_Erase(int_8 addr)
{
    if (!check_address(m, addr)) {
        return;
    }

    int_32 pointer = addr * (m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size);

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[pointer + m.md.block_size + m.md.md_b_size + m.md.num_of_pages * m.md.md_p_size] >> BLOCK_BAD == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return;
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
    m.md.mem_time += m.md.erase_time;
    increase_time(ERASE_TIME);
}

void Flash_Memory::Reset()
{
    memset(m.data, 0L, m.md.true_mem_size);
    m.md.status = 0;
}

u_char* Flash_Memory::Random_Data_Read()
{
    return nullptr;
}

void Flash_Memory::Random_Data_Input()
{

}

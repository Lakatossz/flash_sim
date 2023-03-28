#include "flash/flash.h"

Flash_Memory::Flash_Memory()
{
    m.md.page_size = DEFAULT_PAGE_SIZE;
    m.md.block_size = DEFAULT_BLOCK_SIZE;
}
Flash_Memory::Flash_Memory(int_8 page_size, int_8 block_size, mem_type memory_type)
{
    m.md.page_size = page_size;
    m.md.block_size = block_size;
    m.md.memory_type = memory_type;
    m.md.md_p_size = 2;
    m.md.md_b_size = 2;
}

Flash_Memory::~Flash_Memory()
{

}

bool Flash_Memory::Init()
{
    /** Vypočítám počet bloků podle zadaných vstupních parametrů. */
    m.md.num_of_blocks = (m.md.num_of_pages * m.md.page_size) / m.md.block_size;

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

    return true;
}

u_char* Flash_Memory::Read_Page(int_16 addr)
{
    /** Zkontroluju adresu bloku. */
    if ((addr >> 8) >= m.md.num_of_blocks) {
        cout << "Adresa bloku je příliš velká.\n";
        m.md.status = m.md.status | (1 << 5);
        return nullptr;
    /** Zkontroluju adresu stránky. */
    } else if ((addr & (uint8_t) ~0L) >= m.md.num_of_pages) {
        cout << "Adresa stránky je příliš velká.\n";
        m.md.status = m.md.status | (1 << 5);
        return nullptr;
    }

    int_32 pointer = addr * m.md.num_of_blocks * (m.md.block_size + m.md.md_b_size)
                            + m.md.page_size * (m.md.page_size + m.md.md_p_size);

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[pointer + 1] >> 5 == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return nullptr;
    }

    auto * buf = (u_char *) malloc(sizeof(u_char) * m.md.page_size);
    if (!buf) {
        cout << "Není dostatek paměti.\n";
        return nullptr;
    }

    memcpy(&m.data[pointer], buf, m.md.page_size);

    return buf;
}

u_char Flash_Memory::Read_Status()
{
    return m.md.status;
}

uuid_t* Flash_Memory::Read_ID()
{
    return &m.md.id;
}

void Flash_Memory::Program_Page(int_16 addr, u_char *data)
{
    /** Zkontroluju adresu bloku. */
    if ((addr >> 8) >= m.md.num_of_blocks) {
        cout << "Adresa bloku je příliš velká.\n";
        m.md.status = m.md.status | (1 << 5);
        return;
        /** Zkontroluju adresu stránky. */
    } else if ((addr & (uint8_t) ~0L) >= m.md.num_of_pages) {
        cout << "Adresa stránky je příliš velká.\n";
        m.md.status = m.md.status | (1 << 5);
        return;
    }

    if (!data) {
        cout << "Data nemohou být uložena.\n";
        return;
    }

    int_32 pointer = addr * m.md.num_of_blocks * (m.md.block_size + m.md.md_b_size)
                     + m.md.page_size * (m.md.page_size + m.md.md_p_size);

    /** Zkontroluju, jestli je možné stránku ještě smazat. */
    /** cislo bloku + cislo stranky + 5. pozice znaku */
    if (m.data[pointer + 1] >> 5 == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return;
    }

    memcpy(data, &m.data[pointer], m.md.page_size);
}

void Flash_Memory::Program_Data_Move(int_32 old_row_addr, int_32 new_row_addr)
{

}

void Flash_Memory::Block_Erase(int_8 block_address)
{
    /** Zkontroluju adresu bloku. */
    if (block_address >= m.md.num_of_blocks) {
        cout << "Adresa bloku je příliš velká.\n";
        m.md.status = m.md.status | (1 << 5);
        return;
        /** Zkontroluju, jestli je možné stránku ještě smazat. */
        /** cislo bloku + cislo stranky + 5. pozice znaku */
    } else if (m.data[block_address * m.md.num_of_blocks
                + m.md.page_size * (m.md.page_size + m.md.md_p_size) + 1] >> 5 == 1) {
        cout << "Blok je již požkozený.\n";
        m.md.status = m.md.status | (1 << 5);
        return;
    }

    /** Smažu stránku. */
    memset(&m.data[block_address * m.md.num_of_blocks + (block_address & (uint8_t) ~0L) + m.md.md_b_size],
           0L, m.md.page_size);

    if (m.data[(block_address >> 8) * m.md.num_of_blocks + (block_address & (uint8_t) ~0L) + 3] == 255) {
        m.data[(block_address >> 8) * m.md.num_of_blocks + (block_address & (uint8_t) ~0L) + 2]++;
    } else {
        m.data[(block_address >> 8) * m.md.num_of_blocks + (block_address & (uint8_t) ~0L) + 3]++;
    }
}

void Flash_Memory::Reset()
{
    memset(m.data, 0L, m.md.true_mem_size);
    m.md.status = 0;
}

u_char* Flash_Memory::Random_Data_Read()
{

}

void Flash_Memory::Random_Data_Input(u_char *data)
{

}

#include "flash/flash.h"

Flash_Memory::Flash_Memory()
{
    memory.metadata.page_size = DEFAULT_PAGE_SIZE;
    memory.metadata.block_size = DEFAULT_BLOCK_SIZE;
}
Flash_Memory::Flash_Memory(int_8 page_size, int_8 block_size, mem_type memory_type)
{
    memory.metadata.page_size = page_size;
    memory.metadata.block_size = block_size;
    memory.metadata.memory_type = memory_type;
}

Flash_Memory::~Flash_Memory()
{

}

bool Flash_Memory::Init()
{
    memory.metadata.num_of_blocks =
            (memory.metadata.num_of_pages * memory.metadata.page_size) / memory.metadata.block_size;

    memory.blocks = (block *) malloc(sizeof(block) * memory.metadata.block_size);
    if (memory.blocks == nullptr) {
        return false;
    }

    for (int i = 0; i < memory.metadata.num_of_blocks; ++i) {
        memory.blocks[i].pages = (page *) malloc(sizeof(page) * memory.metadata.page_size);
        if (memory.blocks[i].pages == nullptr) {
            return false;
        }

        for (int j = 0; j < memory.metadata.num_of_pages; ++j) {
            memory.blocks[i].pages[j].data = (cell *) malloc(sizeof(cell) * memory.metadata.memory_type);
            if (memory.blocks[i].pages[j].data == nullptr) {
                return false;
            }

            /** TODO (memory.page_size / memory.memory_type) nebo (memory.page_size) */
            for (int k = 0; k < memory.metadata.page_size / memory.metadata.memory_type; ++k) {
                memory.blocks[i].pages[j].data[k].value = (bool *) malloc(sizeof(bool) * memory.metadata.memory_type);
                if (memory.blocks[i].pages[j].data[k].value == nullptr) {
                    return false;
                }

                memset(memory.blocks[i].pages[j].data[k].value, true, memory.metadata.memory_type);
                memory.blocks[i].pages[j].data[k].id = k;
            }
            memory.blocks[i].pages[j].spare.metadata.id = j;
        }
        memory.blocks[i].metadata.id = i;
    }
    memory.metadata.mem_size = memory.metadata.num_of_blocks * memory.metadata.page_size * memory.metadata.num_of_pages;
    uuid_generate_random(memory.metadata.id);

    return true;
}

void Flash_Memory::Read_Page(int_32 row_addr)
{

}

string Flash_Memory::Read_Cache()
{
    return " ";
}

string Flash_Memory::Read_Status()
{
    return " ";
}

uuid_t* Flash_Memory::Read_ID()
{
    return &memory.metadata.id;
}

void Flash_Memory::Program_Page(int_32 col_addr)
{

}

void Flash_Memory::Program_Page_Cache(string data)
{

}

void Flash_Memory::Program_Data_Move(int_32 old_row_addr, int_32 new_row_addr)
{

}

/*void Flash_Memory::Change_Read_Column(int_32 col_addr)
{

}

void Flash_Memory::Change_Write_Column(int_32 row_addr)
{

}

void Flash_Memory::Change_Row_Address(int_32 row_addr, int_32 col_addr)
{

}*/

void Flash_Memory::Block_Erase(int_32 block_address)
{
    if (block_address >= memory.metadata.num_of_blocks) {
        cout << "Adresa bloku je příliš velká.\n";
        return;
    } else if (memory.blocks[block_address].metadata.bad) {
        cout << "Blok je již požkozený.\n";
        return;
    } else if (memory.blocks[block_address].metadata.erase_number == MAX_ERASE_NUMBER) {
        memory.blocks[block_address].metadata.bad = true;
        cout << "Blok již nejde vymazat.\n";
        return;
    }

    for (int i = 0; i < memory.metadata.num_of_pages; ++i) {
        for (int j = 0; j < memory.metadata.page_size; ++j) {
            memset(memory.blocks[block_address].pages[i].data[j].value, true, memory.metadata.memory_type);
        }
        memset(memory.blocks[block_address].pages[i].spare.ecc, 1, memory.metadata.ecc_size);
        memory.blocks[block_address].pages[i].spare.metadata.data_age = 0;
        memory.blocks[block_address].pages[i].spare.metadata.erase_number++;
        memory.blocks[block_address].pages[i].spare.metadata.wear_level++;
        memory.blocks[block_address].pages[i].spare.metadata.erased = true;
        memory.blocks[block_address].pages[i].spare.metadata.valid = false;
    }

    memory.blocks[block_address].metadata.erase_number++;
    memory.blocks[block_address].metadata.wear_level++;
    memory.blocks[block_address].metadata.erased = true;
    memory.blocks[block_address].metadata.valid = false;
}

void Flash_Memory::Reset()
{
    for (int i = 0; i < memory.metadata.num_of_blocks; ++i) {
        for (int j = 0; j < memory.metadata.num_of_pages; ++j) {
            /** TODO (memory.page_size / memory.memory_type) nebo (memory.page_size) */
            for (int k = 0; k < memory.metadata.page_size / memory.metadata.memory_type; ++k) {
                memset(memory.blocks->pages[j].data[k].value, true, memory.metadata.memory_type);
            }
            memset(memory.blocks->pages[j].spare.ecc, 0, memory.metadata.ecc_size);
            memory.blocks[i].pages[j].spare.metadata.valid = false;
            memory.blocks[i].pages[j].spare.metadata.erased = true;
            memory.blocks[i].pages[j].spare.metadata.data_age = 0;
        }
        memory.blocks[i].metadata.erased = true;
        memory.blocks[i].metadata.valid = false;
    }
}

void Flash_Memory::Random_Data_Read()
{

}

void Flash_Memory::Random_Data_Input()
{

}

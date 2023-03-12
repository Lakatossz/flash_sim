#include "flash/flash.h"

Flash_Memory::Flash_Memory()
{
    memory.page_size = DEFAULT_PAGE_SIZE;
    memory.block_size = DEFAULT_BLOCK_SIZE;
}
Flash_Memory::Flash_Memory(int_8 page_size, int_8 block_size)
{
    memory.page_size = page_size;
    memory.block_size = block_size;
}

Flash_Memory::~Flash_Memory(){

}

bool Flash_Memory::Init()
{
    memory.mem_id = DEFAULT_MEMORY_ID;
    memory.mem_size = memory.block_size * memory.page_size;
    memory.mem_blocks = (block_struct *) malloc(sizeof(block_struct) * memory.block_size);
    if (memory.mem_blocks == nullptr) {
        return false;
    }
    for (int i = 0; i < memory.block_size; ++i) {
        memory.mem_blocks->block_pages[i].page_data = (u_char *) malloc(sizeof(u_char) *memory.page_size);
        if (memory.mem_blocks->block_pages[i].page_data == nullptr) {
            return false;
        }
    }

    return true;
}

string Flash_Memory::Read_ID() {
    return std::string();
}

string Flash_Memory::Read_Parameter_Page_Definition() {
    return std::string();
}

string Flash_Memory::Read_Uniquie_ID() {
    return std::string();
}

void Flash_Memory::Block_Erase(int_32 block_address) {

}

string Flash_Memory::Read_Status() {
    return std::string();
}

string Flash_Memory::Read_Status_Enhanced() {
    return std::string();
}

string Flash_Memory::Status_Read() {
    return std::string();
}

string Flash_Memory::Read(int_32 row_addr) {
    return std::string();
}

void Flash_Memory::Read_Cache() {

}

void Flash_Memory::Page_Program(int_32 col_addr) {

}

void Flash_Memory::Page_Cache_Program() {

}

void Flash_Memory::CopyBack(int_32 old_row_addr, int_32 new_row_addr) {

}

void Flash_Memory::Small_Data_Move() {

}

void Flash_Memory::Change_Read_Column(int_32 col_addr) {

}

void Flash_Memory::Change_Read_Column_Enhanced(int_32 lun_addr, int_32 plane_addr, int_32 col_addr) {

}

void Flash_Memory::Change_Write_Column(int_32 row_addr) {

}

void Flash_Memory::Change_Row_Address(int_32 row_addr, int_32 col_addr) {

}

void Flash_Memory::Calibration_Long() {

}

void Flash_Memory::Calibration_Short() {

}

void Flash_Memory::Set_Features(int_8 feature) {

}

string Flash_Memory::Get_Features(int_8 feature) {
    return std::string();
}

void Flash_Memory::Feature_Parameter(int_8 param) {

}

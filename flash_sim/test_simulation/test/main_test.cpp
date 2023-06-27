#pragma once

#include "include/flash/flash.h"

#define LONG_TEXT "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Sed ac dolor sit amet purus malesuada congue. Etiam ligula pede, sagittis quis, interdum ultricies, scelerisque eu. In rutrum. Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Aenean vel massa quis mauris vehicula lacinia. Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime placeat facere possimus, omnis voluptas assumenda est, omnis dolor repellendus. Fusce tellus. Etiam ligula pede, sagittis quis, interdum ultricies, scelerisque eu. Fusce dui leo, imperdiet in, aliquam sit amet, feugiat eu, orci. Quisque porta. Donec vitae arcu. Quisque tincidunt scelerisque libero. Mauris elementum mauris vitae tortor. Vivamus luctus egestas leo. Cras pede libero, dapibus nec, pretium sit amet, tempor quis. Integer vulputate sem a nibh rutrum consequat. Nulla quis diam. Phasellus et lorem id felis nonummy placerat. Etiam dictum tincidunt diam. Mauris dolor felis, sagittis at, luctus sed, aliquam non, tellus. Praesent dapibus. Nulla est. Nullam sit amet magna in magna gravida vehicula. Aenean id metus id velit ullamcorper pulvinar. Praesent id justo in neque elementum ultrices. Sed convallis magna eu sem. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos hymenaeos. Vivamus porttitor turpis ac leo. Etiam egestas wisi a erat. Donec quis nibh at felis congue commodo. Nam sed tellus id magna elementum tincidunt. In enim a arcu imperdiet malesuada. Mauris dictum facilisis augue. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Cras pede libero, dapibus nec, pretium sit amet, tempor quis. Maecenas fermentum, sem in pharetra pellentesque, velit turpis volutpat ante, in pharetra metus odio a lectus. Nullam justo enim, consectetuer nec, ullamcorper ac, vestibulum in, elit. Curabitur vitae diam non enim vestibulum interdum. Curabitur sagittis hendrerit ante. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Integer lacinia. Suspendisse sagittis ultrices augue. Proin in tellus sit amet nibh dignissim sagittis. Nulla accumsan, elit sit amet varius semper, nulla mauris mollis quam, tempor suscipit diam nulla vel leo. Nunc tincidunt ante vitae massa. In convallis. Proin in tellus sit amet nibh dignissim sagittis. Etiam commodo dui eget wisi. Quisque porta. Praesent dapibus. In rutrum. Duis viverra diam non justo. Nulla accumsan, elit sit amet varius semper, nulla mauris mollis quam, tempor suscipit diam nulla vel leo. Etiam dictum tincidunt diam. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Nam quis nulla. Aliquam id dolor. Quisque tincidunt scelerisque libero."
#define SHORT_TEXT "Lorem ipsum dolor sit amet"

Flash_Memory *memory;

int counter_done = 0, counter_all = 0;

void run_test(bool (*function_test)(), string test_name) {
    counter_all++;
    if ((bool) function_test()) {
        cout << "[v] " << test_name << endl;
        counter_done++;
    } else
        cout << "[x] " << test_name << endl;
}

bool test_memory_init_default_ok() {
    memory = new Flash_Memory();
    return memory->Flash_Init() == EXIT_SUCCESS;
}

bool test_create_specific_memory_ok() {
    memory = new Flash_Memory(512, 8192, 16, NMem_Type::SLC, 10, 15, 20);
    return memory != nullptr && memory->Flash_Init() == EXIT_SUCCESS;
}

bool test_cache_init_ok() {
    return memory->Cache_Init() == EXIT_SUCCESS;
}

bool test_mem_info() {
    return memory->Read_ID() != nullptr;
}

bool test_read_cache_ok() {
    memory->Write_Cache(SHORT_TEXT);
    return memory->Read_Cache() != nullptr
        && memcmp(memory->Read_Cache(), (u_char *)SHORT_TEXT, sizeof(u_char)) == 0;
}

bool test_write_cache_ok() {
    return memory->Write_Cache(SHORT_TEXT) == EXIT_SUCCESS;
}

bool test_write_cache_wrong() {
    return memory->Write_Cache(LONG_TEXT) == EXIT_FAILURE;
}

bool test_program_sector_ok() {
    memory->Write_Cache(SHORT_TEXT);
    return memory->Program_Sector(0x00000000) == EXIT_SUCCESS;
}

bool test_program_sector_wrong() {
    memory->Write_Cache(SHORT_TEXT);
    return memory->Program_Sector(0x00FFFFFF) == EXIT_FAILURE;
}

bool test_read_sector_ok() {
    memory->Write_Cache(SHORT_TEXT);
    memory->Program_Sector(0x00000000);
    return memory->Read_Sector(0x00000000) == EXIT_SUCCESS
        && memcmp(memory->Read_Cache(), (u_char *)SHORT_TEXT, sizeof(u_char)) == 0;
}

bool test_read_sector_wrong() {
    memory->Write_Cache(SHORT_TEXT);
    memory->Program_Sector(0x00FFFFFF);
    return memory->Read_Sector(0x00FFFFFF) == EXIT_FAILURE;
}

bool test_program_page_ok() {
    memory->Write_Cache(SHORT_TEXT);
    return memory->Program_Page(0x00000000) == EXIT_SUCCESS;
}

bool test_program_page_wrong() {
    memory->Write_Cache(SHORT_TEXT);
    return memory->Program_Page(0x00FFFFFF) == EXIT_FAILURE;
}

bool test_read_page_ok() {
    memory->Write_Cache(SHORT_TEXT);
    memory->Program_Page(0x00000000);
    return memory->Read_Page(0x00000000) == EXIT_SUCCESS
           && memcmp(memory->Read_Cache(), (u_char *)SHORT_TEXT, sizeof(u_char)) == 0;
}

bool test_read_page_wrong() {
    memory->Write_Cache(SHORT_TEXT);
    memory->Program_Page(0x00FFFFFF);
    return memory->Read_Page(0x00FFFFFF) == EXIT_FAILURE;
}

bool test_program_data_move_ok() {
    memory->Write_Cache(SHORT_TEXT);
    memory->Program_Page(0x00000000);
    return memory->Program_Data_Move(0x00000000, 0x00000100) == EXIT_SUCCESS
        && memory->Read_Page(0x00000000) == EXIT_SUCCESS;
}

bool test_program_data_move_wrong() {
    memory->Write_Cache(SHORT_TEXT);
    memory->Program_Page(0x00000000);
    return memory->Program_Data_Move(0x00000000, 0x00FFFFFF) == EXIT_FAILURE;
}

bool test_block_erase_ok() {
    memory->Write_Cache(SHORT_TEXT);
    memory->Program_Page(0x00000000);
    return memory->Block_Erase(0x00000000) == EXIT_SUCCESS
        && memory->Read_Page(0x00000000) == EXIT_SUCCESS
        && memcmp(memory->Read_Cache(), (u_char *)"", sizeof(u_char)) == 0;
}

bool test_block_erase_wrong() {
    memory->Write_Cache(SHORT_TEXT);
    memory->Program_Page(0x00000000);
    return memory->Block_Erase(0x00FFFFFF) == EXIT_FAILURE;
}

bool test_num_of_writes_page_ok() {
    return memory->Num_Of_Writes_Page(0x00000000) == 9;
}

bool test_num_of_writes_page_wrong() {
    return memory->Num_Of_Writes_Page(0x00000000) != 1;
}

bool test_num_of_reads_page_ok() {
    return memory->Num_Of_Reads_Page(0x00000000) == 5;
}

bool test_num_of_reads_page_wrong() {
    return memory->Num_Of_Reads_Page(0x00000000) != 1;
}

bool test_num_of_writes_ok() {
    return memory->Num_Of_Writes(0x00000000) == 8;
}

bool test_num_of_writes_wrong() {
    return memory->Num_Of_Writes(0x00000000) != 1;
}

bool test_num_of_reads_ok() {
    return  memory->Num_Of_Reads(0x00000000) == 5;
}

bool test_num_of_reads_wrong() {
    return  memory->Num_Of_Reads(0x00000000) != 1;
}

bool test_ecc_info() {
    return memcmp(memory->ECC_Info(0x00000000), "", sizeof(u_char)) == 0;
}

bool test_ecc_histogram_memory_ok() {
    size_t *histogram = memory->ECC_Histogram();
    return histogram != nullptr && histogram[0] == 0 && histogram[1] == 0;
}

bool test_ecc_histogram_block_ok() {
    size_t *histogram = memory->ECC_Histogram(0x00000000);
    return histogram != nullptr && histogram[0] == 0 && histogram[1] == 0;
}

bool test_ecc_histogram_block_wrong() {
    size_t *histogram = memory->ECC_Histogram(0x00FFFFFF);
    return histogram == nullptr;
}

bool test_read_time_last_ok() {
    memory->Read_Page(0x00000700);
    return memory->Read_Time_Last(0x00000700) == DEFAULT_READ_PAGE_TIME;
}

bool test_read_time_last_wrong() {
    return memory->Read_Time_Last(0x00FFFFFF) == -1;
}

bool test_program_time_last_ok() {
    return memory->Program_Time_Last(0x00000000) == DEFAULT_PAGE_PROG_TIME;
}

bool test_program_time_last_wrong() {
    return memory->Program_Time_Last(0x00FFFFFF) == -1;
}

bool test_read_time_total_ok() {
    return memory->Read_Time_Total(0x00000000) == 55.0;
}

bool test_read_time_total_wrong() {
    return memory->Read_Time_Total(0x00FFFFFF) == -1;
}

bool test_program_time_total_ok() {
    return memory->Program_Time_Total(0x00000000) == 120;
}

bool test_program_time_total_wrong() {
    return memory->Program_Time_Total(0x00FFFFFF) == -1;
}

bool test_com_total_time_ok() {
    return memory->Com_Total_Time(0x00000000) == 0.0;
}

bool test_com_total_time_wrong() {
    return memory->Com_Total_Time(0x00FFFFFF) == -1;
}

bool test_sector_status_page_ok() {
    size_t *data = memory->Sector_Status_Page(0x00000000);
    return data != nullptr && data[0] == 8 && data[1] == 0;
}

bool test_sector_status_page_wrong() {
    return memory->Sector_Status_Page(0x00FFFFFF) == nullptr;
}

bool test_num_of_bad_blocks_ok() {
    return memory->Num_Of_Bad_Blocks() == 0;
}

bool test_num_of_bad_pages_ok() {
    return memory->Num_Of_Bad_Pages() == 0;
}

bool test_ecc_histogram() {
    return memory->ECC_Histogram() != nullptr;
}

bool test_sector_status_block_ok() {
    size_t *data = memory->Sector_Status_Block(0x00000000);
    return data != nullptr && data[0] == 1 && data[1] == 9 && data[2] == 0;
}

bool test_sector_status_block_wrong() {
    size_t *data = memory->Sector_Status_Block(0x00FFFFFF);
    return data == nullptr;
}

bool test_sector_status_memory_ok() {
    size_t *data = memory->Sector_Status_Memory();
    return data != nullptr && data[0] == 0 && data[1] == 0;
}

bool test_memory_num_of_writes_ok() {
    return memory->Num_Of_Writes() == 0;
}

bool test_memory_num_of_reads_ok() {
    return memory->Num_Of_Reads() == 6;
}

bool test_set_program_time_page_ok() {
    return memory->Set_Prog_Time_Page(0x00000000, 5.0) == EXIT_SUCCESS
        && memory->Program_Time_Last(0x00000000) == 5.0;
}

bool test_set_program_time_page_wrong() {
    return memory->Set_Prog_Time_Page(0x00FFFFFF, 0.0) == EXIT_FAILURE;
}

bool test_set_prog_time_block_ok() {
    return memory->Set_Prog_Time_Block(0x00000000, 6.0) == EXIT_SUCCESS
           && memory->Program_Time_Last(0x00000000) == 6.0;
}

bool test_set_prog_time_block_wrong() {
    return memory->Set_Prog_Time_Block(0x00FFFFFF, 0.0) == EXIT_FAILURE;
}

bool test_set_prog_time_mem_ok() {
    return memory->Set_Prog_Time_Mem(7.0) == EXIT_SUCCESS
           && memory->Program_Time_Last(0x00000000) == 7.0;
}

bool test_set_read_time_page_ok() {
    return memory->Set_Read_Time_Page(0x00000000, 5.0) == EXIT_SUCCESS
           && memory->Read_Time_Last(0x00000000) == 5.0;
}

bool test_set_read_time_page_wrong() {
    return memory->Set_Read_Time_Page(0x00FFFFFF, 0.0) == EXIT_FAILURE;
}

bool test_set_read_time_block_ok() {
    return memory->Set_Read_Time_Block(0x00000000, 6.0) == EXIT_SUCCESS
           && memory->Read_Time_Last(0x00000000) == 6.0;
}

bool test_set_read_time_block_wrong() {
    return memory->Set_Read_Time_Block(0x00FFFFFF, 0.0) == EXIT_FAILURE;
}

bool test_set_read_time_mem_ok() {
    return memory->Set_Read_Time_Mem(7.0) == EXIT_SUCCESS
           && memory->Read_Time_Last(0x00000000) == 7.0;
}

bool test_set_erase_time_block_ok() {
    return memory->Set_Erase_Time_Block(0x00000000, 5.0) == EXIT_SUCCESS
           && memory->Erase_Time_Last(0x00000000) == 5.0;
}

bool test_set_erase_time_block_wrong() {
    return memory->Set_Erase_Time_Block(0x00FFFFFF, 5.0) == EXIT_FAILURE;
}

bool test_set_erase_time_mem_ok() {
    return memory->Set_Erase_Time_Mem(6.0) == EXIT_SUCCESS
           && memory->Erase_Time_Last(0x00000000) == 6.0;
}

bool test_save_and_load_memory_ok() {
    return memory->Save_Memory("test_save_memory_ok_file.json") == EXIT_SUCCESS
        && memory->Load_Memory("test_save_memory_ok_file.json") == EXIT_SUCCESS;
}

bool test_save_and_load_memory_wrong() {
    return memory->Save_Memory("test_save_memory_wrong_file.json") == EXIT_FAILURE
           || memory->Load_Memory("test_save_memory_wrong_file.text") == EXIT_FAILURE;
}

bool test_save_and_load_state_ok() {
    bool save_return = memory->Save_State("test_save_state_ok_file.json") == EXIT_SUCCESS;
    memory = memory->Load_State("test_save_state_ok_file.json");
    bool load_return = memory != nullptr && memory->Read_Time_Total(0x00000000) == 7.0;
    return save_return && load_return;
}

bool test_save_and_load_state_wrong() {
    bool save_return = memory->Save_State("test_save_state_wrong_file.json") == EXIT_FAILURE;
    memory = memory->Load_State("test_save_memory_wrong_file.json");
    bool load_return = memory == nullptr || memory->Read_Time_Total(0x00000000) == 1.0;
    return save_return || load_return;
}
bool test_wear_out_block() {
    for (int i = 0; i < MAX_ERASE_NUMBER; ++i) {
        memory->Block_Erase(0x00080000);
    }

    return memory->Block_Erase(0x00080000) == EXIT_FAILURE && memory->Read_Status() == 132;
}

bool test_true_mem_size() {
    return memory->Get_True_Mem_Size() == 132816;
}

bool test_read_status_ok() {
    return memory->Read_Status() == 132;
}

bool test_reset_memory_ok() {
    return memory->Reset() == EXIT_SUCCESS;
}


int run_tests()
{
    run_test(&test_create_specific_memory_ok, "test_create_specific_memory_ok");
    run_test(&test_memory_init_default_ok, "test_memory_init_default_ok");
    run_test(&test_cache_init_ok, "test_cache_init_ok");
    run_test(&test_mem_info, "test_mem_info");
    run_test(&test_read_cache_ok, "test_read_cache_ok");
    run_test(&test_write_cache_ok, "test_write_cache_ok");
    run_test(&test_write_cache_wrong, "test_write_cache_wrong");
    run_test(&test_program_sector_ok, "test_program_sector_ok");
    run_test(&test_program_sector_wrong, "test_program_sector_wrong");
    run_test(&test_read_sector_ok, "test_read_sector_ok");
    run_test(&test_read_sector_wrong, "test_read_sector_wrong");
    run_test(&test_program_page_ok, "test_program_page_ok");
    run_test(&test_program_page_wrong, "test_program_page_wrong");
    run_test(&test_read_page_ok, "test_read_page_ok");
    run_test(&test_read_page_wrong, "test_read_page_wrong");
    run_test(&test_program_data_move_ok, "test_program_data_move_ok");
    run_test(&test_program_data_move_wrong, "test_program_data_move_wrong");
    run_test(&test_block_erase_ok, "test_block_erase_ok");
    run_test(&test_block_erase_wrong, "test_block_erase_wrong");
    run_test(&test_num_of_writes_page_ok, "test_num_of_writes_page_ok");
    run_test(&test_num_of_writes_page_wrong, "test_num_of_writes_page_wrong");
    run_test(&test_num_of_reads_page_ok, "test_num_of_reads_page_ok");
    run_test(&test_num_of_reads_page_wrong, "test_num_of_reads_page_wrong");
    run_test(&test_num_of_writes_ok, "test_num_of_writes_ok");
    run_test(&test_num_of_writes_wrong, "test_num_of_writes_wrong");
    run_test(&test_num_of_reads_ok, "test_num_of_reads_ok");
    run_test(&test_num_of_reads_wrong, "test_num_of_reads_wrong");
    run_test(&test_ecc_info, "test_ecc_info");
    run_test(&test_read_time_last_ok, "test_read_time_last_ok");
    run_test(&test_read_time_last_wrong, "test_read_time_last_wrong");
    run_test(&test_program_time_last_ok, "test_program_time_last_ok");
    run_test(&test_program_time_last_wrong, "test_program_time_last_wrong");
    run_test(&test_read_time_total_ok, "test_read_time_total_ok");
    run_test(&test_read_time_total_wrong, "test_read_time_total_wrong");
    run_test(&test_program_time_total_ok, "test_program_time_total_ok");
    run_test(&test_program_time_total_wrong, "test_program_time_total_wrong");
    run_test(&test_com_total_time_ok, "test_com_total_time_ok");
    run_test(&test_com_total_time_wrong, "test_com_total_time_wrong");
    run_test(&test_sector_status_page_ok, "test_sector_status_page_ok");
    run_test(&test_sector_status_page_wrong, "test_sector_status_page_wrong");
    run_test(&test_num_of_bad_blocks_ok, "test_num_of_bad_blocks_ok");
    run_test(&test_num_of_bad_pages_ok, "test_num_of_bad_pages_ok");
    run_test(&test_ecc_histogram, "test_ecc_histogram");
    run_test(&test_ecc_histogram_memory_ok, "test_ecc_histogram_memory_ok");
    run_test(&test_ecc_histogram_block_ok, "test_ecc_histogram_block_ok");
    run_test(&test_ecc_histogram_block_wrong, "test_ecc_histogram_block_wrong");
    run_test(&test_sector_status_block_ok, "test_sector_status_block_ok");
    run_test(&test_sector_status_block_wrong, "test_sector_status_block_wrong");
    run_test(&test_sector_status_memory_ok, "test_sector_status_memory_ok");
    run_test(&test_memory_num_of_writes_ok, "test_memory_num_of_writes_ok");
    run_test(&test_memory_num_of_reads_ok, "test_memory_num_of_reads_ok");
    run_test(&test_set_program_time_page_ok, "test_set_program_time_page_ok");
    run_test(&test_set_program_time_page_wrong, "test_set_program_time_page_wrong");
    run_test(&test_set_prog_time_block_ok, "test_set_prog_time_block_ok");
    run_test(&test_set_prog_time_block_wrong, "test_set_prog_time_block_wrong");
    run_test(&test_set_prog_time_mem_ok, "test_set_prog_time_mem_ok");
    run_test(&test_set_read_time_page_ok, "test_set_read_time_page_ok");
    run_test(&test_set_read_time_page_wrong, "test_set_read_time_page_wrong");
    run_test(&test_set_read_time_block_ok, "test_set_read_time_block_ok");
    run_test(&test_set_read_time_block_wrong, "test_set_read_time_block_wrong");
    run_test(&test_set_read_time_mem_ok, "test_set_read_time_mem_ok");
    run_test(&test_set_erase_time_block_ok, "test_set_erase_time_block_ok");
    run_test(&test_set_erase_time_block_wrong, "test_set_erase_time_block_wrong");
    run_test(&test_set_erase_time_mem_ok, "test_set_erase_time_mem_ok");
    run_test(&test_save_and_load_memory_ok, "test_save_and_load_memory_ok");
    run_test(&test_save_and_load_memory_wrong, "test_save_and_load_memory_wrong");
//    run_test(&test_save_and_load_state_ok, "test_save_and_load_state_ok");
//    run_test(&test_save_and_load_state_wrong, "test_save_and_load_state_wrong");
    run_test(&test_wear_out_block, "test_wear_out_block");
    run_test(&test_true_mem_size, "test_true_mem_size");
    run_test(&test_read_status_ok, "test_read_status_ok");
    run_test(&test_reset_memory_ok, "test_reset_memory_ok");

    cout << "Dobehlo spravne " << counter_done << " z " << counter_all << " testu\n";

    return EXIT_SUCCESS;
}

#include <../flash_sim/include/flash/flash.h>

#define LONG_TEXT "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Sed ac dolor sit amet purus malesuada congue. Etiam ligula pede, sagittis quis, interdum ultricies, scelerisque eu. In rutrum. Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Aenean vel massa quis mauris vehicula lacinia. Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime placeat facere possimus, omnis voluptas assumenda est, omnis dolor repellendus. Fusce tellus. Etiam ligula pede, sagittis quis, interdum ultricies, scelerisque eu. Fusce dui leo, imperdiet in, aliquam sit amet, feugiat eu, orci. Quisque porta. Donec vitae arcu. Quisque tincidunt scelerisque libero. Mauris elementum mauris vitae tortor. Vivamus luctus egestas leo. Cras pede libero, dapibus nec, pretium sit amet, tempor quis. Integer vulputate sem a nibh rutrum consequat. Nulla quis diam. Phasellus et lorem id felis nonummy placerat. Etiam dictum tincidunt diam. Mauris dolor felis, sagittis at, luctus sed, aliquam non, tellus. Praesent dapibus. Nulla est. Nullam sit amet magna in magna gravida vehicula. Aenean id metus id velit ullamcorper pulvinar. Praesent id justo in neque elementum ultrices. Sed convallis magna eu sem. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos hymenaeos. Vivamus porttitor turpis ac leo. Etiam egestas wisi a erat. Donec quis nibh at felis congue commodo. Nam sed tellus id magna elementum tincidunt. In enim a arcu imperdiet malesuada. Mauris dictum facilisis augue. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Cras pede libero, dapibus nec, pretium sit amet, tempor quis. Maecenas fermentum, sem in pharetra pellentesque, velit turpis volutpat ante, in pharetra metus odio a lectus. Nullam justo enim, consectetuer nec, ullamcorper ac, vestibulum in, elit. Curabitur vitae diam non enim vestibulum interdum. Curabitur sagittis hendrerit ante. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Integer lacinia. Suspendisse sagittis ultrices augue. Proin in tellus sit amet nibh dignissim sagittis. Nulla accumsan, elit sit amet varius semper, nulla mauris mollis quam, tempor suscipit diam nulla vel leo. Nunc tincidunt ante vitae massa. In convallis. Proin in tellus sit amet nibh dignissim sagittis. Etiam commodo dui eget wisi. Quisque porta. Praesent dapibus. In rutrum. Duis viverra diam non justo. Nulla accumsan, elit sit amet varius semper, nulla mauris mollis quam, tempor suscipit diam nulla vel leo. Etiam dictum tincidunt diam. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Nam quis nulla. Aliquam id dolor. Quisque tincidunt scelerisque libero."
#define SHORT_TEXT "Lorem ipsum dolor sit amet"

Flash_Memory *memory;

int counter_done = 0, counter_all = 0;

void run_test(bool (*function_test)(), string test_name) {
    counter_all++;
    if ((bool) function_test()) {
        cout << test_name << " has run " << "succesfuly\n";
        counter_done++;
    } else
        cout << test_name << " has run " << "wrongly\n";
}

bool test_memory_init_default_ok() {
    memory = new Flash_Memory();
    return memory->Flash_Init() == EXIT_SUCCESS;
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


int main(int argc, char** argv)
{

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

    cout << "Dobehlo spravne " << counter_done << " z " << counter_all << " testu\n";

    return EXIT_SUCCESS;
}

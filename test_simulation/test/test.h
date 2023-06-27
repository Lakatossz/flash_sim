#pragma once

#include "test.h"
#include "../../flash_sim/include/flash/flash.h"

#define LONG_TEXT "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Sed ac dolor sit amet purus malesuada congue. Etiam ligula pede, sagittis quis, interdum ultricies, scelerisque eu. In rutrum. Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Aenean vel massa quis mauris vehicula lacinia. Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime placeat facere possimus, omnis voluptas assumenda est, omnis dolor repellendus. Fusce tellus. Etiam ligula pede, sagittis quis, interdum ultricies, scelerisque eu. Fusce dui leo, imperdiet in, aliquam sit amet, feugiat eu, orci. Quisque porta. Donec vitae arcu. Quisque tincidunt scelerisque libero. Mauris elementum mauris vitae tortor. Vivamus luctus egestas leo. Cras pede libero, dapibus nec, pretium sit amet, tempor quis. Integer vulputate sem a nibh rutrum consequat. Nulla quis diam. Phasellus et lorem id felis nonummy placerat. Etiam dictum tincidunt diam. Mauris dolor felis, sagittis at, luctus sed, aliquam non, tellus. Praesent dapibus. Nulla est. Nullam sit amet magna in magna gravida vehicula. Aenean id metus id velit ullamcorper pulvinar. Praesent id justo in neque elementum ultrices. Sed convallis magna eu sem. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos hymenaeos. Vivamus porttitor turpis ac leo. Etiam egestas wisi a erat. Donec quis nibh at felis congue commodo. Nam sed tellus id magna elementum tincidunt. In enim a arcu imperdiet malesuada. Mauris dictum facilisis augue. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Cras pede libero, dapibus nec, pretium sit amet, tempor quis. Maecenas fermentum, sem in pharetra pellentesque, velit turpis volutpat ante, in pharetra metus odio a lectus. Nullam justo enim, consectetuer nec, ullamcorper ac, vestibulum in, elit. Curabitur vitae diam non enim vestibulum interdum. Curabitur sagittis hendrerit ante. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Integer lacinia. Suspendisse sagittis ultrices augue. Proin in tellus sit amet nibh dignissim sagittis. Nulla accumsan, elit sit amet varius semper, nulla mauris mollis quam, tempor suscipit diam nulla vel leo. Nunc tincidunt ante vitae massa. In convallis. Proin in tellus sit amet nibh dignissim sagittis. Etiam commodo dui eget wisi. Quisque porta. Praesent dapibus. In rutrum. Duis viverra diam non justo. Nulla accumsan, elit sit amet varius semper, nulla mauris mollis quam, tempor suscipit diam nulla vel leo. Etiam dictum tincidunt diam. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Nam quis nulla. Aliquam id dolor. Quisque tincidunt scelerisque libero."
#define SHORT_TEXT "Lorem ipsum dolor sit amet"

Flash_Memory *memory;

int counter_done = 0, counter_all = 0;

void run_test(bool (*function_test)(), string test_name);

bool test_memory_init_default_ok();

bool test_create_specific_memory_ok();

bool test_cache_init_ok();

bool test_mem_info();

bool test_read_cache_ok();

bool test_write_cache_ok();

bool test_write_cache_wrong();

bool test_program_sector_ok();

bool test_program_sector_wrong();

bool test_read_sector_ok();

bool test_read_sector_wrong();

bool test_program_page_ok();

bool test_program_page_wrong();

bool test_read_page_ok();

bool test_read_page_wrong();

bool test_program_data_move_ok();

bool test_program_data_move_wrong();

bool test_block_erase_ok();

bool test_block_erase_wrong();

bool test_num_of_writes_page_ok();

bool test_num_of_writes_page_wrong();

bool test_num_of_reads_page_ok();

bool test_num_of_reads_page_wrong();

bool test_num_of_writes_ok();

bool test_num_of_writes_wrong();

bool test_num_of_reads_ok();

bool test_num_of_reads_wrong();

bool test_ecc_info();

bool test_ecc_histogram_memory_ok();

bool test_ecc_histogram_block_ok();

bool test_ecc_histogram_block_wrong();

bool test_read_time_last_ok();

bool test_read_time_last_wrong();

bool test_program_time_last_ok();

bool test_program_time_last_wrong();

bool test_read_time_total_ok();

bool test_read_time_total_wrong();

bool test_program_time_total_ok();

bool test_program_time_total_wrong();

bool test_com_total_time_ok();

bool test_com_total_time_wrong();

bool test_sector_status_page_ok();

bool test_sector_status_page_wrong();

bool test_num_of_bad_blocks_ok();

bool test_num_of_bad_pages_ok();

bool test_ecc_histogram();

bool test_sector_status_block_ok();

bool test_sector_status_block_wrong();

bool test_sector_status_memory_ok();

bool test_memory_num_of_writes_ok();

bool test_memory_num_of_reads_ok();

bool test_set_program_time_page_ok();

bool test_set_program_time_page_wrong();

bool test_set_prog_time_block_ok();

bool test_set_prog_time_block_wrong();

bool test_set_prog_time_mem_ok();

bool test_set_read_time_page_ok();

bool test_set_read_time_page_wrong();

bool test_set_read_time_block_ok();

bool test_set_read_time_block_wrong();

bool test_set_read_time_mem_ok();

bool test_set_erase_time_block_ok();

bool test_set_erase_time_block_wrong();

bool test_set_erase_time_mem_ok();

bool test_save_and_load_memory_ok();

bool test_save_and_load_memory_wrong();

bool test_save_and_load_state_ok();

bool test_save_and_load_state_wrong();

bool test_wear_out_block();

bool test_true_mem_size();

bool test_read_status_ok();

bool test_reset_memory_ok();

int run_tests();


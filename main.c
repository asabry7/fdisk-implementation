#include "MBR.h"
#include "GPT.h"

int main(int argc, char **argv) {
    char buf[SECTOR_SIZE];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    read(fd, buf, SECTOR_SIZE);

    if ((uint8_t)buf[450] == 0xEE) {

        /* GPT Partition Table */
        printf("%-16s%-6s %-10s %-10s %-10s %-10s %-36s\n",
               "Device", "No.", "Start", "End", "Sectors", "Size(MB)", "Type");

        // Read GPT header at LBA 1
        lseek(fd, GPT_HEADER_LBA * SECTOR_SIZE, SEEK_SET);
        read(fd, buf, SECTOR_SIZE);

        // Get the partition entry array starting LBA and the number of partition entries
        uint64_t partition_entry_lba = GPT_ENTRY_ARRAY_LBA;
        uint32_t partition_entry_count = GPT_ENTRIES_NUM;

        // Read partition entries
        GPT_PartitionEntry entry;
        int partition_index = 1;
        for (int i = 0; i < partition_entry_count; i++) {
            lseek(fd, (partition_entry_lba * SECTOR_SIZE) + (i * GPT_ENTRY_SIZE), SEEK_SET);
            read(fd, &entry, GPT_ENTRY_SIZE);
            GPT_print_partition_info(argv[1], partition_index++, &entry);
        }
    }

    
    else 
    {
        /* MBR Partition Table */

        /* Skip the first 446 bytes (IPL part) */
        MBR_PartitionEntry *table_entry_ptr = (MBR_PartitionEntry *)&buf[446];

        printf("%-20s%-6s %-6s %-10s %-10s %-10s %-10s %-6s %-6s\n", "Device", "No.", "Boot", "Start", "End", "Sectors", "Size(MB)", "Id", "Type");

        int partition_index = 1;
        int logical_partition_index = 5;  // Start logical partitions at index 5

        for (int i = 0; i < 4; i++) {
            if (table_entry_ptr[i].lba != 0) {
                MBR_print_partition_info(argv[1], partition_index++, &table_entry_ptr[i], 0);

                // Check if this is an extended partition
                if (table_entry_ptr[i].partition_type == 0x05 || // CHS extended partition
                    table_entry_ptr[i].partition_type == 0x0F || // LBA extended partition
                    table_entry_ptr[i].partition_type == 0x85)   // Linux extended
                { 
                    MBR_parse_ebr(fd, argv[1], table_entry_ptr[i].lba, &logical_partition_index);
                }
            }
        }
    }

    close(fd);
    return 0;
}

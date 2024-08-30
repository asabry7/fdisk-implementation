#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

typedef struct {
    uint8_t status;
    uint8_t first_chs[3];
    uint8_t partition_type;
    uint8_t last_chs[3];
    uint32_t lba;
    uint32_t sector_count;
} PartitionEntry;

void print_partition_info(const char *device, int index, PartitionEntry *entry, uint32_t base_lba) {
    if (entry->lba == 0 && entry->sector_count == 0) {
        return;  // Skip empty partitions
    }

    printf("%-10s%-6d %-6c %-10u %-10u %-10u %-10u %-6X %-6X\n",
           device,
           index,
           entry->status == 0x80 ? '*' : ' ',
           base_lba + entry->lba,
           base_lba + entry->lba + entry->sector_count - 1,
           entry->sector_count,
           (uint32_t)(((uint64_t)entry->sector_count * 512) / (1024 * 1024)),
           entry->partition_type,
           entry->partition_type);  // Adjust Type accordingly
}

void parse_ebr(int fd, const char *device, uint32_t ebr_lba, int *index) {
    char buf[512];
    PartitionEntry *entry;

    while (ebr_lba != 0) {
        lseek(fd, ebr_lba * 512, SEEK_SET);
        read(fd, buf, 512);

        entry = (PartitionEntry *)&buf[446];
        print_partition_info(device, *index, &entry[0], ebr_lba);
        (*index)++;

        if (entry[1].lba != 0) {
            ebr_lba = entry[1].lba + ebr_lba;
        } else {
            break;  // No more logical partitions
        }
    }
}

int main(int argc, char **argv) {
    char buf[512];

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    read(fd, buf, 512);
    PartitionEntry *table_entry_ptr = (PartitionEntry *)&buf[446];

    printf("%-10s%-6s %-6s %-10s %-10s %-10s %-10s %-6s %-6s\n", "Device", "No.", "Boot", "Start", "End", "Sectors", "Size(MB)", "Id", "Type");

    int partition_index = 1;

    for (int i = 0; i < 4; i++) {
        if (table_entry_ptr[i].lba != 0) {
            print_partition_info(argv[1], partition_index++, &table_entry_ptr[i], 0);

            // Check if this is an extended partition
            if (table_entry_ptr[i].partition_type == 0x05 || // CHS extended partition
                table_entry_ptr[i].partition_type == 0x0F || // LBA extended partition
                table_entry_ptr[i].partition_type == 0x85) { // Linux extended
                parse_ebr(fd, argv[1], table_entry_ptr[i].lba, &partition_index);
            }
        }
    }

    close(fd);
    return 0;
}

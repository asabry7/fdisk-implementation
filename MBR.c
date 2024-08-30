#include "MBR.h"


void MBR_print_partition_info(const char *device, int index, MBR_PartitionEntry *entry, uint32_t base_lba) {
    if (entry->lba == 0 && entry->sector_count == 0) {
        return;  // Skip empty partitions
    }

    printf("%-20s%-6d %-6c %-10u %-10u %-10u %-10u %-6X %-6X\n",
           device,
           index,
           entry->status == 0x80 ? '*' : ' ',
           base_lba + entry->lba,
           base_lba + entry->lba + entry->sector_count - 1,
           entry->sector_count,
           (uint32_t)(((uint64_t)entry->sector_count * SECTOR_SIZE) / (1024 * 1024)),
           entry->partition_type,
           entry->partition_type);  // Adjust Type accordingly
}

void MBR_parse_ebr(int fd, const char *device, uint32_t ebr_lba, int *index) {
    char buf[SECTOR_SIZE];
    MBR_PartitionEntry *entry;

    while (ebr_lba != 0) {
        lseek(fd, ebr_lba * SECTOR_SIZE, SEEK_SET);
        read(fd, buf, SECTOR_SIZE);

        entry = (MBR_PartitionEntry *)&buf[446];
        MBR_print_partition_info(device, *index, &entry[0], ebr_lba);
        (*index)++;

        if (entry[1].lba != 0) {
            ebr_lba = entry[1].lba + ebr_lba;
        } else {
            break;  // No more logical partitions
        }
    }
}
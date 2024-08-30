#include "GPT.h"

void GPT_print_partition_info(const char *device, int index, GPT_PartitionEntry *entry) {
     if (entry->starting_lba == 0 && entry->ending_lba == 0) {
        return;  // Skip empty partitions
    }

    char name[73];
    memset(name, 0, sizeof(name));
    for (int i = 0; i < 36; i++) {
        name[i * 2] = entry->partition_name[i] & 0xFF;
        name[i * 2 + 1] = (entry->partition_name[i] >> 8) & 0xFF;
    }

    printf("%-16s%-6d %-10llu %-10llu %-10llu %-10llu %-36s\n",
           device,
           index,
           (unsigned long long)entry->starting_lba,
           (unsigned long long)entry->ending_lba,
           (unsigned long long)(entry->ending_lba - entry->starting_lba + 1),
           (unsigned long long)((entry->ending_lba - entry->starting_lba + 1) * SECTOR_SIZE / (1024 * 1024)),
           name);
}

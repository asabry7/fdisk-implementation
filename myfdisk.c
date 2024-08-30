#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>

typedef struct {
    uint8_t status;
    uint8_t first_chs[3];
    uint8_t partition_type;
    uint8_t last_chs[3];
    uint32_t lba;
    uint32_t sector_count;
} PartitionEntry;

int main(int argc, char **argv)
{
    char buf[512];

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    read(fd, buf, 512);
    close(fd);

    PartitionEntry *table_entry_ptr = (PartitionEntry *)&buf[446];

    printf("%-10s %-6s %-10s %-10s %-10s %-10s %-6s %-6s\n", "Device", "Boot", "Start", "End", "Sectors", "Size", "Id", "Type");

    for (int i = 0; i < 4; i++) {
        printf("%-10s %-6c %-10u %-10u %-10u %-10u %-6X %-6X\n",
               argv[1],
               table_entry_ptr[i].status == 0x80 ? '*' : ' ',
               table_entry_ptr[i].lba,
               table_entry_ptr[i].lba + table_entry_ptr[i].sector_count - 1,
               table_entry_ptr[i].sector_count,
               (uint32_t)(((uint64_t)table_entry_ptr[i].sector_count * 512) / (1024 * 1024)),
               table_entry_ptr[i].partition_type,
               table_entry_ptr[i].partition_type); // Adjust Type accordingly
    }

    return 0;
}

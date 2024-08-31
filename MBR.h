#ifndef _MBR_H_
#define _MBR_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#define SECTOR_SIZE 512
#define CHS_EXTENDED_PARTITION      0x05
#define LBA_EXTENDED_PARTITION      0x0F
#define LINUX_EXTENDED_PARTITION    0x85  

typedef struct {
    uint8_t status;
    uint8_t first_chs[3];
    uint8_t partition_type;
    uint8_t last_chs[3];
    uint32_t lba;
    uint32_t sector_count;
} MBR_PartitionEntry;


void MBR_print_size(uint64_t size_in_sectors);


void MBR_print_partition_info(const char *device, int index, MBR_PartitionEntry *entry, uint32_t base_lba);

void MBR_parse_ebr(int fd, const char *device, uint32_t First_EBR_LBA, int *index);

#endif
#ifndef _GPT_H_
#define _GPT_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

#define SECTOR_SIZE                 512
#define GPT_HEADER_LBA              1
#define GPT_ENTRY_ARRAY_LBA         2
#define GPT_ENTRY_SIZE              128
#define GPT_ENTRIES_NUM             128

#define GPT_TYPE_STRING_LENGTH      36


typedef struct {
    uint8_t type_guid[16];
    uint8_t partition_guid[16];
    uint64_t starting_lba;
    uint64_t ending_lba;
    uint64_t attributes;
    uint16_t partition_name[36];
} GPT_PartitionEntry;


typedef struct {
    const char *guid_prefix;
    const char *name;
} GPT_PartitionType;




void convert_guid_to_string(const unsigned char *guid, char *guid_str);

const char* get_partition_type(const char *type_guid);

void GPT_print_partition_info(const char *device, int index, GPT_PartitionEntry *entry);

#endif
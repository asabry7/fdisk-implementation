#include "GPT.h"

/* Some GPT Partitions */
GPT_PartitionType partition_types[] = {
    {"EBD0A0A2-B9E5-4433-87C0-68B6B72699C7", "Microsoft basic data"},
    {"C12A7328-F81F-11D2-BA4B-00A0C93EC93B", "EFI System"},
    {"E3C9E316-0B5C-4DB8-817D-F92DF00215AE", "Microsoft reserved"},
    {"DE94BBA4-06D1-4D40-A16A-BFD50179D6AC", "Windows recovery environment"},
    {"0FC63DAF-8483-4772-8E79-3D69D8477DE4", "Linux filesystem"},
    {"A19D880F-05FC-4D3B-A006-743F0F84911E", "Linux swap"},
    {"48465300-0000-11AA-AA11-00306543ECAC", "Apple HFS+"},
    {"426F6F74-0000-11AA-AA11-00306543ECAC", "Apple Boot"},
    {NULL, NULL}  
};


/* Function to convert the type GUID from binary to string format */
void convert_guid_to_string(const unsigned char *guid, char *guid_str) {
    sprintf(guid_str,
            "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            guid[3], guid[2], guid[1], guid[0],                             /* Little endian => Reversed byte order */
            guid[5], guid[4],                                               /* Little endian => Reversed byte order */
            guid[7], guid[6],                                               /* Little endian => Reversed byte order */
            guid[8], guid[9],                                               /* Big endian    => Keep original order */
            guid[10], guid[11], guid[12], guid[13], guid[14], guid[15]);    /* Big endian    => Keep original order */
}

/* Function to map the obtained GUID to the suitable GPT type */
const char* GPT_get_partition_type(const char *type_guid) {
    for (int i = 0; partition_types[i].guid_prefix != NULL; i++) {
        if (strncmp(partition_types[i].guid_prefix, type_guid, strlen(partition_types[i].guid_prefix)) == 0) {
            return partition_types[i].name;
        }
    }
    return "Unknown Type";  
}


void GPT_print_partition_info(const char *device, int index, GPT_PartitionEntry *entry) {
    if (entry->starting_lba == 0 && entry->ending_lba == 0) 
    {
        /* Empty Partition => skip */
        return;  
    }

    /* Convert the binary GUID to a string, the length is 36 + 1 for the null terminator */
    char guid_str[ GPT_TYPE_STRING_LENGTH + 1 ];  
    convert_guid_to_string(entry->type_guid, guid_str);

    /* Map the obtained string to the suitable GPT type */
    const char *partition_type = GPT_get_partition_type(guid_str);

    /* Print Partition information */
    printf("%-16s%-6d %-10llu %-10llu %-10llu %-10llu %-36s \n",
           device,
           index,
           (unsigned long long)entry->starting_lba,
           (unsigned long long)entry->ending_lba,
           (unsigned long long)(entry->ending_lba - entry->starting_lba + 1),
           (unsigned long long)((entry->ending_lba - entry->starting_lba + 1) * SECTOR_SIZE / (1024 * 1024)),
           partition_type
           );
}

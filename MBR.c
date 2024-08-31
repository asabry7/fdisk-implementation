#include "MBR.h"

const char* MBR_get_partition_type(unsigned char type) {
    switch (type) {
        case 0x00: return "Empty";
        case 0x01: return "FAT12";
        case 0x04: return "FAT16 (small)";
        case 0x05: return "Extended";
        case 0x06: return "FAT16";
        case 0x07: return "NTFS or exFAT";
        case 0x0B: return "FAT32 (CHS)";
        case 0x0C: return "FAT32 (LBA)";
        case 0x0E: return "FAT16 (LBA)";
        case 0x11: return "Hidden FAT16";
        case 0x12: return "Compaq Diagnostics";
        case 0x14: return "FAT16 (large)";
        case 0x16: return "Hidden FAT16 (LBA)";
        case 0x1B: return "Hidden FAT32 (CHS)";
        case 0x1C: return "Hidden FAT32 (LBA)";
        case 0x1E: return "Hidden FAT16 (LBA)";
        case 0x20: return "Dynamic Disk";
        case 0x27: return "Hidden NTFS/HPFS";
        case 0x39: return "Plan 9";
        case 0x3C: return "PartitionMagic";
        case 0x80: return "Linux Swap";
        case 0x81: return "Linux";
        case 0x82: return "Linux Swap / Solaris";
        case 0x83: return "Linux";
        case 0x84: return "OS/2 hidden";
        case 0x85: return "Linux Extended";
        case 0x86: return "NTFS volume set";
        case 0x87: return "NTFS volume set (no formatting)";
        case 0x8A: return "OS/2 Boot Manager";
        case 0x8B: return "OS/2 Boot Manager";
        case 0xA0: return "IBM ThinkPad";
        case 0xA5: return "FreeBSD/NetBSD";
        case 0xA6: return "OpenBSD";
        case 0xA8: return "Mac OS X";
        case 0xB1: return "BSD";
        case 0xB4: return "QNX4.x";
        case 0xB5: return "QNX4.x";
        case 0xBF: return "Solaris";
        case 0xC0: return "Cisco";
        case 0xC1: return "Novell NetWare";
        case 0xC6: return "Novell NetWare";
        case 0xDA: return "Non-FS";
        case 0xDE: return "Dell Utility";
        case 0xE1: return "DOS 1.x";
        case 0xE3: return "DOS 2.x";
        case 0xE4: return "DOS 3.x";
        case 0xE5: return "DOS 4.x";
        case 0xE6: return "DOS 5.x";
        case 0xE7: return "DOS 6.x";
        case 0xEB: return "DOS 7.x";
        case 0xF0: return "DOS 8.x";
        case 0xF1: return "DOS 9.x";
        case 0xF2: return "NTFS";
        case 0xF4: return "NTFS";
        case 0xF8: return "BeOS";
        case 0xFB: return "Bootable";
        case 0xFE: return "EFI GPT";
        default: return "Unknown";
    }
}

void MBR_print_partition_info(const char *device, int index, MBR_PartitionEntry *entry, uint32_t base_lba) {
    if (entry->lba == 0 && entry->sector_count == 0) 
    {
        /* Empty Partition => skip */
        return;  
    }

    printf("%-20s%-6d %-6c %-10u %-10u %-10u %-10u %-6X %-6s\n",
           device,
           index,
           entry->status == 0x80 ? '*' : ' ',
           base_lba + entry->lba,
           base_lba + entry->lba + entry->sector_count - 1,
           entry->sector_count,
           (uint32_t)(((uint64_t)entry->sector_count * SECTOR_SIZE) / (1024 * 1024)),
           entry->partition_type,
           MBR_get_partition_type(entry->partition_type));  
}

void MBR_parse_ebr(int fd, const char *device, uint32_t CurrentPartition_LBA, int *index) {
    char buf[SECTOR_SIZE];
    MBR_PartitionEntry *entry;

    while (CurrentPartition_LBA != 0) 
    {
        /* Move the file descriptor to the next EBR */
        lseek(fd, CurrentPartition_LBA * SECTOR_SIZE, SEEK_SET);
        read(fd, buf, SECTOR_SIZE);

        /* Skip the first 446 Bytes */
        entry = (MBR_PartitionEntry *)&buf[446];

        /* Print the information of the current EBR */
        MBR_print_partition_info(device, *index, &entry[0], CurrentPartition_LBA);
        (*index)++;

        if (entry[1].lba != 0) 
        {
            /* Move the LBA to the next EBR */
            CurrentPartition_LBA = CurrentPartition_LBA + entry[1].lba;
        } 
        
        else 
        {
            /* No more logical partitions */
            break;  
        }
    }
}
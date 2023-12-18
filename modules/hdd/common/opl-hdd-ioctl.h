#ifndef OPL_HDD_IOCTL_H
#define OPL_HDD_IOCTL_H

// Commands and structures for XATAD.IRX
#define ATA_DEVCTL_IS_48BIT                 0x6840
#define ATA_DEVCTL_SET_TRANSFER_MODE        0x6841
#define ATA_DEVCTL_READ_PARTITION_SECTOR    0x6842
#define ATA_DEVCTL_GET_HIGHEST_UDMA_MODE    0x6843

#define ATA_XFER_MODE_MDMA 0x20
#define ATA_XFER_MODE_UDMA 0x40

// structs for DEVCTL commands

typedef struct
{
    int type;
    int mode;
} hddAtaSetMode_t;

#endif

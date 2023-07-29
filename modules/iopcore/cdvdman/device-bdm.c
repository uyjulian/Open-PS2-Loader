/*
  Copyright 2009-2010, jimmikaelkael
  Licenced under Academic Free License version 3.0
  Review Open PS2 Loader README & LICENSE files for further details.
*/

#include "internal.h"

#include <bdm.h>
#include <bd_defrag.h>

#include "device.h"

#ifdef USE_BDM_ATA
#include "atad.h"

char lba_48bit = 0;
char atad_inited = 0;
#endif

#define U64_2XU32(val)  ((u32*)val)[1], ((u32*)val)[0]

extern struct cdvdman_settings_bdm cdvdman_settings;
static struct block_device *g_bd = NULL;
static u32 g_bd_sectors_per_sector = 4;
static int bdm_io_sema;

extern struct irx_export_table _exp_bdm;

#ifdef USE_BDM_ATA
extern struct irx_export_table _exp_atad;
#endif

//
// BDM exported functions
//

void bdm_connect_bd(struct block_device *bd)
{
    DPRINTF("connecting device %s%dp%d\n", bd->name, bd->devNr, bd->parNr);

    if (g_bd == NULL && bd->devNr == cdvdman_settings.bdDeviceId)
    {
        DPRINTF("attaching to %s%dp%d\n", bd->name, bd->devNr, bd->parNr);

        g_bd = bd;
        g_bd_sectors_per_sector = (2048 / bd->sectorSize);
        // Free usage of block device
        SignalSema(bdm_io_sema);
    }
}

void bdm_disconnect_bd(struct block_device *bd)
{
    DPRINTF("disconnecting device %s%dp%d\n", bd->name, bd->devNr, bd->parNr);

    if (bd->devNr == cdvdman_settings.bdDeviceId)
    {
        DPRINTF("detatching from %s%dp%d\n", bd->name, bd->devNr, bd->parNr);

        // Lock usage of block device
        WaitSema(bdm_io_sema);
        if (g_bd == bd)
            g_bd = NULL;
    }
}

//
// cdvdman "Device" functions
//

void DeviceInit(void)
{
    iop_sema_t smp;

    DPRINTF("%s\n", __func__);

    // Create semaphore, initially locked
    smp.initial = 0;
    smp.max = 1;
    smp.option = 0;
    smp.attr = SA_THPRI;
    bdm_io_sema = CreateSema(&smp);
    
    // Register pseudo bdm entry points. 
    RegisterLibraryEntries(&_exp_bdm);

#ifdef USE_BDM_ATA
    RegisterLibraryEntries(&_exp_atad);

    // Initialize ATA interface which will register the HDD as a block device.
    atad_start();
    atad_inited = 1;
#endif
}

void DeviceDeinit(void)
{
    DPRINTF("%s\n", __func__);
}

int DeviceReady(void)
{
    // DPRINTF("%s\n", __func__);

    return (g_bd == NULL) ? SCECdNotReady : SCECdComplete;
}

void DeviceStop(void)
{
    DPRINTF("%s\n", __func__);

    if (g_bd != NULL)
        g_bd->stop(g_bd);
}

void DeviceFSInit(void)
{
    DPRINTF("DeviceFSInit [BDM]\n");

#ifdef USE_BDM_ATA
    lba_48bit = cdvdman_settings.hddIsLBA48;

    // TODO: there's more cdvdman init stuff after this in device-hdd.c...
    DPRINTF("DiskType=%d Layer1Start=0x%08x Lba48=%d\n", cdvdman_settings.common.media, cdvdman_settings.common.layer1_start, (u32)lba_48bit);
#endif

    DPRINTF("Waiting for device...\n");
    WaitSema(bdm_io_sema);
    DPRINTF("Waiting for device...done!\n");
    SignalSema(bdm_io_sema);
}

void DeviceLock(void)
{
    DPRINTF("%s\n", __func__);

    WaitSema(bdm_io_sema);
}

void DeviceUnmount(void)
{
    DPRINTF("%s\n", __func__);
}

int DeviceReadSectors(u64 lsn, void *buffer, unsigned int sectors)
{
    int rv = SCECdErNO;

    DPRINTF("%s(0x%08x%08x, 0x%p, %u)\n", __func__, U64_2XU32(&lsn), buffer, sectors);

    if (g_bd == NULL)
        return SCECdErTRMOPN;

    WaitSema(bdm_io_sema);
    if (bd_defrag(g_bd, cdvdman_settings.fragfile[0].frag_count, &cdvdman_settings.frags[cdvdman_settings.fragfile[0].frag_start], lsn * 4, buffer, sectors * 4) != (sectors * 4))
        rv = SCECdErREAD;
    SignalSema(bdm_io_sema);

    return rv;
}

//
// oplutils exported function, used by MCEMU
//

void bdm_readSector(u64 lba, unsigned short int nsectors, unsigned char *buffer)
{
    DPRINTF("%s\n", __func__);

    WaitSema(bdm_io_sema);
    g_bd->read(g_bd, lba, buffer, nsectors);
    SignalSema(bdm_io_sema);
}

void bdm_writeSector(u64 lba, unsigned short int nsectors, const unsigned char *buffer)
{
    DPRINTF("%s\n", __func__);

    WaitSema(bdm_io_sema);
    g_bd->write(g_bd, lba, buffer, nsectors);
    SignalSema(bdm_io_sema);
}

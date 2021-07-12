#include <errno.h>
#include <bdm.h>
#include <thevent.h>
#include <stdio.h>
#include <smapregs.h>
#include <dmacman.h>
#include <dev9.h>

#include "udpbd.h"
#include "main.h"

#define M_PRINTF(format, args...) printf("UDPBD: " format, ##args)
#define M_DEBUG                   M_PRINTF
//#define M_DEBUG(...)

#define UDPBD_MAX_RETRIES 4
static struct block_device g_udpbd;
static udpbd_pkt_t g_pkt;
static u8 g_cmdid = 0;
static vu32 g_read_done = 0;
static int g_read_cmdpkt = 0;
static volatile int bdm_connected = 0;
static void *g_buffer;
static unsigned int g_read_size;
static unsigned int g_errno = 0;


static unsigned int _udpbd_read_timeout(void *arg)
{
    g_read_size = 0;
    g_errno = 1;
#if 0
    iSetEventFlag(g_read_done, 2);
#endif
    g_read_done = 2;
    return 0;
}

extern struct SmapDriverData SmapDriverData;
static void debug_smap()
{
    USE_SMAP_EMAC3_REGS;
    USE_SMAP_REGS;
    USE_SMAP_RX_BD;
    int i;
    char bdidx;

    M_DEBUG("SMAP_R_RXFIFO_CTRL:       0x%x\n", SMAP_REG8(SMAP_R_RXFIFO_CTRL));
    M_DEBUG("SMAP_R_RXFIFO_RD_PTR:     0x%x\n", SMAP_REG16(SMAP_R_RXFIFO_RD_PTR));
    //M_DEBUG("SMAP_R_RXFIFO_SIZE:       %d\n", SMAP_REG16(SMAP_R_RXFIFO_SIZE));
    M_DEBUG("SMAP_R_RXFIFO_FRAME_CNT:  %d\n", SMAP_REG8(SMAP_R_RXFIFO_FRAME_CNT));
    //M_DEBUG("SMAP_R_RXFIFO_FRAME_DEC:  %d\n", SMAP_REG8(SMAP_R_RXFIFO_FRAME_DEC));
    M_DEBUG("SMAP_R_EMAC3_RxMODE:      0x%x\n", (unsigned int)SMAP_EMAC3_GET32(SMAP_R_EMAC3_RxMODE));
    M_DEBUG("SMAP_R_EMAC3_INTR_STAT:   0x%x\n", (unsigned int)SMAP_EMAC3_GET32(SMAP_R_EMAC3_INTR_STAT));
    M_DEBUG("SMAP_R_EMAC3_INTR_ENABLE: 0x%x\n", (unsigned int)SMAP_EMAC3_GET32(SMAP_R_EMAC3_INTR_ENABLE));
    bdidx = SmapDriverData.RxBDIndex % SMAP_BD_MAX_ENTRY;
    for (i = 0; i < SMAP_BD_MAX_ENTRY; i++) {
        if (rx_bd[i].ctrl_stat != SMAP_BD_RX_EMPTY ||
            rx_bd[i].reserved != 0 ||
            rx_bd[i].length != 0 ||
            rx_bd[i].pointer != 0 ||
            i == bdidx) {
            if (i == bdidx)
                M_DEBUG(" - rx_bd[%d]: 0x%x / 0x%x / %d / 0x%x <--\n", i, rx_bd[i].ctrl_stat, rx_bd[i].reserved, rx_bd[i].length, rx_bd[i].pointer);
            else
                M_DEBUG(" - rx_bd[%d]: 0x%x / 0x%x / %d / 0x%x\n", i, rx_bd[i].ctrl_stat, rx_bd[i].reserved, rx_bd[i].length, rx_bd[i].pointer);
        }
    }
}

//
// Block device interface
//
static int _udpbd_read(struct block_device *bd, u32 sector, void *buffer, u16 count)
{
    u32 EFBits;
    iop_sys_clock_t clock;
    USE_SMAP_EMAC3_REGS;
    USE_SMAP_REGS;

    //M_DEBUG("%s: sector=%d, count=%d\n", __func__, sector, count);

    g_cmdid++;
    g_buffer = buffer;
    g_read_size = count * 512;
    g_read_cmdpkt = 1; // First reply packet should be cmdpkt==1

    g_pkt.bd.magic = UDPBD_HEADER_MAGIC;
    g_pkt.bd.cmd = UDPBD_CMD_READ;
    g_pkt.bd.cmdid = g_cmdid;
    g_pkt.bd.cmdpkt = 0;
    g_pkt.bd.count = count;
    g_pkt.bd.par1 = sector;
    g_pkt.bd.par2 = 0;

    udp_packet_send((udp_packet_t *)&g_pkt, sizeof(udpbd_header_t) + 2);

    // Set alarm in case somthing hangs
    USec2SysClock((count + 2) * 1000000, &clock);
    SetAlarm(&clock, _udpbd_read_timeout, NULL);

    //wait for data...
    while (!g_read_done) {};
    EFBits = g_read_done;
    g_read_done = 0;
#if 0
    WaitEventFlag(g_read_done, 2 | 1, WEF_OR | WEF_CLEAR, &EFBits);
#endif

    // Cancel alarm
    CancelAlarm(_udpbd_read_timeout, NULL);

    g_buffer = NULL;
    g_read_size = 0;
    g_read_cmdpkt = 0;

    if (EFBits & 1) { // done
        return count;
    }

    M_DEBUG("%s: sector=%d, count=%d ERROR=%d\n", __func__, (int)sector, count, g_errno);
    g_errno = 0;
    debug_smap();

    SMAP_REG16(SMAP_R_INTR_CLR) = SMAP_INTR_EMAC3;
    SMAP_EMAC3_SET32(SMAP_R_EMAC3_INTR_STAT, SMAP_E3_INTR_TX_ERR_0 | SMAP_E3_INTR_SQE_ERR_0 | SMAP_E3_INTR_DEAD_0);
    SMAP_REG16(SMAP_R_INTR_CLR) = SMAP_INTR_RXEND;
    SMAP_REG16(SMAP_R_INTR_CLR) = SMAP_INTR_RXDNV;
    SMAP_REG16(SMAP_R_INTR_CLR) = SMAP_INTR_TXDNV;

    SMAP_EMAC3_SET32(SMAP_R_EMAC3_MODE0, SMAP_E3_TXMAC_ENABLE);
    SMAP_EMAC3_SET32(SMAP_R_EMAC3_MODE0, SMAP_E3_TXMAC_ENABLE | SMAP_E3_RXMAC_ENABLE);

    SMAP_REG16(SMAP_R_RXFIFO_RD_PTR) = 0;

    DelayThread(10000);

    return -EIO;
}

static int udpbd_read(struct block_device *bd, u32 sector, void *buffer, u16 count)
{
    int retries;
    u16 count_left = count;

    M_DEBUG("%s: sector=%d, count=%d\n", __func__, sector, count);

    if (bdm_connected == 0)
    {
        int result;
        if ((result = smap_init()) < 0) {
            M_DEBUG("smap: smap_init -> %d\n", result);
            return -EIO;
        }
    }

    SMAPUnpause();
    while (count_left > 0) {
        u16 count_block = count_left > UDPBD_MAX_SECTOR_READ ? UDPBD_MAX_SECTOR_READ : count_left;

        for (retries = 0; retries < UDPBD_MAX_RETRIES; retries++) {
            if (_udpbd_read(bd, sector, buffer, count_block) == count_block)
                break;
        }

        if (retries == UDPBD_MAX_RETRIES) {
            M_DEBUG("%s: too many errors, disconnecting\n", __func__);
#ifndef NO_BDM
            bdm_disconnect_bd(&g_udpbd);
#endif
            bdm_connected = 0;
            return -EIO;
        }

        count_left -= count_block;
        sector += count_block;
        buffer += count_block * 512;
    }
    SMAPPause();

    return count;
}

static int udpbd_write(struct block_device *bd, u32 sector, const void *buffer, u16 count)
{
    return count;
    //return -EIO;
}

static void udpbd_flush(struct block_device *bd)
{
    // Dummy function
}

static int udpbd_stop(struct block_device *bd)
{
    // Dummy function
    return 0;
}

//
// Public functions
//
int udpbd_init(void)
{
    iop_event_t EventFlagData;

    //M_DEBUG("%s\n", __func__);

    EventFlagData.attr = 0;
    EventFlagData.option = 0;
    EventFlagData.bits = 0;
#if 0
    if (g_read_done <= 0)
        g_read_done = CreateEventFlag(&EventFlagData);
#endif
    g_read_done = 0;

    udp_packet_init((udp_packet_t *)&g_pkt, UDPBD_PORT);

    // Broadcast request for block device information
    g_pkt.bd.magic = UDPBD_HEADER_MAGIC;
    g_pkt.bd.cmd = UDPBD_CMD_INFO;
    g_pkt.bd.cmdid = g_cmdid;
    g_pkt.bd.cmdpkt = 0;
    g_pkt.bd.count = 0;
    g_pkt.bd.par1 = 0;
    g_pkt.bd.par2 = 0;
    udp_packet_send((udp_packet_t *)&g_pkt, sizeof(udpbd_header_t) + 2);

    while (!bdm_connected) {}
#ifndef NO_BDM
    debug_smap();
#endif

    return 0;
}

int udpbd_register_bd(void)
{
    g_udpbd.name = "udp";
    g_udpbd.devNr = 0;
    g_udpbd.parNr = 0;
    g_udpbd.sectorOffset = 0;
    g_udpbd.priv = NULL;
    g_udpbd.read = udpbd_read;
    g_udpbd.write = udpbd_write;
    g_udpbd.flush = udpbd_flush;
    g_udpbd.stop = udpbd_stop;
    g_udpbd.sectorSize = 512;
    g_udpbd.sectorCount = 8388608;
#ifndef NO_BDM
    bdm_connect_bd(&g_udpbd);
#endif
    return 0;
}

static int UDPBDCopyFromFIFOWithDMA(volatile u8 *smap_regbase, void *buffer, int blocks)
{
    USE_SPD_REGS;
    u16 OldDMACtrl;

    // Attempt to steal the DMA channel from the DEV9 module.
    {
        OldDMACtrl = SPD_REG16(SPD_R_DMA_CTRL);

        while (dmac_ch_get_chcr(IOP_DMAC_DEV9) & DMAC_CHCR_TR) {
        }

        SPD_REG16(SPD_R_DMA_CTRL) = 7; // SPEED revision 17 (ES2) and above only.

        SMAP_REG16(SMAP_R_RXFIFO_SIZE) = blocks;
        SMAP_REG8(SMAP_R_RXFIFO_CTRL) = SMAP_RXFIFO_DMAEN;

        // Transfer in 128-byte (32x4) blocks
        // Transfer in 0x80-byte (0x20 by 0x4) blocks
        dmac_request(IOP_DMAC_DEV9, buffer, 0x10, blocks, DMAC_TO_MEM);
        dmac_transfer(IOP_DMAC_DEV9);

        /* Wait for DMA to complete. Do not use a semaphore as thread switching hurts throughput greatly.  */
        while (dmac_ch_get_chcr(IOP_DMAC_DEV9) & DMAC_CHCR_TR) {
        }
        while (SMAP_REG8(SMAP_R_RXFIFO_CTRL) & SMAP_RXFIFO_DMAEN) {
        };

        SPD_REG16(SPD_R_DMA_CTRL) = OldDMACtrl;
    }

    return blocks;
}

void udpbd_rx(u16 pointer)
{
    USE_SMAP_REGS;
    udpbd_header_t hdr;

    hdr.cmd = 0;
    //((u32 *)&hdr)[0] = UDPBD_HEADER_MAGIC;
    ((u32 *)&hdr)[1] = SMAP_REG32(SMAP_R_RXFIFO_DATA); // CMD
    ((u32 *)&hdr)[2] = SMAP_REG32(SMAP_R_RXFIFO_DATA); // PAR1
    ((u32 *)&hdr)[3] = SMAP_REG32(SMAP_R_RXFIFO_DATA); // PAR2

    switch (hdr.cmd) {
        case UDPBD_CMD_INFO:
            if (bdm_connected == 0) {
                g_udpbd.sectorSize = hdr.par1;
                g_udpbd.sectorCount = hdr.par2;
                SMAPPause();
                bdm_connected = 1;
            }
            break;
        case UDPBD_CMD_READ:
            if ((g_buffer != NULL) && (g_read_size >= hdr.par1) && (g_cmdid == hdr.cmdid)) {
                // Validate packet order
                if (hdr.cmdpkt != g_read_cmdpkt) {
                    // Error, wakeup caller
                    g_read_size = 0;
                    g_errno = 2;
#if 0
                    iSetEventFlag(g_read_done, 2);
#endif
                    g_read_done = 2;
                    break;
                }
                g_read_cmdpkt++;

                // Validate packet data size
                if ((hdr.par1 > UDPBD_MAX_DATA) || (hdr.par1 & 127)) {
                    // Error, wakeup caller
                    g_read_size = 0;
                    g_errno = 3;
#if 0
                    iSetEventFlag(g_read_done, 2);
#endif
                    g_read_done = 2;
                    break;
                }

                // Directly DMA the packet data into the user buffer
                UDPBDCopyFromFIFOWithDMA(smap_regbase, (u8 *)g_buffer + ((g_read_cmdpkt - 2) * UDPBD_MAX_DATA), (hdr.par1 / 64));
                //dev9DmaTransfer(1, (u8*)g_buffer + ((hdr.cmdpkt-1) * UDPBD_MAX_DATA), (hdr.par1 / 128)<<16|0x20, DMAC_TO_MEM);

                g_read_size -= hdr.par1;
                if (g_read_size == 0) {
                    // Done, wakeup caller
#if 0
                    iSetEventFlag(g_read_done, 1);
#endif
                    g_read_done = 1;
                    break;
                }
            }
            break;
        case UDPBD_CMD_WRITE:
            break;
    };
}
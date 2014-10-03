/*
  Copyright 2009, Ifcaro, jimmikaelkael
  Licenced under Academic Free License version 3.0
  Review OpenUsbLd README & LICENSE files for further details.
*/

#include "ee_core.h"
#include "util.h"

#define ALL_MODE		-1

typedef struct {
	u32 addr;
	u32 val;
	u32 check;
} game_patch_t;

typedef struct {
	char *game;
	int mode;
	game_patch_t patch;
} patchlist_t;

//Keep patch codes unique!
#define PATCH_GENERIC_NIS		0xDEADBEE0
#define PATCH_GENERIC_CAPCOM		0xBABECAFE
#define PATCH_GENERIC_AC9B		0xDEADBEE1
#define PATCH_GENERIC_SLOW_READS	0xDEADBEE2
#define PATCH_SDF_MACROSS		0x00065405
#define PATCH_SRW_IMPACT		0x0021e808

static const patchlist_t patch_list[] = {
	{ "SLES_524.58", USB_MODE, { PATCH_GENERIC_NIS,		0x00000000, 0x00000000 }}, // Disgaea Hour of Darkness PAL - disable cdvd timeout stuff
	{ "SLUS_206.66", USB_MODE, { PATCH_GENERIC_NIS,		0x00000000, 0x00000000 }}, // Disgaea Hour of Darkness NTSC U - disable cdvd timeout stuff
	{ "SLPS_202.51", USB_MODE, { PATCH_GENERIC_NIS,		0x00000000, 0x00000000 }}, // Makai Senki Disgaea NTSC J - disable cdvd timeout stuff
	{ "SLPS_202.50", USB_MODE, { PATCH_GENERIC_NIS,		0x00000000, 0x00000000 }}, // Makai Senki Disgaea (limited edition) NTSC J - disable cdvd timeout stuff
	{ "SLPS_731.03", USB_MODE, { PATCH_GENERIC_NIS,		0x00000000, 0x00000000 }}, // Makai Senki Disgaea (PlayStation2 the Best) NTSC J - disable cdvd timeout stuff
	{ "SLES_529.51", USB_MODE, { PATCH_GENERIC_NIS,		0x00000000, 0x00000000 }}, // Phantom Brave PAL - disable cdvd timeout stuff
	{ "SLUS_209.55", USB_MODE, { PATCH_GENERIC_NIS,		0x00000000, 0x00000000 }}, // Phantom Brave NTSC U - disable cdvd timeout stuff
	{ "SLPS_203.45", USB_MODE, { PATCH_GENERIC_NIS,		0x00000000, 0x00000000 }}, // Phantom Brave NTSC J - disable cdvd timeout stuff
	{ "SLPS_203.44", USB_MODE, { PATCH_GENERIC_NIS,		0x00000000, 0x00000000 }}, // Phantom Brave (limited edition) NTSC J - disable cdvd timeout stuff
	{ "SLPS_731.08", USB_MODE, { PATCH_GENERIC_NIS,		0x00000000, 0x00000000 }}, // Phantom Brave: 2-shuume Hajime Mashita (PlayStation 2 the Best) NTSC J - disable cdvd timeout stuff
	{ "SLUS_213.17", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00149210, 0x00000000 }}, // SFA anthology US
	{ "SLES_540.85", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00148db0, 0x00000000 }}, // SFA anthology EUR
	{ "SLPM_664.09", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00149210, 0x00000000 }}, // SFZ Generation JP
	{ "SLPM_659.98", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00146fd0, 0x00000000 }}, // Vampire: Darkstakers collection JP
	{ "SLUS_212.00", USB_MODE, { PATCH_GENERIC_AC9B,	0x00000000, 0x00000000 }}, // Armored Core Nine Breaker NTSC U - skip failing case on binding a RPC server
	{ "SLES_538.19", USB_MODE, { PATCH_GENERIC_AC9B,	0x00000000, 0x00000000 }}, // Armored Core Nine Breaker PAL - skip failing case on binding a RPC server
	{ "SLPS_254.08", USB_MODE, { PATCH_GENERIC_AC9B,	0x00000000, 0x00000000 }}, // Armored Core Nine Breaker NTSC J - skip failing case on binding a RPC server
	{ "SLUS_210.05", ALL_MODE, { PATCH_GENERIC_SLOW_READS,	0x00100000, 0x001ac514 }}, // Kingdom Hearts 2 US - [Gummi mission freezing fix (check addr is where to patch,
	{ "SLES_541.14", ALL_MODE, { PATCH_GENERIC_SLOW_READS,	0x00100000, 0x001ac60c }}, // Kingdom Hearts 2 UK - val is the amount of delay cycles)]
	{ "SLES_542.32", ALL_MODE, { PATCH_GENERIC_SLOW_READS,	0x00100000, 0x001ac60c }}, // Kingdom Hearts 2 FR
	{ "SLES_542.33", ALL_MODE, { PATCH_GENERIC_SLOW_READS,	0x00100000, 0x001ac60c }}, // Kingdom Hearts 2 DE
	{ "SLES_542.34", ALL_MODE, { PATCH_GENERIC_SLOW_READS,	0x00100000, 0x001ac60c }}, // Kingdom Hearts 2 IT
	{ "SLES_542.35", ALL_MODE, { PATCH_GENERIC_SLOW_READS,	0x00100000, 0x001ac60c }}, // Kingdom Hearts 2 ES
	{ "SLPM_662.33", ALL_MODE, { PATCH_GENERIC_SLOW_READS,	0x00100000, 0x001ac44c }}, // Kingdom Hearts 2 JPN
	{ "SLPM_666.75", ALL_MODE, { PATCH_GENERIC_SLOW_READS,	0x00149210, 0x001adf64 }}, // Kingdom Hearts 2 Final Mix JPN
	{ "SLUS_212.87", ETH_MODE, { PATCH_GENERIC_SLOW_READS,	0x000c0000, 0x006cd15c }}, // Prince of Persia: The Two Thrones NTSC U - slow down cdvd reads
	{ "SLUS_212.87", HDD_MODE, { PATCH_GENERIC_SLOW_READS,	0x00040000, 0x006cd15c }}, // Prince of Persia: The Two Thrones NTSC U - slow down cdvd reads
	{ "SLES_537.77", ETH_MODE, { PATCH_GENERIC_SLOW_READS,	0x000c0000, 0x006cd6dc }}, // Prince of Persia: The Two Thrones PAL - slow down cdvd reads
	{ "SLES_537.77", HDD_MODE, { PATCH_GENERIC_SLOW_READS,	0x00040000, 0x006cd6dc }}, // Prince of Persia: The Two Thrones PAL - slow down cdvd reads
	{ "SLUS_210.22", ETH_MODE, { PATCH_GENERIC_SLOW_READS,	0x000c0000, 0x0060f42c }}, // Prince of Persia: Warrior Within NTSC U - slow down cdvd reads
	{ "SLUS_210.22", HDD_MODE, { PATCH_GENERIC_SLOW_READS,	0x00040000, 0x0060f42c }}, // Prince of Persia: Warrior Within NTSC U - slow down cdvd reads
	{ "SLES_528.22", ETH_MODE, { PATCH_GENERIC_SLOW_READS,	0x000c0000, 0x0060f4dc }}, // Prince of Persia: Warrior Within PAL - slow down cdvd reads
	{ "SLES_528.22", HDD_MODE, { PATCH_GENERIC_SLOW_READS,	0x00040000, 0x0060f4dc }}, // Prince of Persia: Warrior Within PAL - slow down cdvd reads
	{ "SLUS_214.32", ALL_MODE, { PATCH_GENERIC_SLOW_READS,	0x00080000, 0x002baf34 }}, // NRA Gun Club NTSC U
	{ "SLPM_654.05", HDD_MODE, { PATCH_SDF_MACROSS,		0x00200000, 0x00249b84 }}, // Super Dimensional Fortress Macross JPN
	/*
	{ "SCES_525.82", ALL_MODE, { 0xdeadbee3,		0x00000000, 0x00000000 }}, // EveryBody's Golf PAL
	{ "SCUS_974.01", ALL_MODE, { 0xdeadbee3,		0x00000000, 0x00000000 }}, // Hot Shots Golf FORE! NTSC U
	{ "SCUS_975.15", ALL_MODE, { 0xdeadbee3,		0x00000000, 0x00000000 }}, // Hot Shots Golf FORE! (GH) NTSC U
	{ "SCUS_976.10", ALL_MODE, { 0xdeadbee3,		0x00000000, 0x00000000 }}, // Hot Shots Tennis NTSC U
	{ "SLUS_209.51", ALL_MODE, { 0xdeadbee3,		0x00000000, 0x00000000 }}, // Viewtiful Joe NTSC U
	{ "SLES_526.78", ALL_MODE, { 0xdeadbee3,		0x00000000, 0x00000000 }}, // Viewtiful Joe PAL
	{ "SLPM_656.99", ALL_MODE, { 0xdeadbee3,		0x00000000, 0x00000000 }}, // Viewtiful Joe NTSC J
	{ "SCUS_973.30", ALL_MODE, { 0xdeadbee3,		0x00000000, 0x00000000 }}, // Jak 3 US
	{ "SCUS_975.16", ALL_MODE, { 0xdeadbee3,		0x00000000, 0x00000000 }}, // Jak 3 US (GH)
	{ "SCES_524.60", ALL_MODE, { 0xdeadbee3,		0x00000000, 0x00000000 }}, // Jak 3 PAL
	{ "SCKA_200.40", ALL_MODE, { 0xdeadbee3,		0x00000000, 0x00000000 }}, // Jak 3 NTSC K
	{ "SCUS_974.29", ALL_MODE, { 0xdeadbee3,		0x00000000, 0x00000000 }}, // Jak X NTSC U
	{ "SCES_532.86", ALL_MODE, { 0xdeadbee3,		0x00000000, 0x00000000 }}, // Jak X PAL
	{ "SLES_820.28", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00000000, 0x00000000 }}, // SOTET PAL Disc1
	{ "SLES_820.29", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00000000, 0x00000000 }}, // SOTET PAL Disc2
	{ "SLUS_204.88", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00000000, 0x00000000 }}, // SOTET NTSC-U Disc1
	{ "SLUS_208.91", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00000000, 0x00000000 }}, // SOTET NTSC-U Disc2
	{ "SCPS_550.19", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00000000, 0x00000000 }}, // SOTET NTSC-J
	{ "SLPM_664.78", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00000000, 0x00000000 }}, // SOTET NTSC-J (Ultimate Hits) Disc1
	{ "SLPM_664.79", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00000000, 0x00000000 }}, // SOTET NTSC-J (Ultimate Hits) Disc2
	{ "SLPM_652.09", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00000000, 0x00000000 }}, // SOTET NTSC-J (Limited Edition)
	{ "SLPM_654.38", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00000000, 0x00000000 }}, // SOTET NTSC-J (Director's Cut) Disc1
	{ "SLPM_654.39", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00000000, 0x00000000 }}, // SOTET NTSC-J (Director's Cut) Disc2
	{ "SCAJ_200.70", ALL_MODE, { PATCH_GENERIC_CAPCOM,	0x00000000, 0x00000000 }}, // SOTET NTSC-? (Director's Cut)
	*/
	{ "SLUS_202.30", ALL_MODE, { 0x00132d14,		0x10000018, 0x0c046744 }}, // Max Payne NTSC U - skip IOP reset before to exec demo elfs
	{ "SLES_503.25", ALL_MODE, { 0x00132ce4,		0x10000018, 0x0c046744 }}, // Max Payne PAL - skip IOP reset before to exec demo elfs
	{ "SLUS_204.40", ALL_MODE, { 0x0021bb00,		0x03e00008, 0x27bdff90 }}, // Kya: Dark Lineage NTSC U - disable game debug prints
	{ "SLES_514.73", ALL_MODE, { 0x0021bd10,		0x03e00008, 0x27bdff90 }}, // Kya: Dark Lineage PAL - disable game debug prints
	{ "SLUS_204.96", ALL_MODE, { 0x00104900,		0x03e00008, 0x27bdff90 }}, // V-Rally 3 NTSC U - disable game debug prints
	{ "SLES_507.25", ALL_MODE, { 0x00104518,		0x03e00008, 0x27bdff70 }}, // V-Rally 3 PAL - disable game debug prints
	{ "SLUS_201.99", ALL_MODE, { 0x0012a6d0,		0x24020001, 0x0c045e0a }}, // Shaun Palmer's Pro Snowboarder NTSC U
	{ "SLUS_201.99", ALL_MODE, { 0x0013c55c,		0x10000012, 0x04400012 }}, // Shaun Palmer's Pro Snowboarder NTSC U
	{ "SLES_553.46", ALL_MODE, { 0x0035414C,		0x2402FFFF, 0x0C0EE74E }}, // Rugby League 2: World Cup Edition PAL
	{ "SLPS_251.04", ALL_MODE, { PATCH_SRW_IMPACT,		0x00000000, 0x00000000 }}, // Super Robot Wars IMPACT
	{ NULL,                 0, { 0x00000000,		0x00000000, 0x00000000 }}  // terminater
};

#define JAL(addr)	(0x0c000000 | (((addr) & 0x03ffffff) >> 2))
#define JMP(addr)	(0x08000000 | (0x3ffffff&((addr)>>2)))
#define FNADDR(jal)	(((jal) & 0x03ffffff) << 2)

static int (*cdRead)(u32 lsn, u32 nsectors, void *buf, int *mode);
static unsigned int g_delay_cycles;

// Nippon Ichi Sofwtare games generic patch to disable cdvd timeout
static void NIS_generic_patches(void)
{
	static const u32 NIScdtimeoutpattern[] = {
		0x3c010000,
		0x8c230000,
		0x24630001,
		0x3c010000,
		0xac230000,
		0x3c010000,
		0x8c230000,
		0x2861037b,
		0x14200000,
		0x00000000
	};
	static const u32 NIScdtimeoutpattern_mask[] = {
		0xffff0000,
		0xffff0000,
		0xffffffff,
		0xffff0000,
		0xffff0000,
		0xffff0000,
		0xffff0000,
		0xffffffff,
		0xffff0000,
		0xffffffff
	};
	u32 *ptr;

	ptr = find_pattern_with_mask((u32 *)0x100000, 0x01e00000, NIScdtimeoutpattern, NIScdtimeoutpattern_mask, 0x28);
	if (ptr) {
		u16 jmp = _lw((u32)ptr+32) & 0xffff;
		_sw(0x10000000|jmp, (u32)ptr+32);
	}
}

// Armored Core 9 Breaker generic USB patch
static void AC9B_generic_patches(void)
{
	static u32 AC9Bpattern[] = {
		0x8e450000,
		0x0220202d,
		0x0c000000,
		0x0000302d,
		0x04410003,
		0x00000000,
		0x10000005,
		0x2402ffff,
		0x8e020000,
		0x1040fff6
	};
	static const u32 AC9Bpattern_mask[] = {
		0xffffffff,
		0xffffffff,
		0xfc000000,
		0xffffffff,
		0xffffffff,
		0xffffffff,
		0xffffffff,
		0xffffffff,
		0xffffffff,
		0xffffffff
	};
	u32 *ptr;

	ptr = find_pattern_with_mask((u32 *)0x100000, 0x01e00000, AC9Bpattern, AC9Bpattern_mask, 0x28);
	if (ptr)
		_sw(0, (u32)ptr+36);
}

static int delayed_cdRead(u32 lsn, u32 nsectors, void *buf, int *mode)
{
	int r;
	unsigned int count;

	r = cdRead(lsn, nsectors, buf, mode);
	count = g_delay_cycles;
	while(count--)
		asm("nop\nnop\nnop\nnop");

	return r;
}

static void generic_delayed_cdRead_patches(u32 patch_addr, u32 delay_cycles)
{
	// set configureable delay cycles
	g_delay_cycles = delay_cycles;

	// get original cdRead() pointer
	cdRead = (void *)FNADDR(_lw(patch_addr));

	// overwrite with a JAL to our delayed_cdRead function
	_sw(JAL((u32)delayed_cdRead), patch_addr);
}

static int (*capcom_lmb)(void *modpack_addr, int mod_index, int mod_argc, char **mod_argv);

static void apply_capcom_protection_patch(void *modpack_addr, int mod_index, int mod_argc, char **mod_argv)
{
	u32 iop_addr = _lw((u32)modpack_addr + (mod_index << 3) + 8);
	u32 opcode = 0x10000025;
	SyncDCache((void*)opcode, (void *)((unsigned int)&opcode + sizeof(opcode)));	//Can this be replaced with a write to the uncached segment? I don't understand why the D-cache is a problem when dealing with this EE->IOP RAM window since it uses memcpy(), so I'll leave it with SyncDCache() as usual.
	smem_write((void *)(iop_addr+0x270), (void *)&opcode, sizeof(opcode));

	capcom_lmb(modpack_addr, mod_index, mod_argc, mod_argv);
}

static void generic_capcom_protection_patches(u32 patch_addr)
{
	capcom_lmb = (void *)FNADDR(_lw(patch_addr));
	_sw(JAL((u32)apply_capcom_protection_patch), patch_addr);
}

static void Invoke_CRSGUI_Start(void){
	int (*pCRSGUI_Start)(int arg1, int arg2)=(void*)0x003054b0;

	pCRSGUI_Start(*(int*)0x0078f79c, 0);
}

static void SDF_Macross_patch(void){
	/*	Choujikuu Yousai Macross appears to have a rather large problem with it: it appears to use its GUI before initialization is completed.
		I did not attempt to figure out whether it's really a timing problem (whereby this happens before initialization is completed by another thread... if there is one),
		or if its normal functionality was the result of pure luck that SEGA had.

		The problems that it has are quite evident when this game is run within PCSX2. I still do not know why DECI2 does not detect the TLB exception when it
		dereferences NULL pointers, but it might have something to do with the game accessing the debug registers (PCSX2's logs indicate that). */

	_sw(JMP((unsigned int)&Invoke_CRSGUI_Start), 0x001f8520);
}

extern void SRWI_IncrementCntrlFlag(void);

static void SRWI_IMPACT_patches(void){
	//Phase 1	- Replace all segments of code that increment cntrl_flag with a multithread-safe implementation.
	//In cdvCallBack()
	_sw(JAL((unsigned int)&SRWI_IncrementCntrlFlag), 0x0021e840);
	_sw(0x00000000, 0x0021e84c);
	_sw(0x00000000, 0x0021e854);
	//In cdvMain()
	_sw(0x00000000, 0x00220ac8);
	_sw(JAL((unsigned int)&SRWI_IncrementCntrlFlag), 0x00220ad0);
	_sw(0x00000000, 0x00220ad8);
	_sw(JAL((unsigned int)&SRWI_IncrementCntrlFlag), 0x00220b20);
	_sw(0x00000000, 0x00220b28);
	_sw(0x00000000, 0x00220b30);
	_sw(JAL((unsigned int)&SRWI_IncrementCntrlFlag), 0x00220ba0);
	_sw(0x00000000, 0x00220ba8);

	/* Phase 2
		sceCdError() will be polled continuously until it succeeds in retrieving the CD/DVD drive status.
		However, the callback thread has a higher priority than the main thread
		and this might result in a freeze because the main thread wouldn't ever release the libcdvd semaphore, and so calls to sceCdError() by the callback thread wouldn't succeed.
		This problem occurs more frequently than the one addressed above.

		Since the PlayStation 2 EE uses non-preemptive multitasking, we can solve this problem by lowering the callback thread's priority th below the main thread.
		The problem is solved because the main thread can then interrupt the callback thread until it has completed its tasks.	*/
	//In cdvCallBack()
	_sw(0x24040060, 0x0021e944);	//addiu $a0, $zero, 0x60 (Set the CD/DVD callback thread's priority to 0x60)
}

void apply_patches(void)
{
	const patchlist_t *p;

	// if there are patches matching game name/mode then fill the patch table
	for (p = patch_list; p->game; p++) {
		if ((!_strcmp(GameID, p->game)) && ((p->mode == ALL_MODE) || (GameMode == p->mode))) {
			switch(p->patch.addr){
				case PATCH_GENERIC_NIS:
					NIS_generic_patches();
					break;
				case PATCH_GENERIC_AC9B:
					AC9B_generic_patches();
					break;
				case PATCH_GENERIC_SLOW_READS:
					generic_delayed_cdRead_patches(p->patch.check, p->patch.val); // slow reads generic patch
					break;
				case PATCH_SDF_MACROSS:
					SDF_Macross_patch();
					break;
				case PATCH_GENERIC_CAPCOM:
					generic_capcom_protection_patches(p->patch.val); // Capcom anti cdvd emulator protection patch
					break;
				case PATCH_SRW_IMPACT:
					SRWI_IMPACT_patches();
					break;
				default:	// Single-value patches
					if (_lw(p->patch.addr) == p->patch.check)
						_sw(p->patch.val, p->patch.addr);
			}
		}
	}
}

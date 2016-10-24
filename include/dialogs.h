#ifndef __DIALOGS_H
#define __DIALOGS_H

#include "include/dia.h"

enum UI_ITEMS {
    UIID_BTN_CANCEL = 0,
    UIID_BTN_OK,

    UICFG_THEME = 10,
    UICFG_LANG,
    UICFG_SCROLL,
    UICFG_BGCOL,
    UICFG_UICOL,
    UICFG_TXTCOL,
    UICFG_SELCOL,
    UICFG_AUTOSORT,
    UICFG_COVERART,
    UICFG_WIDESCREEN,
    UICFG_AUTOREFRESH,
    UICFG_VMODE,
    UICFG_INFOPAGE,

    CFG_DEBUG,
    CFG_PS2LOGO,
    CFG_EXITTO,
    CFG_DEFDEVICE,
    CFG_USBMODE,
    CFG_HDDMODE,
    CFG_ETHMODE,
    CFG_APPMODE,
    CFG_CHECKUSBFRAG,
    CFG_LASTPLAYED,
    CFG_LBL_AUTOSTARTLAST,
    CFG_AUTOSTARTLAST,
    CFG_SELECTBUTTON,
    CFG_ENWRITEOP,
    CFG_USBPREFIX,
    CFG_ETHPREFIX,
    CFG_HDDSPINDOWN,

    NETCFG_SHOW_ADVANCED_OPTS,
    NETCFG_PS2_IP_ADDR_TYPE,
    NETCFG_PS2_IP_ADDR_0,
    NETCFG_PS2_IP_ADDR_1,
    NETCFG_PS2_IP_ADDR_2,
    NETCFG_PS2_IP_ADDR_3,
    NETCFG_PS2_NETMASK_0,
    NETCFG_PS2_NETMASK_1,
    NETCFG_PS2_NETMASK_2,
    NETCFG_PS2_NETMASK_3,
    NETCFG_PS2_GATEWAY_0,
    NETCFG_PS2_GATEWAY_1,
    NETCFG_PS2_GATEWAY_2,
    NETCFG_PS2_GATEWAY_3,
    NETCFG_PS2_DNS_0,
    NETCFG_PS2_DNS_1,
    NETCFG_PS2_DNS_2,
    NETCFG_PS2_DNS_3,
    NETCFG_SHARE_ADDR_TYPE,
    NETCFG_SHARE_NB_ADDR,
    NETCFG_SHARE_IP_ADDR_0,
    NETCFG_SHARE_IP_ADDR_1,
    NETCFG_SHARE_IP_ADDR_2,
    NETCFG_SHARE_IP_ADDR_3,
    NETCFG_SHARE_IP_ADDR_DOT_0,
    NETCFG_SHARE_IP_ADDR_DOT_1,
    NETCFG_SHARE_IP_ADDR_DOT_2,
    NETCFG_SHARE_PORT,
    NETCFG_SHARE_NAME,
    NETCFG_SHARE_USERNAME,
    NETCFG_SHARE_PASSWORD,
    NETCFG_ETHOPMODE,
    NETCFG_RECONNECT,
    NETCFG_OK,

    CHTCFG_CHEATCFG,
    CHTCFG_ENABLECHEAT,
    CHTCFG_CHEATMODE,

    GSMCFG_GSCONFIG,
    GSMCFG_ENABLEGSM,
    GSMCFG_GSMVMODE,
    GSMCFG_GSMXOFFSET,
    GSMCFG_GSMYOFFSET,

    COMPAT_DMA = 100,
    COMPAT_ALTSTARTUP,
    COMPAT_GAME,
    COMPAT_GAMEID,
    COMPAT_SAVE,
    COMPAT_STATUS,
    COMPAT_DL_DEFAULTS,
    COMPAT_TEST,
    COMPAT_REMOVE,

    COMPAT_LOADFROMDISC_ID,
#ifdef VMC
    COMPAT_VMC1_ACTION_ID,
    COMPAT_VMC2_ACTION_ID,
    COMPAT_VMC1_DEFINE_ID,
    COMPAT_VMC2_DEFINE_ID,

    VMC_NAME,
    VMC_SIZE,
    VMC_BUTTON_CREATE,
    VMC_BUTTON_DELETE,
    VMC_STATUS,
    VMC_PROGRESS,
    VMC_REFRESH,
#endif

    NETUPD_OPT_UPD_ALL_LBL,
    NETUPD_OPT_UPD_ALL,
    NETUPD_PROGRESS_LBL,
    NETUPD_PROGRESS_PERC_LBL,
    NETUPD_PROGRESS,
    NETUPD_BTN_START,
    NETUPD_BTN_CANCEL,

    COMPAT_MODE_BASE = 200,
};

#define COMPAT_NOEXIT 0x70000000
#define COMPAT_GSMCONFIG (GSMCFG_GSCONFIG | COMPAT_NOEXIT)
#define COMPAT_CHEATCONFIG (CHTCFG_CHEATCFG | COMPAT_NOEXIT)
#define COMPAT_LOADFROMDISC (COMPAT_LOADFROMDISC_ID | COMPAT_NOEXIT)
#ifdef VMC
#define COMPAT_VMC1_ACTION (COMPAT_VMC1_ACTION_ID | COMPAT_NOEXIT)
#define COMPAT_VMC2_ACTION (COMPAT_VMC2_ACTION_ID | COMPAT_NOEXIT)
#define COMPAT_VMC1_DEFINE (COMPAT_VMC1_DEFINE_ID | COMPAT_NOEXIT)
#define COMPAT_VMC2_DEFINE (COMPAT_VMC2_DEFINE_ID | COMPAT_NOEXIT)
#endif

extern struct UIItem diaNetConfig[];
extern struct UIItem diaCompatConfig[];
extern struct UIItem diaUIConfig[];
extern struct UIItem diaGSConfig[];
extern struct UIItem diaCheatConfig[];

extern struct UIItem diaConfig[];
extern struct UIItem diaAbout[];
#ifdef VMC
extern struct UIItem diaVMC[];
#endif
extern struct UIItem diaNetCompatUpdate[];

#endif

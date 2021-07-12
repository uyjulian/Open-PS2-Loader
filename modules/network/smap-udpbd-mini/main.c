#include <errno.h>
#include <stdio.h>
#include <loadcore.h>
#include <thbase.h>
#include <irx.h>

#include "main.h"
#include "xfer.h"
#include "udpbd.h"

IRX_ID("SMAP_driver", 0x2, 0x19);

int _start(int argc, char *argv[])
{
    int result;

    /*	This code was present in SMAP, but cannot be implemented with the default IOP kernel due to MODLOAD missing these functions.
	It may be necessary to prevent SMAP from linking with an old DEV9 module.
	if((ModuleID=SearchModuleByName("dev9"))<0){
		sceInetPrintf("smap: dev9 module not found\n");
		return MODULE_NO_RESIDENT_END;
	}
	if(ReferModuleStatus(ModuleID, &ModStatus)<0){
		sceInetPrintf("smap: can't get dev9 module status\n");
		return MODULE_NO_RESIDENT_END;
	}

	if(ModStatus.version<0x204){
		sceInetPrintf("smap: dev9 module version must be 2.4 or later\n");
		return MODULE_NO_RESIDENT_END;
	} */

#if 0
    if ((result = smap_init(argc, argv)) < 0) {
        DEBUG_PRINTF("smap: smap_init -> %d\n", result);
        return MODULE_NO_RESIDENT_END;
    }
#endif
    udpbd_register_bd();

    return MODULE_RESIDENT_END;
}

#ifndef XFER_H
#define XFER_H


#include "main.h"


void xfer_init(void);
int SMAPSendPacket(const void *buf, unsigned int size);
int HandleRxIntr(struct SmapDriverData *SmapDrivPrivData);


#endif

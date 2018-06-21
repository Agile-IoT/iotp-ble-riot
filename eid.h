#ifndef EID_H
#define EID_H

#include <stdint.h>

void printHex(uint8_t *hex, int offset);
int generateEID(char *ikString, int scaler, int beacon_time_seconds, uint8_t *eid);
#endif

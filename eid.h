#ifndef EID_H
#define EID_H

#include <stdint.h>
#include "periph/flashpage.h"

#define DEFAULT_IK_SIZE 16
#define DEFAULT_TS_SIZE 8
#define OTP_FLASH_PAGE (FLASHPAGE_NUMOF - 1)


static uint8_t page_mem[FLASHPAGE_SIZE] = {0};

void printHex(uint8_t *hex, int offset);
int hex_eid_generator(uint8_t *ik, int scaler, int beacon_time_seconds, uint8_t *eid);
int write_default_ik(void);
int next_otp(uint8_t *otp_value);
int otp_init(void);
uint8_t *parseIk(char *ikString);
uint16_t read_otp_counter(void);
int write_otp_counter(uint16_t value);
int write_otp_key(uint8_t *key);


#endif

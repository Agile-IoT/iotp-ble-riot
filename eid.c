/*
 * eid.c
 *
 *  Created on: June 18, 2018
 *      Author: Eduard Brehm
 *              Emmanuel Baccelli (Copyright (C) 2018 Inria)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "include/crypto/ciphers.h"
#include "crypto/ciphers.c"
#include "crypto/aes.c"
#include "crypto/modes/ecb.c"
#include "eid.h"


char default_ik[DEFAULT_IK_SIZE] = {0xe2, 0xe1, 0x2c, 0x22, 0x81, 0xcd, 0xf3, 0xd3, 0x50, 0xa3, 0x4d, 0xe4, 0xd5, 0xf5, 0x66, 0x12};
char canary[4] = {0xe2, 0xe1, 0x2c, 0x22};
char dummy_flash_data[1]="0";



int modulus(double a, double b) {
	int result = (int) ( a / b );
	return a - ( result ) * b;
}

void printHex(uint8_t *hex, int offset) {
	for(int i = 0; i < AES_BLOCK_SIZE; ++i) {
		if(offset > 0 && i >= offset) {
			break;
		} else {
			printf("%02x ", hex[i]);
		}
	};
	printf("\n");
}

int hex_eid_generator(uint8_t *ik, int scaler, int beacon_time_seconds, uint8_t *eid) {
	printf("Key: \n");
	printHex(ik, 0);
	printf("Counter value: %u\n", beacon_time_seconds);

	uint8_t tkdata[AES_BLOCK_SIZE] = {
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //"\x00" * 11
			0xff,	//"\xFF"
			0x00, 0x00, //"\x00" * 2
			modulus((double) (beacon_time_seconds / (pow(2, 24))), 256), //chr((beacon_time_seconds / (2 ** 24)) % 256)
			modulus((double) (beacon_time_seconds / (pow(2, 16))), 256) //chr((beacon_time_seconds / (2 ** 16)) % 256)
	};
	// printf("Temporary Key Data: ");
	// printHex(tkdata, 0);

	cipher_t tkdataCipher;
	uint8_t tk[AES_KEY_SIZE] = {0};
	cipher_init(&tkdataCipher, CIPHER_AES_128, ik, AES_KEY_SIZE);
	cipher_encrypt(&tkdataCipher, tkdata, tk);
	// printf("Temporary Key: ");
	// printHex(tk, 0);

	beacon_time_seconds = (beacon_time_seconds / pow(2, 0)) * pow(2, 0);
	uint8_t eiddata[AES_BLOCK_SIZE] = {
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //"\x00" * 11
			scaler,
			modulus((double) (beacon_time_seconds / (pow(2, 24))), 256),
			modulus((double) (beacon_time_seconds / (pow(2, 16))), 256),
			modulus((double) (beacon_time_seconds / (pow(2, 8))), 256),
			modulus((double) (beacon_time_seconds / (pow(2, 0))), 256)
	};
	// printf("Ephermal Id Data: ");
	// printHex(eiddata, 0);

	cipher_t tkEidCipher;
	cipher_init(&tkEidCipher, CIPHER_AES_128, tk, AES_KEY_SIZE);
	cipher_encrypt(&tkEidCipher, eiddata, eid);
	// printf("Ephermal Id: ");
	// printHex(eid, 0);
	printf("Generated OTP: ");
    printHex(eid, 8);

	return 0;
}

int canarytest(void) {
    int rc = 0;
    for (int i = 0; i < 4; i++) {
            if (page_mem[DEFAULT_IK_SIZE+DEFAULT_TS_SIZE+i] == canary[i]) {
                continue;
                }
            else {
                rc = 1; break;
            }
        }
    return rc;
}

int write_canary(void) {
    for (int i = 0; i < 4; i++) {
            page_mem[DEFAULT_IK_SIZE+DEFAULT_TS_SIZE+i] = canary[i];
        }
    return 0;
}

int write_otp_key(uint8_t *key) {
    int rc;
    //memcpy(&page_mem[0], default_ik, DEFAULT_IK_SIZE);
    for (int i = 0; i < DEFAULT_IK_SIZE; i++) {
        page_mem[i] = *(key+i);
    }
    page_mem[DEFAULT_IK_SIZE]= 0x00;
    page_mem[DEFAULT_IK_SIZE+1]= 0x00;
    for (unsigned i = DEFAULT_IK_SIZE+2; i < FLASHPAGE_SIZE; i++) {
        memcpy(&page_mem[i], dummy_flash_data, 1);
        }
    write_canary();
    //puts("wrote page_mem");
    
    flashpage_write((int)OTP_FLASH_PAGE, NULL);
    rc = flashpage_write_and_verify((int)OTP_FLASH_PAGE, page_mem);
    if (rc != FLASHPAGE_OK) {
        printf("Error: writing identity key in flash at page %u.\n\n", (int)OTP_FLASH_PAGE);
   	return 1;
      }
    else printf("Successfully wrote identity key in flash memory at page %u.\n\n", (int)OTP_FLASH_PAGE);

    return 0;
}

int otp_init(void) {
    flashpage_read((int)OTP_FLASH_PAGE, page_mem);
    uint8_t ik[DEFAULT_IK_SIZE] = {0};
    uint16_t counter = 0;
    
    if (canarytest()) {
        return write_otp_key((uint8_t *)default_ik);
    }
    else {
        for (int i = 0; i < DEFAULT_IK_SIZE; i++) {
            ik[i] = page_mem[i];
        }
        puts("Detected existing OTP state upon reboot.");
        printf("Current key at flash page %i :\n", (int)OTP_FLASH_PAGE);
        printHex(ik, 0);
        counter = read_otp_counter();
        printf("Counter read: %u\n", counter);
        puts("");
        return 0;
        }
}

uint16_t read_otp_counter(void) {
    flashpage_read((int)OTP_FLASH_PAGE, page_mem);
    uint16_t counter = 0;
    uint8_t high8 = page_mem[DEFAULT_IK_SIZE];
    uint8_t low8 = page_mem[DEFAULT_IK_SIZE+1];
    counter = high8 << 8;
    counter |= low8;
    return counter;    
}

int write_otp_counter(uint16_t value) {
    flashpage_read((int)OTP_FLASH_PAGE, page_mem);
    uint8_t high8 = (value >> 8) & 0x00FF;
    uint8_t low8 = value & 0x00FF;
    page_mem[DEFAULT_IK_SIZE] = high8;
    page_mem[DEFAULT_IK_SIZE+1] = low8;
    write_canary();
    int rc;

    rc = flashpage_write_and_verify((int)OTP_FLASH_PAGE, page_mem);
    if (rc != FLASHPAGE_OK) {
        printf("error: writing counter in page %i failed\n", (int)OTP_FLASH_PAGE);
   	return 1;
      }
    return 0;
}

int next_otp(uint8_t *otp_value)
{    

    uint8_t ik[DEFAULT_IK_SIZE] = {0};
    uint16_t counter = 0;
    uint8_t eid[DEFAULT_IK_SIZE] = {0};
    int scaler = 0;
    
    flashpage_read((int)OTP_FLASH_PAGE, page_mem);

    for (int i = 0; i < DEFAULT_IK_SIZE; i++) {
        ik[i] = page_mem[i];
    }
        
    counter = read_otp_counter();
    hex_eid_generator(ik, scaler, (int)counter, eid);
    
    for (int i = 0; i < DEFAULT_TS_SIZE; i++) {
        *(otp_value+i)=eid[i];
    }
      
    counter++;
    // flashpage_write((int)OTP_FLASH_PAGE, NULL);
    return write_otp_counter(counter);
}
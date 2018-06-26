/*
 * main.c
 *
 *  Created on: June 18, 2018
 *      Authors: Eduard Brehm
 *               Emmanuel Baccelli (Copyright (C) 2018 Inria)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "eid.h"
#include <xtimer.h>
#include "shell.h"
#include "periph/flashpage.h"
#include "ble_sensors.h"

#define LINE_LEN            (16)

char dummy_data[1]="0";


char blestack[THREAD_STACKSIZE_MAIN];


int read_otp_ik(int argc, char **argv) {

    (void)argc;
    (void)argv;
    
    uint8_t ik[DEFAULT_IK_SIZE] = {0};
    flashpage_read(OTP_FLASH_PAGE, page_mem);
    
    for (int i = 0; i < DEFAULT_IK_SIZE; i++) {
        ik[i] = page_mem[i];
    }
    printf("Key read in flash page %i :\n", (int)OTP_FLASH_PAGE);
    printHex(ik, 0);


    return 0;

}



int write_otp_ik(int argc, char **argv) {
    if(!(argc > 1)) {
    puts("Input error: too few arguments");
    return -1;
    }
    
    char *ikString = argv[1];
    
    uint8_t ik[16] = {0};
	char subString[3];
	subString[2] = '\0';
	int j = 0;
	for(int i = 0; i < 16*2; i+=2) {
		memcpy(subString, &ikString[i], 2);
		unsigned int intVal;
		if (sscanf (subString, "%xu", &intVal)!=1) {
		    puts("Error: key not a hex integer.");
		    return 0;
		}
		ik[j] = intVal;
		++j;
	}
    //uint8_t *ik_input = parseIk(ikString);
    printf("Input Key: ");
    printHex(ik, 0);
    write_otp_key(ik);
    
    
    return write_otp_counter(0);

}


static int cmd_info(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Flash start addr:\t0x%08x\n", (int)CPU_FLASH_BASE);
    printf("Page size:\t\t%i\n", (int)FLASHPAGE_SIZE);
    printf("Number of pages:\t%i\n", (int)FLASHPAGE_NUMOF);

    return 0;
}

int read_otp_cnt(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    uint16_t cnt = read_otp_counter();
    printf("Counter read: %u\n", cnt);
    puts("");
    return 0;
}

int write_otp_cnt(int argc, char **argv)
{
    if(argc > 1) {
        int cntr = atoi(argv[1]);
        return write_otp_counter((uint16_t)cntr);
    }
    else {
    puts("Input error: too few arguments");
    return -1;
    }
    return 0;
}

int print_otp(int argc, char **argv) {

    (void)argc;
    (void)argv;
    
    uint8_t ik[DEFAULT_IK_SIZE] = {0};
    uint8_t eid[DEFAULT_IK_SIZE] = {0};
    uint16_t counter = 0;
    flashpage_read(OTP_FLASH_PAGE, page_mem);
    
    counter = read_otp_counter();
    
    for (int i = 0; i < DEFAULT_IK_SIZE; i++) {
        ik[i] = page_mem[i];
    }
    //printf("Key read in flash page %i :\n", (int)OTP_FLASH_PAGE);
    //printHex(ik, 0);
    hex_eid_generator(ik, 0, counter, eid);

    return 0;

}

static const shell_command_t shell_commands[] = {
    { "write_ik", "Reset OTP key", write_otp_ik },
    { "read_ik", "Read OTP key", read_otp_ik },
    { "read_cnt", "Read OTP counter", read_otp_cnt },
    { "write_cnt", "Reset OTP counter", write_otp_cnt },
    { "print_otp", "Print current OTP", print_otp },
    { "info", "Show flash memory information", cmd_info },
    
    { NULL, NULL, NULL }
};

void *blethread_handler(void *arg)
{
    /* remove warning unused parameter arg */
    (void)arg;
    int rc = ble_sensor_init();
    rc++;
    return NULL;
}

int main(void)
{
    puts("OTP application for AGILE over BLE with RIOT");
    puts("Please refer to the README.md for further information\n");

    int rc = otp_init();
    if (rc != 0) {
        puts("Error: problem initializing OTP flash page.");
        return 0;
        }
    
    thread_create(blestack, sizeof(blestack),
                    THREAD_PRIORITY_MAIN - 2,
                    THREAD_CREATE_STACKTEST,
                    blethread_handler,
                    NULL, "ble stack thread");

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}

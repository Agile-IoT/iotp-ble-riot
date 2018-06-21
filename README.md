# OTP for RIOT on BLE-connected sensor

Clone the repository and change directory

    git clone https://github.com/Agile-IoT/iotp-riot.git && cd iotp-riot

In the Makefile, change the device you want to run the code on, according to [RIOT OS Tutorial](https://github.com/RIOT-OS/Tutorials/tree/master/task-01).

    #BOARD ?= native
    BOARD ?=nrf52dk
    
Flash the board and open a shell on the board:

    make all flash term

The output of the shell indicates launching BLE connectivity and sensor data notifications (visible on an AGILE gateway, once connected):

    INFO # main(): This is RIOT! (Version: 2018.07-devel-548-g83abf-HOST.lan)
    INFO # OTP application for AGILE with RIOT
    INFO # Please refer to the README.md for further information
    INFO # 
    INFO # Advertised device name: 'SensorTag RIOT'
    INFO # Starting BLE sensor notifications!
    INFO # Type 'help' for available shell commands.

    
## Write to internal storage

Write an Indentity Key (IK) to the internal storage on the IoT device with the following command:

    write_ik PAGE STRING
    
For example:

    write_ik 127 e2e12c2281cdf3d350a34de4d5f56613

The above example outputs

    INFO # write_ik 127 e2e12c2281cdf3d350a34de4d5f56613
    INFO # got page 127
    INFO # successfully wrote identity key in flash memory.
    
## Read from internal storage  

Read a string from the internal storage with the following pattern

    read_ik PAGE
    
For example:

    read_ik 127
    
The above example outputs

    INFO # Read flash page 127 into local page buffer
    INFO # Key read in internal memory:
    INFO # e2e12c2281cdf3d350a34de4d5f56613

## Periodic EIDs Generation

Once you have set the Idenditiy Key with write_ik, you can launch periodic OTP generation with

    run_otp PAGE
    
Below is an instance of output in the RIOT shell:

    INFO #  run_otp 127
    INFO # Key read in internal memory:
    INFO # e2e12c2281cdf3d350a34de4d5f56613
    INFO # Counter read in internal memory: 
    INFO # 0 
    INFO # Init Key Data: 15 14 59 55 54 91 15 15 54 95 51 15 15 15 55 45 
    INFO # Temporary Key Data: 00 00 00 00 00 00 00 00 00 00 00 ff 00 00 00 00 
    INFO # Temporary Key: 5f 18 ec e8 6e c7 f0 f6 d9 31 cb 35 72 5c 1c 61 
    INFO # Ephermal Id Data: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
    INFO # Ephermal Id: b5 94 53 c4 9b 21 2f 01 74 d6 9a 22 6e cd 7e 31 
    INFO # Generated OTP: b5 94 53 c4 9b 21 2f 01 
    INFO # Key read in internal memory:
    INFO # e2e12c2281cdf3d350a34de4d5f56613
    INFO # Counter read in internal memory: 
    INFO # 1 
    INFO # Init Key Data: 15 14 59 55 54 91 15 15 54 95 51 15 15 15 55 45 
    INFO # Temporary Key Data: 00 00 00 00 00 00 00 00 00 00 00 ff 00 00 00 00 
    INFO # Temporary Key: 5f 18 ec e8 6e c7 f0 f6 d9 31 cb 35 72 5c 1c 61 
    INFO # Ephermal Id Data: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 
    INFO # Ephermal Id: 26 55 8e 29 96 6a 83 b3 b8 58 7f c8 70 c3 e1 a2 
    INFO # Generated OTP: 26 55 8e 29 96 6a 83 b3 
    INFO # Key read in internal memory: 
    INFO # e2e12c2281cdf3d350a34de4d5f56613
    INFO # Counter read in internal memory: 
    INFO # 2 
    INFO # Init Key Data: 15 14 59 55 54 91 15 15 54 95 51 15 15 15 55 45 
    INFO # Temporary Key Data: 00 00 00 00 00 00 00 00 00 00 00 ff 00 00 00 00 
    INFO # Temporary Key: 5f 18 ec e8 6e c7 f0 f6 d9 31 cb 35 72 5c 1c 61 
    INFO # Ephermal Id Data: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 02 
    INFO # Ephermal Id: 83 87 0d 60 84 f3 d2 2d 39 44 4b fd cf 54 fc 8e 
    INFO # Generated OTP: 83 87 0d 60 84 f3 d2 2d 
    INFO # Key read in internal memory:
    INFO # e2e12c2281cdf3d350a34de4d5f56613
    INFO # Counter read in internal memory: 
    ...

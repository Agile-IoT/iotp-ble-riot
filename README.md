# OTP for RIOT on BLE-connected sensor

Clone the repository and clone RIOT with the below command:

    git clone https://github.com/Agile-IoT/iotp-ble-riot.git && cd iotp-ble-riot && git clone https://github.com/RIOT-OS/RIOT.git

In the Makefile, change the device you want to run the code on, according to [RIOT OS Tutorial](https://github.com/RIOT-OS/Tutorials/tree/master/task-01).

    #BOARD ?= native
    BOARD ?=nrf52dk
    
Flash the board and open a shell on the board (use `sudo` if needed):

    make all flash term

The output of the shell indicates launching BLE connectivity and sensor data notifications (visible on an [AGILE gateway](http://agile-iot.eu/wiki/index.php?title=Main_Page), once connected):

    INFO # main(): This is RIOT! (Version: 2018.07-devel-548-g83abf-HOST.lan)
    INFO # OTP application for AGILE with RIOT
    INFO # Please refer to the README.md for further information
    INFO # 
    INFO # Successfully identity key in flash memory at page 124.
    INFO # 
    INFO # Advertised device name: 'SensorTag RIOT'
    INFO # Starting BLE sensor notifications!
    INFO # Type 'help' for available shell commands.

    
## Read OTP key from internal storage  

Read the current key from the internal storage with the following command on the RIOT shell:

    read_ik
    
Example output:

    INFO # Key read in flash page 124
    INFO # e2 e1 2c 22 81 cd f3 d3 50 a3 4d e4 d5 f5 66 18

## Read OTP counter in internal storage

Read the current counter in the internal storage on the IoT device with the following command in the RIOT shell:

    read_cnt

Example output:

    INFO # Counter read: 0
    
## Reset OTP key in internal storage

Write an Indentity Key (IK) to the internal storage on the IoT device with the following command in the RIOT shell:

    write_ik STRING
    
For example:

    write_ik e2e12c2281cdf3d350a34de4d5f56618

The above example outputs

    INFO # Input Key: e2 e1 2c 22 81 cd f3 d3 50 a3 4d e4 d5 f5 66 18
    INFO # successfully wrote identity key in flash memory at page 124.
   
## Reset OTP counter in internal storage

Overwrite the current counter in the internal storage on the IoT device with the following command in the RIOT shell:

    write_cnt INT
    
For example:

    write_cnt 13

## Periodic EIDs Generation

From a remote device (e.g. from a smartphone using the [nRF Connect app](https://www.nordicsemi.com/eng/Products/Nordic-mobile-Apps/nRF-Connect-for-Mobile), or from the [AGILE gateway](http://agile-iot.eu/wiki/index.php?title=Main_Page)) successive One-Time-Password values are exposed as a dedicated BLE GATT service characteristic (with UUID 8491945D785011E8B45ADA24CD01F044) which can be read.

On the IoT device itself, the current OTP value can be printed with the below command:

    print_otp

Example output:

    INFO # Key: 
    INFO # e2 e1 2c 22 81 cd f3 d3 50 a3 4d e4 d5 f5 66 18 
    INFO # Counter value: 8
    INFO # Generated OTP: 45 85 56 bc c9 e2 cb 4d 

## Changing the default OTP parameters

The default flash page number and the default identity key, as well as key size and token size parameters can be changed in the files `eid.h` and `eid.c`

## Checking sensor values

From a remote device (e.g. from a smartphone using the [nRF Connect app](https://www.nordicsemi.com/eng/Products/Nordic-mobile-Apps/nRF-Connect-for-Mobile), or from the [AGILE gateway](http://agile-iot.eu/wiki/index.php?title=Main_Page)) sensor values are exposed and accessible as dedicated BLE GATT services and characteristics --mimicking the TI SensorTag UUIDs.


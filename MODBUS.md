[![smtc-rpi](readmeres/sequent%20.jpg)](https://www.sequentmicrosystems.com)

[![smtc-rpi](readmeres/8-thermo.webp)]([https://www.sequentmicrosystems.com/products/eight-thermocouples-daq-8-layer-stackable-hat-for-raspberry-pi)]

# Modbus
The [Eight Thermocouples DAQ 8-Layer Stackable HAT for Raspberry Pi](https://sequentmicrosystems.com/products/eight-thermocouples-daq-8-layer-stackable-hat-for-raspberry-pi) can be accessed thru Modbus RTU protocol over RS-485 port.
You can set-up the RS-485 port with **smtc** command.

Example:
```bash
~$ smtc 0 cfg485wr 1 9600 1 0 1
```
Set Modbus RTU , Baudrate: 9600bps, 1 Stop Bit,  parity: None, slave address offset: 1
```bash
~$ smtc -h cfg485rd
```
display the full set of options

## Slave Address
The slave address is added with the "stack level" jumpers. For example the jumpers configuration for stack level 1  (one jumper in position ID0) slave address offset to 1 corespond to slave address 2.

## Modbus object types
All modbus RTU object type with standard addresses are implemented : Coils, Discrete Inputs, Input registers, Holding registers.

### Coils

Acces level Read/Write, Size 1 bit

| Device function | Register Address | Modbus Address |
| --- | --- | --- |


### Discrete Inputs

Access level Read Only, Size 1 bit

| Device function | Register Address | Modbus Address |
| --- | --- | --- |



### Input registers

Access level Read Only, Size 16 bits

| Device function | Register Address | Description | Measurement Unit |
| --- | --- | --- | --- |
| IR_TEMP1 | 30001 | Temperature 1 | 0.01C |
| IR_TEMP2 | 30002 | Temperature 2 | 0.01C |
| IR_TEMP3 | 30003 | Temperature 3 | 0.01C |
| IR_TEMP4 | 30004 | Temperature 4 | 0.01C |
| IR_TEMP5 | 30005 | Temperature 5 | 0.01C |
| IR_TEMP6 | 30006 | Temperature 6 | 0.01C |
| IR_TEMP7 | 30007 | Temperature 7 | 0.01C |
| IR_TEMP8 | 30008 | Temperature 8 | 0.01C |
| IR_RAW_MV1 | 30009 | Voltage 1 | uV |
| IR_RAW_MV2 | 30010 | Voltage 2 | uV |
| IR_RAW_MV3 | 30011 | Voltage 3 | uV |
| IR_RAW_MV4 | 30012 | Voltage 4 | uV |
| IR_RAW_MV5 | 30013 | Voltage 5 | uV |
| IR_RAW_MV6 | 30014 | Voltage 6 | uV |
| IR_RAW_MV7 | 30015 | Voltage 7 | uV |
| IR_RAW_MV8 | 30016 | Voltage 8 | uV |



### Holding registers

Access level Read/Write, Size 16 bits

| Device function | Register Address | Modbus Address | Measurement Unit | Range |
| --- | --- | --- | --- | --- |
| HR_LED_TEMP_TH1 | 40001 | 0x00 | C | -300..1000 |
| HR_LED_TEMP_TH1 | 40002 | 0x01 | C | -300..1000 |
| HR_LED_TEMP_TH1 | 40003 | 0x02 | C | -300..1000 |
| HR_LED_TEMP_TH1 | 40004 | 0x03 | C | -300..1000 |
| HR_LED_TEMP_TH1 | 40005 | 0x04 | C | -300..1000 |
| HR_LED_TEMP_TH1 | 40006 | 0x05 | C | -300..1000 |
| HR_LED_TEMP_TH1 | 40007 | 0x06 | C | -300..1000 |
| HR_LED_TEMP_TH1 | 40008 | 0x07 | C | -300..1000 |



## Function codes implemented

* Read Coils (0x01)
* Read Discrete Inputs (0x02)
* Read Holding Registers (0x03)
* Read Input Registers (0x04)
* Write Single Coil (0x05)
* Write Single Register (0x06)
* Write Multiple Coils (0x0f)
* Write Multiple registers (0x10)

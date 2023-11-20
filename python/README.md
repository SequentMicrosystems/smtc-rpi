# sm_tc

This is the python library to control the [Eight Thermocouples DAQ 8-Layer Stackable HAT for Raspberry Pi](https://sequentmicrosystems.com/products/eight-thermocouples-daq-8-layer-stackable-hat-for-raspberry-pi).

## Install

```bash
~$ sudo apt-get update
~$ sudo apt-get install build-essential python-pip python-dev python-smbus git
~$ git clone https://github.com/SequentMicrosystems/smtc-rpi.git
~$ cd smtc-rpi/python/sm_tc/
~/smtc-rpi/python/sm_tc$ sudo python setup.py install
```
or
```
~/smtc-rpi/python/sm_tc$ sudo pip install .
```
If you use python3.x repace the last line with:
```
~/smtc-rpi/python/sm_tc$ sudo python3 setup.py install
```
or
```
~/smtc-rpi/python/sm_tc$ sudo python3 -m pip install .
```
## Update

```bash
~$ cd smtc-rpi/
~/smtc-rpi$ git pull
~$ cd smtc-rpi/python/sm_tc/
~/smtc-rpi/python/sm_tc$ sudo python setup.py install
```
If you use python3.x repace the last line with:
```
~/smtc-rpi/python/sm_tc$ sudo python3 setup.py install
```
## Usage example

```bash
~$ python
Python 3.10.7 (main, Nov  7 2022, 22:59:03) [GCC 8.3.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import sm_tc
>>> tc = sm_tc.SMtc(0)
>>> tc.get_temp(1)
26.5
>>>
```

More usage example in the [examples](examples/) folder

## Functions prototype

### *class sm_tc.SMtc(stack = 0, i2c = 1)*
* Description
  * Init the SMtc object and check the card presence 
* Parameters
  * stack : Card stack level [0..7] set by the jumpers
  * i2c : I2C port number, 1 - Raspberry default , 7 - rock pi 4, etc.
* Returns 
  * card object

#### *set_sensor_type(channel, val)*
* Description
  * Set one channel thermocouple input type 
* Parameters
  * *channel*: The input channel number 1 to 8
  * *val*: The thermocouple type [0..7] -> [B, E, J, K, N, R, S, T]
* Returns
  * none
  
#### *get_sensor_type(channel)*
* Description
  * Get one channel thermocouple input type 
* Parameters
  * *channel*: The input channel number 1 to 8
* Returns
  * The thermocouple type [0..7] -> [B, E, J, K, N, R, S, T]
  
#### *print_sensor_type(channel)*
* Description
  * Print one channel thermocouple input type [B, E, J, K, N, R, S, T]
* Parameters
  * *channel*: The input channel number 1 to 8
* Returns
  * none
   
#### *get_temp(channel)*
* Description
  * Get one channel measured temperature in degee Celsious
* Parameters
  * *channel*: The input channel number 1 to 8
* Returns
  * Temperature in degree Celsious 

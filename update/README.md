# update

This is the [Sequent Microsystems](https://www.sequentmicrosystems.com) Thermocouples Data Aquisition Stackable Card 
firmware update tool.

## Usage

```bash 
~$ git clone https://github.com/SequentMicrosystems/smtc-rpi.git 
~$ cd smtc-rpi/update/ 
~/smtc-rpi/update$ ./update 0 
``` 

If you already cloned the repository, skip the first step.
The command will download the newest firmware version from our server and write it  to the board. 
The stack level of the board must be provided as a parameter.

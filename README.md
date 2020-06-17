# Microcontrollers Final Research Project 
ASU CSE 318 Final Assesment Project - TM4C123G Tiva-C Based microwave system

## Design
The system was designed as a finite state machine on drawio then this model was used to code all the functions and states

## Functions:

```C
void PAUSE ():
```

 It basically pauses the timer and maintain the number of seconds so we can resume it anytime.
```C
uint32_t ATI (char I [10]):
```
It changes an array of char(strings) to an integer.

```C
void STOP ():
```
 It clears the timer and disable the timer.

```C
ENDSEQUENCE():
```
 it flashes the red led 5 times and returns to idle state.

```C
MICROWAVESTART ():
```
 it starts the timer at microwave mode and gives the option to stop or pause the mode, there is a one that works the same way for the grill mode. 

```C
GRILLIDLE ():
```
it gives us the choice to either start or change the timer value and a similar mode exists for the microwave.

```C
WELCOME ():
```
 this function run at start and the microwave idle is set as the default mode with 100 seconds default timer.

```C
Keypad_scanner ():
```
 It uses if loop as the keypad need multiplexing to obtain which button is clicked.

## Finite state machine diagram

![Finite state machine diagram](https://github.com/George-Ayad/Final-Project-CSE318-ASU-CHEP/blob/master/Finite%20State%20Machine%20Diagram.png?raw=true)


## Non-functional merits

### STACK USAGE

| Category          | Max Use | Total Use |
| ----------------- | ------- | --------- |
| interrupt         | 0       | 0         |
| Program entry     | 96      | 96        |
| Uncalled function | 120     | 504       |

7'478 bytes of readonly code memory
634 bytes of readonly data memory
5'745 bytes of readwrite data memory

### Ram 
As we do not utilize the heap, so the stack is equal to the ram 

### Rom 
0x1f68 = 8Kb

### CPU utilization 

We made a macro function to use with debugging at breakpoints to use the systick timer current register to obtain the value of the current at the beginning and end of function ans subtract the to get the total number of cycles and the divide it by the clock frequency.

#### Initialization + Welcome: 207 ms
#### Keypad interrupt: 100ms
#### UART Interrupt Stop: 169 ms
#### UART Interrupt Start: 44 ms
#### UART Interrupt seconds set: 80 us
#### UART Keypad Interrupt for starting any mode: 100ms
#### UART Keypad Interrupt for stopping any mode: 100ms

### Power
####  P= V*I 

Since it is an estimate and at runtime the current does not change as the clock frequency does not change, so we get the run time current from the 

datasheet = 17.6 , and sleep current from datasheet = 11.8 at all peripherals on , so that’s not the most accurate assumption as not all peripherals are on and their usage change through the run time , yet its is the best estimation we can get . If we need the real runtime power, we will probably need to use an oscilloscope probe to get interrupts current as its time is so small, so a multi-meter will not be able to scan it , and since the runtime is so small compared to the sleep time , we can assume that the sleep time is the power consumption of the system.

#### Runtime Power = 3.3*17.6= 58.08mW
##### Sleep Power =3.3*11.8=38.94mW



## Files submitted
main.c
gpio.h
gpio.c
tm4c123gh6pm.h
Final-Project-CSE318-ASU-CHEP.map
Finite State Machine Diagram.pdf
Cycle.mac
Video.m4v

## Links
### Github Repo:
https://github.com/George-Ayad/Final-Project-CSE318-ASU-CHEP

### Project Files:
https://engasuedu-my.sharepoint.com/:f:/g/personal/17p8181_eng_asu_edu_eg/EmDvz-yLx5FPnNd65r5QF_gB_txXB7zLsT4AKNaU07Q9Ow?e=wS629t

### Video:

https://engasuedu-my.sharepoint.com/:v:/g/personal/17p8181_eng_asu_edu_eg/EUlzic_4tX1JtkvcSbqkkxkBchmDyaPwcDnmTVTanLEDCw?e=WhUSOL


## Team Members :
1. Omar Zohdi - 17p8190
2. George Guirguis -17p8181
3. Abanoub Tarek - 17p8196
4. Fady Mina - 17p8187
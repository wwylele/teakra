# APBP

## MMIO Layout

```
Command/Reply registers
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x00C0    |                             REPLY0                            |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x00C2    |                              CMD0                             |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x00C4    |                             REPLY1                            |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x00C6    |                              CMD1                             |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x00C8    |                             REPLY2                            |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x00CA    |                              CMD2                             |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#

REPLY0..2: data sent from DSP to ARM
CMD0..2: data received from ARM to DSP

Semaphore registers
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x00CC    |                         SET_SEMAPHORE                         |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x00CE    |                         MASK_SEMAPHORE                        |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x00D0    |                         ACK_SEMAPHORE                         |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x00D2    |                         GET_SEMAPHORE                         |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#

SET_SEMAPHORE: semaphore sent from DSP to ARM
MASK_SEMAPHORE: masks semaphore interrupt received from ARM to DSP
ACH_SEMAPHORE: acknowledges/clears semaphore received from ARM to DSP
GET_SEMAPHORE: semaphore received from ARM to DSP

Config/status registers
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x00D4    |       |CI2|CI1|           |CI0|                   |END|       |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x00D6    |   |   | C2| C1|   |   | S | C0| R2| R1| R0|   |   |   |   |   |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x00D8    | C2| C1| C0| R2| R1| R0| S |WEM|WFL|RNE|RFL|       |PRS|WTU|RTU|
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#

END: ARM-side registers endianness. 1 to swap byte order of ARM-side registers
R0..R2: 1 when there is data in REPLY0..2
C0..C2: 1 when there is data in CMD0..2
CI0...CI2: 1 to disable interrupt when CMD0..2 is written by ARM
S: 1 when (GET_SEMAPHORE & ~MASK_SEMAPHORE) is non-zero
RTU: ARM-side read transfer underway flag
WTU: ARM-side write transfer underway flag
PRS: peripheral reset flag
RFL: ARM-side read FIFO full flag
RNE: ARM-side read FIFO non-empty flag
WFL: ARM-side write FIFO full flag
WEM: ARM-side write FIFO empty flag
* Note 0x00D8 is a mirror of ARM-side register DSP_PSTS


```

# OCEM

## MMIO Layout

The following MMIO definition is derived from Lauterbach's Teak debugger with conjecture and modification and is not tested at all. Note that this is the only MMIO region that uses odd-address registers

```
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x0060    |                              PFT                              |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x0061    |                                                               |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x0062    |                             PAB1_L                            |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x0063    |               |       ?       |                       |PAB1_H |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x0064    |                             PAB2_L                            |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x0065    |               |       ?       |                       |PAB2_H |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x0066    |                             PAB3_L                            |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x0067    |               |       ?       |                       |PAB3_H |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x0068    |                               |               ?               |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x0069    |                               |               ?               |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x006A    |                               |               ?               |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x006B    |                              DAM?                             |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x006C    |                              DAB?                             |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
|+0x006D  ? |SSE|ILL|BKR|TBF|INT|BRE|P3E|P2E|P1E|EXR|EXW|DVA|DAR|DAW|DVR|DVW|
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#
| ... ?                                                                     |
+-----------#---+---+---+---#---+---+---+---#---+---+---+---#---+---+---+---#

PFT: Program Flow Trace
PAB1_L, PAB1_H: Program Address Break Point #1
PAB2_L, PAB2_H: Program Address Break Point #2
PAB3_L, PAB3_H: Program Address Break Point #3
DAM: Data Address Mask
DAB: Data Address Break Point

DVW: 1 to enable data value break point on data write transaction
DVR: 1 to enable data value break point on data read transaction
DAW: 1 to enable data address break point as a result on data write transaction
DAR: 1 to enable data address break point as a result on data read transaction
DVA: 1 to enable break point as a result of simultaneous data address and data value match
EXW: 1 to enable break point as a result of external register write transaction
EXR: 1 to enable break point as a result of external register read transaction
P1E: 1 to enable program break point 1
P2E: 1 to enable program break point 2
P3E: 1 to enable program break point 3
BRE: 1 to enable break point every time the program jumps instead of executing the next address
INT: 1 to enable break point upon detection of interrupt service routine
TBF: 1 to enable break point as a result of program flow trace buffer full
BKR: 1 to enable break point when returning to the beginning of block repeat loop
ILL: 1 to enable break point on illegal condition
SSE: 1 to enable single step

```

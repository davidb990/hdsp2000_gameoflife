# Conway's Game of Life on a HDSP2000 Display

The repo contains some experiments getting HDSP2000 series displays to communicate with Pico W and RP2040. The experiement of choice is Conway's Game of Life, a simple mathematical game well suited to dot matrix displays.

### HDSP2000
These displays use a shift register to take an input. Once the register is loaded, a column pin is sent high, which outputs the shift register to the column in question across all characters in the display. 

### RP2040 Driver
To load the shift register, this code uses a simple PIO program to control the CLK and DATA pins on the HDSP2000, holding CLK low when data isn't being loaded, and asserting an interrupt once the data has been shifted out to the display.

On core 1, a loop monitors for the IRQ, and sends the column inputs high one at a time, and loads data into the PIO FIFO.

### Hardware Configuration

The pins on the Pico W are used as follows:

| Pin      | Function |
| -------- | -------- |
| 0        | Column 1 |
| 1        | Column 2 |
| 2        | Column 3 |
| 3        | Column 4 |
| 4        | Column 5 |
| 14       | Clock    |
| 15       | Data     |

The columns on the HDSP2000 use a fair bit of current (~400 mA), so a UDN2981 is used between the Pico W and the display to prevent the current of the display columns overwhelming the GPIO pins of the Pico W.

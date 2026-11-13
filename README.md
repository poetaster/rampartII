# rampartII
the continuation of my arduino semi-modular platform using the pi pico zero

I built the original rampart as a means for exploring doing audio with arduinos and interacting with devices like the Bastl Kastle. This generation is no different (new generation Kastles, too :). But, we have more inexpensive and powerful embedded processors now :) I chose the pi pico.

## hardware
This rampart includes a number of things the original did not:
1. display, to minimize creating interfaces with blinking lights with growing complexity
2. Time of Flight sensor, because they're bloody cool
3. midi out and a way to use the audio/clock in as midi in.
4. 4 x 16 bit ADC, 3 on pins, 1 on TRS jack. With power management and providing clean control voltages or audio in
5. capacitive pads instead of buttons, wich is more a layout choice and not ideal, but two buttons + the encoder button remain
6. simple lowpass filter and audio volume control
7. Encoder with button
8. 3 pots for values ABC
9. additional pin ADC which can be used with an LDR (ie. voltage supply directly there) or any other voltage source.

WARNING: there is minimal protective circuitry on the CV/ADC inputs. Do not exceed 4 volts on these.


## designs

Yes, it's done with fritzing. Because it's the only warez that will allow you to do Breadboard, Schematic and PCB views, etc. I breadboard designs first, so I use fritzing.

![top breadboard](rampart_pico_bb.jpg)
![bottom pcb](rampart_pico_pcb.jpg)
![bottom schematic](rampart_pico_schem.jpg)

#!/bin/bash
FILE="homeNet_irSwitch.ino.hex"
find /tmp -name $FILE -exec mv {} . \;
avrdude -c usbasp -p m8 -U flash:w:$FILE -U lfuse:w:0xE3:m -U hfuse:w:0xD9:m

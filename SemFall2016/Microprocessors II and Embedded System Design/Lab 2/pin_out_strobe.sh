#!/bin/sh


echo -n "$1" > /sys/class/gpio/export
echo -n "out" > /sys/class/gpio/gpio$1/direction
echo -n "$2" > /sys/class/gpio/gpio$1/value

echo -n "$1" > /sys/class/gpio/unexport

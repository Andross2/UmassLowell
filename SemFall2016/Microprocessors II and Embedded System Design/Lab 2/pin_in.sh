 #!/bin/sh

echo -n "$1" > /sys/class/gpio/export
echo -n "in" > /sys/class/gpio/gpio$1/direction
echo -n "$2" > /sys/class/gpio/export 2>>error
echo -n "in" > /sys/class/gpio/gpio$2/direction
cat  /sys/class/gpio/gpio$2/value 1> out.txt

echo -n "$1" > /sys/class/gpio/unexport
echo -n "$2" > /sys/class/gpio/unexport


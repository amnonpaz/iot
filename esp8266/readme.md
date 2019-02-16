These esp8266 project use freeRTOS from https://github.com/SuperHouse/esp-open-rtos<br>
<br>
You need to follow the build instruction as described in it's readme. before building,
the following packages must be apt-get'ed:<br>
sudo apt-get install git vim make autoconf libtool gettext gperf bison flex makeinfo texinfo help2man libtool libtool-bin ncurses-bin libncurses libncurses5 libncurses5-dev g++ gcc python python2.7-dev<br>
<br>
Also, pip install pyserial<br>
<br>
When done, <esp-open-sdk folder>/xtensa-lx106-elf/bin should be added to PATH,<br>
and also, ESP_COMMON_DIR should hold <esp-open-rtos folder><br>

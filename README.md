# File transfer over uart (Example for C/C++ course)

## Login
Login with **ssh** to your beaglebone (example: ssh debian@192.168.7.2)

## Clone
```sh
git clone https://github.com/vlabo/file_transfer_over_uart.git
```

## Build
```sh
cd file_transfer_over_uart
make
```

## Run

### Load UART drivers
edit /boot/uEnv.txt (you may want to backup the file first `sudo cp /boot/uEnv.txt /boot/uEnv.txt_backup`)
```sh
sudo nano /boot/uEnv.txt
```
replace:
```
#uboot_overlay_addr6=/lib/firmware/<file6>.dtbo
#uboot_overlay_addr7=/lib/firmware/<file7>.dtbo
```
with:
```
uboot_overlay_addr6=/lib/firmware/BB-UART1-00A0.dtbo
uboot_overlay_addr7=/lib/firmware/BB-UART2-00A0.dtbo
```

reboot:
```sh
sudo reboot
```

### Connect UART1 with UART2

Connect the four pins on the board  
Pin 24 -> Pin 22  
Pin 26 -> Pin 21  

<img width="701" alt="beaglebone_black_pinmap" src="https://user-images.githubusercontent.com/7770953/151859073-42b47729-8630-4865-a562-caca08d7e25e.png">


### Run the servers

You need 3 ssh session (or use [tmux](https://www.ocf.berkeley.edu/~ckuehl/tmux/))

#### session one:
in project directory:
```sh
cd device1
./server 1
```
#### session two:
in project directory:
```sh
cd device2
./server 2
```

### session tree:
in project directory
```sh
cd device1 # or device2
./client -m test_message # send a test message
./client -f ../common.h # send common.h file to the other server

```

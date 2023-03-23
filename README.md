# system programming team project
- repository for system programming team projcet during 3-1 semester.
- made to access easily in RPi.

## how to use wiringPi

- [wiringpi site](wiringpi.com)
- example code written in C is below
```c
#include <wiringPi.h>
int main(void){
	wiringPiSetup();
	pinMode(0, OUTPUT);
	for(;;){
		digitalWrite(0,HIGH); delay(500);
		digitalWrite(0,LOW); delay(500);
	}
	return 0;
}
```
- and compile upper code like this
```
gcc -Wall -o blink blink.c -lwiringPi
```
- how to install latest version of wiringPi : enter below commands below in shell
```
cd /tmp
wget https://project-downloads.drogon.net/wiringpi-latest.deb
sudo dpkg -i wiringpi-latest.deb
```
- before you use gpio, you should give permission to user like this.
```
sudo usermod -a -G gpio <username>
```


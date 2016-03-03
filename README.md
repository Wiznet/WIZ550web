##  H/W Revision 1.1 #

 - EMI Issue로 인해 33R Damping 저항 추가
 	- Ethernet TX/RX Signal 
 - Boot SW 추가(Slide SW) 
 - App_Boot SW 추가(Slide SW)
 	- H/W Trigger pin
 	- 기존 GPIO(D8)로 사용되었던 App_Boot pin을 별도로 Slide SW를 이용하여 구성
 	- Rev1.1은 GPIO(D8) 추가 사용 가능.
 - SWD pin(JTAG) 추가 - 1.27pitch 10pin
 - EEPROM(24AA64I-T/OT)추가
 	- I2C 전용 Peripheral에 연결
 		- Rev 1.0
			- PB06 - D13
			- PB07 - D14
		- Rev 1.1
			- PB06 - I2C_SCL
			- PB07 - I2C_SDA
 - Artwork
 	- Rev1.1은 Artwork 새로 진행.
 	- 나사 홀 Open으로 변경 (외부 노이즈 방지용)
 	- Base board에서 noise가 550web으로 영향 줄 가능성이 있어 나사홀 open
 - Cortex-M4 pin to pin 가능하게 설계
 	- Cortex-M4로 변경 가능 (더 높은 성능 요구 시) 
 - Test point 추가 (3V3D, GND)
 	- DMM이나 오실로스코프 사용 시 사용 될 Test Point 추가
 - GPIO pin list 변경 (Digital & SPI CS pin) 
	 - Rev 1.0
		 - D0 - PC06
		 - D1 - PC07
		 - D2 - PC08
		 - D3 - PC09
		 - D4 - PA08
		 - D8 - PC12
		 - D13 - PB06
		 - D14 - PB07
		 - Flash memory CS (External memory) - PB11
		 - Debug LED 0 - PC04 
		 - Debug LED 1 - PC05
	 - Rev 1.1
	 	 - D0 - PC00
		 - D1 - PC01
		 - D2 - PC02
		 - D3 - PC03
		 - D4 - PC04
		 - D8 - PC05
		 - D13 - PC06
		 - D14 - PC07
		 - Flash memory CS (External memory) - PB09
		 - Debug LED 0 - PA08 
		 - Debug LED 1 - PC12
		 - I2C_EEPROM_SCL - PB06
		 - I2C_EEPROM_SDA - PB07

## F/W 수정사항(github에 2.0.0으로 새로 업데이트 예정) ##

 - GPIO pin 변경 (H/W Rev 1.1 Pinout list 확인)
 - App_Boot pin 변경
 	- 기존 D8(GPIO)로 사용했던 App boot 핀을 변경.
 - Flash memory CS pin 변경
 	- PB11 -> PB09로 변경 
 - EEPROM 코드 추가 (Peri & GPIO 2개 코드로 나뉘어져 있음)
 	- 기존 Serial flash memory로 동작되었던 Config Tool + userhandler 부분을 전부 EEPROM에서 동작하도록 변경.
 	- H/W에서 Peripheral으로 사용할 수 있게 수정하였음. 따라서 Peri용 코드를 사용.
 - BOOT code size 24KB -> 28KB 변경.
 	- BOOT코드 추가로 인한 코드 사이즈 변경
 - TFTP 코드 변경
 	- 이제 EEPROM으로 대체하니깐 Flash memory size를 전부 사용하기 위해 코드 수정.

#WIZ550web
- Embedded Web server module for Things based on W5500 hardwired TCP/IP chip (Non-OS)
- Provides the firmware & web page examples for user’s customization
- 16-Configurable Digital I/O, 4-Analog Input, 2-UART Port
- SD card, Configuration tool, Serial AT command set support

>> [WIZ550web GitHub Page](http://wiznet.github.io/WIZ550web/)


##Images
###WIZ550web Module
- 74.4mm(W) x 30mm(L) x 24mm(H) (±0.5)

<!-- WIZ550web pic -->
![WIZ550web](http://wizwiki.net/wiki/lib/exe/fetch.php?media=products:wiz550web:wiz550web_front.png "WIZ550web")

###WIZ550web Baseboard (Separate purchases)
- DC 9~24V Power Input
- Digital Output 8EA (Relay - HR91C-05)
- Digital Input 8EA (Photocouplers - TLP290-4)
- Analog Input 4EA
- RS-232C / RS-422 interface
- 145mm(W) x 85mm(L) x 28mm(H) (±0.5)

<!-- WIZ550web Baseboard pic -->
<p align="center">
  <img width="70%" src="http://wizwiki.net/wiki/lib/exe/fetch.php?media=products:wiz550web:wiz550web_baseboard_front.png" />
</p>

<!-- WIZ550web EVB pic -->
<p align="center">
  <img width="70%" src="http://wizwiki.net/wiki/lib/exe/fetch.php?media=products:wiz550web:wiz550web_evb_side.png" />
</p>

For more details, please refer to [WIZ550web Wiki page](http://wizwiki.net/wiki/doku.php?id=products:wiz550web:start) in [WIZnet Wiki](http://wizwiki.net).

##Features
- WIZnet W5500 Hardwired TCP/IP chip
  - Hardwired TCP/IP embedded Ethernet controller
  - SPI (Serial Peripheral Interface) Microcontroller Interface
  - Hardwired TCP/IP stack supports TCP, UDP, IPv4, ICMP, ARP, IGMP, and PPPoE protocols
  - Easy to implement the other network protocols
- ST Microelectronics STM32F103RBT6
  - ARM 32-bit Cortex™-M3 microcontroller running at up to 72MHz
  - 128kB on-chip flash / 20kB on-chip SRAM / Various peripherals
- Adesto Technologies AT45DB081D-SU
  - 8-Megabit Serial Flash Memory 
- SD Card Slot
- 2.54mm Pin Header x 2


## Software
These are Firmware projects (source code) based on Eclipse IDE for C/C++ Developers (ARM GCC 4.8.3).
- Firmware source code
  - Application
  - Boot
- Web page examples
  - [Basic Demo Web Pages](http://wizwiki.net/wiki/doku.php?id=products:wiz550web:wiz550webgsg_en#basic_demo_web_pages)
- Configuration tool (Java)
  - [Download Page Link](http://wizwiki.net/wiki/doku.php?id=products:wiz550web:wiz550web_download)

## Hardware material, Documents and Others
Various materials are could be found at [WIZ550web Wiki page](http://wizwiki.net/wiki/doku.php?id=products:wiz550web:start) in [WIZnet Wiki](http://wizwiki.net).

- Documents
  - Getting Started Guide
  - User's Guide (for AT command)
  - Future Plan
- Technical Reference (Datasheet)
  - Hardware Specification
  - AC/DC Characteristics
  - Reference Schematic & Parts
  - Dimension
- See also 


## Revision History
v1.1.1 Stable
- Sep. 2015
- History
  - Bug fixed: Socket and data length handling problems in some web browsers (e.g., ie11)
  - Modified the TCP socket state transition handler of the HTTP server routine for clarity
  - Added the custom command handler in userHandler.c/h
    - Users can add custom commands using this function form
    - e.g., I/O control commands without web pages
  - Changed some letters in code: convert uppercase to lowercase
    - e.g., WIZ550WEB -> WIZ550web

v1.1.0 Develop
- Jan. 2015
- History
  - Added the FTP Server feature. (F_APP_FTP)
  - Added the Data Flash feature on FatFs. (F_SPI_FLASH)
  - We support a storage of data flash as well as SD card above v1.1.0 release.
    - You can use one of a SD card and a data flash. The mount priority of a SD card is higher than a data flash.
    - If you wish to use a SD card, you must copy the web page to a SD card and insert a SD card into a slot.
    - If there is no SD card after detecting during about 3 seconds, you can use a data flash.
      - You must have the initialization process of data flash at least once.
      - When SW1 and SW2 are pressed at the same time, the data flash is initialized by FatFs. And reset a target.
      - You must copy the web page to a data flash by FTP client tool.([ALFTP](http://www.altools.com/ALTools/ALFTP.aspx))
      - Refer to WIZ550web+FatFS+FTPServer Project Tutorial. http://youtu.be/XtnT2_CNgaY
      - Refer to WIZ550web+WindowsFTP Tutorial. Need to apply a commit [cfce843](https://github.com/Wiznet/WIZ550web/commit/cfce843031bf4657fc9530e5c505a9a3d555fc91). http://youtu.be/kelGSGj3kOQ
      - Refer to WIZ550web+LinuxFTP Tutorial. http://youtu.be/6qsPZA5QKEI

v1.0.1
- Jan. 2015
- History
  - Added the 'Get/Set Interface functions' for easy to customize user's web pages
  - HTTP Server operation stability improvement
  - Modified some comments and fixed some typos

v1.0.0
- First release : Nov. 2014

##  H/W Revision 1.1 #

 - Add Resistor 33R Damping for EMI issue
 	- Ethernet TX/RX Signal 
 - Changed MCU
 	- STM32F103RBT6(128KB Flash, 20KB SRAM) -> STM32F103RCT6(256KB Flash, 48KB SRAM)
 - Add Boot SW(Slide SW) 
 - Add App_Boot SW (Slide SW)
 	- App_boot SW is H/W Trigger pin
 	- This pin H/W Rev1.0 is D8, H/W rev1.1 is using App_boot Slide SW.
 - Add SWD pin(JTAG) - 1.27pitch 10pin
 - Add EEPROM(24AA64I-T/OT)
 	- Connect to I2C Peripheral pin
 		- Rev 1.0
			- PB06 - D13
			- PB07 - D14
		- Rev 1.1
			- PB06 - I2C_SCL
			- PB07 - I2C_SDA
 - Artwork
 	- Renewal Artwork
 	- changed screw hole pixed open (protect EMI)
 - Changed possible Cortex-M4 pin to pin matching
 - Add Test point (3V3D, GND)
 - Changed GPIO pin list (Digital & SPI CS pin) 
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

## F/W Modify list ##

 - Changed MCU Memory size
 	- ldscripts -> mem.ld Memory Size change
 	- RAM, FLASH size change
 	- BOOT is 24KB -> 28KB
 	- APP is 97KB -> 222KB
 - Changed GPIO pin (H/W Rev 1.1 Pinout list 확인)
 - Changed App_Boot pin code
 	- This pin H/W Rev1.0 is D8, H/W rev1.1 is using App_boot Slide SW.
 - Changed Flash memory CS pin
 	- PB11 -> PB09
 - Add EEPROM code
 	- H/W Rev1.0 is Serial flash memory operation.
 	- But, H/W Rev1.1 is EEPROM operation. So, TFTP and operation code is using EEPROM
 - Changed TFTP code
 	- It is changed EEPROM operation.

#Tool & Compiler
- Compiler: gcc-arm-none-eabi-4_8-2014q1-20140314-win32.exe (https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q1-update)
- Java: http://www.oracle.com/technetwork/java/javase/downloads/index.html 
- Eclipse: eclipse-cpp-kepler-SR2-win32-x86_64 (http://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/keplersr2)
- Make file: http://gnuwin32.sourceforge.net/packages/make.htm
	- Make file need three files: make.exe, rm.exe, echo.exe file
- Rev1.1(F/W2.0.0) compile procedure(Eclipse Menu)
	- Project -> Build configurations -> Set Active -> Rev1.1 Check.

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

#WIZ550web
- Embedded Web server module for Things based on W5500 hardwired TCP/IP chip (Non-OS)
- Provides the firmware & web page examples for user’s customization
- 16-Configurable Digital I/O, 4-Analog Input, 2-UART Port
- SD card, Configuration tool, Serial AT command set support

>> [WIZ550web GitHub Page](wiznet.github.io/WIZ550web/)


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
v1.0.1
- Jan. 2014
- History
  - Added the 'Get/Set Interface functions' for easy to customize user's web pages
  - HTTP Server operation stability improvement
  - Modified some comments and fixed some typos

v1.0.0
- First release : Nov. 2014

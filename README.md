# m5stack-starter

Includes WIFI, display driver, and reads some buttons.

The intent is for this to have support for all those features that I would
expect to want in most quick m5stack programs. All it does presently
is connect to wifi, get the time, and respond to mDNS requests.

## Building

`make menuconfig` You'll need to set the wifi username and password here, at least. 

`make`

`export ESPPORT=/dev/tty.myportdevice`

`make flash`

## References

The TFT library is from https://github.com/loboris/ESP32_TFT_library

## Notes

### Converting exception backtrace addresses to lines

When you get a panic message like this one: 

```
Guru Meditation Error: Core  0 panic'ed (LoadProhibited). Exception was unhandled.
Core 0 register dump:
PC      : 0x400848ab  PS      : 0x00060630  A0      : 0x80084b0d  A1      : 0x3ffbf2d0
A2      : 0x00000001  A3      : 0x00000000  A4      : 0x3ffc5144  A5      : 0x00000000
A6      : 0x00000000  A7      : 0xff000000  A8      : 0x00000000  A9      : 0x3ffbf190
A10     : 0x00000000  A11     : 0x00000000  A12     : 0x00000000  A13     : 0x00000000
A14     : 0x00000000  A15     : 0x00000000  SAR     : 0x00000004  EXCCAUSE: 0x0000001c
EXCVADDR: 0x00000024  LBEG    : 0x4000c2e0  LEND    : 0x4000c2f6  LCOUNT  : 0xffffffff

Backtrace: 0x400848ab:0x3ffbf2d0 0x40084b0a:0x3ffbf2f0 0x40084c7e:0x3ffbf310 0x400ed055:0x3ffbf340 0x400d3223:0x3ffbf370
```

Copy/paste those backtrace addresses and use `xtensa-esp32-elf-addr2line`:
`xtensa-esp32-elf-addr2line 0x400848ab:0x3ffbf2d0 0x40084b0a:0x3ffbf2f0 0x40084c7e:0x3ffbf310 0x400ed055:0x3ffbf340 0x400d3223:0x3ffbf370`
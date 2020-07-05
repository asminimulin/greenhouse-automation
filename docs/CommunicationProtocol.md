# Reference to Arduino-Esp protocol

## Setting yellow sensor address
   * Command:
      * 0x20
   * Arguments data:
      * 8 bytes of DS18B20's address(Little endian)
   * Response:
      * 0      - _OK_
      * 255    - _ERROR_

## Setting green sensor address
   * Command:
      * 0x21
   * Arguments data:
      * 8 bytes of DS18B20's address(Little endian)
   * Response:
      * 0      -  _OK_
      * 255    - _ERROR_

## Setting outside sensor address
   * Command:
      * 0x22
   * Arguments data:
      * 8 bytes of DS18B20's address(Little endian)
   * Response:
      * 0      -  _OK_
      * 255    - _ERROR_

## Setting yellow window driver's address
   * Command:
      * 0x23
   * Arguments data:
      * 8 bytes of DS2413's address(Little endian)
   * Response:
      * 0      -  _OK_
      * 255    - _ERROR_

## Setting green window driver's address
   * Command:
      * 0x24
   * Arguments data:
      * 8 bytes of DS2413's address(Little endian)
   * Response:
      * 0      -  _OK_
      * 255    - _ERROR_

## Setting vent driver's address
   * Command:
      * 0x25
   * Arguments data:
      * 8 bytes of DS2413's address(Little endian)
   * Response:
      * 0      -  _OK_
      * 255    - _ERROR_

## Getting measures
   * Command:
      * 0x31
   * Arguments data:
      No arguments
   * Response:
      1) _OK_
      2) _measuresSize_ - size of measures data
      3) _measuresSize_ bytes - measures data

## Gettings settings
   * Command: 
      0x32
   * Arguments data:
      No arguments
   * Response:
      1) 0 - _OK_
      2) _settingsSize_ - size of settings data
      3) _settingsSize_ bytes - settings data

## Setting settings
   * Command:
      0x33
   * Arguments data:
      * _settingsSize_ bytes - settings data
   * Response:
      * 0 - _OK_
      * 255 - _ERROR_
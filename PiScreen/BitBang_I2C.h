//
// Bit Bang I2C library
// Copyright (c) 2018 BitBank Software, Inc.
// Written by Larry Bank (bitbank@pobox.com)
// Project started 10/12/2018
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#pragma once

#include <stdint.h>

// supported devices
enum DEVICE_TYPE
{
    DEVICE_UNKNOWN = 0,
    DEVICE_SSD1306,
    DEVICE_SH1106,
};

typedef struct
{
    int file_i2c;
    int iBus;
} BBI2C;

/// Read N bytes
int I2CRead(BBI2C *pI2C, uint8_t iAddr, uint8_t *pData, int iLen);

/// Read N bytes starting at a specific I2C internal register
int I2CReadRegister(BBI2C *pI2C, uint8_t iAddr, uint8_t u8Register, uint8_t *pData, int iLen);

/// Write I2C data
/// quits if a NACK is received and returns 0
/// otherwise returns the number of bytes written
int I2CWrite(BBI2C *pI2C, uint8_t iAddr, uint8_t *pData, int iLen);

/// Test if an address responds
/// returns 0 if no response, 1 if it responds
uint8_t I2CTest(BBI2C *pI2C, uint8_t addr);

/// Scans for I2C devices on the bus
/// returns a bitmap of devices which are present (128 bits = 16 bytes, LSB first)
/// A set bit indicates that a device responded at that address
void I2CScan(BBI2C *pI2C, uint8_t *pMap);

/// Initialize the I2C BitBang library
void I2CInit(BBI2C *pI2C);

/// Figure out what device is at that address returns the enumerated value
DEVICE_TYPE I2CDiscoverDevice(BBI2C *pI2C, uint8_t i);

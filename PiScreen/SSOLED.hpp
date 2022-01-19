#pragma once

#include "I2CDevice.hpp"

// Make the Linux library interface C instead of C++

// 4 possible font sizes: 8x8, 16x32, 6x8, 16x16 (stretched from 8x8)
enum OLED_FONT_SIZE
{
    FONT_6x8 = 0,
#define FONT_SMALL FONT_6x8
    FONT_8x8,
#define FONT_NORMAL FONT_8x8
    FONT_12x16,
#define FONT_MIDDLE FONT_12x16
    FONT_16x16,
#define FONT_STRETCHED FONT_16x16
    FONT_16x32
#define FONT_LARGE FONT_16x32
};
// 4 possible rotation angles for oledScaledString()
enum OLED_FLIP_ANGLE
{
    ROT_0 = 0,
    ROT_90,
    ROT_180,
    ROT_270
};

// Rotation and flip angles to draw tiles
enum OLED_ANGLE
{
    ANGLE_0 = 0,
    ANGLE_90,
    ANGLE_180,
    ANGLE_270,
    ANGLE_FLIPX,
    ANGLE_FLIPY
};

// Return value from oledInit()
enum OLED_DEVICE_TYPE
{
    OLED_NOT_FOUND = -1, // no display found
    WHYTHIS,             // SSD1306 found at 0x3C
    _unused2,            // SSD1306 found at 0x3D
    OLED_SH1106_3C,      // SH1106 found at 0x3C
    OLED_SH1106_3D,      // SH1106 found at 0x3D
    OLED_SH1107_3C,      // SH1107
    OLED_SH1107_3D
};

class SSOLED
{
  public:
    //
    // Initializes the OLED controller into "page mode" on I2C
    // If SDAPin and SCLPin are not -1, then bit bang I2C on those pins
    // Otherwise use the Wire library.
    // If you don't need to use a separate reset pin, set it to -1
    //
    SSOLED(int busId, int iAddr, bool bFlip, bool bInvert);

    //
    // Get the underlying device type.
    //
    OLED_DEVICE_TYPE getDeviceType() const;

    //
    // Provide or revoke a back buffer for your OLED graphics
    // This allows you to manage the RAM used by ss_oled on tiny
    // embedded platforms like the ATmega series
    // Pass NULL to revoke the buffer. Make sure you provide a buffer
    // large enough for your display (e.g. 128x64 needs 1K - 1024 bytes)
    //
    void setBackBuffer(uint8_t *pBuffer);
    //
    // Sets the brightness (0=off, 255=brightest)
    //
    void setContrast(unsigned char ucContrast);
    //
    // Load a 128x64 1-bpp Windows bitmap
    // Pass the pointer to the beginning of the BMP file
    // First pass version assumes a full screen bitmap
    //
    int loadBMP(uint8_t *pBMP, int bInvert, bool bRender);
    //
    // Power up/down the display
    // useful for low power situations
    //
    void setPower(bool bOn);
    //
    // Set the current cursor position
    // The column represents the pixel column (0-127)
    // The row represents the text row (0-7)
    //
    void setCursorPos(int x, int y);

    //
    // Turn text wrap on or off for the oldWriteString() function
    //
    void setTextWrap(bool bWrap);
    //
    // Draw a string of normal (8x8), small (6x8) or large (16x32) characters
    // At the given col+row with the given scroll offset. The scroll offset allows you to
    // horizontally scroll text which does not fit on the width of the display. The offset
    // represents the pixels to skip when drawing the text. An offset of 0 starts at the beginning
    // of the text.
    // The system remembers where the last text was written (the cursor position)
    // To continue writing from the last position, set the x,y values to -1
    // The text can optionally wrap around to the next line by calling oledSetTextWrap(true);
    // otherwise text which would go off the right edge will not be drawn and the cursor will
    // be left "off screen" until set to a new position explicitly
    //
    //  Returns 0 for success, -1 for invalid parameter
    //
    int writeString(int iScrollX, int x, int y, const char *szMsg, OLED_FONT_SIZE iSize = FONT_NORMAL, bool bInvert = false, bool bRender = true);
    //
    // Draw a string with a fractional scale in both dimensions
    // the scale is a 16-bit integer with and 8-bit fraction and 8-bit mantissa
    // To draw at 1x scale, set the scale factor to 256. To draw at 2x, use 512
    // The output must be drawn into a memory buffer, not directly to the display
    //
    int scaledString(int x, int y, const char *szMsg, int iSize, int bInvert, int iXScale, int iYScale, OLED_FLIP_ANGLE iRotation);
    //
    // Fill the frame buffer with a byte pattern
    // e.g. all off (0x00) or all on (0xff)
    //
    void fill(unsigned char ucData, bool bRender);
    //
    // Set (or clear) an individual pixel
    // The local copy of the frame buffer is used to avoid
    // reading data from the display controller
    // (which isn't possible in most configurations)
    // This function needs the USE_BACKBUFFER macro to be defined
    // otherwise, new pixels will erase old pixels within the same byte
    //
    int setPixel(int x, int y, unsigned char ucColor, bool bRender);
    //
    // Dump an entire custom buffer to the display
    // useful for custom animation effects
    //
    void drawBuffer(uint8_t *pBuffer = nullptr);
    //
    // Render a window of pixels from a provided buffer or the library's internal buffer
    // to the display. The row values refer to byte rows, not pixel rows due to the memory
    // layout of OLEDs. Pass a src pointer of NULL to use the internal backing buffer
    // returns 0 for success, -1 for invalid parameter
    //
    int drawGFX(uint8_t *pSrc, int iSrcCol, int iSrcRow, int iDestCol, int iDestRow, int iWidth, int iHeight, int iSrcPitch);

    //
    // Draw a line between 2 points
    //
    void drawLine(int x1, int y1, int x2, int y2, bool bRender);
    //
    // Play a frame of animation data
    // The animation data is assumed to be encoded for a full frame of the display
    // Given the pointer to the start of the compressed data,
    // it returns the pointer to the start of the next frame
    // Frame rate control is up to the calling program to manage
    // When it finishes the last frame, it will start again from the beginning
    //
    uint8_t *playAnimFrame(uint8_t *pAnimation, uint8_t *pCurrent, int iLen);

    //
    // Scroll the internal buffer by 1 scanline (up/down)
    // width is in pixels, lines is group of 8 rows
    // Returns 0 for success, -1 for invalid parameter
    //
    int scrollBuffer(int iStartCol, int iEndCol, int iStartRow, int iEndRow, bool bUp);
    //
    // Draw a sprite of any size in any position
    // If it goes beyond the left/right or top/bottom edges
    // it's trimmed to show the valid parts
    // This function requires a back buffer to be defined
    // The priority color (0 or 1) determines which color is painted
    // when a 1 is encountered in the source image.
    // e.g. when 0, the input bitmap acts like a mask to clear
    // the destination where bits are set.
    //
    void drawSprite(uint8_t *pSprite, int cx, int cy, int iPitch, int x, int y, uint8_t iPriority);
    //
    // Draw a 16x16 tile in any of 4 rotated positions
    // Assumes input image is laid out like "normal" graphics with
    // the MSB on the left and 2 bytes per line
    // On AVR, the source image is assumed to be in FLASH memory
    // The function can draw the tile on byte boundaries, so the x value
    // can be from 0 to 112 and y can be from 0 to 6
    //
    void drawTile(const uint8_t *pTile, int x, int y, OLED_ANGLE iRotation, int bInvert, bool bRender);
    //
    // Draw an outline or filled ellipse
    //
    void drawEllipse(int iCenterX, int iCenterY, int iRadiusX, int iRadiusY, uint8_t ucColor, uint8_t bFilled);
    //
    // Draw an outline or filled rectangle
    //
    void drawRectangle(int x1, int y1, int x2, int y2, uint8_t ucColor, uint8_t bFilled);

  private:
    void p_WriteFlashBlock(uint8_t *s, int iLen);
    void p_WriteDataBlock(unsigned char *ucBuf, int iLen, bool bRender);
    void p_RepeatByte(uint8_t b, int iLen);
    void p_SetPosition(int x, int y, bool bRender);
    void p_I2CWrite(unsigned char *pData, int iLen);
    void p_WriteCommand(unsigned char c);
    void p_WriteCommand(unsigned char c, unsigned char d);
    void p_DrawScaledPixel(int iCX, int iCY, int x, int y, int iXFrac, int iYFrac, uint8_t ucColor);
    void p_DrawScaledLine(int iCX, int iCY, int x, int y, int iXFrac, int iYFrac, uint8_t ucColor);
    void p_BresenhamCircle(int iCX, int iCY, int x, int y, int iXFrac, int iYFrac, uint8_t ucColor, uint8_t bFill);

  private:
    OLED_DEVICE_TYPE m_DeviceType = OLED_NOT_FOUND;
    // requested address or 0xff for automatic detection
    uint8_t m_Addr = 0;
    uint8_t m_CursorX = 0, m_CursorY = 0;
    uint8_t m_X = 0, m_Y = 0;
    bool m_Wrap = false;
    bool m_Flip = false;
    int m_ScreenOffset = 0;
    uint8_t *m_ucScreen = nullptr;
    I2CDevice *m_I2CDevice = nullptr;
};

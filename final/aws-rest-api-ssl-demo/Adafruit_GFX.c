/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!
 
Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h"
#include "glcdfont.h"
//#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

int cursor_x=0;
int cursor_y=0;
unsigned char textsize=1;
unsigned int textcolor = 0xFFFF;
unsigned int textbgcolor = 0xFFFF;
char wrap = 1;


/*
Adafruit_GFX(int w, int h):
  WIDTH(w), HEIGHT(h)
{
  _width    = WIDTH;
  _height   = HEIGHT;
  rotation  = 0;
  cursor_y  = cursor_x    = 0;
  textsize  = 1;
  textcolor = textbgcolor = 0xFFFF;
  wrap      = true;
}
*/

// Draw a circle outline
void drawCircle(int x0, int y0, int r, unsigned int color) {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  drawPixel(x0  , y0+r, color);
  drawPixel(x0  , y0-r, color);
  drawPixel(x0+r, y0  , color);
  drawPixel(x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

void drawCircleHelper( int x0, int y0,
               int r, unsigned char cornername, unsigned int color) {
  int f     = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x     = 0;
  int y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void fillCircle(int x0, int y0, int r,
			      unsigned int color) {
  drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void fillCircleHelper(int x0, int y0, int r,
    unsigned char cornername, int delta, unsigned int color) {

  int f     = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x     = 0;
  int y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

// Bresenham's algorithm - thx wikpedia
void drawLine(int x0, int y0, int x1, int y1, unsigned int color) {
  int steep;
  int dx, dy;
	int err;
	int ystep;
						
	steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  dx = x1 - x0;
  dy = abs(y1 - y0);

  err = dx / 2;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// Draw a rectangle
void drawRect(int x, int y,
			    int w, int h,
			    unsigned int color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}
/*
void drawFastVLine(int x, int y,
				 int h, unsigned int color) {		 
					 
  // Update in subclasses if desired!
  drawLine(x, y, x, y+h-1, color);
}

void drawFastHLine(int x, int y,
				 int w, unsigned int color) {
  // Update in subclasses if desired!
  drawLine(x, y, x+w-1, y, color);
}

void fillRect(int x, int y, int w, int h,
			    unsigned int color) {
  int i;
						
  // Update in subclasses if desired!
  for (i=x; i<x+w; i++) {
    drawFastVLine(i, y, h, color);
  }
}

void fillScreen(unsigned int color) {
  fillRect(0, 0, _width, _height, color);
}
*/
// Draw a rounded rectangle


/*
void drawBitmap(int x, int y,
			      const unsigned char *bitmap, int w, int h,
			      unsigned int color) {

  int i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        drawPixel(x+i, y+j, color);
      }
    }
  }
}
*/
// Draw a 1-bit color bitmap at the specified x, y position from the
// provided bitmap buffer (must be PROGMEM memory) using color as the
// foreground color and bg as the background color.
/*
void drawBitmap(int x, int y,
            const unsigned char *bitmap, int w, int h,
            unsigned int color, unsigned int bg) {

  int i, j, byteWidth = (w + 7) / 8;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        drawPixel(x+i, y+j, color);
      }
      else {
      	drawPixel(x+i, y+j, bg);
      }
    }
  }
}

//Draw XBitMap Files (*.xbm), exported from GIMP,
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void drawXBitmap(int x, int y,
                              const unsigned char *bitmap, int w, int h,
                              unsigned int color) {
  
  int i, j, byteWidth = (w + 7) / 8;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i % 8))) {
        drawPixel(x+i, y+j, color);
      }
    }
  }
}

#if ARDUINO >= 100
size_t write(unsigned char c) {
#else
void write(unsigned char c) {
#endif
  if (c == '\n') {
    cursor_y += textsize*8;
    cursor_x  = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
    cursor_x += textsize*6;
    if (wrap && (cursor_x > (_width - textsize*6))) {
      cursor_y += textsize*8;
      cursor_x = 0;
    }
  }
#if ARDUINO >= 100
  return 1;
#endif
}
*/
// Draw a character
void drawChar(int x, int y, unsigned char c,
			    unsigned int color, unsigned int bg, unsigned char size) {

  unsigned char line;	
  char i;						
  char j;						
						
  if((x >= WIDTH)            || // Clip right
     (y >= HEIGHT)           || // Clip bottom
     ((x + 6 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;

  for (i=0; i<6; i++ ) {
    if (i == 5) 
      line = 0x0;
    else 
      line = font[(c*5)+i];
    for (j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, color);
        else {  // big size
          fillRect(x+(i*size), y+(j*size), size, size, color);
        } 
      } else if (bg != color) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, bg);
        else {  // big size
          fillRect(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

void Outstr (char * str) {
	char * ptr;
	
	ptr = str;
	while (*ptr) {
		drawChar(cursor_x, cursor_y, *ptr++, textcolor, textbgcolor, textsize);
		cursor_x += 6*textsize;
	}
}

void setCursor(int x, int y) {
  cursor_x = x;
  cursor_y = y;
}

void setTextSize(unsigned char s) {
  textsize = (s > 0) ? s : 1;
}

void setTextColorTransparent(unsigned int c) {
  // For 'transparent' background, we'll set the bg 
  // to the same as fg instead of using a flag
  textcolor = textbgcolor = c;
}

void setTextColor(unsigned int c, unsigned int b) {
  textcolor   = c;
  textbgcolor = b; 
}


void setTextWrap(char w) {
  wrap = w;
}
/*
unsigned char getRotation(void) {
  return rotation;
}


void setRotation(unsigned char x) {
  rotation = (x & 3);
  switch(rotation) {
   case 0:
   case 2:
    _width  = WIDTH;
    _height = HEIGHT;
    break;
   case 1:
   case 3:
    _width  = HEIGHT;
    _height = WIDTH;
    break;
  }
}

*/
// Return the size of the display (per current rotation)
int width(void) {
//  return _width;
	return WIDTH;
}
 
int height(void) {
//  return _height;
	return HEIGHT;
}

// Draw a goal
void drawGoalBottom(int x, int y,
                int w, int h,
                unsigned int color) {
  //drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}

// Draw a goal
void drawGoalTop(int x, int y,
                int w, int h,
                unsigned int color) {
  drawFastHLine(x, y, w, color);
  //drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}

// Draw a goal
void drawGoalLeft(int x, int y,
                int w, int h,
                unsigned int color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  //drawFastVLine(x+w-1, y, h, color);
}

// Draw a goal
void drawGoalRight(int x, int y,
                int w, int h,
                unsigned int color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  //drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}


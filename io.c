/*
 * io.c - 
 */

#include <io.h>

#include <types.h>

/**************/
/** Screen  ***/
/**************/

#define NUM_COLUMNS 80
#define NUM_ROWS    25

Byte x, y=19;

/* Read a byte from 'port' */
Byte inb (unsigned short port)
{
  Byte v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
  return v;
}
void scroll() {
  Word *screen = (Word *)0xb8000;
  for(int i_y = 1; i_y < NUM_ROWS; i_y++) { // Move one row up
    for(int i_x = 0; i_x < NUM_COLUMNS; i_x++) {
      if(i_y == 3) {                                                                                            // COMMENT FOR REMOVAL IF NEEDED
        if(i_x < NUM_COLUMNS - 5) screen[(i_y - 1) * NUM_COLUMNS + i_x + 5] = screen[i_y  * NUM_COLUMNS + i_x]; // OFFSET FOR BETTER 
      }                                                                                                         // VISUALIZATION OF KEYPRESS CHAR
      else screen[(i_y - 1) * NUM_COLUMNS + i_x] = screen[i_y  * NUM_COLUMNS + i_x]; 
    }
  }

  for(int i_x = 0; i_x < NUM_COLUMNS; i_x++) { // Leave last row blank
    screen[(NUM_ROWS - 1) * NUM_COLUMNS + i_x] = 0; 
  }

  --y;

}

void printc(char c)
{
  __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c)); /* Magic BOCHS debug: writes 'c' to port 0xe9 */
  if (c=='\n')
  {
    x = 0;
    y=(y+1);
  }
  else
  {
  Word ch = (Word) (c & 0x00FF) | 0x0200;
	Word *screen = (Word *)0xb8000;
	screen[(y * NUM_COLUMNS + x)] = ch;
    if (++x >= NUM_COLUMNS)
    {
      x = 0;
      y=(y+1);
    }
  }
  if( y >= NUM_ROWS) scroll();
}

void printc_xy(Byte mx, Byte my, char c)
{
  Byte cx, cy;
  cx=x;
  cy=y;
  x=mx;
  y=my;
  printc(c);
  x=cx;
  y=cy;
}

void printk(char *string)
{
  int i;
  for (i = 0; string[i]; i++)
    printc(string[i]);
}

void printc_color(char c, char color)
{
  __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c)); /* Magic BOCHS debug: writes 'c' to port 0xe9 */
  if (c=='\n')
  {
    x = 0;
    y=(y+1);
  }
  else
  {
  Word ch = (Word) (c & 0x00FF) | (Word) (color << 8);
	Word *screen = (Word *)0xb8000;
	screen[(y * NUM_COLUMNS + x)] = ch;
    if (++x >= NUM_COLUMNS)
    {
      x = 0;
      y=(y+1);
    }
  }
  if( y >= NUM_ROWS) scroll();
}

void printk_color(char *string, char color)
{
  int i;
  for (i = 0; string[i]; i++)
    printc_color(string[i], color);
}

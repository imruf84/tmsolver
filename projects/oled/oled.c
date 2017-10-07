#include <glib.h>
#include <glib/gprintf.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>
#include <stdbool.h>

#include "ssd1306.h"
#include "font5x7.h"

#define rows 8
#define cols 21
char char_buffer[rows][cols];
struct display_info disp;


void drawchar(int x, int y, char c)
{
    if (x > 20) return;
    if (y > 7) return;
	if (c == '\n') return;

    for (int i = 0; i < 5; i++)
    {
        disp.buffer[y*128+x*6+i] = font[i+c*5];
    }
}

void drawtext(int x, int y, char *text)
{
    for (int i = 0; i < strlen(text); i++)
    {
        drawchar(x+i, y, text[i]);
    }
}

void update_display_by_char_buffer()
{
	for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			drawchar(i, j, char_buffer[j][i]);
		}
	}

	ssd1306_send_buffer(&disp);
}

void scrolltop()
{
	// Szkrollozás felfelé.
	for (int i = 0; i < cols; i++)
	{
		for (int j = 1; j < rows; j++)
		{
			char_buffer[j-1][i] = char_buffer[j][i];
		}
	}
	// Üres sor létrehozása a képernyő alján.
	for (int i = 0; i < cols; i++)
	{
		char_buffer[rows-1][i] = ' ';
	}
}


int cursor = 0;
void writetext(char *text, bool wrap)
{
	for (int i = 0; i < strlen(text); i++)
	{
		if ('\n' == text[i])
		{
			scrolltop();
			cursor = 0;
			continue;
		}

		char_buffer[rows-1][cursor++] = text[i];

		if (cursor > 20)
		{
			if (wrap)
			{
				scrolltop();
				cursor = 0;
			}
			else
			{
				break;
			}
		}

	}

	update_display_by_char_buffer();
}


int main(int argc, char** argv)
{

	char filename[] = "/dev/i2c-0";
	const gchar* errmsg;

	memset(&disp, 0, sizeof(disp));

	disp.address = SSD1306_I2C_ADDR;

	if (ssd1306_open(&disp, filename) < 0 || ssd1306_init(&disp) < 0)
	{
		printf("\nERROR: %s\n\n", g_strerror(errno));
	}

	// Karakterbuffer inicializálása.
	for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			char_buffer[j][i] = ' ';
		}
	}
	update_display_by_char_buffer();

	while (1)
	{
		char c[4096];
		int len = 0;
		while((len = read(STDIN_FILENO, c, sizeof(c))) > 0)
		{
			c[len] = '\0';
			writetext(c, true);
		}
        usleep(1000);
	}

	return EXIT_SUCCESS;
}

/*
 * This file is part of NativeOS: next-gen x86 operating system
 * Copyright (C) 2017-2018 Izan Beltrán <izanbf1803@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * File: driver/keyboard_map.c
 * Description: keyboard map to values.
 */

#include <driver/keyboard.h>

void keyboard_map()
{
	int i;
	for (i = 0; i < 128; i++) {
		keyboard_dict[i] = 
		keyboard_dict_altgr[i] = 
		keyboard_dict_shift[i] = 
		keyboard_dict_mayus[i] = '\0';
	}
	
	keyboard_dict[2] = '1';
	keyboard_dict[3] = '2';
	keyboard_dict[4] = '3';
	keyboard_dict[5] = '4';
	keyboard_dict[6] = '5';
	keyboard_dict[7] = '6';
	keyboard_dict[8] = '7';
	keyboard_dict[9] = '8';
	keyboard_dict[10] = '9';
	keyboard_dict[11] = '0';
	keyboard_dict[12] = '\'';
	keyboard_dict[13] = '¡';
	keyboard_dict[16] = 'q';
	keyboard_dict[17] = 'w';
	keyboard_dict[18] = 'e';
	keyboard_dict[19] = 'r';
	keyboard_dict[20] = 't';
	keyboard_dict[21] = 'y';
	keyboard_dict[22] = 'u';
	keyboard_dict[23] = 'i';
	keyboard_dict[24] = 'o';
	keyboard_dict[25] = 'p';
	keyboard_dict[26] = '`';
	keyboard_dict[27] = '+';
	keyboard_dict[30] = 'a';
	keyboard_dict[31] = 's';
	keyboard_dict[32] = 'd';
	keyboard_dict[33] = 'f';
	keyboard_dict[34] = 'g';
	keyboard_dict[35] = 'h';
	keyboard_dict[36] = 'j';
	keyboard_dict[37] = 'k';
	keyboard_dict[38] = 'l';
	keyboard_dict[39] = 'ñ';
	keyboard_dict[40] = '´';
	keyboard_dict[43] = 'ç';
    keyboard_dict[44] = 'z';
    keyboard_dict[45] = 'x';
    keyboard_dict[46] = 'c';
    keyboard_dict[47] = 'v';
    keyboard_dict[48] = 'b';
    keyboard_dict[49] = 'n';
    keyboard_dict[50] = 'm';
    keyboard_dict[51] = ',';
    keyboard_dict[52] = '.';
    keyboard_dict[53] = '-';
    keyboard_dict[57] = ' ';    // Space

	keyboard_dict_shift[16] = 'Q';
	keyboard_dict_shift[17] = 'W';
	keyboard_dict_shift[18] = 'E';
	keyboard_dict_shift[19] = 'R';
	keyboard_dict_shift[20] = 'T';
	keyboard_dict_shift[21] = 'Y';
	keyboard_dict_shift[22] = 'U';
	keyboard_dict_shift[23] = 'I';
	keyboard_dict_shift[24] = 'O';
	keyboard_dict_shift[25] = 'P';
	keyboard_dict_shift[30] = 'A';
	keyboard_dict_shift[31] = 'S';
	keyboard_dict_shift[32] = 'D';
	keyboard_dict_shift[33] = 'F';
	keyboard_dict_shift[34] = 'G';
	keyboard_dict_shift[35] = 'H';
	keyboard_dict_shift[36] = 'J';
	keyboard_dict_shift[37] = 'K';
	keyboard_dict_shift[38] = 'L';
	keyboard_dict_shift[39] = 'Ñ';
	keyboard_dict_shift[43] = 'Ç';
    keyboard_dict_shift[44] = 'Z';
    keyboard_dict_shift[45] = 'X';
    keyboard_dict_shift[46] = 'C';
    keyboard_dict_shift[47] = 'V';
    keyboard_dict_shift[48] = 'B';
    keyboard_dict_shift[49] = 'N';
    keyboard_dict_shift[50] = 'M';
	keyboard_dict_shift[2] = '!';
	keyboard_dict_shift[3] = '"';
	keyboard_dict_shift[4] = '·';
	keyboard_dict_shift[5] = '$';
	keyboard_dict_shift[6] = '%';
	keyboard_dict_shift[7] = '&';
	keyboard_dict_shift[8] = '/';
	keyboard_dict_shift[9] = '(';
	keyboard_dict_shift[10] = ')';
	keyboard_dict_shift[11] = '=';
	keyboard_dict_shift[12] = '\'';
	keyboard_dict_shift[13] = '¿';
	keyboard_dict_shift[26] = '^';
	keyboard_dict_shift[27] = '*';
	keyboard_dict_shift[40] = '¨';
    keyboard_dict_shift[51] = ';';
    keyboard_dict_shift[52] = ':';
    keyboard_dict_shift[53] = '_';

	keyboard_dict_mayus[16] = 'Q';
	keyboard_dict_mayus[17] = 'W';
	keyboard_dict_mayus[18] = 'E';
	keyboard_dict_mayus[19] = 'R';
	keyboard_dict_mayus[20] = 'T';
	keyboard_dict_mayus[21] = 'Y';
	keyboard_dict_mayus[22] = 'U';
	keyboard_dict_mayus[23] = 'I';
	keyboard_dict_mayus[24] = 'O';
	keyboard_dict_mayus[25] = 'P';
	keyboard_dict_mayus[30] = 'A';
	keyboard_dict_mayus[31] = 'S';
	keyboard_dict_mayus[32] = 'D';
	keyboard_dict_mayus[33] = 'F';
	keyboard_dict_mayus[34] = 'G';
	keyboard_dict_mayus[35] = 'H';
	keyboard_dict_mayus[36] = 'J';
	keyboard_dict_mayus[37] = 'K';
	keyboard_dict_mayus[38] = 'L';
	keyboard_dict_mayus[39] = 'Ñ';
	keyboard_dict_mayus[43] = 'Ç';
    keyboard_dict_mayus[44] = 'Z';
    keyboard_dict_mayus[45] = 'X';
    keyboard_dict_mayus[46] = 'C';
    keyboard_dict_mayus[47] = 'V';
    keyboard_dict_mayus[48] = 'B';
    keyboard_dict_mayus[49] = 'N';
    keyboard_dict_mayus[50] = 'M';

	keyboard_dict_altgr[2] = '|';
	keyboard_dict_altgr[3] = '@';
	keyboard_dict_altgr[4] = '#';
	keyboard_dict_altgr[5] = '~';
	keyboard_dict_altgr[6] = '½';
	keyboard_dict_altgr[7] = '¬';
	keyboard_dict_altgr[26] = '[';
	keyboard_dict_altgr[27] = ']';
	keyboard_dict_altgr[40] = '{';
	keyboard_dict_mayus[43] = '}';
}
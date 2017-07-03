/*
 * This file is part of NativeOS: next-gen x86 operating system
 * Copyright (C) 2015-2016 Dani Rodríguez, 2017-2018 Izan Beltrán <izanbf1803@gmail.com>
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
 * File: driver/keyboard.h
 * Description: keyboard controller
 */

#ifndef DRIVER_KEYBOARD_H_
#define DRIVER_KEYBOARD_H_

#define KEY_ESC 1
#define KEY_RET 14
#define KEY_TAB 15
#define KEY_INSERT 28
#define KEY_CTRL 29
#define KEY_SHIFTL 42
#define KEY_SHIFTR 54
#define KEY_ALTGR 56
#define KEY_BLOQ_MAYUS 58 
#define KEY_UP 72
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_DOWN 80

char keyboard_dict[128];
char keyboard_dict_shift[128];
char keyboard_dict_mayus[128];
char keyboard_dict_altgr[128];

void keyboard_map();
void keyboard_init(void);

#endif // DRIVER_KEYBOARD_H_

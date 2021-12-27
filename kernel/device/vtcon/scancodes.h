/**
 * \file
 * \brief Scancodes
 *
 * Not the prettiest way to do this at the moment, but this file contains
 * the translation tables used to convert between a scancode and a virtual
 * key in the different keyboard layouts that are supported at the moment.
 */
#pragma once

/* Non printable characters from the ASCII table. */
#define VK_NUL 0
#define VK_SOH 1
#define VK_STX 2
#define VK_ETX 3
#define VK_EOT 4
#define VK_ENQ 5
#define VK_ACK 6
#define VK_BEL 7
#define VK_BS 8
#define VK_HT 9
#define VK_LF 10
#define VK_VT 11
#define VK_FF 12
#define VK_CR 13
#define VK_SO 14
#define VK_SI 15
#define VK_DLE 16
#define VK_DC1 17
#define VK_DC2 18
#define VK_DC3 19
#define VK_DC4 20
#define VK_NAK 21
#define VK_SYN 22
#define VK_ETB 23
#define VK_CAN 24
#define VK_EM 25
#define VK_SUB 26
#define VK_ESC 27
#define VK_FS 28
#define VK_GS 29
#define VK_RS 30
#define VK_US 31
#define VK_SP 32
#define VK_DEL 127
#define VK_CUP 224
#define VK_CLFT 225
#define VK_CRGT 226
#define VK_CDWN 227
#define VK_PGUP 228
#define VK_PGDN 229
#define VK_INSERT 230
#define VK_HOME 231
#define VK_END 232
#define VK_LCTL 233
#define VK_RCTL 234
#define VK_LSHF 235
#define VK_RSHF 236
#define VK_LALT 237
#define VK_RALT 238
#define VK_CAPL 239
#define VK_NUML 240
#define VK_SCRL 241
#define VK_BTAB 242
#define VK_F1 243
#define VK_F2 244
#define VK_F3 245
#define VK_F4 246
#define VK_F5 247
#define VK_F6 248
#define VK_F7 249
#define VK_F8 250
#define VK_F9 251
#define VK_F10 252
#define VK_F11 253
#define VK_F12 254

typedef struct kbdev {
	unsigned short vk;
	unsigned short flags;
} kbdev_t;

typedef struct scancode {
	unsigned char nomod; /**< Mapping for no mod is down. */
	unsigned char shift; /**< Mapping for SHIFT is on. */
	unsigned char ctrl; /**< Mapping for CTRL is on. */
	unsigned char ctrlshift; /**< Mapping for CTRL-SHIFT is on. */
	unsigned char alt; /**< Mapping for ALT is on. */
	unsigned char altshift; /**< Mapping for SHIFT-ALT is on. */
	unsigned char altctrl; /**< Mapping for CTRL-ALT is on. */
	unsigned char altctrlshift; /**< Mapping for CTRL-SHIFT-ALT is on. */
} scancode_t;

static struct scancode us_scancodes_1[] = {
    {VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL},
    {VK_ESC, VK_ESC, VK_ESC, VK_ESC, VK_ESC, VK_ESC, VK_ESC, VK_ESC},
    {'1', '!', VK_NUL, VK_NUL, '1', '!', VK_NUL, VK_NUL},
    {'2', '@', VK_NUL, VK_NUL, '1', '@', VK_NUL, VK_NUL},
    {'3', '#', VK_NUL, VK_NUL, '1', '#', VK_NUL, VK_NUL},
    {'4', '$', VK_NUL, VK_NUL, '1', '$', VK_NUL, VK_NUL},
    {'5', '%', VK_NUL, VK_NUL, '1', '%', VK_NUL, VK_NUL},
    {'6', '^', VK_NUL, VK_NUL, '1', '^', VK_NUL, VK_NUL},
    {'7', '&', VK_NUL, VK_NUL, '1', '&', VK_NUL, VK_NUL},
    {'8', '*', VK_NUL, VK_NUL, '1', '*', VK_NUL, VK_NUL},
    {'9', '(', VK_NUL, VK_NUL, '1', '(', VK_NUL, VK_NUL},
    {'0', ')', VK_NUL, VK_NUL, '1', ')', VK_NUL, VK_NUL},
    {'-', '_', VK_US, VK_US, '-', '_', VK_US, VK_US},
    {'=', '+', VK_NUL, VK_NUL, '=', '+', VK_NUL, VK_NUL},
    {VK_BS, VK_BS, VK_DEL, VK_DEL, VK_BS, VK_BS, VK_DEL, VK_DEL},
    {VK_HT, VK_BTAB, VK_NUL, VK_NUL, VK_VT, VK_BTAB, VK_NUL, VK_NUL},
    {'q', 'Q', VK_DC1, VK_DC1, 'q', 'Q', VK_DC1, VK_DC1},
    {'w', 'W', VK_ETB, VK_ETB, 'w', 'W', VK_ETB, VK_ETB},
    {'e', 'E', VK_ENQ, VK_ENQ, 'e', 'E', VK_ENQ, VK_ENQ},
    {'r', 'R', VK_DC2, VK_DC2, 'r', 'R', VK_DC2, VK_DC2},
    {'t', 'T', VK_DC4, VK_DC4, 't', 'T', VK_DC4, VK_DC4},
    {'y', 'Y', VK_EM, VK_EM, 'y', 'Y', VK_EM, VK_EM},
    {'u', 'U', VK_NAK, VK_NAK, 'u', 'U', VK_NAK, VK_NAK},
    {'i', 'I', VK_HT, VK_HT, 'i', 'I', VK_HT, VK_HT},
    {'o', 'O', VK_SI, VK_SI, 'o', 'O', VK_SI, VK_SI},
    {'p', 'P', VK_DLE, VK_DLE, 'p', 'P', VK_DLE, VK_DLE},
    {'[', '{', VK_ESC, VK_ESC, '[', '{', VK_ESC, VK_ESC},
    {']', '}', VK_GS, VK_GS, ']', '}', VK_GS, VK_GS},
    {VK_LF, VK_LF, VK_CR, VK_CR, VK_LF, VK_LF, VK_CR, VK_CR},
    {VK_LCTL, VK_LCTL, VK_LCTL, VK_LCTL, VK_LCTL, VK_LCTL, VK_LCTL, VK_LCTL},
    {'a', 'A', VK_SOH, VK_SOH, 'a', 'A', VK_SOH, VK_SOH},
    {'s', 'S', VK_DC3, VK_DC3, 's', 'S', VK_DC3, VK_DC3},
    {'d', 'D', VK_EOT, VK_EOT, 'd', 'D', VK_EOT, VK_EOT},
    {'f', 'F', VK_ACK, VK_ACK, 'f', 'F', VK_ACK, VK_ACK},
    {'g', 'G', VK_BEL, VK_BEL, 'g', 'G', VK_BEL, VK_BEL},
    {'h', 'H', VK_BS, VK_BS, 'h', 'H', VK_BS, VK_BS},
    {'j', 'J', VK_LF, VK_LF, 'j', 'J', VK_LF, VK_LF},
    {'k', 'K', VK_VT, VK_VT, 'k', 'K', VK_VT, VK_VT},
    {'l', 'L', VK_FF, VK_FF, 'l', 'L', VK_FF, VK_FF},
    {';', ':', VK_NUL, VK_NUL, ';', ':', VK_NUL, VK_NUL},
    {'\'', '"', VK_NUL, VK_NUL, '\'', '"', VK_NUL, VK_NUL},
    {'`', '~', VK_NUL, VK_NUL, '`', '~', VK_NUL, VK_NUL},
    {VK_LSHF, VK_LSHF, VK_LSHF, VK_LSHF, VK_LSHF, VK_LSHF, VK_LSHF, VK_LSHF},
    {'\\', '|', VK_FS, VK_FS, '\\', '|', VK_FS, VK_FS},
    {'z', 'Z', VK_SUB, VK_SUB, 'z', 'Z', VK_SUB, VK_SUB},
    {'x', 'X', VK_CAN, VK_CAN, 'x', 'X', VK_CAN, VK_CAN},
    {'c', 'C', VK_ETX, VK_ETX, 'c', 'C', VK_ETX, VK_ETX},
    {'v', 'V', VK_SYN, VK_SYN, 'v', 'V', VK_SYN, VK_SYN},
    {'b', 'B', VK_STX, VK_STX, 'b', 'B', VK_STX, VK_STX},
    {'n', 'N', VK_SO, VK_SO, 'n', 'N', VK_SO, VK_SO},
    {'m', 'M', VK_CR, VK_CR, 'm', 'M', VK_CR, VK_CR},
    {',', '<', VK_NUL, VK_NUL, ',', '<', VK_NUL, VK_NUL},
    {'.', '>', VK_NUL, VK_NUL, '.', '>', VK_NUL, VK_NUL},
    {'/', '?', VK_NUL, VK_NUL, '/', '?', VK_NUL, VK_NUL},
    {VK_RSHF, VK_RSHF, VK_RSHF, VK_RSHF, VK_RSHF, VK_RSHF, VK_RSHF, VK_RSHF},
    {'*', '*', '*', '*', '*', '*', '*', '*'},
    {VK_LALT, VK_LALT, VK_LALT, VK_LALT, VK_LALT, VK_LALT, VK_LALT, VK_LALT},
    {' ', ' ', VK_NUL, ' ', ' ', ' ', VK_NUL, ' '},
    {VK_CAPL, VK_CAPL, VK_CAPL, VK_CAPL, VK_CAPL, VK_CAPL, VK_CAPL, VK_CAPL},
    {VK_F1, VK_F1, VK_F1, VK_F1, VK_F1, VK_F1, VK_F1, VK_F1},
    {VK_F2, VK_F2, VK_F2, VK_F2, VK_F2, VK_F2, VK_F2, VK_F2},
    {VK_F3, VK_F3, VK_F3, VK_F3, VK_F3, VK_F3, VK_F3, VK_F3},
    {VK_F4, VK_F4, VK_F4, VK_F4, VK_F4, VK_F4, VK_F4, VK_F4},
    {VK_F5, VK_F5, VK_F5, VK_F5, VK_F5, VK_F5, VK_F5, VK_F5},
    {VK_F6, VK_F6, VK_F6, VK_F6, VK_F6, VK_F6, VK_F6, VK_F6},
    {VK_F7, VK_F7, VK_F7, VK_F7, VK_F7, VK_F7, VK_F7, VK_F7},
    {VK_F8, VK_F8, VK_F8, VK_F8, VK_F8, VK_F8, VK_F8, VK_F8},
    {VK_F9, VK_F9, VK_F9, VK_F9, VK_F9, VK_F9, VK_F9, VK_F9},
    {VK_F10, VK_F10, VK_F10, VK_F10, VK_F10, VK_F10, VK_F10, VK_F10},
    {VK_NUML, VK_NUML, VK_NUML, VK_NUML, VK_NUML, VK_NUML, VK_NUML, VK_NUML},
    {VK_SCRL, VK_SCRL, VK_SCRL, VK_SCRL, VK_SCRL, VK_SCRL, VK_SCRL, VK_SCRL},
    {'7', '7', '7', '7', '7', '7', '7', '7'},
    {'8', '8', '8', '8', '8', '8', '8', '8'},
    {'9', '9', '9', '9', '9', '9', '9', '9'},
    {'-', '-', '-', '-', '-', '-', '-', '-'},
    {'4', '4', '4', '4', '4', '4', '4', '4'},
    {'5', '5', '5', '5', '5', '5', '5', '5'},
    {'6', '6', '6', '6', '6', '6', '6', '6'},
    {'+', '+', '+', '+', '+', '+', '+', '+'},
    {'1', '1', '1', '1', '1', '1', '1', '1'},
    {'2', '2', '2', '2', '2', '2', '2', '2'},
    {'3', '3', '3', '3', '3', '3', '3', '3'},
    {'0', '0', '0', '0', '0', '0', '0', '0'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL},
    {VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL},
    {VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL},
    {VK_F11, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL},
    {VK_F12, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL, VK_NUL},
    {0, 0, 0, 0, 0, 0, 0, 0},
    /* TODO: Add 0xE0 and 0xF0. */
};

#pragma once

#include <stdint.h>

void port_out_byte(uint16_t port, uint8_t value);
void port_out_word(uint16_t port, uint16_t value);
void port_out_long(uint16_t port, uint32_t value);
uint8_t port_in_byte(uint16_t port);
uint16_t port_in_word(uint16_t port);
uint32_t port_in_long(uint16_t port);
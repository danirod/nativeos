#pragma once

#include <stddef.h>

/**
 * @brief Copies at most len characters from src into dst.
 * @param dst the target string where to put the copied characters.
 * @param src the source string to copy characters from.
 * @param len the maximum amount of characters to copy.
 * @return a pointer to dst string.
 */
char* strncpy(char* dst, const char* src, size_t len);

/**
 * @brief Copies the string src into dst.
 * @param dst the target string where to put the copied characters.
 * @param src the source string to copy characters form.
 * @return a pointer to dst string.
 */
char* strcpy(char* dst, const char* src);

/**
 * @brief Computes the length of the string s.
 * @param s the string whose length we want to compute.
 * @return the computed length of the string s.
 */
size_t strlen(const char* s);

/**
 * @brief Computes the length of the string s as long as doesn't exceed maxlen.
 * @param s the string whose length we want to compute.
 * @param maxlen the maximum amount of characters to compute.
 * @return the computed length of the string s or maxlen if it's larger.
 */
size_t strnlen(const char *s, size_t maxlen);

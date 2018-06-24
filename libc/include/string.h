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

/**
 * @brief Compare lexicographically both strings.
 * @param s1 the first string to compare
 * @param s2 the second string to compare
 * @return <0 if s1 > s2, >0 if s1 < s2, 0 if s1 == s2
 */
int strcmp(const char *s1, const char *s2);

/**
 * @brief Compare lexicographically no more than n characters from both strings.
 * @param s1 the first string to compare
 * @param s2 the second string to compare
 * @param n the maximum number of characters to compare
 * @return <0 if s1 > s2, >0 if s1 < s2, 0 if s1 == s2
 */
int strncmp(const char *s1, const char *s2, size_t n);


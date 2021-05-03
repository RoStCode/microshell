
#ifndef TEST_FUNC_H
#define TEST_FUNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unity.h"

#define TEST_FUNC_IO_BUFFER_SIZE    1024

extern char g_write_buf[];
extern char g_read_buf[];

extern uint8_t g_file_buffer_buf[];

extern struct ush_object g_ush;

void test_func_init(void);
void test_func_write(const char *text);
void test_func_read_all(void);
void test_func_ask(const char *request, const char *response);

#ifdef __cplusplus
}
#endif

#endif /* TEST_FUNC_H */

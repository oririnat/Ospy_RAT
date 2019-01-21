#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define HASH_LEN 34
 
// leftrotate function definition
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))


char * md5(const uint8_t * initial_msg, size_t initial_len);
/*
Copyright (c) 2025 Kurt Manrique-Nino

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ARGPARSING_C
#define ARGPARSING_C

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct ArgParsing_C ArgParsing_C;

union data_C {
    char* text;
    union intdata{
        uint64_t number_u64;
        int64_t  number_i64;
    } intdata;
    bool flag;
};


enum APDataType_C {
    TEXT         = 0x00000001,
    FLAG         = 0x00000002,
    UNSIGNED_INT = 0x00000004,
    SIGNED_INT   = 0x00000008,
};
typedef enum APDataType_C APDataType_C;


struct APTableEntry_C {
    char* abbr_form;
    char* full_form;
    union data_C data;
    APDataType_C data_type;
    bool required;
    bool initialized;
};
typedef struct APTableEntry_C APTableEntry_C;


ArgParsing_C* ArgParsing_C_get_instance();
void ArgParsing_C_set_input_args(ArgParsing_C*, int, char**);
int ArgParsing_C_set_arg_table(ArgParsing_C*, APTableEntry_C*, size_t);
int ArgParsing_C_parse(ArgParsing_C*);
size_t ArgParsing_C_get_arg_value_bytesize(ArgParsing_C*, const char*, bool);
int ArgParsing_C_get_value_TEXT(ArgParsing_C*, const char*, bool, char*, size_t);
bool ArgParsing_C_get_value_FLAG(ArgParsing_C*, const char*, bool);
uint64_t ArgParsing_C_get_value_UNSIGNED_INT(ArgParsing_C*, const char*, bool);
int64_t ArgParsing_C_get_value_SIGNED_INT(ArgParsing_C*, const char*, bool);


#ifdef __cplusplus
}
#endif

#endif // #ifndef ARGPARSING_C
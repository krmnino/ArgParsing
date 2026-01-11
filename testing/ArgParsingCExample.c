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

#include <stdio.h>
#include <stdlib.h>

#include "../src/ArgParsing_C.h"


#define MAX_BUFF_LEN 512


int main(int argc, char* argv[]){
    int ret;
    char str_buff1[MAX_BUFF_LEN];
    char* str_buff2;
    size_t arg_txt_len;

    ArgParsing_C* ap = ArgParsing_C_get_instance();

    ArgParsing_C_set_input_args(ap, argc, argv);

    APTableEntry_C arg_table[] = {
        { .abbr_form="a", .full_form="arg1", .initialized=false, .data_type=FLAG         , .required=true , .default_value=false, .data={0}  },
        { .abbr_form="z", .full_form="arg2", .initialized=false, .data_type=TEXT         , .required=true , .default_value=false, .data={0}  },
        { .abbr_form="" , .full_form="arg3", .initialized=false, .data_type=UNSIGNED_INT , .required=false, .default_value=false, .data={0}  },
        { .abbr_form="9", .full_form="arg4", .initialized=false, .data_type=FLAG         , .required=false, .default_value=false, .data={0}  },
        { .abbr_form="m", .full_form="arg5", .initialized=false, .data_type=UNSIGNED_INT , .required=false, .default_value=true , .data.number_i64=123 },
        { .abbr_form="n", .full_form="arg6", .initialized=false, .data_type=TEXT         , .required=false, .default_value=true , .data.text="initial_value" },
        { .abbr_form="x", .full_form="arg7", .initialized=false, .data_type=FLAG         , .required=false, .default_value=true , .data.flag=true },
        { .abbr_form="y", .full_form="arg8", .initialized=false, .data_type=TEXT         , .required=false, .default_value=true , .data.text="init text" },
    };

    ret = ArgParsing_C_set_arg_table(ap, arg_table, sizeof(arg_table) / sizeof(APTableEntry_C));
    if(ret != 0){
        printf("ERROR: ArgParsing_C_set_arg_table() return code -> %d\n", ret);
        return -1;
    }
    
    ret = ArgParsing_C_parse(ap);
    if(ret != 0){
        printf("ERROR: ArgParsing_C_parse() return code -> %d\n", ret);
        return -1;
    }

    printf("a/arg1: %d\n", ArgParsing_C_get_value_FLAG(ap, "arg1", false));
    arg_txt_len = ArgParsing_C_get_arg_value_bytesize(ap, "arg1", false);
    printf("a/arg1 byte_size: %ld\n", arg_txt_len);
    
    memset((void*)&str_buff1, 0, MAX_BUFF_LEN);
    ArgParsing_C_get_value_TEXT(ap, "z", true, (char*)&str_buff1, MAX_BUFF_LEN);
    printf("z/arg2: %s\n", str_buff1);
    arg_txt_len = ArgParsing_C_get_arg_value_bytesize(ap, "z", true);
    printf("z/arg2 byte_size: %ld\n", arg_txt_len);

    printf("  arg3: %ld\n", ArgParsing_C_get_value_UNSIGNED_INT(ap, "arg3", false));
    arg_txt_len = ArgParsing_C_get_arg_value_bytesize(ap, "arg3", false);
    printf("  arg3 byte_size: %ld\n", arg_txt_len);

    printf("9/arg4: %d\n", ArgParsing_C_get_value_FLAG(ap, "9", true));
    arg_txt_len = ArgParsing_C_get_arg_value_bytesize(ap, "9", true);
    printf("9/arg4 byte_size: %ld\n", arg_txt_len);

    printf("m/arg5: %ld\n", ArgParsing_C_get_value_UNSIGNED_INT(ap, "m", true));
    arg_txt_len = ArgParsing_C_get_arg_value_bytesize(ap, "m", true);
    printf("m/arg5 byte_size: %ld\n", arg_txt_len);

    str_buff2 = (char*)calloc(arg_txt_len + 1, sizeof(char));
    ArgParsing_C_get_value_TEXT(ap, "n", true, str_buff2, MAX_BUFF_LEN);
    printf("n/arg6: %s\n", str_buff2);
    arg_txt_len = ArgParsing_C_get_arg_value_bytesize(ap, "m", true);
    printf("n/arg6 byte_size: %ld\n", arg_txt_len);
    free(str_buff2);

    printf("x/arg7: %d\n", ArgParsing_C_get_value_FLAG(ap, "x", true));
    arg_txt_len = ArgParsing_C_get_arg_value_bytesize(ap, "x", true);
    printf("x/arg7 byte_size: %ld\n", arg_txt_len);

    memset((void*)&str_buff1, 0, MAX_BUFF_LEN);
    ArgParsing_C_get_value_TEXT(ap, "arg8", false, (char*)&str_buff1, MAX_BUFF_LEN);
    printf("y/arg8: %s\n", str_buff1);
    arg_txt_len = ArgParsing_C_get_arg_value_bytesize(ap, "z", true);
    printf("y/arg8 byte_size: %ld\n", arg_txt_len);

    return 0;
}
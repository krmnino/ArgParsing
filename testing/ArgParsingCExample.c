#include <stdio.h>

#include "../src/ArgParsing_C.h"

int main(int argc, char* argv[]){
    int ret;

    ArgParsing_C* ap = ArgParsing_C_get_instance();

    ArgParsing_C_set_input_args(ap, argc, argv);

    APTableEntry_C arg_table[] = {
        { .abbr_form = "a", .full_form = "arg1", .data_type=FLAG        , .required=true  },
        { .abbr_form = "x", .full_form = "arg2", .data_type=TEXT        , .required=false },
        { .abbr_form = "" , .full_form = "arg3", .data_type=UNSIGNED_INT, .required=false },
        { .abbr_form = "9", .full_form = "arg4", .data_type=FLAG        , .required=false },
        { .abbr_form = "z", .full_form = "arg5", .data_type=TEXT        , .required=true  },
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

    printf("a/arg1: %d\n" , ArgParsing_C_get_value_FLAG(ap, "arg1", false));
    printf("x/arg2: %s\n" , ArgParsing_C_get_value_TEXT(ap, "x", true));
    printf("  arg3: %ld\n", ArgParsing_C_get_value_UNSIGNED_INT(ap, "arg3", false));
    printf("9/arg4: %d\n" , ArgParsing_C_get_value_FLAG(ap, "9", true));
    printf("z/arg5: %s\n" , ArgParsing_C_get_value_TEXT(ap, "z", true));

    return 0;
}
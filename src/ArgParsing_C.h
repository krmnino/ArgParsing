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
const char* ArgParsing_C_get_value_TEXT(ArgParsing_C*, const char*, bool);
bool ArgParsing_C_get_value_FLAG(ArgParsing_C*, const char*, bool);
uint64_t ArgParsing_C_get_value_UNSIGNED_INT(ArgParsing_C*, const char*, bool);
int64_t ArgParsing_C_get_value_SIGNED_INT(ArgParsing_C*, const char*, bool);

#ifdef __cplusplus
}
#endif

#endif // #ifndef ARGPARSING_C
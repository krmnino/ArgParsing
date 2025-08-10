#ifndef UTILS
#define UTILS

#include "../src/ArgParsing.hpp"
#include "../res/Randomizer.hpp"

#define MAX_ABBR_FORM_ID_LEN 1
#define MAX_FULL_FORM_ID_LEN 10
#define BUILD_MAX_ATTEMPTS 1000

#define PRT_IDX_STR_WIDTH 5
#define PRT_ABBR_FORM_STR_WIDTH 10
#define PRT_FULL_FORM_STR_WIDTH 10
#define PRT_DATA_TYPE_STR_WIDTH 9
#define PRT_REQUIRED_STR_WIDTH 8
#define PRT_INITIALIZED_STR_WIDTH 11
#define PRT_VALUE_STR_WIDTH 64

enum class Scenarios {
    OK                    =  0x00000001,
    MISSING_FIRST_DASH    =  0x00000002,
    MISSING_REQUIRED_ARG  =  0x00000004,
    UNKNOWN_ARGUMENT      =  0x00000008,
    REPEATED_ARGUMENT     =  0x00000010,
    MUST_BE_FLAG          =  0x00000020,
    BAD_NUMERIC_VALUE     =  0x00000040,
    EMPTY_ARG_LIST        =  0x00000080,
    VALID_FLAG_GROUP      =  0x00000100,
    INVALID_FLAG_GROUP    =  0x00000200,
};


// ArgTableBuilder.cpp
int build_entry(Randomizer*, std::vector<APTableEntry>&, uint32_t);
int build_arg_table(Randomizer*, std::vector<APTableEntry>&);
bool contains_data_type(std::vector<APTableEntry>&, APDataType);
bool contains_required(std::vector<APTableEntry>&);
size_t count_args_by_type(std::vector<APTableEntry>&, APDataType);
void display_arg_table(std::vector<APTableEntry>&);

// Utils.cpp
std::string space_padding(std::string, size_t, std::string);

#endif
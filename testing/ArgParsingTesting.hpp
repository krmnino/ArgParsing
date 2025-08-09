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

int build_arg_table_entry(Randomizer*, std::vector<APTableEntry>&, uint32_t);
int build_arg_table(Randomizer*, std::vector<APTableEntry>&);
void display_arg_table(std::vector<APTableEntry>&);
std::string space_padding(std::string, size_t, std::string);

#endif
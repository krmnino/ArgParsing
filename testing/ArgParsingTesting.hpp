#ifndef UTILS
#define UTILS

#include "../src/ArgParsing.hpp"
#include "../res/Randomizer.hpp"

#define MAX_ABBR_FORM_ID_LEN 1
#define MAX_FULL_FORM_ID_LEN 10
#define BUILD_MAX_ATTEMPTS 1000

int build_arg_table_entry(Randomizer*, std::vector<APTableEntry>&, uint32_t);
int build_arg_table(Randomizer*, std::vector<APTableEntry>&);
void display_arg_table(std::vector<APTableEntry>&);
std::string space_padding(std::string&, size_t);

#endif
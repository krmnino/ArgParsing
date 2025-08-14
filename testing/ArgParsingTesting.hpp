#ifndef UTILS
#define UTILS

#include "../src/ArgParsing.hpp"
#include "../res/Randomizer.hpp"

#include <sstream>
#include <memory>


#define MAX_ABBR_FORM_ID_LEN 1
#define MAX_FULL_FORM_ID_LEN 10
#define BUILD_MAX_ATTEMPTS 1000
#define MAX_TEXT_ARG_LEN 128

#define PRT_IDX_STR_WIDTH 5
#define PRT_ABBR_FORM_STR_WIDTH 10
#define PRT_FULL_FORM_STR_WIDTH 10
#define PRT_DATA_TYPE_STR_WIDTH 9
#define PRT_REQUIRED_STR_WIDTH 8
#define PRT_INITIALIZED_STR_WIDTH 11
#define PRT_VALUE_STR_WIDTH 64

enum class ScenarioType {
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


class ScenarioData{
    public:
    std::vector<APTableEntry> res_argtab; // Result argument table
    std::vector<APTableEntry> exp_argtab; // Expected argument table
    char** argv;
    size_t n_args;
    int argc;
    ScenarioType type;
    ScenarioData() {}
    ~ScenarioData() {
        if(this->argv != nullptr){
            // Loop through the array deallocating strings one by one
            for(int i = 0; argv[i] != nullptr; i++){
                delete[] this->argv[i];
            }
            // Finally delete the whole array
            delete[] this->argv;
        }
    }
};


class TestcaseData{
    public:
    std::vector<APTableEntry> ini_argtab; // Initial argument table
    ScenarioData* s_arr;
    uint32_t allowed_data_types;
    TestcaseData() {}
    ~TestcaseData() {
        delete[] this->s_arr;
    }
};


// Utils.cpp
std::string space_padding(std::string, size_t, std::string);
std::string describe_argv(int, char**);


// ArgTableBuilder.cpp
int build_initial_arg_table(Randomizer*, TestcaseData&);
int build_entry(Randomizer*, std::vector<APTableEntry>&, uint32_t);
bool contains_data_type(std::vector<APTableEntry>&, APDataType);
bool contains_required(std::vector<APTableEntry>&);
size_t count_args_by_type(std::vector<APTableEntry>&, APDataType);
size_t count_required_args(std::vector<APTableEntry>&);
bool abbr_form_available(std::vector<APTableEntry>&, size_t);
std::string arg_table_to_string(std::vector<APTableEntry>&);


// ScenarioBuilder.cpp
void build_scenario(Randomizer* rnd, ScenarioData&);
void build_OK_scenario(Randomizer* rnd, ScenarioData&);
void build_MISSING_FIRST_DASH_scenario(Randomizer* rnd, std::vector<APTableEntry>&);
void build_MISSING_REQUIRED_ARG_scenario(Randomizer* rnd, std::vector<APTableEntry>&);
void build_UNKNOWN_ARGUMENT_scenario(Randomizer* rnd, std::vector<APTableEntry>&);
void build_REPEATED_ARGUMENT_scenario(Randomizer* rnd, std::vector<APTableEntry>&);
void build_MUST_BE_FLAG_scenario(Randomizer* rnd, std::vector<APTableEntry>&);
void build_BAD_NUMERIC_VALUE_scenario(Randomizer* rnd, std::vector<APTableEntry>&);
void build_EMPTY_ARG_LIST_scenario(Randomizer* rnd, std::vector<APTableEntry>&);
void build_VALID_FLAG_GROUP_scenario(Randomizer* rnd, std::vector<APTableEntry>&);
void build_INVALID_FLAG_GROUP_scenario(Randomizer* rnd, std::vector<APTableEntry>&);
uint32_t check_allowed_scenarios(std::vector<APTableEntry>&, uint32_t);
void vector_to_char_array(std::vector<std::string>&, ScenarioData&);


// Template utility functions
template<typename T> std::string integer_to_hex_string(T num){
    std::stringstream buffer;
    buffer << "0x" << std::hex << num;
    return buffer.str();
}

#endif
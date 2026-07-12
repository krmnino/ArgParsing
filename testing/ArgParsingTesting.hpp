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

#ifndef UTILS
#define UTILS

#include "../src/ArgParsing.hpp"
#include "../res/Randomizer.hpp"
#include "ErrorReporter.hpp"

#include <atomic>
#include <iostream>
#include <memory>
#include <signal.h>
#include <sstream>
#include <unordered_map>


#define MAX_ABBR_FORM_ID_LEN 1
#define MAX_FULL_FORM_ID_LEN 10
#define BUILD_MAX_ATTEMPTS 1000
#define MAX_TEXT_ARG_LEN 128
#define MAX_ARGS 100
#define ALPHANUM_DICT "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

#define PRT_IDX_STR_WIDTH 5
#define PRT_ABBR_FORM_STR_WIDTH 10
#define PRT_FULL_FORM_STR_WIDTH 10
#define PRT_DATA_TYPE_STR_WIDTH 24
#define PRT_REQUIRED_STR_WIDTH 8
#define PRT_DEFAULT_V_STR_WIDTH 9
#define PRT_INITIALIZED_STR_WIDTH 11
#define PRT_VALUE_STR_WIDTH MAX_TEXT_ARG_LEN


static std::unordered_map<std::string, bool> valid_flag_values_dict = {
    {"0"     , false },
    {"false" , false },
    {"FALSE" , false },
    {"1"     , true  },
    {"true"  , true  },
    {"TRUE"  , true  },
};


/*===================================================================*/
/*                         ScenarioData.cpp                          */
/*===================================================================*/
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
    EXPECTING_VALUE       =  0x00000400,
};
#define MAX_SCENARIO_TYPES 11


enum class ErrorType {
    OK          =  0x00000000,
    ERROR_MSG   =  0x00000001,
    ARGTAB_SIZE =  0x00000002,
    ABBR_FORM   =  0x00000004,
    FULL_FORM   =  0x00000008,
    DATA_TYPE   =  0x00000010,
    REQUIRED    =  0x00000020,
    DEFAULT     =  0x00000040,
    INITIALIZED =  0x00000080,
    VALUE       =  0x00000100,
};
#define N_ERROR_TYPES 9


std::string ErrorType_to_string(ErrorType);


class ScenarioData{
    private:
    // Attributes
    std::unique_ptr<std::vector<APTableEntry>> ini_argtab{}; // Initial argument table reference
    std::vector<APTableEntry> exp_argtab{}; // Expected argument table
    std::vector<APTableEntry> res_argtab{}; // Result argument table
    std::vector<ErrorType> arg_tab_miscompare{};
    std::string res_error_message{};
    std::string exp_error_message{};
    char** argv{};
    ErrorType error_types;
    uint32_t n_args{};
    uint32_t seed{};
    ScenarioType type{};
    int argc{};

    // Methods
    ErrorType error_type_bitwise_or(ErrorType, ErrorType);
    void validate_arg_table_excluding_values2();
    void validate_arg_table_values_only2();
    // OK_Scenario.cpp
    void build_OK_scenario(Randomizer*);
    void validate_OK_scenario();
    // MISSING_FIRST_DASH_Scenario.cpp
    void build_MISSING_FIRST_DASH_scenario(Randomizer*);
    void validate_MISSING_FIRST_DASH_scenario(ErrorReporter*);
    // MISSING_REQUIRED_ARG_Scenario.cpp
    void build_MISSING_REQUIRED_ARG_scenario(Randomizer*);
    void validate_MISSING_REQUIRED_ARG_scenario(ErrorReporter*);
    // UNKNOWN_ARGUMENT_Scenario.cpp
    void build_UNKNOWN_ARGUMENT_scenario(Randomizer*);
    void validate_UNKNOWN_ARGUMENT_scenario(ErrorReporter*);
    // REPEATED_ARGUMENT_Scenario.cpp
    void build_REPEATED_ARGUMENT_scenario(Randomizer*);
    void validate_REPEATED_ARGUMENT_scenario(ErrorReporter*);
    // MUST_BE_FLAG_Scenario.cpp
    void build_MUST_BE_FLAG_scenario(Randomizer*);
    void validate_MUST_BE_FLAG_scenario(ErrorReporter*);
    // BAD_NUMERIC_VALUE_Scenario.cpp
    void build_BAD_NUMERIC_VALUE_scenario(Randomizer*);
    void validate_BAD_NUMERIC_VALUE_scenario(ErrorReporter*);
    // EMPTY_ARG_LIST_Scenario.cpp
    void build_EMPTY_ARG_LIST_scenario(Randomizer*);
    void validate_EMPTY_ARG_LIST_scenario(ErrorReporter*);
    // VALID_FLAG_GROUP_Scenario.cpp
    void build_VALID_FLAG_GROUP_scenario(Randomizer*);
    void validate_VALID_FLAG_GROUP_scenario(ErrorReporter*);
    // INVALID_FLAG_GROUP_Scenario.cpp
    void build_INVALID_FLAG_GROUP_scenario(Randomizer*);
    void validate_INVALID_FLAG_GROUP_scenario(ErrorReporter*);
    // EXPECTING_VALUE_Scenario.cpp
    void build_EXPECTING_VALUE_scenario(Randomizer*);
    void validate_EXPECTING_VALUE_scenario(ErrorReporter*);

    public:
    // Methods
    ScenarioData();
    ScenarioData(Randomizer*, ScenarioType, std::vector<APTableEntry>&, uint32_t);
    ScenarioData(const ScenarioData&);
    ~ScenarioData();
    ScenarioData& operator=(const ScenarioData&);
    int get_argc();
    char** get_argv();
    ErrorType get_error_types();
    void validate(ErrorReporter*, size_t);
    void display();
    void collect_ap_data(ArgParsing*);
};


/*===================================================================*/
/*                         TestcaseData.cpp                          */
/*===================================================================*/
enum class BuildStatus{
    UNINITIALIZED         = 0,
    OK                    = 1,
    ALREADY_INITIALIZED   = 2,
    MAX_ATTMPTS_ARG_TABLE = 3,
    MAX_ATTMPTS_SCENARIOS = 4,
};


std::string BuildStatus_to_string(BuildStatus);
uint32_t check_allowed_scenarios(std::vector<APTableEntry>&, uint32_t);


class TestcaseData{
    private:
    // Attributes
    BuildStatus status{};
    std::vector<APTableEntry> init_argtab{}; // Initial argument table
    std::vector<ScenarioData> s_arr{};
    size_t tc_number{};

    public:
    // Methods
    TestcaseData();
    TestcaseData(Randomizer*, uint32_t, uint32_t, size_t);
    ~TestcaseData();
    BuildStatus get_status();
    std::vector<APTableEntry>& get_init_argtab();
    size_t get_n_scenarios();
    ScenarioData& get_scenario(size_t);
    void display();
};


typedef struct APValuePackage APValuePackage;
struct APValuePackage{
    APValue apv;                // Output
    std::string stringified;    // Output
    APDataType data_type;       // Input
    bool to_string;             // Input
};


// Utils.cpp
std::string space_padding(std::string, size_t, std::string);
std::string describe_argv(int, char**);
std::string ScenarioType_to_string(ScenarioType);
std::string APDataType_to_string(APDataType);
std::string bool_to_string(bool);
std::string APValue_to_string(APValue&, APDataType);
std::string arg_table_to_string(std::vector<APTableEntry>&);
std::string arg_table_ini_exp_res(std::vector<APTableEntry>&, std::vector<APTableEntry>&, std::vector<APTableEntry>&);
std::string APErrRsn_to_string(APErrRsn);
void vector_to_char_array(std::vector<std::string>&, char**&);
size_t arg_table_count_data_type(std::vector<APTableEntry>&, APDataType);
bool arg_table_is_required_arg(std::vector<APTableEntry>&, size_t);
size_t arg_table_count_required(std::vector<APTableEntry>&);
size_t arg_table_count_type(std::vector<APTableEntry>&, APDataType);
bool arg_table_is_abbr_form_available(std::vector<APTableEntry>&, size_t);
size_t arg_table_count_abbr_form(std::vector<APTableEntry>&);
int32_t arg_table_find_arg_index(std::vector<APTableEntry>&, std::string&, bool);
void gen_arg_value(Randomizer*, APValuePackage&);


// ArgTableBuilder.cpp
int build_arg_table(Randomizer*, std::vector<APTableEntry>&, uint32_t);
int build_entry(Randomizer*, std::vector<APTableEntry>&);


// Validation.cpp
void validate_error_msg(ErrorReporter*, std::string&, std::string&);
void validate_arg_table_excluding_values(ErrorReporter*, std::vector<APTableEntry>&, std::vector<APTableEntry>&);
void validate_arg_table_values_only(ErrorReporter*, std::vector<APTableEntry>&, std::vector<APTableEntry>&);


// Template utility functions
template<typename T> std::string integer_to_hex_string(T num){
    std::stringstream buffer;
    buffer << "0x" << std::hex << num;
    return buffer.str();
}

#endif
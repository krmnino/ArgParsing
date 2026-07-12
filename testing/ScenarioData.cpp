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


#include "ArgParsingTesting.hpp"


std::string ErrorType_to_string(ErrorType in_status){
    switch (in_status){
    case ErrorType::OK:
        return "ErrorType::OK";
    case ErrorType::ERROR_MSG:
        return "ErrorType::ERROR_MSG";
    case ErrorType::ARGTAB_SIZE:
        return "ErrorType::ARGTAB_SIZE";
    case ErrorType::ABBR_FORM:
        return "ErrorType::ABBR_FORM";
    case ErrorType::FULL_FORM:
        return "ErrorType::FULL_FORM";
    case ErrorType::DATA_TYPE:
        return "ErrorType::DATA_TYPE";
    case ErrorType::REQUIRED:
        return "ErrorType::REQUIRED";
    case ErrorType::DEFAULT:
        return "ErrorType::DEFAULT";
    case ErrorType::INITIALIZED:
        return "ErrorType::INITIALIZED";
    case ErrorType::VALUE:
        return "ErrorType::VALUE";
    default:
        return "ErrorType::UNDEFINED";
    }
}


ErrorType ScenarioData::error_type_bitwise_or(ErrorType in_type1, ErrorType in_type2){
    return (ErrorType)((int)in_type1 | (int)in_type2);
}


void ScenarioData::validate_arg_table_excluding_values2(){
    // Result vs. Expected argument table size
    if(this->res_argtab.size() == this->exp_argtab.size()){
        this->error_types = error_type_bitwise_or(this->error_types, ErrorType::ARGTAB_SIZE);
    }
    // Result vs. Expected argument tables
    for(size_t i = 0; i < this->exp_argtab.size(); i++){
        // abbr_form field should not be altered
        if(this->res_argtab[i].abbr_form != this->exp_argtab[i].abbr_form){
            this->error_types = error_type_bitwise_or(this->error_types, ErrorType::ABBR_FORM);
            this->arg_tab_miscompare[i] = error_type_bitwise_or(this->arg_tab_miscompare[i], ErrorType::ABBR_FORM);
        }
        // full_form field should not be altered
        if(this->res_argtab[i].full_form != this->exp_argtab[i].full_form){
            this->error_types = error_type_bitwise_or(this->error_types, ErrorType::FULL_FORM);
            this->arg_tab_miscompare[i] = error_type_bitwise_or(this->arg_tab_miscompare[i], ErrorType::FULL_FORM);
        }
        // data_type field should not be altered
        if(this->res_argtab[i].data_type != this->exp_argtab[i].data_type){
            this->error_types = error_type_bitwise_or(this->error_types, ErrorType::DATA_TYPE);
            this->arg_tab_miscompare[i] = error_type_bitwise_or(this->arg_tab_miscompare[i], ErrorType::DATA_TYPE);
        }
        // required field should not be altered
        if(this->res_argtab[i].required != this->exp_argtab[i].required){
            this->error_types = error_type_bitwise_or(this->error_types, ErrorType::REQUIRED);
            this->arg_tab_miscompare[i] = error_type_bitwise_or(this->arg_tab_miscompare[i], ErrorType::REQUIRED);
        }
        // default_value field should not be altered
        if(this->res_argtab[i].default_value != this->exp_argtab[i].default_value){
            this->error_types = error_type_bitwise_or(this->error_types, ErrorType::DEFAULT);
            this->arg_tab_miscompare[i] = error_type_bitwise_or(this->arg_tab_miscompare[i], ErrorType::DEFAULT);
        }
    }
}


void ScenarioData::validate_arg_table_values_only2(){
    // Result vs. Expected argument tables
    for(size_t i = 0; i < this->exp_argtab.size(); i++){
        // initialized should match the expected
        if(this->res_argtab[i].initialized != this->exp_argtab[i].initialized){
            this->error_types = error_type_bitwise_or(this->error_types, ErrorType::INITIALIZED);
            this->arg_tab_miscompare[i] = error_type_bitwise_or(this->arg_tab_miscompare[i], ErrorType::INITIALIZED);
        }
        // value should match the expected
        // uninitialized arguments with default values must remain unchanged
        if(this->exp_argtab[i].initialized || (!this->exp_argtab[i].initialized && this->exp_argtab[i].default_value)){
            switch(this->exp_argtab[i].data_type){
            case APDataType::UNSIGNED_INT:
                if(this->res_argtab[i].value.number_u64 != this->exp_argtab[i].value.number_u64){
                    this->error_types = error_type_bitwise_or(this->error_types, ErrorType::VALUE);
                    this->arg_tab_miscompare[i] = error_type_bitwise_or(this->arg_tab_miscompare[i], ErrorType::VALUE);
                }
                break;
            case APDataType::SIGNED_INT:
                if(this->res_argtab[i].value.number_i64 != this->exp_argtab[i].value.number_i64){
                    this->error_types = error_type_bitwise_or(this->error_types, ErrorType::VALUE);
                    this->arg_tab_miscompare[i] = error_type_bitwise_or(this->arg_tab_miscompare[i], ErrorType::VALUE);
                }
                break;
            case APDataType::TEXT:
                if(*this->res_argtab[i].value.text != *this->exp_argtab[i].value.text){
                    this->error_types = error_type_bitwise_or(this->error_types, ErrorType::VALUE);
                    this->arg_tab_miscompare[i] = error_type_bitwise_or(this->arg_tab_miscompare[i], ErrorType::VALUE);
                }
                break;
            case APDataType::FLAG:
                if(this->res_argtab[i].value.flag != this->exp_argtab[i].value.flag){
                    this->error_types = error_type_bitwise_or(this->error_types, ErrorType::VALUE);
                    this->arg_tab_miscompare[i] = error_type_bitwise_or(this->arg_tab_miscompare[i], ErrorType::VALUE);
                }
                break;
            default:
                this->error_types = error_type_bitwise_or(this->error_types, ErrorType::VALUE);
                this->arg_tab_miscompare[i] = error_type_bitwise_or(this->arg_tab_miscompare[i], ErrorType::VALUE);
                break;
            }
        }
    }
}


ScenarioData::ScenarioData() {}


ScenarioData::ScenarioData(Randomizer* in_rnd, ScenarioType in_type, std::vector<APTableEntry>& in_init_table, uint32_t in_seed){
    this->type = in_type;
    this->seed = in_seed;
    this->ini_argtab = std::make_unique<std::vector<APTableEntry>>(in_init_table);
    // Initialize vector of ErrorType elements
    this->arg_tab_miscompare.reserve(in_init_table.size());
    for(size_t i = 0; i < this->arg_tab_miscompare.size(); i++){
        this->arg_tab_miscompare.push_back(ErrorType::OK);
    }
    // Copy initial arg table to scenario expected table data 
    this->exp_argtab = in_init_table;
    switch(this->type){
    case ScenarioType::OK:
        this->n_args = in_rnd->gen_integral_range<uint32_t>(arg_table_count_required(this->exp_argtab), this->exp_argtab.size());
        this->build_OK_scenario(in_rnd);
        break;
    case ScenarioType::MISSING_FIRST_DASH:
        this->n_args = in_rnd->gen_integral_range<uint32_t>(1, this->exp_argtab.size());
        this->build_MISSING_FIRST_DASH_scenario(in_rnd);
        break;
    case ScenarioType::MISSING_REQUIRED_ARG:
        this->n_args = in_rnd->gen_integral_range<uint32_t>(arg_table_count_required(this->exp_argtab), this->exp_argtab.size());
        this->build_MISSING_REQUIRED_ARG_scenario(in_rnd);
        break;
    case ScenarioType::UNKNOWN_ARGUMENT:
        // Extra room (+1) for unknown argument
        this->n_args = in_rnd->gen_integral_range<uint32_t>(arg_table_count_required(this->exp_argtab), this->exp_argtab.size()) + 1;
        this->build_UNKNOWN_ARGUMENT_scenario(in_rnd);
        break;
    case ScenarioType::REPEATED_ARGUMENT:
        // We need at least 1 argument to repeat it
        // Extra room (+1) for unknown argument
        this->n_args = in_rnd->gen_integral_range<uint32_t>(arg_table_count_required(this->exp_argtab) + 1, this->exp_argtab.size()) + 1;
        this->build_REPEATED_ARGUMENT_scenario(in_rnd);
        break;
    case ScenarioType::MUST_BE_FLAG:
        this->n_args = in_rnd->gen_integral_range<uint32_t>(arg_table_count_required(this->exp_argtab), this->exp_argtab.size());
        this->build_MUST_BE_FLAG_scenario(in_rnd);
        break;
    case ScenarioType::BAD_NUMERIC_VALUE:
        this->n_args = in_rnd->gen_integral_range<uint32_t>(arg_table_count_required(this->exp_argtab), this->exp_argtab.size());
        this->build_BAD_NUMERIC_VALUE_scenario(in_rnd);
        break;
    case ScenarioType::EMPTY_ARG_LIST:
        this->n_args = in_rnd->gen_integral_range<uint32_t>(1, MAX_ARGS);
        this->build_EMPTY_ARG_LIST_scenario(in_rnd);
        break;
    case ScenarioType::VALID_FLAG_GROUP:
        this->n_args = in_rnd->gen_integral_range<uint32_t>(2, this->exp_argtab.size());
        this->build_VALID_FLAG_GROUP_scenario(in_rnd);
        break;
    case ScenarioType::INVALID_FLAG_GROUP:
        this->n_args = in_rnd->gen_integral_range<uint32_t>(2, this->exp_argtab.size());
        this->build_INVALID_FLAG_GROUP_scenario(in_rnd);
        break;
    case ScenarioType::EXPECTING_VALUE:
        this->n_args = in_rnd->gen_integral_range<uint32_t>(2, this->exp_argtab.size());
        this->build_EXPECTING_VALUE_scenario(in_rnd);
        break;
    default:
        std::cerr << "ERROR: Invalid ScenarioType provided to build_scenario(): " << (int)this->type << std::endl;
    }
}


ScenarioData::~ScenarioData() {
    if(this->argv != nullptr){
        // Loop through the array deallocating strings one by one
        for(int i = 0; i < this->argc; i++){
            delete[] this->argv[i];
        }
        // Finally delete the whole array
        delete[] this->argv;
    }
}


ScenarioData& ScenarioData::operator=(const ScenarioData& in_data){
    this->ini_argtab = std::make_unique<std::vector<APTableEntry>>(*in_data.ini_argtab);
    this->res_argtab = in_data.res_argtab;
    this->exp_argtab = in_data.exp_argtab;
    this->res_error_message = in_data.res_error_message;
    this->exp_error_message = in_data.exp_error_message;
    this->seed = in_data.seed;
    this->n_args = in_data.n_args;
    this->argc = in_data.argc;
    this->type = in_data.type;
    if(in_data.argv != nullptr && in_data.argc != 0){
        this->argv = new char*[in_data.argc + 1];
        for(int i = 0; i < in_data.argc; i++){
            this->argv[i] = new char[std::strlen(in_data.argv[i]) + 1];
            std::strcpy(this->argv[i], in_data.argv[i]);
        }
    }
    return *this;
}


ScenarioData::ScenarioData(const ScenarioData& in_data){
    this->ini_argtab = std::make_unique<std::vector<APTableEntry>>(*in_data.ini_argtab);
    this->res_argtab = in_data.res_argtab;
    this->exp_argtab = in_data.exp_argtab;
    this->res_error_message = in_data.res_error_message;
    this->exp_error_message = in_data.exp_error_message;
    this->seed = in_data.seed;
    this->n_args = in_data.n_args;
    this->argc = in_data.argc;
    this->type = in_data.type;
    if(in_data.argv != nullptr && in_data.argc != 0){
        this->argv = new char*[in_data.argc + 1];
        for(int i = 0; i < in_data.argc; i++){
            this->argv[i] = new char[std::strlen(in_data.argv[i]) + 1];
            std::strcpy(this->argv[i], in_data.argv[i]);
        }
    }
}


int ScenarioData::get_argc(){
    return this->argc;
}


char** ScenarioData::get_argv(){
    return this->argv;
}


void ScenarioData::validate(ErrorReporter* er, size_t tc_counter){
    std::string buffer{};

    buffer = "ArgParsingTesting - " + ScenarioType_to_string(this->type);
    er->begin_test(buffer);
    buffer = "SEED             : " + std::to_string(this->seed);
    er->log_it(buffer);
    buffer = "TESTCASE COUNTER : " + std::to_string(tc_counter);
    er->log_it(buffer);
    switch (this->type){
    case ScenarioType::OK:
        this->validate_OK_scenario();
        break;
    case ScenarioType::MISSING_FIRST_DASH:
        this->validate_MISSING_FIRST_DASH_scenario(er);
        break;
    case ScenarioType::MISSING_REQUIRED_ARG:
        this->validate_MISSING_REQUIRED_ARG_scenario(er);
        break;
    case ScenarioType::UNKNOWN_ARGUMENT:
        this->validate_UNKNOWN_ARGUMENT_scenario(er);
        break;
    case ScenarioType::REPEATED_ARGUMENT:
        this->validate_REPEATED_ARGUMENT_scenario(er);
        break;
    case ScenarioType::MUST_BE_FLAG:
        this->validate_MUST_BE_FLAG_scenario(er);
        break;
    case ScenarioType::BAD_NUMERIC_VALUE:
        this->validate_BAD_NUMERIC_VALUE_scenario(er);
        break;
    case ScenarioType::EMPTY_ARG_LIST:
        this->validate_EMPTY_ARG_LIST_scenario(er);
        break;
    case ScenarioType::VALID_FLAG_GROUP:
        this->validate_VALID_FLAG_GROUP_scenario(er);
        break;
    case ScenarioType::INVALID_FLAG_GROUP:
        this->validate_INVALID_FLAG_GROUP_scenario(er);
        break;
    case ScenarioType::EXPECTING_VALUE:
        this->validate_EXPECTING_VALUE_scenario(er);
        break;
    default:
        std::cerr << "ERROR: Invalid ScenarioType provided to validate(): " << (int)this->type << std::endl;
    break;
    }
    er->end_test();
}


void ScenarioData::display(){
    uint32_t shifter{};
    ErrorType curr_err;

    std::cout << ">>> START TEST" << std::endl;
    std::cout << "ArgParsingTesting - " + ScenarioType_to_string(this->type) << std::endl;
    std::cout << "SEED             : " + std::to_string(this->seed) << std::endl;

    // Print error types
    if(this->error_types != ErrorType::OK){
        shifter = 1;
        for(size_t i = 0; i < N_ERROR_TYPES; i++){
            curr_err = (ErrorType)((int)this->error_types & shifter); 
            if(curr_err != ErrorType::OK){
                std::cout << "- " << ErrorType_to_string(curr_err) << std::endl;
            }
            shifter = shifter << 1;
        }
    }
    else{
        std::cout << ">>> NO ERRORS FOUND" << std::endl;
    }
    
    std::cout << ">>> START OF ARGUMENT TABLES (INITIAL/EXPECTED/RESULT)" << std::endl;
    std::cout << arg_table_ini_exp_res(*this->ini_argtab, this->exp_argtab, this->res_argtab) << std::endl;
    std::cout << "<<< END OF ARGUMENT TABLES (INITIAL/EXPECTED/RESULT)" << std::endl;
    std::cout << ">>> START OF ARGV <<<" << std::endl;
    std::cout << describe_argv(this->argc, this->argv) << std::endl;
    std::cout << "<<< END OF ARGV <<<" << std::endl;
    std::cout << "<<< END TEST" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;
}


void ScenarioData::collect_ap_data(ArgParsing* ap){
    ap->get_arg_table(this->res_argtab);
    ap->get_error_msg(this->res_error_message);
}
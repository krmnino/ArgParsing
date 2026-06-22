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


ScenarioData::ScenarioData() {}


ScenarioData::~ScenarioData() {
    if(this->argv != nullptr){
        // Loop through the array deallocating strings one by one
        for(int i = 0; argv[i] != nullptr; i++){
            delete[] this->argv[i];
        }
        // Finally delete the whole array
        delete[] this->argv;
    }
}


ScenarioData& ScenarioData::operator=(const ScenarioData& in_data){
    this->res_argtab = in_data.res_argtab;
    this->exp_argtab = in_data.exp_argtab;
    this->res_error_message = in_data.res_error_message;
    this->exp_error_message = in_data.exp_error_message;
    this->n_args = in_data.n_args;
    this->argc = in_data.argc;
    if(in_data.argv != nullptr){
        this->argv = new char*[in_data.argc + 1];
        for(int i = 0; i < in_data.argc; i++){
            this->argv[i] = new char[in_data.argc + 1];
            strcpy(this->argv[i], in_data.argv[i]);
        }
    }
    this->argv = in_data.argv;
    return *this;
}


void ScenarioData::validate(ErrorReporter* er, uint32_t seed, size_t tc_counter){
    std::string buffer{};

    buffer = "ArgParsingTesting - " + ScenarioType_to_string(this->type);
    er->begin_test(buffer);
    buffer = "SEED             : " + std::to_string(seed);
    er->log_it(buffer);
    buffer = "TESTCASE COUNTER : " + std::to_string(tc_counter);
    er->log_it(buffer);
    switch (this->type){
    case ScenarioType::OK:
        validate_OK_scenario(er, *this);
        break;
    case ScenarioType::MISSING_FIRST_DASH:
        validate_MISSING_FIRST_DASH_scenario(er, *this);
        break;
    case ScenarioType::MISSING_REQUIRED_ARG:
        validate_MISSING_REQUIRED_ARG_scenario(er, *this);
        break;
    case ScenarioType::UNKNOWN_ARGUMENT:
        validate_UNKNOWN_ARGUMENT_scenario(er, *this);
        break;
    case ScenarioType::REPEATED_ARGUMENT:
        validate_REPEATED_ARGUMENT_scenario(er, *this);
        break;
    case ScenarioType::MUST_BE_FLAG:
        validate_MUST_BE_FLAG_scenario(er, *this);
        break;
    case ScenarioType::BAD_NUMERIC_VALUE:
        validate_BAD_NUMERIC_VALUE_scenario(er, *this);
        break;
    case ScenarioType::EMPTY_ARG_LIST:
        validate_EMPTY_ARG_LIST_scenario(er, *this);
        break;
    case ScenarioType::VALID_FLAG_GROUP:
        validate_VALID_FLAG_GROUP_scenario(er, *this);
        break;
    case ScenarioType::INVALID_FLAG_GROUP:
        validate_INVALID_FLAG_GROUP_scenario(er, *this);
        break;
    case ScenarioType::EXPECTING_VALUE:
        validate_EXPECTING_VALUE_scenario(er, *this);
        break;
    default:
        std::cerr << "ERROR: Invalid ScenarioType provided to validate(): " << (int)this->type << std::endl;
    break;
    }
    er->end_test();
}
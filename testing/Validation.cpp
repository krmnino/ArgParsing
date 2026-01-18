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


void validate(ErrorReporter* er, uint32_t seed, size_t tc_counter, TestcaseData& tc){
    std::string buffer{};

    for(size_t i = 0; i < tc.n_scenarios; i++){
        buffer = "ArgParsingTesting - " + ScenarioType_to_string(tc.s_arr[i].type);
        er->begin_test(buffer);
        buffer = "SEED             : " + std::to_string(seed);
        er->log_it(buffer);
        buffer = "TESTCASE COUNTER : " + std::to_string(tc_counter);
        er->log_it(buffer);
        er->log_it(">>> START OF INITIAL ARGUMENT TABLE <<<");
        buffer = arg_table_to_string(tc.ini_argtab);
        er->log_it(buffer);
        er->log_it(">>> END OF INITIAL ARGUMENT TABLE <<<");
        switch (tc.s_arr[i].type){
        case ScenarioType::OK:
            validate_OK_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::MISSING_FIRST_DASH:
            validate_MISSING_FIRST_DASH_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::MISSING_REQUIRED_ARG:
            validate_MISSING_REQUIRED_ARG_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::UNKNOWN_ARGUMENT:
            validate_UNKNOWN_ARGUMENT_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::REPEATED_ARGUMENT:
            validate_REPEATED_ARGUMENT_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::MUST_BE_FLAG:
            validate_MUST_BE_FLAG_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::BAD_NUMERIC_VALUE:
            validate_BAD_NUMERIC_VALUE_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::EMPTY_ARG_LIST:
            validate_EMPTY_ARG_LIST_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::VALID_FLAG_GROUP:
            validate_VALID_FLAG_GROUP_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::INVALID_FLAG_GROUP:
            validate_INVALID_FLAG_GROUP_scenario(er, tc.s_arr[i]);
            break;
        default:
        break;
        }
        er->end_test();
    }
}


void collect_ap_data(ScenarioData& scenario, ArgParsing* ap){
    ap->get_arg_table(scenario.res_argtab);
    ap->get_error_msg(scenario.res_error_message);
}


void validate_error_msg(ErrorReporter* er, std::string& res, std::string& exp){
    er->log_it("Result   : res_error_message = \"" + res + "\"");
    er->log_it("Expected : exp_error_message = \"" + exp + "\"");
    if(res != exp){
        er->mark_error();
        er->log_it("!!! ERROR: error_message MISMATCH");
    }
}


void validate_arg_table_ex_values(ErrorReporter* er, std::vector<APTableEntry>& res, std::vector<APTableEntry>& exp){
    // Result vs. Expected argument table size
    er->log_it("Result   : size of argtab = " + std::to_string(res.size()));
    er->log_it("Expected : size of argtab = " + std::to_string(exp.size()));
    if(res.size() != exp.size()){
        er->mark_error();
        er->log_it("!!! ERROR: size of argtab MISMATCH");
    }
    // Result vs. Expected argument tables
    for(size_t i = 0; i < exp.size(); i++){
        // abbr_form field should not be altered
        if(res[i].abbr_form != exp[i].abbr_form){
            er->mark_error();
            er->log_it("!!! ERROR: abbr_form FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : abbr_form = " + res[i].abbr_form);
            er->log_it("Expected : abbr_form = " + exp[i].abbr_form);
        }
        // full_form field should not be altered
        if(res[i].full_form != exp[i].full_form){
            er->mark_error();
            er->log_it("!!! ERROR: full_form FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : full_form = " + res[i].full_form);
            er->log_it("Expected : full_form = " + exp[i].full_form);
        }
        // data_type field should not be altered
        if(res[i].data_type != exp[i].data_type){
            er->mark_error();
            er->log_it("!!! ERROR: data_type FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : data_type = " + APDataType_to_string(res[i].data_type));
            er->log_it("Expected : data_type = " + APDataType_to_string(exp[i].data_type));
        }
        // required field should not be altered
        if(res[i].required != exp[i].required){
            er->mark_error();
            er->log_it("!!! ERROR: required FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : required = " + bool_to_string(res[i].required));
            er->log_it("Expected : required = " + bool_to_string(exp[i].required));
        }
    }
}


void validate_arg_table_values_only(ErrorReporter* er, std::vector<APTableEntry>& res, std::vector<APTableEntry>& exp){
    // Result vs. Expected argument tables
    for(size_t i = 0; i < exp.size(); i++){
        // initialized should match the expected
        if(res[i].initialized != exp[i].initialized){
            er->mark_error();
            er->log_it("!!! ERROR: initialized FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : initialized = " + bool_to_string(res[i].initialized));
            er->log_it("Expected : initialized = " + bool_to_string(exp[i].initialized));
        }
        // value should match the expected
        // uninitialized arguments with default values must remain unchanged
        if(exp[i].initialized || (!exp[i].initialized && exp[i].default_value)){
            switch(exp[i].data_type){
            case APDataType::UNSIGNED_INT:
                if(res[i].value.number_u64 != exp[i].value.number_u64){
                    er->mark_error();
                    er->log_it("!!! ERROR: value FIELD MISMATCH");
                    er->log_it("Index    : " + std::to_string(i));
                    er->log_it("Result   : value = " + res[i].value.number_u64);
                    er->log_it("Expected : value = " + exp[i].value.number_u64);
                }
                break;
            case APDataType::SIGNED_INT:
                if(res[i].value.number_i64 != exp[i].value.number_i64){
                    er->mark_error();
                    er->log_it("!!! ERROR: value FIELD MISMATCH");
                    er->log_it("Index    : " + std::to_string(i));
                    er->log_it("Result   : value = " + res[i].value.number_i64);
                    er->log_it("Expected : value = " + exp[i].value.number_i64);
                }
                break;
            case APDataType::TEXT:
                if(*res[i].value.text != *exp[i].value.text){
                    er->mark_error();
                    er->log_it("!!! ERROR: value FIELD MISMATCH");
                    er->log_it("Index    : " + std::to_string(i));
                    er->log_it("Result   : value = " + *res[i].value.text);
                    er->log_it("Expected : value = " + *exp[i].value.text);
                }
                break;
            case APDataType::FLAG:
                if(res[i].value.flag != exp[i].value.flag){
                    er->mark_error();
                    er->log_it("!!! ERROR: value FIELD MISMATCH");
                    er->log_it("Index    : " + std::to_string(i));
                    er->log_it("Result   : value = " + res[i].value.flag);
                    er->log_it("Expected : value = " + exp[i].value.flag);
                }
                break;
            default:
                break;
            }
        }
    }
}
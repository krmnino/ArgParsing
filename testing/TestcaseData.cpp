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


std::string TDBuildStatus_to_string(TDBuildStatus in_status){
    switch (in_status){
    case TDBuildStatus::UNINITIALIZED:
        return "TDBuildStatus::UNINITIALIZED";
    case TDBuildStatus::OK:
        return "TDBuildStatus::OK";
    case TDBuildStatus::ALREADY_INITIALIZED:
        return "TDBuildStatus::ALREADY_INITIALIZED";
    case TDBuildStatus::MAX_ATTMPTS_ARG_TABLE:
        return "TDBuildStatus::MAX_ATTMPTS_ARG_TABLE";
    case TDBuildStatus::MAX_ATTMPTS_SCENARIOS:
        return "TDBuildStatus::MAX_ATTMPTS_SCENARIOS";
    default:
        return "TDBuildStatus::UNDEFINED";
    }
}


uint32_t check_allowed_scenarios(std::vector<APTableEntry>& arg_table, uint32_t input_allowed_scenarios){
    uint32_t mask{};
    uint32_t allowed_scenarios{};
    size_t valid_args_for_group{};
    
    // Allow everything first
    allowed_scenarios = (uint32_t)ScenarioType::OK                     |
                        (uint32_t)ScenarioType::MISSING_FIRST_DASH     |
                        (uint32_t)ScenarioType::MISSING_REQUIRED_ARG   |
                        (uint32_t)ScenarioType::UNKNOWN_ARGUMENT       |
                        (uint32_t)ScenarioType::REPEATED_ARGUMENT      |
                        (uint32_t)ScenarioType::MUST_BE_FLAG           |
                        (uint32_t)ScenarioType::BAD_NUMERIC_VALUE      |
                        (uint32_t)ScenarioType::EMPTY_ARG_LIST         |
                        (uint32_t)ScenarioType::VALID_FLAG_GROUP       |
                        (uint32_t)ScenarioType::INVALID_FLAG_GROUP     |
                        (uint32_t)ScenarioType::EXPECTING_VALUE        ;
    
    // Mask out with input parm before disabling scenario by scenario based on input argument table
    allowed_scenarios = allowed_scenarios & input_allowed_scenarios;

    // Check if ScenarioType::OK scenario can be tested
    // Check if ScenarioType::REPEATED_ARGUMENT scenario can be tested
    if(arg_table.size() == 0){
        mask = ~(uint32_t)ScenarioType::OK;
        allowed_scenarios = allowed_scenarios & mask;
        mask = ~(uint32_t)ScenarioType::REPEATED_ARGUMENT;
        allowed_scenarios = allowed_scenarios & mask;
    }
    
    // ScenarioType::UNKNOWN_ARGUMENT is always allowed
    
    // Check if ScenarioType::MISSING_FIRST_DASH scenario can be tested
    if(arg_table_count_abbr_form(arg_table) == 0){
        mask = ~(uint32_t)ScenarioType::MISSING_FIRST_DASH;
        allowed_scenarios = allowed_scenarios & mask;
    }

    // Check if ScenarioType::MISSING_REQUIRED_ARG scenario can be tested
    if(arg_table_count_required(arg_table) == 0){
        mask = ~(uint32_t)ScenarioType::MISSING_REQUIRED_ARG;
        allowed_scenarios = allowed_scenarios & mask;
    }
    
    // Check if ScenarioType::MUST_BE_FLAG scenario can be tested
    if(arg_table_count_data_type(arg_table, APDataType::FLAG) == 0){
        mask = ~(uint32_t)ScenarioType::MUST_BE_FLAG;
        allowed_scenarios = allowed_scenarios & mask;
    }
    
    // Check if ScenarioType::BAD_NUMERIC_VALUE scenario can be tested
    if(arg_table_count_data_type(arg_table, APDataType::UNSIGNED_INT) == 0 && 
       arg_table_count_data_type(arg_table, APDataType::SIGNED_INT) == 0){
        mask = ~(uint32_t)ScenarioType::BAD_NUMERIC_VALUE;
        allowed_scenarios = allowed_scenarios & mask;
    }

    // Check if ScenarioType::EMPTY_ARG_LIST scenario can be tested
    if(arg_table.size() != 0){
        mask = ~(uint32_t)ScenarioType::EMPTY_ARG_LIST;
        allowed_scenarios = allowed_scenarios & mask;
    }
    
    // Check if ScenarioType::VALID_FLAG_GROUP scenario can be tested
    // Check if ScenarioType::INVALID_FLAG_GROUP scenario can be tested
    valid_args_for_group = 0;
    for(size_t i = 0; i < arg_table.size(); i++){
        if(arg_table[i].abbr_form != "" && arg_table[i].data_type == APDataType::FLAG){
            valid_args_for_group++;
        }
    }
    if(valid_args_for_group < 2){
        mask = ~(uint32_t)ScenarioType::VALID_FLAG_GROUP;
        allowed_scenarios = allowed_scenarios & mask;
        mask = ~(uint32_t)ScenarioType::INVALID_FLAG_GROUP;
        allowed_scenarios = allowed_scenarios & mask;
    }

    // Check if ScenarioType::EXPECTING_VALUE scenario can be tested
    if(arg_table_count_data_type(arg_table, APDataType::TEXT) == 0 &&
       arg_table_count_data_type(arg_table, APDataType::UNSIGNED_INT) == 0 &&  
       arg_table_count_data_type(arg_table, APDataType::SIGNED_INT) == 0){
        mask = ~(uint32_t)ScenarioType::EXPECTING_VALUE;
        allowed_scenarios = allowed_scenarios & mask;
    }
    return allowed_scenarios;
}


TestcaseData::TestcaseData() {}


TestcaseData::~TestcaseData() {}


TestcaseData::TestcaseData(Randomizer* in_rnd, uint32_t in_n_scenarios, uint32_t in_scenario_types){
    ScenarioData local_sc{};
    uint32_t scenario_type_pool{};
    uint32_t picked_scenario_type{};
    uint32_t attempt_counter{};
    uint32_t shifter{};
    uint32_t n_args{};
    int ret{};
    bool invalid{};
    
    if(this->status != TDBuildStatus::UNINITIALIZED){
        this->status = TDBuildStatus::ALREADY_INITIALIZED;
        return;
    }
    
    // Attempt building a argument table and check scenarios that can be tested
    attempt_counter = 0;
    while(true){
        invalid = false;
        if(attempt_counter > BUILD_MAX_ATTEMPTS){
            this->status = TDBuildStatus::MAX_ATTMPTS_ARG_TABLE;
            return;
        }
        n_args = in_rnd->gen_integral_range<uint32_t>(0, MAX_ARGS);
        ret = build_arg_table(in_rnd, this->init_argtab, n_args);
        // If returned -1, set invalid on
        if(ret != 0){
            invalid = true;
        }
        scenario_type_pool = check_allowed_scenarios(this->init_argtab, in_scenario_types);
        // If returned scenario pool is zero/empty, set invalid on
        if(scenario_type_pool == 0x00000000){
            invalid = true;
        }
        // If invalid is not set, then stop right now
        if(!invalid){
            break;
        }
        attempt_counter++;
    }
    
    // Generate scenarios based on argument table and allowed scenario types
    this->s_arr.reserve(in_n_scenarios);
    for(size_t i = 0; i < in_n_scenarios; i++){
        // Pick a single scenario from the pool
        attempt_counter = 0;
        while(true){
            if(attempt_counter > BUILD_MAX_ATTEMPTS){
                this->status = TDBuildStatus::MAX_ATTMPTS_SCENARIOS;
                return;
            }
            shifter = in_rnd->gen_integral_range<uint32_t>(0, MAX_SCENARIO_TYPES - 1);
            picked_scenario_type = (1 << shifter);
            if((scenario_type_pool & picked_scenario_type) != 0){
                break;
            }
            attempt_counter++;
        }
        local_sc.type = (ScenarioType)picked_scenario_type;
        // Copy initial arg table to scenario expected table data 
        local_sc.exp_argtab.reserve(this->init_argtab.size());
        local_sc.exp_argtab = this->init_argtab;
        this->s_arr.push_back(local_sc);
        // Build the scenario: expected table and argv
        ret = build_scenario(in_rnd, this->s_arr[i]);
        if(ret != 0){
            this->status = TDBuildStatus::MAX_ATTMPTS_SCENARIOS;
            return;
        }
    }
    this->status = TDBuildStatus::OK;
}


TDBuildStatus TestcaseData::get_status(){
    return this->status;
}


std::vector<APTableEntry>& TestcaseData::get_init_argtab(){
    return this->init_argtab;
}


size_t TestcaseData::get_n_scenarios(){
    return this->s_arr.size();
}


ScenarioData& TestcaseData::get_scenario(size_t idx){
    return this->s_arr[idx];
}
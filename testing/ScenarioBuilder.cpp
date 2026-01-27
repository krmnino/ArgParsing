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
#include <algorithm>


int build_scenario(Randomizer* rnd, ScenarioData& scenario){
    switch(scenario.type){
    case ScenarioType::OK:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(arg_table_count_required(scenario.exp_argtab), scenario.exp_argtab.size());
        build_OK_scenario(rnd, scenario);
        break;
    case ScenarioType::MISSING_FIRST_DASH:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(1, scenario.exp_argtab.size());
        build_MISSING_FIRST_DASH_scenario(rnd, scenario);
        break;
    case ScenarioType::MISSING_REQUIRED_ARG:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(arg_table_count_required(scenario.exp_argtab), scenario.exp_argtab.size());
        build_MISSING_REQUIRED_ARG_scenario(rnd, scenario);
        break;
    case ScenarioType::UNKNOWN_ARGUMENT:
        // Extra room (+1) for unknown argument
        scenario.n_args = rnd->gen_integral_range<uint32_t>(arg_table_count_required(scenario.exp_argtab), scenario.exp_argtab.size()) + 1;
        build_UNKNOWN_ARGUMENT_scenario(rnd, scenario);
        break;
    case ScenarioType::REPEATED_ARGUMENT:
        // We need at least 1 argument to repeat it
        // Extra room (+1) for unknown argument
        scenario.n_args = rnd->gen_integral_range<uint32_t>(arg_table_count_required(scenario.exp_argtab) + 1, scenario.exp_argtab.size()) + 1;
        build_REPEATED_ARGUMENT_scenario(rnd, scenario);
        break;
    case ScenarioType::MUST_BE_FLAG:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(arg_table_count_required(scenario.exp_argtab), scenario.exp_argtab.size());
        build_MUST_BE_FLAG_scenario(rnd, scenario);
        break;
    case ScenarioType::BAD_NUMERIC_VALUE:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(arg_table_count_required(scenario.exp_argtab), scenario.exp_argtab.size());
        build_BAD_NUMERIC_VALUE_scenario(rnd, scenario);
        break;
    case ScenarioType::EMPTY_ARG_LIST:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(1, MAX_ARGS);
        build_EMPTY_ARG_LIST_scenario(rnd, scenario);
        break;
    case ScenarioType::VALID_FLAG_GROUP:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(2, scenario.exp_argtab.size());
        build_VALID_FLAG_GROUP_scenario(rnd, scenario);
        break;
    case ScenarioType::INVALID_FLAG_GROUP:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(2, scenario.exp_argtab.size());
        build_INVALID_FLAG_GROUP_scenario(rnd, scenario);
        break;
    case ScenarioType::EXPECTING_VALUE:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(2, scenario.exp_argtab.size());
        build_EXPECTING_VALUE_scenario(rnd, scenario);
        break;
    default:
        std::cerr << "ERROR: Invalid ScenarioType provided to build_scenario(): " << (int)scenario.type << std::endl;
        return -1;
    }
    return 0;
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

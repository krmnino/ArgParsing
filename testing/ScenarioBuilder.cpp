#include "ArgParsingTesting.hpp"
#include <algorithm>

void build_scenario(Randomizer* rnd, ScenarioData& scenario){
    switch(scenario.type){
    case ScenarioType::OK:
        scenario.n_args = rnd->gen_integral_range<size_t>(arg_table_count_required(scenario.exp_argtab),scenario.exp_argtab.size());
        build_OK_scenario(rnd, scenario);
        break;
    case ScenarioType::MISSING_FIRST_DASH:
        scenario.n_args = rnd->gen_integral_range<size_t>(1, scenario.exp_argtab.size());
        build_MISSING_FIRST_DASH_scenario(rnd, scenario);
        break;
    case ScenarioType::MISSING_REQUIRED_ARG:
        scenario.n_args = rnd->gen_integral_range<size_t>(arg_table_count_required(scenario.exp_argtab), scenario.exp_argtab.size());
        build_MISSING_REQUIRED_ARG_scenario(rnd, scenario);
        break;
    case ScenarioType::UNKNOWN_ARGUMENT:
        // Extra room (+1) for unknown argument
        scenario.n_args = rnd->gen_integral_range<size_t>(arg_table_count_required(scenario.exp_argtab), scenario.exp_argtab.size()) + 1;
        build_UNKNOWN_ARGUMENT_scenario(rnd, scenario);
        break;
    case ScenarioType::REPEATED_ARGUMENT:
        // We need at least 1 argument to repeat it
        // Extra room (+1) for unknown argument
        scenario.n_args = rnd->gen_integral_range<size_t>(arg_table_count_required(scenario.exp_argtab) + 1, scenario.exp_argtab.size()) + 1;
        build_REPEATED_ARGUMENT_scenario(rnd, scenario);
        break;
    case ScenarioType::MUST_BE_FLAG:
        scenario.n_args = rnd->gen_integral_range<size_t>(arg_table_count_required(scenario.exp_argtab),scenario.exp_argtab.size());
        build_MUST_BE_FLAG_scenario(rnd, scenario);
        break;
    case ScenarioType::BAD_NUMERIC_VALUE:
        break;
    case ScenarioType::EMPTY_ARG_LIST:
        break;
    case ScenarioType::VALID_FLAG_GROUP:
        break;
    case ScenarioType::INVALID_FLAG_GROUP:
        break;
    default:
        break;
    }
}

void build_BAD_NUMERIC_VALUE_scenario(Randomizer* rnd, std::vector<APTableEntry>&){
}

void build_EMPTY_ARG_LIST_scenario(Randomizer* rnd, std::vector<APTableEntry>&){
}

void build_VALID_FLAG_GROUP_scenario(Randomizer* rnd, std::vector<APTableEntry>&){
}

void build_INVALID_FLAG_GROUP_scenario(Randomizer* rnd, std::vector<APTableEntry>&){
}

uint32_t check_allowed_scenarios(std::vector<APTableEntry>& arg_table, uint32_t input_allowed_scenarios){
    uint32_t mask;
    uint32_t allowed_scenarios;
    
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
                        (uint32_t)ScenarioType::INVALID_FLAG_GROUP     ;
    
    // Mask out with input parm before disabling scenario by scenario based on input argument table
    allowed_scenarios = allowed_scenarios & input_allowed_scenarios;

    // Check if Scenarios::OK scenario can be tested
    // Check if Scenarios::REPEATED_ARGUMENT scenario can be tested
    if(arg_table.size() == 0){
        mask = ~(uint32_t)ScenarioType::OK;
        allowed_scenarios = allowed_scenarios & mask;
        mask = ~(uint32_t)ScenarioType::REPEATED_ARGUMENT;
        allowed_scenarios = allowed_scenarios & mask;
    }
    
    // Scenarios::UNKNOWN_ARGUMENT is always allowed
    
    // Check if Scenarios::MISSING_FIRST_DASH scenario can be tested
    if(arg_table_count_abbr_form(arg_table) == 0){
        mask = ~(uint32_t)ScenarioType::MISSING_FIRST_DASH;
        allowed_scenarios = allowed_scenarios & mask;
    }

    // Check if Scenarios::MISSING_REQUIRED_ARG scenario can be tested
    if(arg_table_count_required(arg_table) == 0){
        mask = ~(uint32_t)ScenarioType::MISSING_REQUIRED_ARG;
        allowed_scenarios = allowed_scenarios & mask;
    }
    
    // Check if Scenarios::MUST_BE_FLAG scenario can be tested
    if(arg_table_count_data_type(arg_table, APDataType::FLAG) == 0){
        mask = ~(uint32_t)ScenarioType::MUST_BE_FLAG;
        allowed_scenarios = allowed_scenarios & mask;
    }
    
    // Check if Scenarios::BAD_NUMERIC_VALUE scenario can be tested
    if(arg_table_count_data_type(arg_table, APDataType::NUMBER) == 0){
        mask = ~(uint32_t)ScenarioType::BAD_NUMERIC_VALUE;
        allowed_scenarios = allowed_scenarios & mask;
    }

    // Check if Scenarios::EMPTY_ARG_LIST scenario can be tested
    if(arg_table.size() != 0){
        mask = ~(uint32_t)ScenarioType::EMPTY_ARG_LIST;
        allowed_scenarios = allowed_scenarios & mask;
    }
    
    // Check if Scenarios::VALID_FLAG_GROUP scenario can be tested
    // Check if Scenarios::INVALID_FLAG_GROUP scenario can be tested
    if(arg_table_count_type(arg_table, APDataType::FLAG) < 2){
        mask = ~(uint32_t)ScenarioType::VALID_FLAG_GROUP;
        allowed_scenarios = allowed_scenarios & mask;
        mask = ~(uint32_t)ScenarioType::INVALID_FLAG_GROUP;
        allowed_scenarios = allowed_scenarios & mask;
    }

    return allowed_scenarios;
}

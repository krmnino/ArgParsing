#include "ArgParsingTesting.hpp"

void build_scenario(Randomizer* rnd, ScenarioData& scenario){
    switch(scenario.type){
    case ScenarioType::OK:
        scenario.n_args = rnd->gen_integral_range<size_t>(count_required_args(scenario.exp_argtab),
                                                          scenario.exp_argtab.size());
        build_OK_scenario(rnd, scenario);
        break;
    case ScenarioType::MISSING_FIRST_DASH:
        break;
    case ScenarioType::MISSING_REQUIRED_ARG:
        break;
    case ScenarioType::UNKNOWN_ARGUMENT:
        break;
    case ScenarioType::REPEATED_ARGUMENT:
        break;
    case ScenarioType::MUST_BE_FLAG:
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

void build_OK_scenario(Randomizer* rnd, ScenarioData& scenario){
    std::stringstream buffer;
    std::string arg_id;
    std::string value;
    int64_t result_i64;
    size_t rand_idx;
    size_t n_initialized;
    uint32_t result_u32;
    bool use_hex;
    
    // Loop through sequentially and initialize all the required arguments first
    n_initialized = 0;
    for(size_t i = 0; i < scenario.exp_argtab.size(); i++){
        // If not required, then skip it
        if(!scenario.exp_argtab[i].required){
            continue;
        }

        // If argument has abbreviated form, then use it
        if(abbr_form_available(scenario.exp_argtab, i)){
            arg_id = "-" + scenario.exp_argtab[i].abbr_form;
        }
        else{
            arg_id = "--" + scenario.exp_argtab[i].full_form;
        }
    
        // Generate data for arguments that need it
        switch (scenario.exp_argtab[i].data_type){
        case APDataType::NUMBER:
            // 50% chance of either hex or decimal
            use_hex = rnd->gen_bool();
            result_i64 = rnd->gen_integral<int64_t>();
            if(use_hex){
                value = integer_to_hex_string(result_i64);
            }
            else{
                value = std::to_string(result_i64);
            }
            break;
        case APDataType::TEXT:
            result_u32 = rnd->gen_integral_range<size_t>(1, MAX_TEXT_ARG_LEN);
            value = rnd->gen_string(result_u32, nullptr);
            break;    
        case APDataType::FLAG:
            value = "1";
            break;    
        default:
            break;
        }

        // Update the expected argument table
        scenario.exp_argtab[i].initialized = true; 
        scenario.exp_argtab[i].value = value;

        // Update the argv vector with argument we just created
        scenario.argv.push_back(arg_id);
        if(scenario.exp_argtab[i].data_type != APDataType::FLAG){
            scenario.argv.push_back(value);
        }

        n_initialized++;
    }

    // Then loop and initialize any remaining non-required arguments
    while(n_initialized < scenario.n_args){
        // Pick a random argument from the table
        rand_idx = rnd->gen_integral_range<size_t>(0, scenario.exp_argtab.size() - 1); 
        if(scenario.exp_argtab[rand_idx].initialized){
            continue;
        }
        
        // If argument has abbreviated form, then use it
        if(abbr_form_available(scenario.exp_argtab, rand_idx)){
            arg_id = "-" + scenario.exp_argtab[rand_idx].abbr_form;
        }
        else{
            arg_id = "--" + scenario.exp_argtab[rand_idx].full_form;
        }
    
        // Generate data for arguments that need it
        switch (scenario.exp_argtab[rand_idx].data_type){
        case APDataType::NUMBER:
            use_hex = rnd->gen_bool();
            if(use_hex){
                value = integer_to_hex_string(rnd->gen_integral<int64_t>());
            }
            else{
                value = std::to_string(rnd->gen_integral<int64_t>());
            }
            break;    
        case APDataType::TEXT:
            result_u32 = rnd->gen_integral_range<size_t>(1, MAX_TEXT_ARG_LEN);
            value = rnd->gen_string(result_u32, nullptr);
            break;    
        case APDataType::FLAG:
            value = "1";
            break;    
        default:
            break;
        }

        // Update the expected argument table
        scenario.exp_argtab[rand_idx].initialized = true; 
        scenario.exp_argtab[rand_idx].value = value;

        // Update the argv vector with argument we just created
        scenario.argv.push_back(arg_id);
        if(scenario.exp_argtab[rand_idx].data_type != APDataType::FLAG){
            scenario.argv.push_back(value);
        }
        
        n_initialized++;
    }
}

void build_MISSING_FIRST_DASH_scenario(Randomizer* rnd, std::vector<APTableEntry>&){
}

void build_MISSING_REQUIRED_ARG_scenario(Randomizer* rnd, std::vector<APTableEntry>&){
}

void build_UNKNOWN_ARGUMENT_scenario(Randomizer* rnd, std::vector<APTableEntry>&){
}

void build_REPEATED_ARGUMENT_scenario(Randomizer* rnd, std::vector<APTableEntry>&){
}

void build_MUST_BE_FLAG_scenario(Randomizer* rnd, std::vector<APTableEntry>&){
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
    // Check if Scenarios::MISSING_FIRST_DASH scenario can be tested
    // Check if Scenarios::REPEATED_ARGUMENT scenario can be tested
    if(arg_table.size() == 0){
        mask = ~(uint32_t)ScenarioType::OK;
        allowed_scenarios = allowed_scenarios & mask;
        mask = ~(uint32_t)ScenarioType::MISSING_FIRST_DASH;
        allowed_scenarios = allowed_scenarios & mask;
        mask = ~(uint32_t)ScenarioType::REPEATED_ARGUMENT;
        allowed_scenarios = allowed_scenarios & mask;
    }
    
    // Scenarios::UNKNOWN_ARGUMENT is always allowed

    // Check if Scenarios::MISSING_REQUIRED_ARG scenario can be tested
    if(!contains_required(arg_table)){
        mask = ~(uint32_t)ScenarioType::MISSING_REQUIRED_ARG;
        allowed_scenarios = allowed_scenarios & mask;
    }
    
    // Check if Scenarios::MUST_BE_FLAG scenario can be tested
    if(!contains_data_type(arg_table, APDataType::FLAG)){
        mask = ~(uint32_t)ScenarioType::MUST_BE_FLAG;
        allowed_scenarios = allowed_scenarios & mask;
    }
    
    // Check if Scenarios::BAD_NUMERIC_VALUE scenario can be tested
    if(!contains_data_type(arg_table, APDataType::NUMBER)){
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
    if(count_args_by_type(arg_table, APDataType::FLAG) < 2){
        mask = ~(uint32_t)ScenarioType::VALID_FLAG_GROUP;
        allowed_scenarios = allowed_scenarios & mask;
        mask = ~(uint32_t)ScenarioType::INVALID_FLAG_GROUP;
        allowed_scenarios = allowed_scenarios & mask;
    }

    return allowed_scenarios;
}
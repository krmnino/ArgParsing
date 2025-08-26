#include "ArgParsingTesting.hpp"

void build_scenario(Randomizer* rnd, ScenarioData& scenario){
    switch(scenario.type){
    case ScenarioType::OK:
        scenario.n_args = rnd->gen_integral_range<size_t>(count_required_args(scenario.exp_argtab),scenario.exp_argtab.size());
        build_OK_scenario(rnd, scenario);
        break;
    case ScenarioType::MISSING_FIRST_DASH:
        scenario.n_args = rnd->gen_integral_range<size_t>(1, scenario.exp_argtab.size());
        build_MISSING_FIRST_DASH_scenario(rnd, scenario);
        break;
    case ScenarioType::MISSING_REQUIRED_ARG:
        scenario.n_args = rnd->gen_integral_range<size_t>(count_required_args(scenario.exp_argtab), scenario.exp_argtab.size());
        build_MISSING_REQUIRED_ARG_scenario(rnd, scenario);
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
    std::vector<std::string> argv;
    std::string arg_id;
    std::string value;
    int64_t result_i64;
    size_t rand_idx;
    size_t n_initialized;
    uint32_t result_u32;
    bool result_bool;

    
    // Add the placeholder program name for the first element of argv
    scenario.argc = 0;
    argv.push_back("PGM_PLACEHOLDER");
    scenario.argc++;
    
    // Set expected error code
    scenario.exp_error_message = "";

    // Loop through sequentially and initialize all the required arguments first
    n_initialized = 0;
    for(size_t i = 0; i < scenario.exp_argtab.size(); i++){
        // If not required, then skip it
        if(!scenario.exp_argtab[i].required){
            continue;
        }

        // If argument has abbreviated form, 50% chance of using it
        if(is_abbr_form_available(scenario.exp_argtab, i)){
            result_bool = rnd->gen_bool();
            if(result_bool){
                arg_id = "-" + scenario.exp_argtab[i].abbr_form;
            }
            else{
                arg_id = "--" + scenario.exp_argtab[i].full_form;
            }
        }
        else{
            arg_id = "--" + scenario.exp_argtab[i].full_form;
        }
    
        // Generate data for arguments that need it
        switch (scenario.exp_argtab[i].data_type){
        case APDataType::NUMBER:
            // 50% chance of either hex or decimal
            result_bool = rnd->gen_bool();
            result_i64 = rnd->gen_integral<int64_t>();
            if(result_bool){
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
        argv.push_back(arg_id);
        if(scenario.exp_argtab[i].data_type != APDataType::FLAG){
            argv.push_back(value);
        }

        // Update argc appropiately
        switch (scenario.exp_argtab[i].data_type){
        case APDataType::NUMBER:
        case APDataType::TEXT:
            scenario.argc += 2;
            break;
        case APDataType::FLAG:
            scenario.argc++;
            break;
        default:
            break;
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
        if(is_abbr_form_available(scenario.exp_argtab, rand_idx)){
            result_bool = rnd->gen_bool();
            if(result_bool){
                arg_id = "-" + scenario.exp_argtab[rand_idx].abbr_form;
            }
            else{
                arg_id = "--" + scenario.exp_argtab[rand_idx].full_form;
            }
        }
        else{
            arg_id = "--" + scenario.exp_argtab[rand_idx].full_form;
        }
    
        // Generate data for arguments that need it
        switch (scenario.exp_argtab[rand_idx].data_type){
        case APDataType::NUMBER:
            result_bool = rnd->gen_bool();
            if(result_bool){
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
        argv.push_back(arg_id);
        if(scenario.exp_argtab[rand_idx].data_type != APDataType::FLAG){
            argv.push_back(value);
        }

        // Update argc appropiately
        switch (scenario.exp_argtab[rand_idx].data_type){
        case APDataType::NUMBER:
        case APDataType::TEXT:
            scenario.argc += 2;
            break;
        case APDataType::FLAG:
            scenario.argc++;
            break;
        default:
            break;
        }

        n_initialized++;
    }

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(argv, scenario);
}

void build_MISSING_FIRST_DASH_scenario(Randomizer* rnd, ScenarioData& scenario){
    std::vector<std::string> argv;
    std::string arg_id;
    std::string value;
    size_t rand_idx;
    size_t error_arg_idx;
    size_t n_initialized;
    size_t error_arg_n;
    uint32_t result_u32;
    bool result_bool;

    // Add the placeholder program name for the first element of argv
    scenario.argc = 0;
    argv.push_back("PGM_PLACEHOLDER");
    scenario.argc++;
    
    // Find which argument to inject error and when to do it
    while(true){
        // Pick a random argument
        rand_idx = rnd->gen_integral_range<size_t>(0, scenario.exp_argtab.size() - 1);
        if(is_abbr_form_available(scenario.exp_argtab, rand_idx)){
            error_arg_idx = rand_idx;
            // Find a spot when to inject it
            error_arg_n = rnd->gen_integral_range<size_t>(0, scenario.n_args - 1);
            break;
        }
    }
    
    // Set expected error code
    scenario.exp_error_message = "ERROR: all argument identifiers must start with a dash (-).";

    // Initialize any arguments regardless if they are required or not
    n_initialized = 0;
    while(n_initialized < scenario.n_args){
        // Is it time to inject the error? If so, get the index from earlier
        if(n_initialized == error_arg_n){
            rand_idx = error_arg_idx;
        }
        else{
            // Pick a random argument from the table
            rand_idx = rnd->gen_integral_range<size_t>(0, scenario.exp_argtab.size() - 1); 
            if(scenario.exp_argtab[rand_idx].initialized || rand_idx == error_arg_idx){
                continue;
            }
        }

        // If argument has abbreviated form, then use it if injecting error, or 50% of times normally
        if(is_abbr_form_available(scenario.exp_argtab, rand_idx)){
            result_bool = rnd->gen_bool();
            // Skip the single dash
            if(n_initialized == error_arg_n){
                arg_id = scenario.exp_argtab[rand_idx].abbr_form;
            }
            else if(result_bool){
                arg_id = "-" + scenario.exp_argtab[rand_idx].abbr_form;
            }
            else{
                arg_id = "--" + scenario.exp_argtab[rand_idx].full_form;
            }
        }
        else{
            arg_id = "--" + scenario.exp_argtab[rand_idx].full_form;
        }
    
        // Generate data for arguments that need it
        switch (scenario.exp_argtab[rand_idx].data_type){
        case APDataType::NUMBER:
            result_bool = rnd->gen_bool();
            if(result_bool){
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
        argv.push_back(arg_id);
        if(scenario.exp_argtab[rand_idx].data_type != APDataType::FLAG){
            argv.push_back(value);
        }

        // Update argc appropiately
        switch (scenario.exp_argtab[rand_idx].data_type){
        case APDataType::NUMBER:
        case APDataType::TEXT:
            scenario.argc += 2;
            break;
        case APDataType::FLAG:
            scenario.argc++;
            break;
        default:
            break;
        }

        n_initialized++;
    }

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(argv, scenario);
}

void build_MISSING_REQUIRED_ARG_scenario(Randomizer* rnd, ScenarioData& scenario){
    std::vector<std::string> argv;
    std::string arg_id;
    std::string value;
    size_t rand_idx;
    size_t error_arg_idx;
    size_t n_initialized;
    int64_t result_i64;
    uint32_t result_u32;
    bool result_bool;

    // Add the placeholder program name for the first element of argv
    scenario.argc = 0;
    argv.push_back("PGM_PLACEHOLDER");
    scenario.argc++;

    // Find which argument to inject error and when to do it
    while(true){
        // Pick a random argument
        rand_idx = rnd->gen_integral_range<size_t>(0, scenario.exp_argtab.size() - 1);
        if(is_required_arg(scenario.exp_argtab, rand_idx)){
            error_arg_idx = rand_idx;
            break;
        }
    }

    // Set expected error code
    scenario.exp_error_message = "ERROR: the required argument --" + scenario.exp_argtab[rand_idx].full_form + " is missing.";

    // Loop through sequentially and initialize all the required arguments first, except the one we picked for error injection
    n_initialized = 0;
    for(size_t i = 0; i < scenario.exp_argtab.size(); i++){
        // Is it time to inject the error? If so, skip building the argument
        if(i == error_arg_idx){
            n_initialized++;
            continue;
        }
        // If not required, then skip it
        if(!scenario.exp_argtab[i].required){
            continue;
        }

        // If argument has abbreviated form, 50% chance of using it
        if(is_abbr_form_available(scenario.exp_argtab, i)){
            result_bool = rnd->gen_bool();
            if(result_bool){
                arg_id = "-" + scenario.exp_argtab[i].abbr_form;
            }
            else{
                arg_id = "--" + scenario.exp_argtab[i].full_form;
            }
        }
        else{
            arg_id = "--" + scenario.exp_argtab[i].full_form;
        }
    
        // Generate data for arguments that need it
        switch (scenario.exp_argtab[i].data_type){
        case APDataType::NUMBER:
            // 50% chance of either hex or decimal
            result_bool = rnd->gen_bool();
            result_i64 = rnd->gen_integral<int64_t>();
            if(result_bool){
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
        argv.push_back(arg_id);
        if(scenario.exp_argtab[i].data_type != APDataType::FLAG){
            argv.push_back(value);
        }

        // Update argc appropiately
        switch (scenario.exp_argtab[i].data_type){
        case APDataType::NUMBER:
        case APDataType::TEXT:
            scenario.argc += 2;
            break;
        case APDataType::FLAG:
            scenario.argc++;
            break;
        default:
            break;
        }

        n_initialized++;
    }

    // Then loop and initialize any remaining non-required arguments
    while(n_initialized < scenario.n_args){
        // Pick a random argument from the table
        rand_idx = rnd->gen_integral_range<size_t>(0, scenario.exp_argtab.size() - 1); 
        if(scenario.exp_argtab[rand_idx].initialized || rand_idx == error_arg_idx){
            continue;
        }

        // If argument has abbreviated form, use this form 50% of the times
        if(is_abbr_form_available(scenario.exp_argtab, rand_idx)){
            result_bool = rnd->gen_bool();
            if(result_bool){
                arg_id = "-" + scenario.exp_argtab[rand_idx].abbr_form;
            }
            else{
                arg_id = "--" + scenario.exp_argtab[rand_idx].full_form;
            }
        }
        else{
            arg_id = "--" + scenario.exp_argtab[rand_idx].full_form;
        }
    
        // Generate data for arguments that need it
        switch (scenario.exp_argtab[rand_idx].data_type){
        case APDataType::NUMBER:
            result_bool = rnd->gen_bool();
            if(result_bool){
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
        argv.push_back(arg_id);
        if(scenario.exp_argtab[rand_idx].data_type != APDataType::FLAG){
            argv.push_back(value);
        }

        // Update argc appropiately
        switch (scenario.exp_argtab[rand_idx].data_type){
        case APDataType::NUMBER:
        case APDataType::TEXT:
            scenario.argc += 2;
            break;
        case APDataType::FLAG:
            scenario.argc++;
            break;
        default:
            break;
        }

        n_initialized++;
    }

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(argv, scenario);
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
    // Check if Scenarios::REPEATED_ARGUMENT scenario can be tested
    if(arg_table.size() == 0){
        mask = ~(uint32_t)ScenarioType::OK;
        allowed_scenarios = allowed_scenarios & mask;
        mask = ~(uint32_t)ScenarioType::REPEATED_ARGUMENT;
        allowed_scenarios = allowed_scenarios & mask;
    }
    
    // Scenarios::UNKNOWN_ARGUMENT is always allowed
    
    // Check if Scenarios::MISSING_FIRST_DASH scenario can be tested
    if(count_abbr_form_args(arg_table) == 0){
        mask = ~(uint32_t)ScenarioType::MISSING_FIRST_DASH;
        allowed_scenarios = allowed_scenarios & mask;
    }

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

void vector_to_char_array(std::vector<std::string>& input_vect, ScenarioData& scenario){
    // Allocate the array of char pointers + 1 for the null terminator
    scenario.argv = new char*[input_vect.size() + 1];
    // Loop through the argv vector, allocate and copy over the individual strings
    for(size_t i = 0; i < input_vect.size(); i++){
        scenario.argv[i] = new char[input_vect[i].size() + 1];
        strcpy(scenario.argv[i], input_vect[i].c_str());
    }
    // Set null terminator
    scenario.argv[input_vect.size()] = nullptr;
}

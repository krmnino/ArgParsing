#include "ArgParsingTesting.hpp"
#include <algorithm>

static const char* alphanum_dict = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

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
    std::vector<std::string> arg_id_accumulator;
    std::vector<std::string> argv;
    std::string arg_id;
    std::string no_dashes_arg_id;
    std::string value;
    size_t rand_idx;
    size_t n_initialized;
    uint32_t result_u32;
    int32_t arg_table_idx;
    bool result_bool;
   
    // Set expected error code
    scenario.exp_error_message = "";

    // Make room in the accumulator argv
    arg_id_accumulator.reserve(scenario.n_args);
    
    // Loop through sequentially and initialize all the required arguments first
    n_initialized = 0;
    for(size_t i = 0; i < scenario.exp_argtab.size(); i++){
        // If not required, then skip it
        if(!scenario.exp_argtab[i].required){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        if(arg_table_is_abbr_form_available(scenario.exp_argtab, i)){
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
        // Add it to the a accumulator
        arg_id_accumulator.push_back(arg_id);
        // Mark randomly picked argument as initialized
        scenario.exp_argtab[i].initialized = true; 
        n_initialized++;
    }

    // Then loop and initialize any remaining non-required arguments
    while(n_initialized < scenario.n_args){
        // Pick a random argument from the table
        rand_idx = rnd->gen_integral_range<size_t>(0, scenario.exp_argtab.size() - 1); 
        if(scenario.exp_argtab[rand_idx].initialized){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        if(arg_table_is_abbr_form_available(scenario.exp_argtab, rand_idx)){
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
        // Add it to the a accumulator
        arg_id_accumulator.push_back(arg_id);
        // Mark randomly picked argument as initialized
        scenario.exp_argtab[rand_idx].initialized = true; 
        n_initialized++;
    }

    // Shuffle the arguments since required ones where picked first
    rnd->shuffle<std::string>(arg_id_accumulator);
    
    // Add the placeholder program name for the first element of argv
    scenario.argc = 0;
    argv.push_back("PGM_PLACEHOLDER");
    scenario.argc++;

    // Loop through the arguments and set random values (for non-FLAG types only)
    for(size_t i = 0; i < arg_id_accumulator.size(); i++){
        arg_id = arg_id_accumulator[i];
        // Find their index in the argument table
        if(arg_id[1] == '-'){
            no_dashes_arg_id = arg_id.substr(2);
            arg_table_idx = arg_table_find_arg_index(scenario.exp_argtab, no_dashes_arg_id, false);
        }
        else{
            no_dashes_arg_id = arg_id.substr(1);
            arg_table_idx = arg_table_find_arg_index(scenario.exp_argtab, no_dashes_arg_id, true);
        }
        // Generate data for arguments that need it
        switch (scenario.exp_argtab[arg_table_idx].data_type){
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
        // Set argument value
        scenario.exp_argtab[arg_table_idx].value = value;
        // Update the argv vector with argument we just created
        argv.push_back(arg_id);
        if(scenario.exp_argtab[arg_table_idx].data_type != APDataType::FLAG){
            argv.push_back(value);
        }
        // Update argc appropiately
        switch (scenario.exp_argtab[arg_table_idx].data_type){
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
    }

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(argv, scenario.argv);
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
        if(arg_table_is_abbr_form_available(scenario.exp_argtab, rand_idx)){
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
        if(arg_table_is_abbr_form_available(scenario.exp_argtab, rand_idx)){
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
    vector_to_char_array(argv, scenario.argv);
}

void build_MISSING_REQUIRED_ARG_scenario(Randomizer* rnd, ScenarioData& scenario){
    std::vector<std::string> arg_id_accumulator;
    std::vector<std::string> argv;
    std::string arg_id;
    std::string no_dashes_arg_id;
    std::string value;
    size_t rand_idx;
    size_t error_arg_idx;
    size_t n_initialized;
    uint32_t result_u32;
    int32_t arg_table_idx;
    bool result_bool;

    // Find which argument to inject error and when to do it
    while(true){
        // Pick a random argument
        rand_idx = rnd->gen_integral_range<size_t>(0, scenario.exp_argtab.size() - 1);
        if(arg_table_is_required_arg(scenario.exp_argtab, rand_idx)){
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
        // If argument has abbreviated form, then use it 50% of the times
        if(arg_table_is_abbr_form_available(scenario.exp_argtab, i)){
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
        // Add it to the a accumulator
        arg_id_accumulator.push_back(arg_id);
        // Mark randomly picked argument as initialized
        scenario.exp_argtab[i].initialized = true; 
        n_initialized++;
    }

    // Then loop and initialize any remaining non-required arguments
    while(n_initialized < scenario.n_args){
        // Pick a random argument from the table, as long is not the error inject
        rand_idx = rnd->gen_integral_range<size_t>(0, scenario.exp_argtab.size() - 1); 
        if(scenario.exp_argtab[rand_idx].initialized || rand_idx == error_arg_idx){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        if(arg_table_is_abbr_form_available(scenario.exp_argtab, rand_idx)){
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
        // Add it to the a accumulator
        arg_id_accumulator.push_back(arg_id);
        // Mark randomly picked argument as initialized
        scenario.exp_argtab[rand_idx].initialized = true; 
        n_initialized++;
    }

    // Shuffle the arguments since required ones where picked first
    rnd->shuffle<std::string>(arg_id_accumulator);

    // Add the placeholder program name for the first element of argv
    scenario.argc = 0;
    argv.push_back("PGM_PLACEHOLDER");
    scenario.argc++;

    // Loop through the arguments and set random values (for non-FLAG types only)
    for(size_t i = 0; i < arg_id_accumulator.size(); i++){
        arg_id = arg_id_accumulator[i];
        // Find their index in the argument table
        if(arg_id[1] == '-'){
            no_dashes_arg_id = arg_id.substr(2);
            arg_table_idx = arg_table_find_arg_index(scenario.exp_argtab, no_dashes_arg_id, false);
        }
        else{
            no_dashes_arg_id = arg_id.substr(1);
            arg_table_idx = arg_table_find_arg_index(scenario.exp_argtab, no_dashes_arg_id, true);
        }
        // Generate data for arguments that need it
        switch (scenario.exp_argtab[arg_table_idx].data_type){
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
        // Set argument value
        scenario.exp_argtab[arg_table_idx].value = value;
        // Update the argv vector with argument we just created
        argv.push_back(arg_id);
        if(scenario.exp_argtab[arg_table_idx].data_type != APDataType::FLAG){
            argv.push_back(value);
        }
        // Update argc appropiately
        switch (scenario.exp_argtab[arg_table_idx].data_type){
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
    }

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(argv, scenario.argv);
}

void build_UNKNOWN_ARGUMENT_scenario(Randomizer* rnd, ScenarioData& scenario){
    std::vector<std::string> arg_id_accumulator;
    std::vector<std::string> argv;
    std::string arg_id;
    std::string no_dashes_arg_id;
    std::string value;
    std::string error_arg;
    size_t rand_idx;
    size_t n_initialized;
    uint32_t result_u32;
    uint32_t shifter;
    int32_t arg_table_idx;
    int error_arg_idx;
    APDataType error_arg_data_type;
    APDataType arg_data_type;
    bool result_bool;

    // Make room in the accumulator argv
    arg_id_accumulator.reserve(scenario.n_args);

    // Try to generate an argument identifer that is not in the table
    n_initialized = 0;
    while(true){
        // True if using abbreviated form, false for full form
        result_bool = rnd->gen_bool();
        if(result_bool){
            error_arg = rnd->gen_string(MAX_ABBR_FORM_ID_LEN, alphanum_dict);
            error_arg_idx = arg_table_find_arg_index(scenario.exp_argtab, error_arg, true);
            error_arg = "-" + error_arg;
        }
        else{
            result_u32 = rnd->gen_integral_range<uint32_t>(2, MAX_FULL_FORM_ID_LEN);
            error_arg = rnd->gen_string(result_u32, alphanum_dict);
            error_arg_idx = arg_table_find_arg_index(scenario.exp_argtab, error_arg, false);
            error_arg = "--" + error_arg;
        }
        // If no match found, stop it
        if(error_arg_idx == -1){
            break;
        }
    }
    // Add it to the a accumulator
    arg_id_accumulator.push_back(error_arg);
    n_initialized++;

    // Pick a data type for unknown argument
    shifter = rnd->gen_integral_range<uint32_t>(0, MAX_TYPES - 1);
    error_arg_data_type = (APDataType)(1 << shifter);

    // Set expected error code
    scenario.exp_error_message = "ERROR: the provided argument " + error_arg + " is an unknown.";

    // Loop through sequentially and initialize all the required arguments first
    for(size_t i = 0; i < scenario.exp_argtab.size(); i++){
        // If not required, then skip it
        if(!scenario.exp_argtab[i].required){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        if(arg_table_is_abbr_form_available(scenario.exp_argtab, i)){
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
        // Add it to the a accumulator
        arg_id_accumulator.push_back(arg_id);
        // Mark randomly picked argument as initialized
        scenario.exp_argtab[i].initialized = true; 
        n_initialized++;
    }

    // Then loop and initialize any remaining non-required arguments
    while(n_initialized < scenario.n_args){
        // Pick a random argument from the table
        rand_idx = rnd->gen_integral_range<size_t>(0, scenario.exp_argtab.size() - 1); 
        if(scenario.exp_argtab[rand_idx].initialized){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        if(arg_table_is_abbr_form_available(scenario.exp_argtab, rand_idx)){
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
        // Add it to the a accumulator
        arg_id_accumulator.push_back(arg_id);
        // Mark randomly picked argument as initialized
        scenario.exp_argtab[rand_idx].initialized = true; 
        n_initialized++;
    }

    // Shuffle the arguments since required ones where picked first
    rnd->shuffle<std::string>(arg_id_accumulator);
    
    // Add the placeholder program name for the first element of argv
    scenario.argc = 0;
    argv.push_back("PGM_PLACEHOLDER");
    scenario.argc++;

    // Loop through the arguments and set random values (for non-FLAG types only)
    for(size_t i = 0; i < arg_id_accumulator.size(); i++){
        arg_id = arg_id_accumulator[i];
        // If dealing with error argument, skip searching the argument table
        if(arg_id == error_arg){
            arg_data_type = error_arg_data_type;
        }
        else{
            // Find their index in the argument table
            if(arg_id[1] == '-'){
                no_dashes_arg_id = arg_id.substr(2);
                arg_table_idx = arg_table_find_arg_index(scenario.exp_argtab, no_dashes_arg_id, false);
            }
            else{
                no_dashes_arg_id = arg_id.substr(1);
                arg_table_idx = arg_table_find_arg_index(scenario.exp_argtab, no_dashes_arg_id, true);
            }
            arg_data_type = scenario.exp_argtab[arg_table_idx].data_type;
        }
        // Generate data for arguments that need it
        switch (arg_data_type){
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
        // Set argument value (only for valid arguments)
        if(arg_id != error_arg){
            scenario.exp_argtab[arg_table_idx].value = value;
        }
        // Update the argv vector with argument we just created
        argv.push_back(arg_id);
        if(arg_data_type != APDataType::FLAG){
            argv.push_back(value);
        }
        // Update argc appropiately
        switch (arg_data_type){
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
    }

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(argv, scenario.argv);
}

void build_REPEATED_ARGUMENT_scenario(Randomizer* rnd, ScenarioData& scenario){
    std::vector<std::string> arg_id_accumulator;
    std::vector<std::string> argv;
    std::string arg_id;
    std::string no_dashes_arg_id;
    std::string value;
    size_t rand_idx;
    size_t n_initialized;
    size_t error_arg_n;
    uint32_t result_u32;
    int32_t arg_table_idx;
    bool result_bool;
   
    // Make room in the accumulator argv
    arg_id_accumulator.reserve(scenario.n_args);
    
    // Loop through sequentially and initialize all the required arguments first
    n_initialized = 0;
    for(size_t i = 0; i < scenario.exp_argtab.size(); i++){
        // If not required, then skip it
        if(!scenario.exp_argtab[i].required){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        if(arg_table_is_abbr_form_available(scenario.exp_argtab, i)){
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
        // Add it to the a accumulator
        arg_id_accumulator.push_back(arg_id);
        // Mark randomly picked argument as initialized
        scenario.exp_argtab[i].initialized = true; 
        n_initialized++;
    }

    // Then loop and initialize any remaining non-required arguments (skipping the last one)
    while(n_initialized < scenario.n_args - 1){
        // Pick a random argument from the table
        rand_idx = rnd->gen_integral_range<size_t>(0, scenario.exp_argtab.size() - 1); 
        if(scenario.exp_argtab[rand_idx].initialized){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        if(arg_table_is_abbr_form_available(scenario.exp_argtab, rand_idx)){
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
        // Add it to the a accumulator
        arg_id_accumulator.push_back(arg_id);
        // Mark randomly picked argument as initialized
        scenario.exp_argtab[rand_idx].initialized = true; 
        n_initialized++;
    }

    // Pick a random argument identifier from the accumulator and add it again
    error_arg_n = rnd->gen_integral_range<size_t>(0, (n_initialized - 1));
    arg_id_accumulator.push_back(arg_id_accumulator[error_arg_n]);
    n_initialized++;
    
    // Set expected error code
    scenario.exp_error_message = "ERROR: the provided argument " + arg_id_accumulator[error_arg_n] + " is repeated.";

    // Shuffle the arguments since required ones where picked first
    rnd->shuffle<std::string>(arg_id_accumulator);
    
    // Add the placeholder program name for the first element of argv
    scenario.argc = 0;
    argv.push_back("PGM_PLACEHOLDER");
    scenario.argc++;


    // Loop through the arguments and set random values (for non-FLAG types only)
    for(size_t i = 0; i < arg_id_accumulator.size(); i++){
        arg_id = arg_id_accumulator[i];
        // Find their index in the argument table
        if(arg_id[1] == '-'){
            no_dashes_arg_id = arg_id.substr(2);
            arg_table_idx = arg_table_find_arg_index(scenario.exp_argtab, no_dashes_arg_id, false);
        }
        else{
            no_dashes_arg_id = arg_id.substr(1);
            arg_table_idx = arg_table_find_arg_index(scenario.exp_argtab, no_dashes_arg_id, true);
        }
        // Generate data for arguments that need it
        switch (scenario.exp_argtab[arg_table_idx].data_type){
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
        // Set argument value (only for the non-repeated ones)
        if(!scenario.exp_argtab[arg_table_idx].initialized){
            scenario.exp_argtab[arg_table_idx].value = value;
        }
        // Update the argv vector with argument we just created
        argv.push_back(arg_id);
        if(scenario.exp_argtab[arg_table_idx].data_type != APDataType::FLAG){
            argv.push_back(value);
        }
        // Update argc appropiately
        switch (scenario.exp_argtab[arg_table_idx].data_type){
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
    }

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(argv, scenario.argv);
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

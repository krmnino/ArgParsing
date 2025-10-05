#include "ArgParsingTesting.hpp"

void build_MUST_BE_FLAG_scenario(Randomizer* rnd, ScenarioData& sc){
    const char* valid_flag_values[] = VALID_FLAG_VALUES;
    std::vector<std::string> arg_id_accumulator;
    std::vector<std::string> argv;
    std::string arg_id;
    std::string no_dashes_arg_id;
    std::string value_for_argv;
    std::string flag_value;
    union data value{};
    size_t rand_idx;
    size_t n_initialized;
    uint32_t result_u32;
    uint32_t shifter;
    uint32_t err_data_types;
    int32_t error_table_idx;
    int32_t arg_table_idx;
    APDataType arg_data_type;
    bool result_bool;
    bool invalid;
    bool use_flag_value;

    // Make room in the accumulator argv
    arg_id_accumulator.reserve(sc.n_args);

    // Find a FLAG argument to inject error
    n_initialized = 0;
    while(true){
        // Pick a random argument
        rand_idx = rnd->gen_integral_range<size_t>(0, sc.exp_argtab.size() - 1);
        if(sc.exp_argtab[rand_idx].data_type == APDataType::FLAG){
            error_table_idx = rand_idx;
            break;
        }
    }
    // Pick identifier type, initialize it, add it to the accumulator
    // 50% chance of using abbreviated form if available
    result_bool = rnd->gen_bool();
    if(arg_table_is_abbr_form_available(sc.exp_argtab, error_table_idx) && result_bool){
        arg_id = "-" + sc.exp_argtab[error_table_idx].abbr_form;
    }
    else{
        arg_id = "--" + sc.exp_argtab[error_table_idx].full_form;
    }
    arg_id_accumulator.push_back(arg_id);
    sc.exp_argtab[error_table_idx].initialized = true;
    n_initialized++;

    // Set expected error message 
    sc.exp_error_message = APErrRsn_to_string(APErrRsn::MUST_BE_FLAG) + ": the provided argument " + arg_id + " is of type FLAG. It must be especified alone or followed by one of these values: \"0\", \"1\", \"false\", or \"true\".";
    
    // Loop through sequentially and initialize all the required arguments first
    for(size_t i = 0; i < sc.exp_argtab.size(); i++){
        // If injected error argument is required, skip it
        if(error_table_idx == (int32_t)i){
            continue;
        }
        // If not required, then skip it
        if(!sc.exp_argtab[i].required){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        result_bool = rnd->gen_bool();
        if(arg_table_is_abbr_form_available(sc.exp_argtab, i) && result_bool){
            arg_id = "-" + sc.exp_argtab[i].abbr_form;
        }
        else{
            arg_id = "--" + sc.exp_argtab[i].full_form;
        }
        // Add it to the accumulator
        arg_id_accumulator.push_back(arg_id);
        sc.exp_argtab[i].initialized = true; 
        n_initialized++;
    }

    // Then loop and initialize any remaining non-required arguments
    while(n_initialized < sc.n_args){
        // Pick a random argument from the table
        rand_idx = rnd->gen_integral_range<size_t>(0, sc.exp_argtab.size() - 1); 
        if(error_table_idx == (int32_t)rand_idx){
            continue;
        }
        if(sc.exp_argtab[rand_idx].initialized){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        result_bool = rnd->gen_bool();
        if(arg_table_is_abbr_form_available(sc.exp_argtab, rand_idx) && result_bool){
            arg_id = "-" + sc.exp_argtab[rand_idx].abbr_form;
        }
        else{
            arg_id = "--" + sc.exp_argtab[rand_idx].full_form;
        }
        // Add it to the accumulator
        arg_id_accumulator.push_back(arg_id);
        // Mark randomly picked argument as initialized
        sc.exp_argtab[rand_idx].initialized = true; 
        n_initialized++;
    }

    // Shuffle the arguments since required ones were picked first
    rnd->shuffle<std::string>(arg_id_accumulator);

    // Add the placeholder program name for the first element of argv
    sc.argc = 0;
    argv.push_back("PGM_PLACEHOLDER");
    sc.argc++;

    // Loop through the arguments and set random values
    for(size_t i = 0; i < arg_id_accumulator.size(); i++){
        arg_id = arg_id_accumulator[i];
        // Find their index in the argument table
        if(arg_id[1] == '-'){
            no_dashes_arg_id = arg_id.substr(2);
            arg_table_idx = arg_table_find_arg_index(sc.exp_argtab, no_dashes_arg_id, false);
        }
        else{
            no_dashes_arg_id = arg_id.substr(1);
            arg_table_idx = arg_table_find_arg_index(sc.exp_argtab, no_dashes_arg_id, true);
        }

        // If we are injecting the error, pick a different data type for it
        // Else, use the data type that's in the argument table
        if(arg_table_idx == error_table_idx){
            err_data_types = (uint32_t)APDataType::UNSIGNED_INT || 
                             (uint32_t)APDataType::TEXT;
            while(true){
                shifter = rnd->gen_integral_range<uint32_t>(0, MAX_TYPES - 1);
                if((err_data_types & (1 << shifter)) != 0){
                    break;
                }
            }
            arg_data_type = (APDataType)(1 << shifter);
        }
        else{
            arg_data_type = sc.exp_argtab[arg_table_idx].data_type;
        }

        // Generate data for arguments that need it
        while(true){
            use_flag_value = false;
            switch (arg_data_type){
            case APDataType::UNSIGNED_INT:
                value.intdata.number_u64 = rnd->gen_integral<uint64_t>();
                // Pick between hex or decimal
                result_bool = rnd->gen_bool();
                if(result_bool){
                    value_for_argv = integer_to_hex_string<uint64_t>(value.intdata.number_u64);
                }
                else{
                    value_for_argv = std::to_string(value.intdata.number_u64);
                }
                break;   
            case APDataType::SIGNED_INT:
                value.intdata.number_i64 = rnd->gen_integral<int64_t>();
                // Pick between hex or decimal
                result_bool = rnd->gen_bool();
                if(result_bool){
                    value_for_argv = integer_to_hex_string<int64_t>(value.intdata.number_i64);
                }
                else{
                    value_for_argv = std::to_string(value.intdata.number_i64);
                }
                break;
            case APDataType::TEXT:
                result_u32 = rnd->gen_integral_range<uint32_t>(1, MAX_TEXT_ARG_LEN);
                value.text = new std::string(rnd->gen_string(result_u32, nullptr));
                value_for_argv = *value.text;
                break;    
            case APDataType::FLAG:
                use_flag_value = rnd->gen_bool();
                // Whether to include a value for FLAG argument or not
                if(use_flag_value){
                    result_u32 = rnd->gen_integral_range<uint32_t>(0, (sizeof(valid_flag_values) / sizeof(valid_flag_values[0])) -1);
                    flag_value = valid_flag_values[result_u32];
                    value_for_argv = valid_flag_values_dict.at(flag_value);
                }
                value.flag = true;  
            default:
                break;
            }

            // If current argument is not the inject error argument, exit right away
            if(arg_table_idx != error_table_idx){
                break;
            }
            
            // Validate picked value for argument, this is done to isolate testing to just MUST_BE_FLAG
            invalid = false;
            // Value's first character cannot be a dash (-)
            if(value_for_argv[0] == '-'){
                invalid = true;
            }
            // Value cannot match one of the valid FLAG values (only applies for FLAG argument injecting the error)
            for(size_t i = 0; i < sizeof(valid_flag_values) / sizeof(valid_flag_values[0]); i++){
                if(value_for_argv == valid_flag_values[i]){
                    invalid = true;
                    break;
                }
            }
            // Value should not match any of the argument identifiers
            if(arg_table_find_arg_index(sc.exp_argtab, value_for_argv, false) != -1 || 
               arg_table_find_arg_index(sc.exp_argtab, value_for_argv, true) != -1){
                invalid = true;
            }
            if(!invalid){
                break;
            }
            // If we are looping again, make sure to free up value.text if used
            if(arg_data_type == APDataType::TEXT){
                delete value.text;
            }
        }
        // Set argument value
        switch(sc.exp_argtab[arg_table_idx].data_type){
        case APDataType::UNSIGNED_INT:
            sc.exp_argtab[arg_table_idx].data.intdata.number_u64 = value.intdata.number_u64;
            break;
        case APDataType::SIGNED_INT:
            sc.exp_argtab[arg_table_idx].data.intdata.number_i64 = value.intdata.number_i64;
            break;
        case APDataType::TEXT:
            sc.exp_argtab[arg_table_idx].data.text = new std::string(*value.text);
            break;
        case APDataType::FLAG:
            sc.exp_argtab[arg_table_idx].data.flag = value.flag;
            break;
        default:
            break;
        }
        
        // Update the argv vector with argument we just created
        argv.push_back(arg_id);
        if(arg_data_type != APDataType::FLAG){
            argv.push_back(value_for_argv);
        }
        else if(arg_data_type == APDataType::FLAG && use_flag_value){
            argv.push_back(flag_value);
        }

        // Update argc appropiately
        switch (arg_data_type){
        case APDataType::UNSIGNED_INT:
        case APDataType::SIGNED_INT:
        case APDataType::TEXT:
            sc.argc += 2;
            break;
        case APDataType::FLAG:
            if(use_flag_value){
                sc.argc += 2;
            }
            else{
                sc.argc++;
            }
            break;
        default:
            break;
        }

        // Deallocate text value if used
        if(arg_data_type == APDataType::TEXT){
            delete value.text;
        }
    }

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(argv, sc.argv);
}

void validate_MUST_BE_FLAG_scenario(ErrorReporter* er, ScenarioData& sc){
    std::string buffer;
    // Result vs. Expected error mesage
    er->log_it("Result   : res_error_message = \"" + sc.res_error_message + "\"");
    er->log_it("Expected : exp_error_message = \"" + sc.exp_error_message + "\"");
    if(sc.res_error_message != sc.exp_error_message){
        er->mark_error();
        er->log_it("!!! ERROR: error_message MISMATCH");
    }
    er->log_it(">>> START OF ARGV <<<");
    er->log_it(describe_argv(sc.argc, sc.argv));
    er->log_it(">>> END OF ARGV <<<");
    // Result vs. Expected argument table size
    er->log_it("Result   : size of result argtab = " + std::to_string(sc.res_argtab.size()));
    er->log_it("Expected : size of result argtab = " + std::to_string(sc.exp_argtab.size()));
    if(sc.res_argtab.size() != sc.exp_argtab.size()){
        er->mark_error();
        er->log_it("!!! ERROR: size of argtab MISMATCH");
    }
    er->log_it(">>> START OF RESULT ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.res_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF RESULT ARGUMENT TABLE <<<");
    er->log_it(">>> START OF EXPECTED ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.exp_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF EXPECTED ARGUMENT TABLE <<<");
}
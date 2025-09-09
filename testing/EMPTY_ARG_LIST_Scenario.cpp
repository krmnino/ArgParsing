#include "ArgParsingTesting.hpp"

void build_EMPTY_ARG_LIST_scenario(Randomizer* rnd, ScenarioData& sc){
    std::vector<APTableEntry> non_empty_table;
    std::vector<std::string> arg_id_accumulator;
    std::vector<std::string> argv;
    std::string arg_id;
    std::string no_dashes_arg_id;
    std::string value;
    size_t n_initialized;
    uint32_t attempt_counter;
    uint32_t result_u32;
    int32_t arg_table_idx;
    int32_t ret;
    bool result_bool;
    bool invalid;
    
    // Generate non-empty argument table 
    attempt_counter = 0;
    while(true){
        invalid = false;
        if(attempt_counter > BUILD_MAX_ATTEMPTS){
            std::cerr << "ERROR: maximum build attempt for argument table reached." << std::endl;
            return;
        }
        ret = build_arg_table(rnd, non_empty_table, sc.n_args);
        // If returned -1, try build another argument table
        if(ret != 0){
            invalid = true;
        }
        // If invalid is not set, then stop right now
        if(!invalid){
            break;
        }
        attempt_counter++;
    }

    // Loop through sequentially and initialize all the arguments in the non-empty argument table
    n_initialized = 0;
    for(size_t i = 0; i < non_empty_table.size(); i++){
        // If argument has abbreviated form, then use it 50% of the times
        result_bool = rnd->gen_bool();
        if(arg_table_is_abbr_form_available(non_empty_table, i)){
            arg_id = "-" + non_empty_table[i].abbr_form;
        }
        else{
            arg_id = "--" + non_empty_table[i].full_form;
        }
        // Add it to the accumulator
        arg_id_accumulator.push_back(arg_id);
        n_initialized++;
    }

    // Shuffle the arguments since they were initialized sequentially
    rnd->shuffle<std::string>(arg_id_accumulator);

    // Add the placeholder program name for the first element of argv
    sc.argc = 0;
    argv.push_back("PGM_PLACEHOLDER");
    sc.argc++;

    // Set expected error message 
    sc.exp_error_message = APErrRsn_to_string(APErrRsn::UNKNOWN_ARGUMENT) + ": the provided argument " + arg_id_accumulator[0] + " is an unknown.";;

    // Loop through the arguments and set random values (for non-FLAG types only)
    for(size_t i = 0; i < arg_id_accumulator.size(); i++){
        arg_id = arg_id_accumulator[i];
        // Find their index in the argument table
        if(arg_id[1] == '-'){
            no_dashes_arg_id = arg_id.substr(2);
            arg_table_idx = arg_table_find_arg_index(non_empty_table, no_dashes_arg_id, false);
        }
        else{
            no_dashes_arg_id = arg_id.substr(1);
            arg_table_idx = arg_table_find_arg_index(non_empty_table, no_dashes_arg_id, true);
        }
        // Generate data for arguments that need it
        switch (non_empty_table[arg_table_idx].data_type){
        case APDataType::NUMBER:
            // Pick between hex or decimal
            result_bool = rnd->gen_bool();
            if(result_bool){
                value = integer_to_hex_string(rnd->gen_integral<int64_t>());
            }
            else{
                value = std::to_string(rnd->gen_integral<int64_t>());
            }
            break;    
        case APDataType::TEXT:
            result_u32 = rnd->gen_integral_range<uint32_t>(1, MAX_TEXT_ARG_LEN);
            value = rnd->gen_string(result_u32, nullptr);
            break;    
        case APDataType::FLAG:
            value = "1";
            break;    
        default:
            break;
        }
        // Update the argv vector with argument we just created
        argv.push_back(arg_id);
        if(non_empty_table[arg_table_idx].data_type != APDataType::FLAG){
            argv.push_back(value);
        }
        // Update argc appropiately
        switch (non_empty_table[arg_table_idx].data_type){
        case APDataType::NUMBER:
        case APDataType::TEXT:
            sc.argc += 2;
            break;
        case APDataType::FLAG:
            sc.argc++;
            break;
        default:
            break;
        }
    }

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(argv, sc.argv);
}

void validate_EMPTY_ARG_LIST_scenario(ErrorReporter* er, ScenarioData& sc){
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
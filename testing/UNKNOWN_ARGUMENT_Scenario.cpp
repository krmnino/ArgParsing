#include "ArgParsingTesting.hpp"

static const char* alphanum_dict = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

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
    // Add it to the accumulator
    arg_id_accumulator.push_back(error_arg);
    n_initialized++;

    // Pick a data type for unknown argument
    shifter = rnd->gen_integral_range<uint32_t>(0, MAX_TYPES - 1);
    error_arg_data_type = (APDataType)(1 << shifter);

    // Set expected error message 
    scenario.exp_error_message = APErrRsn_to_string(APErrRsn::UNKNOWN_ARGUMENT) + ": the provided argument " + error_arg + " is an unknown.";

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
        // Add it to the accumulator
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
        // Add it to the accumulator
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

void validate_UNKNOWN_ARGUMENT_scenario(ErrorReporter* er, ScenarioData& sc){
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

#include "ArgParsingTesting.hpp"

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

    // Find which argument to inject error
    while(true){
        // Pick a random argument
        rand_idx = rnd->gen_integral_range<size_t>(0, scenario.exp_argtab.size() - 1);
        if(arg_table_is_required_arg(scenario.exp_argtab, rand_idx)){
            error_arg_idx = rand_idx;
            break;
        }
    }

    // Set expected error message 
    scenario.exp_error_message = APErrRsn_to_string(APErrRsn::MISSING_REQUIRED_ARG) + ": the required argument --" + scenario.exp_argtab[rand_idx].full_form + " is missing.";

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
        result_bool = rnd->gen_bool();
        if(arg_table_is_abbr_form_available(scenario.exp_argtab, i) && result_bool){
            arg_id = "-" + scenario.exp_argtab[i].abbr_form;
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
        // Pick a random argument from the table, as long is not the error inject
        rand_idx = rnd->gen_integral_range<size_t>(0, scenario.exp_argtab.size() - 1); 
        if(scenario.exp_argtab[rand_idx].initialized || rand_idx == error_arg_idx){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        result_bool = rnd->gen_bool();
        if(arg_table_is_abbr_form_available(scenario.exp_argtab, rand_idx) && result_bool){
            arg_id = "-" + scenario.exp_argtab[rand_idx].abbr_form;
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

    // Shuffle the arguments since required ones were picked first
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

void validate_MISSING_REQUIRED_ARG_scenario(ErrorReporter* er, ScenarioData& sc){
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
#include "ArgParsingTesting.hpp"

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

        // Is it time to inject the error?
        if(n_initialized == error_arg_n){
            arg_id = scenario.exp_argtab[rand_idx].abbr_form;
        }
        else{
            // If argument has abbreviated form, then use it 50% of the times
            result_bool = rnd->gen_bool();
            if(arg_table_is_abbr_form_available(scenario.exp_argtab, rand_idx) && result_bool){
                arg_id = "-" + scenario.exp_argtab[rand_idx].abbr_form;
            }
            else{
                arg_id = "--" + scenario.exp_argtab[rand_idx].full_form;
            }
        }
        
        // Generate data for arguments that need it
        switch (scenario.exp_argtab[rand_idx].data_type){
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

void validate_MISSING_FIRST_DASH_scenario(ErrorReporter* er, ScenarioData& sc){
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
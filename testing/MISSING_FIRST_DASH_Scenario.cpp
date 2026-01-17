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


void build_MISSING_FIRST_DASH_scenario(Randomizer* rnd, ScenarioData& sc){
    APValuePackage arg_val_package;
    std::vector<std::string> argv{};
    std::string arg_id{};
    std::string value_for_argv{};
    std::string flag_value{};
    APValue loc_value{};
    size_t rand_idx{};
    size_t error_arg_idx{};
    size_t n_initialized{};
    size_t error_arg_n{};
    bool result_bool{};
    bool use_flag_value{};

    // Add the placeholder program name for the first element of argv
    sc.argc = 0;
    argv.push_back("PGM_PLACEHOLDER");
    sc.argc++;
    
    // Find which argument to inject error and when to do it
    while(true){
        // Pick a random argument
        rand_idx = rnd->gen_integral_range<size_t>(0, sc.exp_argtab.size() - 1);
        if(arg_table_is_abbr_form_available(sc.exp_argtab, rand_idx)){
            error_arg_idx = rand_idx;
            // Find a spot when to inject it
            error_arg_n = rnd->gen_integral_range<size_t>(0, sc.n_args - 1);
            break;
        }
    }

    // Set expected error message 
    sc.exp_error_message = APErrRsn_to_string(APErrRsn::MISSING_FIRST_DASH) + ": all argument identifiers must start with a dash (-).";

    // Initialize any arguments regardless if they are required or not
    n_initialized = 0;
    while(n_initialized < sc.n_args){
        // Is it time to inject the error? If so, get the index from earlier
        if(n_initialized == error_arg_n){
            rand_idx = error_arg_idx;
        }
        else{
            // Pick a random argument from the table
            rand_idx = rnd->gen_integral_range<size_t>(0, sc.exp_argtab.size() - 1); 
            if(sc.exp_argtab[rand_idx].initialized || rand_idx == error_arg_idx){
                continue;
            }
        }
        // Mark randomly picked argument as initialized
        sc.exp_argtab[rand_idx].initialized = true; 

        // Is it time to inject the error?
        if(n_initialized == error_arg_n){
            arg_id = sc.exp_argtab[rand_idx].abbr_form;
        }
        else{
            // If argument has abbreviated form, then use it 50% of the times
            result_bool = rnd->gen_bool();
            if(arg_table_is_abbr_form_available(sc.exp_argtab, rand_idx) && result_bool){
                arg_id = "-" + sc.exp_argtab[rand_idx].abbr_form;
            }
            else{
                arg_id = "--" + sc.exp_argtab[rand_idx].full_form;
            }
        }

        // Generate data for arguments that need it
        arg_val_package.data_type = sc.exp_argtab[rand_idx].data_type;
        arg_val_package.to_string = true;
        gen_arg_value(rnd, arg_val_package);

        // Set argument value
        copy_APValue(arg_val_package.apv, sc.exp_argtab[rand_idx].value, sc.exp_argtab[rand_idx].data_type);

        // Update the argv vector with argument we just created
        // Update argc appropiately
        argv.push_back(arg_id);
        if(sc.exp_argtab[rand_idx].data_type != APDataType::FLAG){
            argv.push_back(arg_val_package.stringified);
            sc.argc += 2;
        }
        else{
            use_flag_value = rnd->gen_bool();
            if(use_flag_value || !sc.exp_argtab[rand_idx].value.flag){
                argv.push_back(arg_val_package.stringified);
                sc.argc += 2;
            }
            else{
                sc.argc++;
            }
        }
        
        n_initialized++;
    }

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(argv, sc.argv);
}


void validate_MISSING_FIRST_DASH_scenario(ErrorReporter* er, ScenarioData& sc){
    std::string buffer{};

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
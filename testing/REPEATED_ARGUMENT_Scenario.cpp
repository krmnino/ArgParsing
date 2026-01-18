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


void build_REPEATED_ARGUMENT_scenario(Randomizer* rnd, ScenarioData& sc){
    APValuePackage arg_val_package;
    std::vector<std::string> arg_id_accumulator{};
    std::vector<std::string> argv{};
    std::string arg_id{};
    std::string no_dashes_arg_id{};
    std::string flag_value{};
    size_t rand_idx{};
    size_t n_initialized{};
    size_t error_arg_n{};
    int32_t arg_table_idx{};
    bool result_bool{};
    bool use_flag_value{};
    bool randomize_form{};
   
    // Make room in the accumulator argv
    arg_id_accumulator.reserve(sc.n_args);
    
    // Loop through sequentially and initialize all the required arguments first
    n_initialized = 0;
    for(size_t i = 0; i < sc.exp_argtab.size(); i++){
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
        // Mark randomly picked argument as initialized
        sc.exp_argtab[i].initialized = true; 
        n_initialized++;
    }

    // Then loop and initialize any remaining non-required arguments (skipping the last one)
    while(n_initialized < sc.n_args - 1){
        // Pick a random argument from the table
        rand_idx = rnd->gen_integral_range<size_t>(0, sc.exp_argtab.size() - 1); 
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

    // Pick a random argument identifier from the accumulator and add it again
    randomize_form = false;
    error_arg_n = rnd->gen_integral_range<size_t>(0, (n_initialized - 1));
    arg_id = arg_id_accumulator[error_arg_n];
    // If picked identifier is full form, check if abbreviated form is available
    // to allow randomizing form
    if(arg_id[1] == '-'){
        no_dashes_arg_id = arg_id.substr(2);
        arg_table_idx = arg_table_find_arg_index(sc.exp_argtab, no_dashes_arg_id, false);
        if(arg_table_is_abbr_form_available(sc.exp_argtab, arg_table_idx)){
            randomize_form = true;
        }
        else{
            randomize_form = false;
        }
    }
    // Else, if in abbreviated form already, allow randomizing form 
    else{
        no_dashes_arg_id = arg_id.substr(1);
        arg_table_idx = arg_table_find_arg_index(sc.exp_argtab, no_dashes_arg_id, true);
        randomize_form = true;
    }
    // If randomize form is allowed, randomly pick between abbreviated or full form
    if(randomize_form){
        // allow_randomization(TRUE) && result_bool(TRUE) -> abbreviated form
        result_bool = rnd->gen_bool();
        if(result_bool){
            arg_id_accumulator.push_back("-" + sc.exp_argtab[arg_table_idx].abbr_form);
        }
        // allow_randomization(TRUE) && result_bool(FALSE) -> full form
        else{
            arg_id_accumulator.push_back("--" + sc.exp_argtab[arg_table_idx].full_form);
        }
        // Set expected error message 
        sc.exp_error_message = APErrRsn_to_string(APErrRsn::REPEATED_ARGUMENT) + ": the provided argument -" + sc.exp_argtab[arg_table_idx].abbr_form +
                               "/--" + sc.exp_argtab[arg_table_idx].full_form + " is repeated.";
    }
    // allow_randomization(FALSE) -> full form
    else{
        arg_id_accumulator.push_back("--" + sc.exp_argtab[arg_table_idx].full_form);
        // Set expected error message 
        sc.exp_error_message = APErrRsn_to_string(APErrRsn::REPEATED_ARGUMENT) + ": the provided argument --" + sc.exp_argtab[arg_table_idx].full_form + " is repeated.";
    }
    n_initialized++;

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

        // Generate data for arguments that need it
        arg_val_package.data_type = sc.exp_argtab[arg_table_idx].data_type;
        arg_val_package.to_string = true;
        gen_arg_value(rnd, arg_val_package);

        // Set argument value
        copy_APValue(arg_val_package.apv, sc.exp_argtab[arg_table_idx].value, sc.exp_argtab[arg_table_idx].data_type);

        // Update the argv vector with argument we just created
        // Update argc appropiately
        argv.push_back(arg_id);
        if(sc.exp_argtab[arg_table_idx].data_type != APDataType::FLAG){
            argv.push_back(arg_val_package.stringified);
            sc.argc += 2;
        }
        else{
            use_flag_value = rnd->gen_bool();
            if(use_flag_value || !sc.exp_argtab[arg_table_idx].value.flag){
                argv.push_back(arg_val_package.stringified);
                sc.argc += 2;
            }
            else{
                sc.argc++;
            }
        }
    }

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(argv, sc.argv);
}


void validate_REPEATED_ARGUMENT_scenario(ErrorReporter* er, ScenarioData& sc){
    std::string buffer{};

    er->log_it(">>> START OF EXPECTED ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.exp_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF EXPECTED ARGUMENT TABLE <<<");
    er->log_it(">>> START OF ARGV <<<");
    er->log_it(describe_argv(sc.argc, sc.argv));
    er->log_it(">>> END OF ARGV <<<");
    er->log_it(">>> START OF RESULT ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.res_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF RESULT ARGUMENT TABLE <<<");

    // Result vs. Expected error mesage
    validate_error_msg(er, sc.res_error_message, sc.exp_error_message);
    
    // Result vs. Expected argument tables (excluding values)
    validate_arg_table_ex_values(er, sc.res_argtab, sc.exp_argtab);
}

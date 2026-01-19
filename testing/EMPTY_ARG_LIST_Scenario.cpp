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


void build_EMPTY_ARG_LIST_scenario(Randomizer* rnd, ScenarioData& sc){
    APValuePackage arg_val_package;
    std::vector<APTableEntry> non_empty_table{};
    std::vector<std::string> arg_id_accumulator{};
    std::vector<std::string> argv{};
    std::string arg_id{};
    std::string no_dashes_arg_id{};
    std::string flag_value{};
    size_t n_initialized{};
    uint32_t attempt_counter{};
    int32_t arg_table_idx{};
    int32_t ret{};
    bool result_bool{};
    bool invalid{};
    bool use_flag_value{};
    
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
    result_bool = result_bool ^ result_bool; // Tell compiler to calm down
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
        arg_val_package.data_type = non_empty_table[arg_table_idx].data_type;
        arg_val_package.to_string = true;
        gen_arg_value(rnd, arg_val_package);

        // Set argument value
        copy_APValue(arg_val_package.apv, non_empty_table[arg_table_idx].value, non_empty_table[arg_table_idx].data_type);

        // Update the argv vector with argument we just created
        // Update argc appropiately
        argv.push_back(arg_id);
        if(non_empty_table[arg_table_idx].data_type != APDataType::FLAG){
            argv.push_back(arg_val_package.stringified);
            sc.argc += 2;
        }
        else{
            use_flag_value = rnd->gen_bool();
            if(use_flag_value || !non_empty_table[arg_table_idx].value.flag){
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


void validate_EMPTY_ARG_LIST_scenario(ErrorReporter* er, ScenarioData& sc){
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
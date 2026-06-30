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


void ScenarioData::build_VALID_FLAG_GROUP_scenario(Randomizer* rnd){
    APValuePackage arg_val_package;
    std::vector<std::string> arg_id_accumulator{};
    std::vector<std::string> loc_argv{};
    std::string arg_id{};
    std::string no_dashes_arg_id{};
    std::string flag_value{};
    std::string group_buffer{};
    size_t rand_idx{};
    size_t n_initialized{};
    size_t valid_args_for_group{};
    size_t args_in_group{};
    int32_t arg_table_idx{};
    bool result_bool{};
    bool use_flag_value{};

    // Set expected error message 
    this->exp_error_message = "";

    // Make room in the accumulator argv
    arg_id_accumulator.reserve(this->n_args);

    // Count number of valid arguments to form a group. Then, decide size of the group
    valid_args_for_group = 0;
    for(size_t i = 0; i < this->exp_argtab.size(); i++){
        if(this->exp_argtab[i].abbr_form != "" && this->exp_argtab[i].data_type == APDataType::FLAG){
            valid_args_for_group++;
        }
    }
    args_in_group = rnd->gen_integral_range<size_t>(2, valid_args_for_group);

    // Pick random arguments to from the group, mark them as initialized accordingly
    n_initialized = 0;
    group_buffer = "-";
    while(n_initialized < args_in_group){
        // Pick a random argument from the table
        rand_idx = rnd->gen_integral_range<size_t>(0, this->exp_argtab.size() - 1); 
        if(this->exp_argtab[rand_idx].initialized     || 
           this->exp_argtab[rand_idx].abbr_form == "" ||
           this->exp_argtab[rand_idx].data_type != APDataType::FLAG){
            continue;
        }
        group_buffer += this->exp_argtab[rand_idx].abbr_form;
        this->exp_argtab[rand_idx].initialized = true;
        n_initialized++;
    }

    // Add group to the accumulator
    arg_id_accumulator.push_back(group_buffer);

    // Loop through sequentially and initialize all the required arguments
    for(size_t i = 0; i < this->exp_argtab.size(); i++){
        // If not required, then skip it
        if(!this->exp_argtab[i].required || this->exp_argtab[i].initialized){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        result_bool = rnd->gen_bool();
        if(arg_table_is_abbr_form_available(this->exp_argtab, i)){
            arg_id = "-" + this->exp_argtab[i].abbr_form;
        }
        else{
            arg_id = "--" + this->exp_argtab[i].full_form;
        }
        // Add it to the accumulator
        arg_id_accumulator.push_back(arg_id);
        // Mark randomly picked argument as initialized
        this->exp_argtab[i].initialized = true; 
        n_initialized++;
    }

    // Then loop and initialize any remaining non-required arguments
    while(n_initialized < this->n_args){
        // Pick a random argument from the table
        rand_idx = rnd->gen_integral_range<size_t>(0, this->exp_argtab.size() - 1); 
        if(this->exp_argtab[rand_idx].initialized){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        result_bool = rnd->gen_bool();
        if(arg_table_is_abbr_form_available(this->exp_argtab, rand_idx) && result_bool){
            arg_id = "-" + this->exp_argtab[rand_idx].abbr_form;
        }
        else{
            arg_id = "--" + this->exp_argtab[rand_idx].full_form;
        }
        // Add it to the accumulator
        arg_id_accumulator.push_back(arg_id);
        // Mark randomly picked argument as initialized
        this->exp_argtab[rand_idx].initialized = true; 
        n_initialized++;
    }

    // Shuffle the arguments since required ones and group buffer were picked first
    rnd->shuffle<std::string>(arg_id_accumulator);

    // Add the placeholder program name for the first element of argv
    loc_argv.push_back("PGM_PLACEHOLDER");

    // Loop through the arguments and set random values (for non-FLAG types only)
    for(size_t i = 0; i < arg_id_accumulator.size(); i++){
        arg_id = arg_id_accumulator[i];
        if(arg_id == group_buffer){
            // Set intial values for all the FLAG arguments in the group
            for(size_t j = 1; j < group_buffer.size(); j++){
                arg_id = group_buffer[j];
                arg_table_idx = arg_table_find_arg_index(this->exp_argtab, arg_id, true);
                this->exp_argtab[arg_table_idx].value.flag = true;
            }
            // Update the argv vector with the group argument
            loc_argv.push_back(group_buffer);
            // Update argc appropiately
            this->argc++;
        }
        else{
            // Find their index in the argument table
            if(arg_id[1] == '-'){
                no_dashes_arg_id = arg_id.substr(2);
                arg_table_idx = arg_table_find_arg_index(this->exp_argtab, no_dashes_arg_id, false);
            }
            else{
                no_dashes_arg_id = arg_id.substr(1);
                arg_table_idx = arg_table_find_arg_index(this->exp_argtab, no_dashes_arg_id, true);
            }

            // Generate data for arguments that need it
            arg_val_package.data_type = this->exp_argtab[arg_table_idx].data_type;
            arg_val_package.to_string = true;
            gen_arg_value(rnd, arg_val_package);

            // Set argument value
            this->exp_argtab[arg_table_idx].value = arg_val_package.apv;

            // Update the argv vector with argument we just created
            // Update argc appropiately
            loc_argv.push_back(arg_id);
            if(this->exp_argtab[arg_table_idx].data_type != APDataType::FLAG){
                loc_argv.push_back(arg_val_package.stringified);
            }
            else{
                use_flag_value = rnd->gen_bool();
                if(use_flag_value || !this->exp_argtab[arg_table_idx].value.flag){
                    loc_argv.push_back(arg_val_package.stringified);
                }
            }
        }
    }
    this->argc = loc_argv.size();


    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(loc_argv, this->argv);
}


void ScenarioData::validate_VALID_FLAG_GROUP_scenario(ErrorReporter* er){
    std::string buffer{};

    er->log_it(">>> START OF EXPECTED ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(this->exp_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF EXPECTED ARGUMENT TABLE <<<");
    er->log_it(">>> START OF ARGV <<<");
    er->log_it(describe_argv(this->argc, this->argv));
    er->log_it(">>> END OF ARGV <<<");
    er->log_it(">>> START OF RESULT ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(this->res_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF RESULT ARGUMENT TABLE <<<");

    // Result vs. Expected error mesage
    validate_error_msg(er, this->res_error_message, this->exp_error_message);
    
    // Result vs. Expected argument tables (excluding values)
    validate_arg_table_ex_values(er, this->res_argtab, this->exp_argtab);
        
    // Result vs. Expected argument tables (argument values only)
    validate_arg_table_values_only(er, this->res_argtab, this->exp_argtab);
}
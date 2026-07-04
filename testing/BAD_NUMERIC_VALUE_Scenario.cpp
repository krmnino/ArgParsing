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


static const char* non_num_dict = "GHIJKLMNOPQRSTUVWXYZghijklmnopqrstuvwxyz!@#$%^&*()";


template <typename T>
std::string generate_bad_number(Randomizer* rnd){
    std::string ret_value{};
    size_t non_num_dict_idx{};
    size_t value_idx{};
    bool result_bool{};

    // 50% chances of hex or decimal
    result_bool = rnd->gen_bool();
    if(result_bool){
        ret_value = integer_to_hex_string(rnd->gen_integral<int64_t>());
        // 50% chance of removing leading zero of hex number
        if(result_bool){
            ret_value = ret_value.substr(1);
        }
    }
    else{
        ret_value = std::to_string(rnd->gen_integral<int64_t>());
    }
    // Add non-numeric character to string value
    non_num_dict_idx = rnd->gen_integral_range<size_t>(0, (sizeof(non_num_dict) - 1));
    value_idx = rnd->gen_integral_range<size_t>(0, (ret_value.size() - 1));
    ret_value.insert(value_idx, 1, non_num_dict[non_num_dict_idx]);
    return ret_value;
}


void ScenarioData::build_BAD_NUMERIC_VALUE_scenario(Randomizer* rnd){
    APValuePackage arg_val_package;
    std::vector<std::string> arg_id_accumulator{};
    std::vector<std::string> loc_argv{};
    std::string arg_id{};
    std::string no_dashes_arg_id{};
    std::string flag_value{};
    size_t rand_idx{};
    size_t n_initialized{};
    int32_t error_table_idx{};
    int32_t arg_table_idx{};
    bool result_bool{};
    bool use_flag_value{};

    // Make room in the accumulator argv
    arg_id_accumulator.reserve(this->n_args);

    // Find a NUMBER argument to inject error
    n_initialized = 0;
    while(true){
        // Pick a random argument
        rand_idx = rnd->gen_integral_range<size_t>(0, this->exp_argtab.size() - 1);
        if(this->exp_argtab[rand_idx].data_type == APDataType::UNSIGNED_INT || 
           this->exp_argtab[rand_idx].data_type == APDataType::SIGNED_INT){
            error_table_idx = rand_idx;
            break;
        }
    }
    // Pick identifier type, initialize it, add it to the accumulator
    // 50% chance of using abbreviated form if available
    result_bool = rnd->gen_bool();
    if(arg_table_is_abbr_form_available(this->exp_argtab, error_table_idx) && result_bool){
        arg_id = "-" + this->exp_argtab[error_table_idx].abbr_form;
    }
    else{
        arg_id = "--" + this->exp_argtab[error_table_idx].full_form;
    }
    // Add it to the accumulator
    arg_id_accumulator.push_back(arg_id);
    this->exp_argtab[error_table_idx].initialized = true;
    n_initialized++;
    
    // Loop through sequentially and initialize all the required arguments first
    for(size_t i = 0; i < this->exp_argtab.size(); i++){
        // Skip the argument injecting the error
        if(error_table_idx == (int32_t)i){
            continue;
        }
        // If not required, then skip it
        if(!this->exp_argtab[i].required){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        result_bool = rnd->gen_bool();
        if(arg_table_is_abbr_form_available(this->exp_argtab, i) && result_bool){
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
        // Skip the argument injecting the error
        if(error_table_idx == (int32_t)rand_idx){
            continue;
        }
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

    // Shuffle the arguments since required ones were picked first
    rnd->shuffle<std::string>(arg_id_accumulator);
    
    // Add the placeholder program name for the first element of argv
    loc_argv.push_back("PGM_PLACEHOLDER");

    // Loop through the arguments and set random values
    for(size_t i = 0; i < arg_id_accumulator.size(); i++){
        arg_id = arg_id_accumulator[i];
        // Find their index in the argument table
        if(arg_id[1] == '-'){
            no_dashes_arg_id = arg_id.substr(2);
            arg_table_idx = arg_table_find_arg_index(this->exp_argtab, no_dashes_arg_id, false);
        }
        else{
            no_dashes_arg_id = arg_id.substr(1);
            arg_table_idx = arg_table_find_arg_index(this->exp_argtab, no_dashes_arg_id, true);
        }

        // It is time to inject the error
        if(error_table_idx == arg_table_idx){
            switch (this->exp_argtab[arg_table_idx].data_type){
            case APDataType::UNSIGNED_INT:
                arg_val_package.stringified = generate_bad_number<uint64_t>(rnd);
                break;
            case APDataType::SIGNED_INT:
                arg_val_package.stringified = generate_bad_number<int64_t>(rnd);
                break;
            default:
                break;
            }
        }
        else{
            // Generate data for arguments that need it
            arg_val_package.data_type = this->exp_argtab[arg_table_idx].data_type;
            arg_val_package.to_string = true;
            gen_arg_value(rnd, arg_val_package);
    
            // Set argument value
            this->exp_argtab[arg_table_idx].value = arg_val_package.apv;
        }

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

        // Set expected error message 
        if(error_table_idx == arg_table_idx){
            this->exp_error_message = APErrRsn_to_string(APErrRsn::BAD_NUMERIC_VALUE) + "" ": \"" + arg_val_package.stringified +
                                   "\" provided to the argument --" + this->exp_argtab[error_table_idx].full_form +
                                   " is not a valid numeric value.";
        }
    }
    this->argc = loc_argv.size();

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(loc_argv, this->argv);
}


void ScenarioData::validate_BAD_NUMERIC_VALUE_scenario(ErrorReporter* er){
    std::string buffer{};

    er->log_it(">>> START OF ARGUMENT TABLES (INITIAL/EXPECTED/RESULT) <<<");
    buffer = arg_table_ini_exp_res(*this->ini_argtab, this->exp_argtab, this->res_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF ARGUMENT TABLES (INITIAL/EXPECTED/RESULT) <<<");
    er->log_it(">>> START OF ARGV <<<");
    er->log_it(describe_argv(this->argc, this->argv));
    er->log_it(">>> END OF ARGV <<<");

    // Result vs. Expected error mesage
    validate_error_msg(er, this->res_error_message, this->exp_error_message);
    
    // Result vs. Expected argument tables (excluding values)
    validate_arg_table_ex_values(er, this->res_argtab, this->exp_argtab);
}
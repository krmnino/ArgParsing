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


void ScenarioData::build_MUST_BE_FLAG_scenario(Randomizer* rnd){
    const char* valid_flag_values[] = VALID_FLAG_VALUES;
    APValuePackage arg_val_package;
    std::vector<std::string> arg_id_accumulator{};
    std::vector<std::string> loc_argv{};
    std::string arg_id{};
    std::string no_dashes_arg_id{};
    std::string flag_value{};
    APValue loc_value{};
    size_t rand_idx{};
    size_t n_initialized{};
    uint32_t shifter{};
    uint32_t err_data_types{};
    int32_t error_table_idx{};
    int32_t arg_table_idx{};
    APDataType arg_data_type{};
    bool result_bool{};
    bool invalid{};
    bool use_flag_value{};

    // Make room in the accumulator argv
    arg_id_accumulator.reserve(this->n_args);

    // Find a FLAG argument to inject error
    n_initialized = 0;
    while(true){
        // Pick a random argument
        rand_idx = rnd->gen_integral_range<size_t>(0, this->exp_argtab.size() - 1);
        if(this->exp_argtab[rand_idx].data_type == APDataType::FLAG){
            error_table_idx = rand_idx;
            break;
        }
    }
    // Pick identifier type, initialize it, add it to the accumulator
    if(arg_table_is_abbr_form_available(this->exp_argtab, error_table_idx)){
        result_bool = rnd->gen_bool();
        // 50% chance of using abbreviated form if available
        if(result_bool){
            arg_id = "-" + this->exp_argtab[error_table_idx].abbr_form;
        }
        else{
            arg_id = "--" + this->exp_argtab[error_table_idx].full_form;
        }
        // Set expected error message 
        this->exp_error_message = APErrRsn_to_string(APErrRsn::MUST_BE_FLAG) + ": the provided argument -" + 
                               this->exp_argtab[error_table_idx].abbr_form + "/--" +
                               this->exp_argtab[error_table_idx].full_form +
                               " is of type FLAG. It must be especified alone or followed by one of these values: \"0\", \"1\", \"false\", or \"true\".";
    }
    else{
        arg_id = "--" + this->exp_argtab[error_table_idx].full_form;
        // Set expected error message 
        this->exp_error_message = APErrRsn_to_string(APErrRsn::MUST_BE_FLAG) + ": the provided argument --" + 
                               this->exp_argtab[error_table_idx].full_form +
                               " is of type FLAG. It must be especified alone or followed by one of these values: \"0\", \"1\", \"false\", or \"true\".";
    }
    arg_id_accumulator.push_back(arg_id);
    this->exp_argtab[error_table_idx].initialized = true;
    n_initialized++;

    
    // Loop through sequentially and initialize all the required arguments first
    for(size_t i = 0; i < this->exp_argtab.size(); i++){
        // If injected error argument is required, skip it
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
        this->exp_argtab[i].initialized = true; 
        n_initialized++;
    }

    // Then loop and initialize any remaining non-required arguments
    while(n_initialized < this->n_args){
        // Pick a random argument from the table
        rand_idx = rnd->gen_integral_range<size_t>(0, this->exp_argtab.size() - 1); 
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

        // If we are injecting the error, pick a different data type for it
        // Else, use the data type that's in the argument table
        if(arg_table_idx == error_table_idx){
            err_data_types = (uint32_t)APDataType::UNSIGNED_INT ||
                             (uint32_t)APDataType::SIGNED_INT ||
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
            arg_data_type = this->exp_argtab[arg_table_idx].data_type;
        }

        // Generate data for arguments that need it
        while(true){
            arg_val_package.data_type = arg_data_type;
            arg_val_package.to_string = true;
            gen_arg_value(rnd, arg_val_package);

            // If current argument is not the inject error argument, exit right away
            if(arg_table_idx != error_table_idx){
                break;
            }
            
            // Else, we are dealing with the inject error argument 
            // Validate picked value for argument, this is done to isolate testing to just MUST_BE_FLAG
            invalid = false;
            // Value's first character cannot be a dash (-)
            if(arg_val_package.stringified[0] == '-'){
                invalid = true;
            }
            // Value cannot match one of the valid FLAG values (only applies for FLAG argument injecting the error)
            for(size_t i = 0; i < sizeof(valid_flag_values) / sizeof(valid_flag_values[0]); i++){
                if(arg_val_package.stringified == valid_flag_values[i]){
                    invalid = true;
                    break;
                }
            }
            // Value should not match any of the argument identifiers
            if(arg_table_find_arg_index(this->exp_argtab, arg_val_package.stringified, false) != -1 || 
               arg_table_find_arg_index(this->exp_argtab, arg_val_package.stringified, true) != -1){
                invalid = true;
            }
            if(!invalid){
                break;
            }
        }
       
        // Set argument value
        if(arg_table_idx != error_table_idx){
            this->exp_argtab[arg_table_idx].value = arg_val_package.apv;
        }
        
        // Update the argv vector with argument we just created
        // Update argc appropiately
        loc_argv.push_back(arg_id);
        if(this->exp_argtab[arg_table_idx].data_type != APDataType::FLAG || arg_table_idx == error_table_idx){
            loc_argv.push_back(arg_val_package.stringified);
        }
        else{
            use_flag_value = rnd->gen_bool();
            if(use_flag_value || !this->exp_argtab[arg_table_idx].value.flag){
                loc_argv.push_back(arg_val_package.stringified);
            }
        }
    }
    this->argc = loc_argv.size();

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(loc_argv, this->argv);
}


void ScenarioData::validate_MUST_BE_FLAG_scenario(ErrorReporter* er){
    // Validate error message
    if(this->res_error_message != this->exp_error_message){
        this->error_types = this->error_type_bitwise_or(error_types, ErrorType::ERROR_MSG);
    }

    // Validate argument tables (excluding values)
    this->validate_arg_table_excluding_values();
}
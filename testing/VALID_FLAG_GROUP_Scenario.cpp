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


void build_VALID_FLAG_GROUP_scenario(Randomizer* rnd, ScenarioData& sc){
    const char* valid_flag_values[] = VALID_FLAG_VALUES;
    std::vector<std::string> arg_id_accumulator{};
    std::vector<std::string> argv{};
    std::string arg_id{};
    std::string no_dashes_arg_id{};
    std::string value_for_argv{};
    std::string flag_value{};
    std::string group_buffer{};
    APValue loc_value{};
    size_t rand_idx{};
    size_t n_initialized{};
    size_t valid_args_for_group{};
    size_t args_in_group{};
    uint32_t result_u32{};
    int32_t arg_table_idx{};
    bool result_bool{};
    bool use_flag_value{};

    // Set expected error message 
    sc.exp_error_message = "";

    // Make room in the accumulator argv
    arg_id_accumulator.reserve(sc.n_args);

    // Count number of valid arguments to form a group. Then, decide size of the group
    valid_args_for_group = 0;
    for(size_t i = 0; i < sc.exp_argtab.size(); i++){
        if(sc.exp_argtab[i].abbr_form != "" && sc.exp_argtab[i].data_type == APDataType::FLAG){
                valid_args_for_group++;
        }
    }
    args_in_group = rnd->gen_integral_range<size_t>(2, valid_args_for_group);

    // Pick random arguments to from the group, mark them as initialized accordingly
    n_initialized = 0;
    group_buffer = "-";
    while(n_initialized < args_in_group){
        // Pick a random argument from the table
        rand_idx = rnd->gen_integral_range<size_t>(0, sc.exp_argtab.size() - 1); 
        if(sc.exp_argtab[rand_idx].initialized     || 
           sc.exp_argtab[rand_idx].abbr_form == "" ||
           sc.exp_argtab[rand_idx].data_type != APDataType::FLAG){
            continue;
        }
        group_buffer += sc.exp_argtab[rand_idx].abbr_form;
        sc.exp_argtab[rand_idx].initialized = true;
        n_initialized++;
    }

    // Add group to the accumulator
    arg_id_accumulator.push_back(group_buffer);

    // Loop through sequentially and initialize all the required arguments
    for(size_t i = 0; i < sc.exp_argtab.size(); i++){
        // If not required, then skip it
        if(!sc.exp_argtab[i].required || sc.exp_argtab[i].initialized){
            continue;
        }
        // If argument has abbreviated form, then use it 50% of the times
        result_bool = rnd->gen_bool();
        if(arg_table_is_abbr_form_available(sc.exp_argtab, i)){
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

    // Then loop and initialize any remaining non-required arguments
    while(n_initialized < sc.n_args){
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

    // Shuffle the arguments since required ones and group buffer were picked first
    rnd->shuffle<std::string>(arg_id_accumulator);

    // Add the placeholder program name for the first element of argv
    sc.argc = 0;
    argv.push_back("PGM_PLACEHOLDER");
    sc.argc++;

    // Loop through the arguments and set random values (for non-FLAG types only)
    for(size_t i = 0; i < arg_id_accumulator.size(); i++){
        arg_id = arg_id_accumulator[i];
        if(arg_id == group_buffer){
            // Set intial values for all the FLAG arguments in the group
            for(size_t j = 1; j < group_buffer.size(); j++){
                arg_id = group_buffer[j];
                arg_table_idx = arg_table_find_arg_index(sc.exp_argtab, arg_id, true);
                sc.exp_argtab[arg_table_idx].value.flag = true;
            }
            // Update the argv vector with the group argument
            argv.push_back(group_buffer);
            // Update argc appropiately
            sc.argc++;
        }
        else{
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
            use_flag_value = false;
            switch (sc.exp_argtab[arg_table_idx].data_type){
            case APDataType::UNSIGNED_INT:
                loc_value.number_u64 = rnd->gen_integral<uint64_t>();
                // Pick between hex or decimal
                result_bool = rnd->gen_bool();
                if(result_bool){
                    value_for_argv = integer_to_hex_string<uint64_t>(loc_value.number_u64);
                }
                else{
                    value_for_argv = std::to_string(loc_value.number_u64);
                }
                break;
            case APDataType::SIGNED_INT:
                loc_value.number_i64 = rnd->gen_integral<int64_t>();
                // Pick between hex or decimal
                result_bool = rnd->gen_bool();
                if(result_bool){
                    value_for_argv = integer_to_hex_string<int64_t>(loc_value.number_i64);
                }
                else{
                    value_for_argv = std::to_string(loc_value.number_i64);
                }
                break;    
            case APDataType::TEXT:
                result_u32 = rnd->gen_integral_range<uint32_t>(1, MAX_TEXT_ARG_LEN);
                loc_value.text = std::make_shared<std::string>(rnd->gen_string(result_u32, nullptr));
                value_for_argv = *loc_value.text;
                break;      
            case APDataType::FLAG:
                use_flag_value = rnd->gen_bool();
                // Whether to include a value for FLAG argument or not
                if(use_flag_value){
                    result_u32 = rnd->gen_integral_range<uint32_t>(0, (sizeof(valid_flag_values) / sizeof(valid_flag_values[0])) - 1);
                    flag_value = valid_flag_values[result_u32];
                    loc_value.flag = valid_flag_values_dict.at(flag_value);
                }
                else{
                    loc_value.flag = true;
                }
                break;    
            default:
                break;
            }

            // Set argument value
            switch(sc.exp_argtab[arg_table_idx].data_type){
            case APDataType::UNSIGNED_INT:
                sc.exp_argtab[arg_table_idx].value.number_u64 = loc_value.number_u64;
                break;
            case APDataType::SIGNED_INT:
                sc.exp_argtab[arg_table_idx].value.number_i64 = loc_value.number_i64;
                break;
            case APDataType::TEXT:
                sc.exp_argtab[arg_table_idx].value.text = std::make_shared<std::string>(*loc_value.text);
                break;
            case APDataType::FLAG:
                sc.exp_argtab[arg_table_idx].value.flag = loc_value.flag;
                break;
            default:
                break;
            }
            
            // Update the argv vector with argument we just created
            argv.push_back(arg_id);
            if(sc.exp_argtab[arg_table_idx].data_type != APDataType::FLAG){
                argv.push_back(value_for_argv);
            }
            else if(sc.exp_argtab[arg_table_idx].data_type == APDataType::FLAG && use_flag_value){
                argv.push_back(flag_value);
            }

            // Update argc appropiately
            switch (sc.exp_argtab[arg_table_idx].data_type){
            case APDataType::UNSIGNED_INT:
            case APDataType::SIGNED_INT:
            case APDataType::TEXT:
                sc.argc += 2;
                break;
            case APDataType::FLAG:
                if(use_flag_value){
                    sc.argc += 2;
                }
                else{
                    sc.argc++;
                }
                break;
            default:
                break;
            }
        }
    }

    // Convert std::vector<std::string> to char** so it can simulate the char* argv[]
    vector_to_char_array(argv, sc.argv);
}


void validate_VALID_FLAG_GROUP_scenario(ErrorReporter* er, ScenarioData& sc){
    std::string buffer{};

    // Result vs. Expected error mesage
    er->log_it("Result   : res_error_message = \"" + sc.res_error_message + "\"");
    er->log_it("Expected : exp_error_message = \"" + sc.exp_error_message + "\"");
    if(sc.res_error_message != sc.exp_error_message){
        er->mark_error();
        er->log_it("!!! ERROR: error_message MISMATCH");
    }
    // Result vs. Expected argument table size
    er->log_it("Result   : size of argtab = " + std::to_string(sc.res_argtab.size()));
    er->log_it("Expected : size of argtab = " + std::to_string(sc.exp_argtab.size()));
    if(sc.res_argtab.size() != sc.exp_argtab.size()){
        er->mark_error();
        er->log_it("!!! ERROR: size of argtab MISMATCH");
    }
    // Result vs. Expected argument tables
    for(size_t i = 0; i < sc.exp_argtab.size(); i++){
        // abbr_form field should not be altered
        if(sc.res_argtab[i].abbr_form != sc.exp_argtab[i].abbr_form){
            er->mark_error();
            er->log_it("!!! ERROR: abbr_form FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : abbr_form = " + sc.res_argtab[i].abbr_form);
            er->log_it("Expected : abbr_form = " + sc.exp_argtab[i].abbr_form);
        }
        // full_form field should not be altered
        if(sc.res_argtab[i].full_form != sc.exp_argtab[i].full_form){
            er->mark_error();
            er->log_it("!!! ERROR: full_form FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : full_form = " + sc.res_argtab[i].full_form);
            er->log_it("Expected : full_form = " + sc.exp_argtab[i].full_form);
        }
        // data_type field should not be altered
        if(sc.res_argtab[i].data_type != sc.exp_argtab[i].data_type){
            er->mark_error();
            er->log_it("!!! ERROR: data_type FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : data_type = " + APDataType_to_string(sc.res_argtab[i].data_type));
            er->log_it("Expected : data_type = " + APDataType_to_string(sc.exp_argtab[i].data_type));
        }
        // required field should not be altered
        if(sc.res_argtab[i].required != sc.exp_argtab[i].required){
            er->mark_error();
            er->log_it("!!! ERROR: required FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : required = " + bool_to_string(sc.res_argtab[i].required));
            er->log_it("Expected : required = " + bool_to_string(sc.exp_argtab[i].required));
        }
        // initialized should match the expected
        if(sc.res_argtab[i].initialized != sc.exp_argtab[i].initialized){
            er->mark_error();
            er->log_it("!!! ERROR: initialized FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : initialized = " + bool_to_string(sc.res_argtab[i].initialized));
            er->log_it("Expected : initialized = " + bool_to_string(sc.exp_argtab[i].initialized));
        }
        // value should match the expected
        if(sc.exp_argtab[i].initialized){
            switch(sc.exp_argtab[i].data_type){
            case APDataType::UNSIGNED_INT:
                if(sc.res_argtab[i].value.number_u64 != sc.exp_argtab[i].value.number_u64){
                    er->mark_error();
                    er->log_it("!!! ERROR: value FIELD MISMATCH");
                    er->log_it("Index    : " + std::to_string(i));
                    er->log_it("Result   : value = " + sc.res_argtab[i].value.number_u64);
                    er->log_it("Expected : value = " + sc.exp_argtab[i].value.number_u64);
                }
                break;
            case APDataType::SIGNED_INT:
                if(sc.res_argtab[i].value.number_i64 != sc.exp_argtab[i].value.number_i64){
                    er->mark_error();
                    er->log_it("!!! ERROR: value FIELD MISMATCH");
                    er->log_it("Index    : " + std::to_string(i));
                    er->log_it("Result   : value = " + sc.res_argtab[i].value.number_i64);
                    er->log_it("Expected : value = " + sc.exp_argtab[i].value.number_i64);
                }
                break;
            case APDataType::TEXT:
                if(*sc.res_argtab[i].value.text != *sc.exp_argtab[i].value.text){
                    er->mark_error();
                    er->log_it("!!! ERROR: value FIELD MISMATCH");
                    er->log_it("Index    : " + std::to_string(i));
                    er->log_it("Result   : value = " + *sc.res_argtab[i].value.text);
                    er->log_it("Expected : value = " + *sc.exp_argtab[i].value.text);
                }
                break;
            case APDataType::FLAG:
                if(sc.res_argtab[i].value.flag != sc.exp_argtab[i].value.flag){
                    er->mark_error();
                    er->log_it("!!! ERROR: value FIELD MISMATCH");
                    er->log_it("Index    : " + std::to_string(i));
                    er->log_it("Result   : value = " + sc.res_argtab[i].value.flag);
                    er->log_it("Expected : value = " + sc.exp_argtab[i].value.flag);
                }
                break;
            default:
                break;
            }
        }
    }
    er->log_it(">>> START OF ARGV <<<");
    er->log_it(describe_argv(sc.argc, sc.argv));
    er->log_it(">>> END OF ARGV <<<");
    er->log_it(">>> START OF RESULT ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.res_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF RESULT ARGUMENT TABLE <<<");
    er->log_it(">>> START OF EXPECTED ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.exp_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF EXPECTED ARGUMENT TABLE <<<");
}
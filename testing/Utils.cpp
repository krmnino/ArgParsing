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


std::string space_padding(std::string input_str, size_t width, std::string pad){
    std::string ret{};
    
    ret = input_str;
    if(input_str.size() >= width){
        return ret;
    }
    size_t padding = width - input_str.size();
    for(size_t i = 0; i < padding; i++){
        ret += pad;
    }
    return ret;
}

std::string row_print(std::vector<std::string>& row_contents, std::vector<size_t>& row_widths, std::string padding, std::string delimiter){
    std::stringstream buffer{};

    // Validate both vectors' lengths match
    if(row_contents.size() != row_widths.size()){
        return "";
    }

    // Print elements 
    for(size_t i = 0; i < row_contents.size(); i++){
        buffer << row_contents[i];
        for(size_t j = 0; j < (row_widths[i] - row_contents[i].size()); j++){
            buffer << padding;
        }
        if(i < row_contents.size() - 1){
            buffer << delimiter;
        }
        else{   
            buffer << std::endl;
        }
    }
    return buffer.str();
}


std::string describe_argv(int input_argc, char** input_argv){
    std::string buffer{};

    if(input_argv == nullptr){
        return buffer;
    }
    buffer += " ";
    buffer += space_padding("INDEX", PRT_IDX_STR_WIDTH, " ") + " | ";
    buffer += space_padding("VALUE", PRT_VALUE_STR_WIDTH, " ") + "\n";
    buffer += "-";
    buffer += space_padding("-", PRT_IDX_STR_WIDTH, "-") + "-+-";
    buffer += space_padding("-", PRT_VALUE_STR_WIDTH, "-") + "\n";
    for(int i = 0; i < input_argc; i++){
        buffer += " ";
        buffer += space_padding(std::to_string(i), PRT_IDX_STR_WIDTH, " ") + " | ";
        buffer += space_padding(input_argv[i], PRT_VALUE_STR_WIDTH, " ") + "\n";
    }
    return buffer;
}


std::string ScenarioType_to_string(ScenarioType st){
    std::string ret{};

    ret = "ScenarioType::";
    switch (st){
    case ScenarioType::OK:
        ret += "OK";
        break;
    case ScenarioType::MISSING_FIRST_DASH:
        ret += "MISSING_FIRST_DASH";
        break;
    case ScenarioType::MISSING_REQUIRED_ARG:
        ret += "MISSING_REQUIRED_ARG";
        break;
    case ScenarioType::UNKNOWN_ARGUMENT:
        ret += "UNKNOWN_ARGUMENT";
        break;
    case ScenarioType::REPEATED_ARGUMENT:
        ret += "REPEATED_ARGUMENT";
        break;
    case ScenarioType::MUST_BE_FLAG:
        ret += "MUST_BE_FLAG";
        break;
    case ScenarioType::BAD_NUMERIC_VALUE:
        ret += "BAD_NUMERIC_VALUE";
        break;
    case ScenarioType::EMPTY_ARG_LIST:
        ret += "EMPTY_ARG_LIST";
        break;
    case ScenarioType::VALID_FLAG_GROUP:
        ret += "VALID_FLAG_GROUP";
        break;
    case ScenarioType::INVALID_FLAG_GROUP:
        ret += "INVALID_FLAG_GROUP";
        break;
    case ScenarioType::EXPECTING_VALUE:
        ret += "EXPECTING_VALUE";
        break;
    default:
        ret += "UNDEFINED";
        break;
    }
    return ret;
}


std::string bool_to_string(bool data){
    return (data) ? "true" : "false";
}


std::string arg_table_to_string(std::vector<APTableEntry>& arg_table){
    std::stringstream buffer{};
    std::string idx_str{};
    std::string abbr_form_str{};
    std::string full_form_str{};
    std::string data_type_str{};
    std::string required_str{};
    std::string default_value_str{};
    std::string initialized_str{};
    std::string value_str{};
    std::vector<size_t> table_max_lengths{};
    std::vector<std::string> row_print_buffer{};

    // Initialize vectors of field lengths and buffer for row printing
    table_max_lengths.reserve(argtab_header.size());
    row_print_buffer.reserve(argtab_header.size());
    for(size_t i = 0; i < argtab_header.size(); i++){
        table_max_lengths.push_back(argtab_header[i].size());
        row_print_buffer.push_back("");
    }

    // Find largest values for initial/expected/result table's rows, for each row field
    for(size_t i = 0; i < arg_table.size(); i++){
        for(size_t j = 1; j < argtab_header.size(); j++){
            table_max_lengths[j] = std::max(arg_table[i].get_by_idx_for_iniexpres_tab(j).size(), table_max_lengths[j]);
        }
    }
    
    // Print table header
    buffer << row_print(argtab_header, table_max_lengths, " ", " | ");
    for(size_t i = 0; i < argtab_header.size(); i++){
        row_print_buffer[i] = "";
    }
    buffer << row_print(row_print_buffer, table_max_lengths, "-", "-+-");

    // Print table contents
    for(size_t i = 0; i < arg_table.size(); i++){
        row_print_buffer[0] = std::to_string(i);
        for(size_t j = 1; j < argtab_header.size(); j++){
            row_print_buffer[j] = arg_table[i].get_by_idx_for_iniexpres_tab(j);
        }
        buffer << row_print(row_print_buffer, table_max_lengths, " ", " | ");
        row_print_buffer[0] = "";
    } 

    return buffer.str();
}


std::string arg_table_ini_exp_res(std::vector<APTableEntry>& ini_argtab,
                                  std::vector<APTableEntry>& exp_argtab,
                                  std::vector<APTableEntry>& res_argtab,
                                  std::vector<ErrorType>& arg_tab_miscompare){
    std::stringstream buffer{};
    std::string idx_str{};
    std::string abbr_form_str{};
    std::string full_form_str{};
    std::string data_type_str{};
    std::string required_str{};
    std::string default_value_str{};
    std::string initialized_str{};
    std::string value_str{};
    std::vector<size_t> table_max_lengths{};
    std::vector<std::string> row_print_buffer{};

    // Validate initial/expected/result table lengths
    if(ini_argtab.size() != exp_argtab.size() && exp_argtab.size() != res_argtab.size()){
        return "";
    }

    // Initialize vectors of field lengths and buffer for row printing
    table_max_lengths.reserve(argtab_err_header.size());
    row_print_buffer.reserve(argtab_err_header.size());
    for(size_t i = 0; i < argtab_err_header.size(); i++){
        table_max_lengths.push_back(argtab_err_header[i].size());
        row_print_buffer.push_back("");
    }

    // Find largest values for initial/expected/result table's rows, for each row field
    for(size_t i = 0; i < ini_argtab.size(); i++){
        for(size_t j = 0; j < argtab_err_header.size(); j++){
            if(argtab_err_header[j] == "INDEX"){
                table_max_lengths[j] = std::max(std::to_string(j).size(), table_max_lengths[j]);
            }
            else if(argtab_err_header[j] == "ERRORS"){
                if((arg_tab_miscompare[j]) != ErrorType::OK){
                    table_max_lengths[j] = std::max(std::string("-").size(), table_max_lengths[j]);
                }
                else{
                    table_max_lengths[j] = std::max(ErrorType_to_string(arg_tab_miscompare[j]).size(), table_max_lengths[j]);
                }
            }
            else{
                table_max_lengths[j] = std::max(ini_argtab[i].get_by_idx_for_iniexpres_tab(j - 2).size(), table_max_lengths[j]);
                table_max_lengths[j] = std::max(exp_argtab[i].get_by_idx_for_iniexpres_tab(j - 2).size(), table_max_lengths[j]);
                table_max_lengths[j] = std::max(res_argtab[i].get_by_idx_for_iniexpres_tab(j - 2).size(), table_max_lengths[j]);
            }
        }
    }
    
    // Print table header
    buffer << row_print(argtab_err_header, table_max_lengths, " ", " | ");
    for(size_t i = 0; i < argtab_err_header.size(); i++){
        row_print_buffer[i] = "";
    }
    buffer << row_print(row_print_buffer, table_max_lengths, "-", "-+-");

    // Print table contents
    for(size_t i = 0; i < ini_argtab.size(); i++){
        // Index value to string
        row_print_buffer[0] = std::to_string(i);
        
        // ErrorTypes to string if not ErrorType::OK
        if(arg_tab_miscompare[i] != ErrorType::OK){
            row_print_buffer[1] = ErrorType_to_string(arg_tab_miscompare[i]);
        }
        else{
            row_print_buffer[1] = "-";
        }

        // Print contents from initial argument table
        for(size_t j = 2; j < argtab_err_header.size(); j++){
            row_print_buffer[j] = ini_argtab[i].get_by_idx_for_iniexpres_tab(j - 2);
        }
        buffer << row_print(row_print_buffer, table_max_lengths, " ", " | ");
        row_print_buffer[0] = "";
        row_print_buffer[1] = "";

        // Print contents from expected argument table
        for(size_t j = 2; j < argtab_err_header.size(); j++){
            row_print_buffer[j] = exp_argtab[i].get_by_idx_for_iniexpres_tab(j - 2);
        }
        buffer << row_print(row_print_buffer, table_max_lengths, " ", " | ");
        
        // Print contents from result argument table
        for(size_t j = 2; j < argtab_err_header.size(); j++){
            row_print_buffer[j] = res_argtab[i].get_by_idx_for_iniexpres_tab(j - 2);
        }
        buffer << row_print(row_print_buffer, table_max_lengths, " ", " | ");
    } 

    return buffer.str();
}


std::string APErrRsn_to_string(APErrRsn rsn){
    switch (rsn){
    case APErrRsn::MISSING_FIRST_DASH:
        return "APErrRsn::MISSING_FIRST_DASH";
    case APErrRsn::MISSING_REQUIRED_ARG:
        return "APErrRsn::MISSING_REQUIRED_ARG";
    case APErrRsn::UNKNOWN_ARGUMENT:
        return "APErrRsn::UNKNOWN_ARGUMENT";
    case APErrRsn::REPEATED_ARGUMENT:
        return "APErrRsn::REPEATED_ARGUMENT";
    case APErrRsn::MUST_BE_FLAG:
        return "APErrRsn::MUST_BE_FLAG";
    case APErrRsn::BAD_NUMERIC_VALUE:
        return "APErrRsn::BAD_NUMERIC_VALUE";
    case APErrRsn::EXPECTING_VALUE:
        return "APErrRsn::EXPECTING_VALUE";
    default:
        break;
    }
    return "APErrRsn::UNDEFINED";
}


void vector_to_char_array(std::vector<std::string>& input_vect, char**& output_char_arr){
    // Allocate the array of char pointers + 1 for the null terminator
    output_char_arr = new char*[input_vect.size() + 1];
    // Loop through the argv vector, allocate and copy over the individual strings
    for(size_t i = 0; i < input_vect.size(); i++){
        output_char_arr[i] = new char[input_vect[i].size() + 1];
        strcpy(output_char_arr[i], input_vect[i].c_str());
    }
    // Set null terminator
    output_char_arr[input_vect.size()] = nullptr;
}


size_t arg_table_count_data_type(std::vector<APTableEntry>& arg_table, APDataType input_data_type){
    size_t accumulator{};

    accumulator = 0;
    for(size_t i = 0; i < arg_table.size(); i++){
        if(arg_table[i].data_type == input_data_type){
            accumulator++;
        }
    }
    return accumulator;
}


bool arg_table_is_required_arg(std::vector<APTableEntry>& arg_table, size_t idx){
    return (arg_table[idx].required) ? true : false;
}


size_t arg_table_count_required(std::vector<APTableEntry>& arg_table){
    size_t accumulator{};
    
    accumulator = 0;
    for(size_t i = 0; i < arg_table.size(); i++){
        if(arg_table_is_required_arg(arg_table, i)){
            accumulator++;
        }
    }
    return accumulator;
}


size_t arg_table_count_type(std::vector<APTableEntry>& arg_table, APDataType input_data_type){
    size_t accumulator{};
    
    accumulator = 0;
    for(size_t i = 0; i < arg_table.size(); i++){
        if(arg_table[i].data_type == input_data_type){
            accumulator++;
        }
    }
    return accumulator;
}


bool arg_table_is_abbr_form_available(std::vector<APTableEntry>& arg_table, size_t idx){
    return (arg_table[idx].abbr_form.size() == 0) ? false : true;
}


size_t arg_table_count_abbr_form(std::vector<APTableEntry>& arg_table){
    size_t accumulator{};
    
    accumulator = 0;
    for(size_t i = 0; i < arg_table.size(); i++){
        if(arg_table_is_abbr_form_available(arg_table, i)){
            accumulator++;
        }
    }
    return accumulator;
}


int32_t arg_table_find_arg_index(std::vector<APTableEntry>& arg_table, std::string& arg_key, bool is_abbr_input){
    for(size_t i = 0; i < arg_table.size(); i++){
        if(is_abbr_input){
            if(arg_table[i].abbr_form == arg_key){
                return i;
            }
        }
        else{
            if(arg_table[i].full_form == arg_key){
                return i;
            }
        }
    }
    return -1;
}


void gen_arg_value(Randomizer* rnd, APValuePackage& package){
    const char* valid_flag_values[] = VALID_FLAG_VALUES;
    std::stringstream stream;
    uint32_t result_u32;
    bool result_bool;

    result_u32 = 0;
    switch (package.data_type){
    case APDataType::UNSIGNED_INT:
        package.apv.number_u64 = rnd->gen_integral<uint64_t>();
        if(package.to_string){
            // Stringify as hexadecimal or decimal
            result_bool = rnd->gen_bool();
            if(package.to_string){
                package.stringified = integer_to_hex_string<uint64_t>(package.apv.number_u64);
            }
            else{
                package.stringified = std::to_string(package.apv.number_u64);
            }
        }
        break;    
    case APDataType::SIGNED_INT:
        package.apv.number_i64 = rnd->gen_integral<int64_t>();
        if(package.to_string){
            // Stringify as hexadecimal or decimal
            result_bool = rnd->gen_bool();
            if(result_bool){
                package.stringified = integer_to_hex_string<int64_t>(package.apv.number_i64);
            }
            else{
                package.stringified = std::to_string(package.apv.number_i64);
            }
        }
        break;    
    case APDataType::TEXT:
        result_u32 = rnd->gen_integral_range<uint32_t>(1, MAX_TEXT_ARG_LEN);
        package.apv.text = std::make_shared<std::string>(rnd->gen_string(result_u32, nullptr));
        if(package.to_string){
            package.stringified = *package.apv.text;
        }
        break;    
    case APDataType::FLAG:
        if(package.to_string){
            result_u32 = rnd->gen_integral_range<uint32_t>(0, (sizeof(valid_flag_values) / sizeof(valid_flag_values[0])) - 1);
            package.stringified = valid_flag_values[result_u32];
            package.apv.flag = valid_flag_values_dict.at(package.stringified);
        }
        else{
            package.apv.flag = rnd->gen_bool();
        }
        break;    
    case APDataType::FLOAT:
        package.apv.number_fpt = rnd->gen_float<double>();
        if(package.to_string){
            stream << std::fixed << std::setprecision(8) << package.apv.number_fpt;
            package.stringified = stream.str(); 
        }
        break;
    default:
        break;
    }
}

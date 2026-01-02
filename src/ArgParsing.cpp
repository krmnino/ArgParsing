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


#include "ArgParsing.hpp"
#include "ArgParsing_C.h"


ArgParsing::ArgParsing() {
    this->state = APState::ARGV_BEGIN;
    this->argc = 0;
    this->argv = nullptr;
    this->argv_idx = 1; // Set to 1 to skip program name
    this->eval_arg_idx = 0;
    this->is_table_set = false;
    #ifdef DEBUG
    this->error_msg = "";
    #endif
}


ArgParsing::~ArgParsing() {}


int32_t ArgParsing::get_index_in_arg_table(std::string& arg_key, bool is_abbr_input){
    for(int32_t i = 0; i < (int32_t)this->arg_table.size(); i++){
        if(is_abbr_input){
            if(this->arg_table[i].abbr_form == arg_key){
                return i;
            }
        }
        else{
            if(this->arg_table[i].full_form == arg_key){
                return i;
            }    
        }
    }
    #ifndef DEBUG
    std::cerr << "ERROR: Argument with identification \"" << arg_key <<
              "\" [is_abbr_input:" << is_abbr_input << "]" <<" does not exist within the argument table." << std::endl;
    #endif
    return -1;
}


int ArgParsing::set_input_args(int input_argc, char** input_argv){
    // Validate input
    if(input_argv == nullptr){
        #ifndef DEBUG
        std::cerr << "ERROR: input_argc cannot be less than 1." << std::endl;
        #endif
        return -1;
    }
    if(input_argv == nullptr){
        #ifndef DEBUG
        std::cerr << "ERROR: input_argv is a nullptr." << std::endl;
        #endif
        return -1;
    }

    this->argc = input_argc;
    this->argv = input_argv;
    return 0;
}


int ArgParsing::set_arg_table(APTableEntry* arg_table_ptr, size_t n_entries){
    const char* valid_flag_values[] = VALID_FLAG_VALUES;
    if(this->is_table_set){
        return -1;
    }
    // Copy array contents over to std::vector
    this->arg_table.assign(arg_table_ptr, arg_table_ptr + n_entries);

    for(size_t i = 0; i < this->arg_table.size(); i++){
        for(size_t j = 0; j < sizeof(valid_flag_values) / sizeof(valid_flag_values[0]); j++){
            if(this->arg_table[i].abbr_form != "" && this->arg_table[i].abbr_form == valid_flag_values[j]){
                std::cerr << "ERROR: -" << this->arg_table[i].abbr_form << " is not an allowed abbreviated form argument identifer." << std::endl;
                return -1;
            }
            if(this->arg_table[i].full_form == valid_flag_values[j]){
                std::cerr << "ERROR: --" << this->arg_table[i].full_form << " is not an allowed full form argument identifer." << std::endl;
                return -1;
            }
        }
    }
    // Check for duplicate abbreviated form identifiers
    for(size_t i = 0; i < this->arg_table.size(); i++){
        if(this->arg_table[i].abbr_form == ""){
            continue;
        }
        for(size_t j = i + 1; j < this->arg_table.size(); j++){
            if(this->arg_table[i].abbr_form == this->arg_table[j].abbr_form){
                std::cerr << "ERROR: -" << this->arg_table[j].abbr_form << " is a duplicate abbreviated form argument identifier." << std::endl;
                return -1;
            }
        }
    }
    // Check for duplicate full form identifiers
    for(size_t i = 0; i < this->arg_table.size(); i++){
        for(size_t j = i + 1; j < this->arg_table.size(); j++){
            if(i == j){
                continue;
            }
            if(this->arg_table[i].full_form == this->arg_table[j].full_form){
                std::cerr << "ERROR: --" << this->arg_table[j].full_form << " is a duplicate full form argument identifier." << std::endl;
                return -1;
            }
        }
    }
    is_table_set = true;
    return 0;
}


int ArgParsing::set_arg_table(std::vector<APTableEntry>& arg_table){
    size_t n_entries{};
    APTableEntry* arg_table_ptr{};
    
    n_entries = arg_table.size();
    arg_table_ptr = arg_table.data();
    return set_arg_table(arg_table_ptr, n_entries);
}


size_t ArgParsing::get_arg_value_bytesize(std::string arg_id, bool is_abbr_input){
    int32_t arg_table_idx{};
    size_t ret_size{};

    // Check if argument identifier exists
    arg_table_idx = this->get_index_in_arg_table(arg_id, is_abbr_input);
    if(arg_table_idx == -1){
        return 0;
    }
    
    // Set the size value to return
    switch (this->arg_table[arg_table_idx].data_type){
    case APDataType::TEXT:
        if(this->arg_table[arg_table_idx].data.text != nullptr){
            ret_size = (*this->arg_table[arg_table_idx].data.text).size();
        }
        break;
    case APDataType::FLAG:
        ret_size = sizeof(bool);
        break;
    case APDataType::UNSIGNED_INT:
        ret_size = sizeof(uint64_t);    
        break;
    case APDataType::SIGNED_INT:
        ret_size = sizeof(int64_t);    
        break;
    default:
        break;
    }
    return ret_size;
}


bool ArgParsing::is_valid_hex(std::string& input){
    std::string input_copy{};
    bool valid{};
    
    // Validate input
    if(input.size() == 0){
        return false;
    }
       
    input_copy = input;
    valid = true;
    // A hex number must always start with '0x'
    if(input_copy[0] != '0' || input_copy[1] != 'x'){
        return false;
    }
    input_copy = input_copy.substr(2);
    // Check each character is a hex digit
    for(size_t i = 0; i < input_copy.size() && valid; i++){
        if(!((input_copy[i] >= '0' && input_copy[i] <= '9') ||
             (input_copy[i] >= 'A' && input_copy[i] <= 'F') ||
             (input_copy[i] >= 'a' && input_copy[i] <= 'f'))){
            valid = false;
        }
    }
    return valid;
}


bool ArgParsing::is_valid_dec(std::string& input){
    std::string input_copy{};
    bool valid{};

    // Validate input
    if(input.size() == 0){
        return false;
    }

    input_copy = input;
    valid = true;
    // Might be a negative number. If so, skip the minus sign
    if(input_copy[0] == '-'){
        input_copy = input_copy.substr(1);
    }
    // Check each character is a decimal digit
    for(size_t i = 0; i < input_copy.size() && valid; i++){
        if(!(input_copy[i] >= '0' && input_copy[i] <= '9')){
            valid = false;
        }
    }
    return valid;
}


void ArgParsing::arg_begin(){
    std::string curr{};

    // We guarantee that curr is not an empty string
    curr = this->argv[argv_idx];
    // If first char is not a dash, set error state
    if(curr[0] != '-'){
        this->state = APState::ERROR;
        this->reason = APErrRsn::MISSING_FIRST_DASH;
        return;
    }
    // Whether the parameter identifier is in full or abbreviated form 
    if(curr[1] == '-'){
        this->state = APState::ARGV_FULL_ID;
        this->arg_full_form();
    }
    else{
        this->state = APState::ARGV_ABBR_ID;
        this->arg_abbr_form();
    }
}


void ArgParsing::arg_abbr_form(){
    std::string abbr_arg{};

    abbr_arg = this->argv[this->argv_idx];
    if(abbr_arg.size() == 2){
        abbr_arg = std::string(1, abbr_arg[1]);
        // Search for abbreviated identifier in argument table 
        // If not found, returned index is -1 and set error state
        this->eval_arg_idx = this->get_index_in_arg_table(abbr_arg, true);
        if(this->eval_arg_idx == -1){
            this->state = APState::ERROR;
            this->reason = APErrRsn::UNKNOWN_ARGUMENT;
            this->err_msg_data.push_back("-" + abbr_arg);
            return;
        }
        // Check the current argument hasn't been passed more than once 
        if(this->arg_table[this->eval_arg_idx].initialized){
            this->state = APState::ERROR;
            this->reason = APErrRsn::REPEATED_ARGUMENT;
            this->err_msg_data.push_back("-" + abbr_arg);
            return;    
        }
        // Set FLAG argument value to true
        if(this->arg_table[this->eval_arg_idx].data_type == APDataType::FLAG){
            this->arg_table[this->eval_arg_idx].data.flag = true;
        }
        // Set to initialized and update state
        this->arg_table[this->eval_arg_idx].initialized = true;
        this->state = APState::ARGV_VALUE;
    }
    else{
        // Loop through group of abbreviated form identifiers in table
        // All identifiers must be of FLAG type
        for(size_t i = 1; i < strlen(this->argv[this->argv_idx]); i++){
            abbr_arg = std::string(1, this->argv[this->argv_idx][i]);
            // Search for abbreviated identifier in argument table
            // If not found, returned index is -1 and set error state
            this->eval_arg_idx = this->get_index_in_arg_table(abbr_arg, true);
            if(this->eval_arg_idx == -1){
                this->state = APState::ERROR;
                this->reason = APErrRsn::UNKNOWN_ARGUMENT;
                this->err_msg_data.push_back("-" + abbr_arg);
                return;
            }
            // Check the current argument hasn't been passed more than once
            if(this->arg_table[this->eval_arg_idx].initialized){
                this->state = APState::ERROR;
                this->reason = APErrRsn::REPEATED_ARGUMENT;
                this->err_msg_data.push_back("-" + abbr_arg);
                return;    
            }
            // Since it is a group of identifiers, we can only accept arguments of type FLAG
            if(this->arg_table[this->eval_arg_idx].data_type != APDataType::FLAG){
                this->state = APState::ERROR;
                this->reason = APErrRsn::MUST_BE_FLAG;
                this->err_msg_data.push_back("-" + abbr_arg);
                return;    
            }
            this->arg_table[this->eval_arg_idx].data.flag = true;
            this->arg_table[this->eval_arg_idx].initialized = true;
        }
        this->state = APState::ARGV_BEGIN;
    }
    this->prev_argv_element = this->argv[this->argv_idx];
    this->argv_idx++;
}


void ArgParsing::arg_full_form(){
    std::string full_arg{};
    
    full_arg = this->argv[this->argv_idx];
    full_arg = full_arg.substr(2);
    // Search for full identifier in argument table
    // If not found, returned index is -1 and set error state
    this->eval_arg_idx = this->get_index_in_arg_table(full_arg, false);
    if(this->eval_arg_idx == -1){
        this->state = APState::ERROR;
        this->reason = APErrRsn::UNKNOWN_ARGUMENT;
        this->err_msg_data.push_back("--" + full_arg);
        return;
    }
    // Check the current argument hasn't been passed more than once
    if(this->arg_table[this->eval_arg_idx].initialized){
        this->state = APState::ERROR;
        this->reason = APErrRsn::REPEATED_ARGUMENT;
        this->err_msg_data.push_back("--" + full_arg);
        return;    
    }    
    // Set FLAG argument value to true
    if(this->arg_table[this->eval_arg_idx].data_type == APDataType::FLAG){
        this->arg_table[this->eval_arg_idx].data.flag = true;
    }

    // Set to initialized and update state
    this->state = APState::ARGV_VALUE;
    this->arg_table[this->eval_arg_idx].initialized = true;
    this->prev_argv_element = this->argv[this->argv_idx];
    this->argv_idx++;
}


void ArgParsing::arg_value(){
    std::string value{};

    value = this->argv[this->argv_idx];
    switch(this->arg_table[this->eval_arg_idx].data_type){
    case APDataType::UNSIGNED_INT:
    case APDataType::SIGNED_INT:
        // Check if it is a hexadecimal value.
        // If so, validate it. Otherwise validate it as decimal
        if(value.size() >= 2){
            if(this->is_valid_hex(value)){
                value = value.substr(2);
                this->arg_table[this->eval_arg_idx].data.intdata.number_u64 = std::stoul(value, nullptr, 16);
            }
            else if(this->is_valid_dec(value)){
                this->arg_table[this->eval_arg_idx].data.intdata.number_u64 = std::stoul(value);
            }
            else{
                this->state = APState::ERROR;
                this->reason = APErrRsn::BAD_NUMERIC_VALUE;
                this->err_msg_data.push_back(value);
                this->err_msg_data.push_back("--" + this->arg_table[this->eval_arg_idx].full_form);
                return;
            }
        }
        else{
            if(this->is_valid_dec(value)){
                this->arg_table[this->eval_arg_idx].data.intdata.number_u64 = std::stoi(value);
            }
            else{
                this->state = APState::ERROR;
                this->reason = APErrRsn::BAD_NUMERIC_VALUE;
                this->err_msg_data.push_back(value);
                this->err_msg_data.push_back("--" + this->arg_table[this->eval_arg_idx].full_form);
                return;
            }
        }
        break;
    case APDataType::TEXT:
        this->arg_table[this->eval_arg_idx].data.text = new std::string(value);
        break;
    case APDataType::FLAG:
        if(this->validate_flag_value(value)){
            return;
        }
    default:
        break;
    }
    this->state = APState::ARGV_BEGIN;
    this->prev_argv_element = this->argv[this->argv_idx];
    this->argv_idx++;
}


std::string ArgParsing::APErrRsn_to_string(APErrRsn rsn){
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
    default:
        break;
    }
    return "APErrRsn::UNDEFINED";
}


void ArgParsing::display_error_msg(){
    std::string rsn_str{};
    
    rsn_str = APErrRsn_to_string(this->reason);
    switch (this->reason){
    case APErrRsn::MISSING_FIRST_DASH:
        this->error_msg = rsn_str + ": all argument identifiers must start with a dash (-).";
        break;
    case APErrRsn::MISSING_REQUIRED_ARG:    
        this->error_msg = rsn_str + ": the required argument " + err_msg_data[0] + " is missing.";
        break;
    case APErrRsn::UNKNOWN_ARGUMENT:    
        this->error_msg = rsn_str + ": the provided argument " + err_msg_data[0] + " is an unknown.";
        break;
    case APErrRsn::REPEATED_ARGUMENT:    
        this->error_msg = rsn_str + ": the provided argument " + err_msg_data[0] + " is repeated.";
        break;
    case APErrRsn::MUST_BE_FLAG:    
        this->error_msg = rsn_str + ": the provided argument " + err_msg_data[0] + " is of type FLAG. It must be especified alone or followed by one of these values: \"0\", \"1\", \"false\", or \"true\".";
        break;
    case APErrRsn::BAD_NUMERIC_VALUE:    
        this->error_msg = rsn_str + ": \"" + err_msg_data[0] + "\" provided to the argument " + err_msg_data[1] + " is not a valid numeric value.";
        break;
    default:
        break;
    }
    #ifndef DEBUG
    std::cout << this->error_msg << std::endl;
    #endif
}


bool ArgParsing::validate_flag_value(std::string& value){
    const char* valid_flag_values[] = VALID_FLAG_VALUES;
    std::string prev_arg_id{};
    std::string arg_id{};
    int table_idx{};
    bool found{}; 
    bool group_id{}; 
    bool arg_value_fn_quick_exit{}; 
    
    arg_value_fn_quick_exit = false;
    found = false;
    group_id = false;
    for(size_t i = 0; i < sizeof(valid_flag_values) / sizeof(valid_flag_values[0]); i++){
        if(value == valid_flag_values[i]){
            found = true;
            break;
        }
    }
    // Found a valid value match
    if(found){
        // Set value of previous FLAG argument based on the current value
        // This will override the previously set value by arg_abbr_form() or arg_full_form()
        if(this->prev_argv_element[1] == '-'){
            prev_arg_id = this->prev_argv_element.substr(2);
            table_idx = get_index_in_arg_table(prev_arg_id, false);
            this->arg_table[table_idx].data.flag = true;
        }
        else{
            prev_arg_id = this->prev_argv_element.substr(1);
            table_idx = get_index_in_arg_table(prev_arg_id, true);
            this->arg_table[table_idx].data.flag = true;
        }
        if(value == "1" || value == "true" || value == "TRUE"){
            this->arg_table[table_idx].data.flag = true;
        }
        else{
            this->arg_table[table_idx].data.flag = false;
        }
    }
    // Check if it is a valid abbreviated/full form identifier that can be processed
    else if(value[0] == '-' && value.size() > 1){
        if(value.size() > 2 && value[1] == '-'){
            arg_id = value.substr(2);
            table_idx = get_index_in_arg_table(arg_id, false);
        }
        else if(value.size() == 2){
            arg_id = value.substr(1);
            table_idx = get_index_in_arg_table(arg_id, true);
        }
        else{
            group_id = true;
        }
        // Value is actually a valid argument identifier, initialize the previous FLAG
        // argument, then go analyze the next valid argument
        if(table_idx != -1 || group_id){
            this->state = APState::ARGV_BEGIN;
            arg_value_fn_quick_exit = true;
        }
        // Value cannot be an argument identifier, error out
        else{
            this->state = APState::ERROR;
            this->reason = APErrRsn::UNKNOWN_ARGUMENT;
            this->err_msg_data.push_back(value);
            arg_value_fn_quick_exit = true;
        }
    }
    else{
        // Otherwise, it may be an argument identifier missing the dashes. If so, pass 
        // the hot potato to the caller so they can figure out the error. Else, error out right away.
        arg_id = value;
        if(arg_id.size() == 1){
            table_idx = get_index_in_arg_table(arg_id, true);
        }
        else{
            table_idx = get_index_in_arg_table(arg_id, false);
        }
        if(table_idx != -1){
            this->state = APState::ARGV_BEGIN;
            arg_value_fn_quick_exit = true;
        }
        else{
            this->state = APState::ERROR;
            this->reason = APErrRsn::MUST_BE_FLAG;
            this->err_msg_data.push_back(this->prev_argv_element);
            arg_value_fn_quick_exit = true;
        }
    }
    return arg_value_fn_quick_exit;
}


int ArgParsing::parse(){
    while(this->argv_idx < this->argc){
        if(this->state == APState::ERROR){
            break;
        }
        else if(this->state == APState::ARGV_BEGIN){
            this->arg_begin();
        }
        else if(this->state == APState::ARGV_VALUE){
            this->arg_value();
        }
    }
    if(this->state == APState::ERROR){
        this->display_error_msg();
        return -1;
    }
    for(size_t i = 0; i < this->arg_table.size(); i++){
        if(this->arg_table[i].required && !this->arg_table[i].initialized){
            this->state = APState::ERROR;
            this->reason = APErrRsn::MISSING_REQUIRED_ARG;
            this->err_msg_data.push_back("--" + this->arg_table[i].full_form);
            break;
        }
    }
    if(this->state == APState::ERROR){
        this->display_error_msg();
        return -1;
    }
    return 0;
}


#ifdef DEBUG
void ArgParsing::get_arg_table(std::vector<APTableEntry>& target){
    // Copy the vector first
    target = this->arg_table;
    // For any initialized TEXT type arugments, copy their values
    for(size_t i = 0; i < this->arg_table.size(); i++){
        if(target[i].initialized && target[i].data_type == APDataType::TEXT){
            target[i].data.text = new std::string(*this->arg_table[i].data.text);
        }
    }
}


void ArgParsing::get_error_msg(std::string& target){
    target = this->error_msg;
}


void ArgParsing::display_arg_table(){
    std::string data_type_str{};
    std::string value_str{};
    std::string required_str{};
    std::string initialized_str{};

    for(size_t i = 0; i < this->arg_table.size(); i++){
        std::cout << "Abbreviated Form: " << this->arg_table[i].abbr_form << std::endl;
        std::cout << "Full Form:        " << this->arg_table[i].full_form << std::endl;
        switch (this->arg_table[i].data_type){
        case APDataType::FLAG:
            data_type_str = "FLAG";
            break;        
        case APDataType::UNSIGNED_INT:
            data_type_str = "UNSIGNED_INT";
            break;        
        case APDataType::SIGNED_INT:
            data_type_str = "SIGNED_INT";
            break;        
        case APDataType::TEXT:
            data_type_str = "TEXT";
            break;        
        default:
            data_type_str = "NONE";
            break;
        }
        std::cout << "Data Type:        " << data_type_str << std::endl;
        if(this->arg_table[i].initialized){
            switch (this->arg_table[i].data_type){
            case APDataType::FLAG:
                if(this->arg_table[i].data.flag){
                    value_str = "true";
                }
                else{
                    value_str = "false";
                }
                break;        
            case APDataType::UNSIGNED_INT:
                value_str = std::to_string(this->arg_table[i].data.intdata.number_u64);
                break;        
            case APDataType::SIGNED_INT:
                value_str = std::to_string(this->arg_table[i].data.intdata.number_i64);
                break;        
            case APDataType::TEXT:
                value_str = *this->arg_table[i].data.text;
                break;        
            default:
                value_str = "";
                break;
            }
        }
        else{
            value_str = "";
        }
        std::cout << "Value:            " << value_str << std::endl;
        if(this->arg_table[i].required){
            required_str = "TRUE";
        }
        else{
            required_str = "FALSE";
        }
        std::cout << "Is required?:     " <<  required_str << std::endl;
        if(this->arg_table[i].initialized){
            initialized_str = "TRUE";
        }
        else{
            initialized_str = "FALSE";
        }
        std::cout << "Is initialized?:     " <<  initialized_str << std::endl;
        std::cout << "--------------------------------" << std::endl;
    }
}
#endif

///////////////////////////////////////////////////////////////
// C INTERFACE DEFINITION 
///////////////////////////////////////////////////////////////

#ifndef DEBUG
ArgParsing_C* ArgParsing_C_get_instance(){
    return reinterpret_cast<ArgParsing_C*>(&ArgParsing::get_instance());
}
#endif


void ArgParsing_C_set_input_args(ArgParsing_C* apc, int input_argc, char** input_argv){
    reinterpret_cast<ArgParsing*>(apc)->set_input_args(input_argc, input_argv);
}


int ArgParsing_C_set_arg_table(ArgParsing_C* apc, APTableEntry_C* input_arg_table, size_t n_entries){
    APTableEntry_C* input_entry_curr{};
    APTableEntry new_entry{};
    std::vector<APTableEntry> new_arg_table{};

    // If no entries passed, can't process them
    if(n_entries == 0){
        return -1;
    }
    
    // Set pointer to the first element in the input array
    input_entry_curr = input_arg_table;
    new_arg_table.reserve(n_entries);

    // Loop though each entry in the array and copy the data to the vector
    for(size_t i = 0; i < n_entries; i++){
        if(strlen(input_entry_curr->abbr_form) != 0){
            new_entry.abbr_form = input_entry_curr->abbr_form;
        }
        if(strlen(input_entry_curr->full_form) == 0){
            return -1;
        }
        new_entry.full_form = input_entry_curr->full_form;
        new_entry.required = input_entry_curr->required;
        switch (input_entry_curr->data_type){
        case TEXT:
            new_entry.data_type = APDataType::TEXT;
            break;        
        case FLAG:
            new_entry.data_type = APDataType::FLAG;
            break;        
        case UNSIGNED_INT:
            new_entry.data_type = APDataType::UNSIGNED_INT;
            break;        
        case SIGNED_INT:
            new_entry.data_type = APDataType::SIGNED_INT;
            break;        
        default:
            return -1;
            break;
        }
        new_arg_table.push_back(new_entry);
        new_entry = {};
        input_entry_curr++;
    }
    return reinterpret_cast<ArgParsing*>(apc)->set_arg_table(new_arg_table);
}


int ArgParsing_C_parse(ArgParsing_C* apc){
    return reinterpret_cast<ArgParsing*>(apc)->parse();
}


size_t ArgParsing_C_get_arg_value_bytesize(ArgParsing_C* apc, const char* arg_key, bool is_abbr_input){
    return reinterpret_cast<ArgParsing*>(apc)->get_arg_value_bytesize((std::string)arg_key, is_abbr_input);
}


int ArgParsing_C_get_value_TEXT(ArgParsing_C* apc, const char* arg_key, bool is_abbr_input, char* output_buffer, size_t len_output_buffer){
    std::string val{};
    size_t arg_value_len{};
    
    arg_value_len = reinterpret_cast<ArgParsing*>(apc)->get_arg_value_bytesize((std::string)arg_key, is_abbr_input);
    // If argument value length is 0 or if output buffer is smaller than argument length, then stop it
    if(arg_value_len == 0 || 
       arg_value_len > len_output_buffer + 1){
        return -1;
    }
    
    // Copy string value to output buffer
    val = (reinterpret_cast<ArgParsing*>(apc)->get_arg_value<std::string>((std::string)arg_key, is_abbr_input));
    memcpy(output_buffer, val.c_str(), arg_value_len);
    return 0;
}


bool ArgParsing_C_get_value_FLAG(ArgParsing_C* apc, const char* arg_key, bool is_abbr_input){
    return reinterpret_cast<ArgParsing*>(apc)->get_arg_value<bool>((std::string)arg_key, is_abbr_input);
}


uint64_t ArgParsing_C_get_value_UNSIGNED_INT(ArgParsing_C* apc, const char* arg_key, bool is_abbr_input){
    return reinterpret_cast<ArgParsing*>(apc)->get_arg_value<uint64_t>((std::string)arg_key, is_abbr_input);
}


int64_t ArgParsing_C_get_value_SIGNED_INT(ArgParsing_C* apc, const char* arg_key, bool is_abbr_input){
    return reinterpret_cast<ArgParsing*>(apc)->get_arg_value<int64_t>((std::string)arg_key, is_abbr_input);
}
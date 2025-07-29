#include "ArgParsing.hpp"

ArgParsing::ArgParsing() {
    this->state = APState::ARGV_BEGIN;
    this->argc = 0;
    this->argv = nullptr;
    this->argv_idx = 1;
    this->eval_arg_idx = 0;
    this->is_table_set = false;
};

int ArgParsing::get_index_in_arg_table(std::string& arg_key, bool is_abbr_input){
    for(size_t i = 0; i < this->arg_table.size(); i++){
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
    return -1;
}

void ArgParsing::set_input_args(int input_argc, char** input_argv){
    this->argc = input_argc;
    this->argv = input_argv;
}

int ArgParsing::set_arg_table(APTableEntry* arg_table_ptr, size_t n_entries){
    if(this->is_table_set){
        return -1;
    }
    // Copy array contents over to std::vector
    this->arg_table.assign(arg_table_ptr, arg_table_ptr + n_entries);
    // Check for duplicate abbreviated form identifiers
    for(size_t i = 0; i < this->arg_table.size(); i++){
        if(this->arg_table[i].abbr_form == ""){
            continue;
        }
        for(size_t j = i + 1; j < this->arg_table.size(); j++){
            if(this->arg_table[i].abbr_form == this->arg_table[j].abbr_form){
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
                return -1;
            }
        }
    }
    is_table_set = true;
    return 0;
}

bool ArgParsing::is_valid_hex(std::string& input){
    std::string input_copy = input;
    bool valid = true;
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
    std::string input_copy = input;
    size_t i;
    bool valid = true;
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

void ArgParsing::parse(){
    while(this->argv_idx < this->argc){
        if(this->state == APState::ERROR){
            std::cout << "ERROR!" << std::endl;
            break;
        }
        else if(this->state == APState::ARGV_BEGIN){
            this->arg_begin();
        }
        else if(this->state == APState::ARGV_VALUE){
            this->arg_value();
        }
    }
}

void ArgParsing::arg_begin(){
    // An identifier cannot be less than 2 characters in length
    std::string curr = this->argv[argv_idx];
    if(curr.size() < 2){
        this->state = APState::ERROR;
        return;
    }
    // Parameter identifiers must always start with a single dash (-)
    if(curr[0] != '-'){
        this->state = APState::ERROR;
        return;
    }
    // Whether the parameter identifier is in full or abbreviated form 
    if(curr[1] == '-'){\
        this->state = APState::ARGV_FULL_ID;
        this->arg_full_form();
    }
    else{
        this->state = APState::ARGV_ABBR_ID;
        this->arg_abbr_form();
    }
}

void ArgParsing::arg_abbr_form(){
    std::string abbr_arg;
    std::string curr = this->argv[this->argv_idx];
    if(curr.size() == 2){
        abbr_arg = std::string(1, this->argv[this->argv_idx][1]);
        // Search for abbreviated identifier in argument table 
        // If not found, returned index is -1 and set error state
        this->eval_arg_idx = this->get_index_in_arg_table(abbr_arg, true);
        if(this->eval_arg_idx == -1){
            this->state = APState::ERROR;
            return;
        }
        // Check the argument data type and update the state
        if(this->arg_table[this->eval_arg_idx].data_type == APDataType::FLAG){
            this->arg_table[this->eval_arg_idx].value = "1";
            this->state = APState::ARGV_BEGIN;
        }
        else{
            this->state = APState::ARGV_VALUE;
        }
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
                return;
            }
            // Since it is a group of identifiers, we can only accept arguments of type FLAG
            if(this->arg_table[this->eval_arg_idx].data_type != APDataType::FLAG){
                this->state = APState::ERROR;
                return;    
            }
            this->arg_table[this->eval_arg_idx].value = "1";
        }
        this->state = APState::ARGV_BEGIN;
    }
    this->argv_idx++;
}

void ArgParsing::arg_full_form(){
    // TODO
}

void ArgParsing::arg_value(){
    std::string value = this->argv[this->argv_idx];
    switch(this->arg_table[this->eval_arg_idx].data_type){
    case APDataType::NUMBER:
        // Check if it is a hexadecimal value.
        // If so, validate it. Otherwise validate it as decimal
        if(value.size() >= 2){
            if(this->is_valid_hex(value)){
                this->arg_table[this->eval_arg_idx].value = value;
            }
            else if(this->is_valid_dec(value)){
                this->arg_table[this->eval_arg_idx].value = value;
            }
            else{
                this->state = APState::ERROR;
                return;
            }
        }
        else{
            if(this->is_valid_dec(value)){
                this->arg_table[this->eval_arg_idx].value = value;
            }
            else{
                this->state = APState::ERROR;
                return;
            }
        }
        break;
    case APDataType::TEXT:
        this->arg_table[this->eval_arg_idx].value = value;
        break;
    default:
        break;
    }
    this->state = APState::ARGV_BEGIN;
    this->argv_idx++;
}

void ArgParsing::display_arg_table(){
    std::string data_type_str;
    std::string is_required_str;
    for(size_t i = 0; i < this->arg_table.size(); i++){
        std::cout << "Abbreviated Form: " << this->arg_table[i].abbr_form << std::endl;
        std::cout << "Full Form:        " << this->arg_table[i].abbr_form << std::endl;
        switch (this->arg_table[i].data_type){
        case APDataType::FLAG:
            data_type_str = "FLAG";
            break;        
        case APDataType::NUMBER:
            data_type_str = "NUMBER";
            break;        
        case APDataType::TEXT:
            data_type_str = "TEXT";
            break;        
        default:
            data_type_str = "NONE";
            break;
        }
        std::cout << "Data Type:        " << data_type_str << std::endl;
        std::cout << "Value:            " << this->arg_table[i].value << std::endl;
        if(this->arg_table[i].is_required){
            is_required_str = "TRUE";
        }
        else{
            is_required_str = "FALSE";
        }
        std::cout << "Is required?:     " <<  is_required_str << std::endl;
        std::cout << "--------------------------------" << std::endl;
    }
}
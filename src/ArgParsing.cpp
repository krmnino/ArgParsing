#include "ArgParsing.hpp"

ArgParsing::ArgParsing() {
    this->state = APState::ARGV_BEGIN;
    this->argc = 0;
    this->argv = nullptr;
    this->argc_idx = 1;
    this->is_table_set = false;
};

bool ArgParsing::abbr_arg_exists(std::string& abbr_arg){
    for(size_t i = 0; i < this->arg_table.size(); i++){
        if(this->arg_table[i].abbr_form == abbr_arg){
            return true;
        }
    }
    return false;
}

size_t ArgParsing::get_index_in_arg_table(std::string& arg_key, bool is_abbr_input){
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
    this->arg_table.assign(arg_table_ptr, arg_table_ptr + n_entries);
    is_table_set = true;
    return 0;
}

void ArgParsing::parse(){
    while(this->argc_idx < this->argc){
        if(this->state == APState::ERROR){
            std::cout << "ERROR!" << std::endl;
            break;
        }
        else if(this->state == APState::ARGV_BEGIN){
            this->arg_begin();
        }
        else if(this->state == APState::ARGV_VALUE){
            this->arg_begin();
        }
    }
}

void ArgParsing::arg_begin(){
    char* curr;
    // An identifier cannot be less than 2 characters in length
    curr = this->argv[argc_idx];
    if(strlen(curr) < 2){
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
    char* curr;
    std::string abbr_arg;
    size_t arg_table_idx;
    curr = this->argv[this->argc_idx];
    if(strlen(curr) == 2){
        // Search abbreviated identifier in table
        abbr_arg = std::string(1, this->argv[this->argc_idx][1]);
        if(!this->abbr_arg_exists(abbr_arg)){
            this->state = APState::ERROR;
            return;
        }
        arg_table_idx = this->get_index_in_arg_table(abbr_arg, true);
        if(this->arg_table[arg_table_idx].data_type == APDataType::FLAG){
            this->arg_table[arg_table_idx].value = "1";
            this->state = APState::ARGV_BEGIN;
        }
        else{
            this->state = APState::ARGV_VALUE;
        }
        this->argc_idx++;
    }
    else{
        // Loop through group of abbreviated identifiers in table
        // All identifiers must be of FLAG type
        for(size_t i = 1; i < strlen(this->argv[this->argc_idx]); i++){
            abbr_arg = std::string(1, this->argv[this->argc_idx][i]);
            if(!this->abbr_arg_exists(abbr_arg)){
                this->state = APState::ERROR;
                return;
            }
            arg_table_idx = this->get_index_in_arg_table(abbr_arg, true);
            if(this->arg_table[arg_table_idx].data_type != APDataType::FLAG){
                this->state = APState::ERROR;
                return;    
            }
            this->arg_table[arg_table_idx].value = "1";
        }
        this->state = APState::ARGV_BEGIN;
        this->argc_idx++;
    }
}

void ArgParsing::arg_full_form(){

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
        case APDataType::STRING:
            data_type_str = "STRING";
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
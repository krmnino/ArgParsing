#ifndef ARGPARSING
#define ARGPARSING

#include <iostream>
#include <vector>
#include <string.h>

#define VALID_FLAG_VALUES {"0", "1", "true", "false", "TRUE", "FALSE"}

enum class APState {
    DONE,
    ERROR,
    ARGV_BEGIN,
    ARGV_ABBR_ID,
    ARGV_FULL_ID,
    ARGV_VALUE,
};


enum class APErrRsn {
    MISSING_FIRST_DASH    =  0x00000001,
    MISSING_REQUIRED_ARG  =  0x00000002,
    UNKNOWN_ARGUMENT      =  0x00000004,
    REPEATED_ARGUMENT     =  0x00000008,
    MUST_BE_FLAG          =  0x00000010,
    BAD_NUMERIC_VALUE     =  0x00000020,
};


enum class APDataType {
    TEXT         = 0x00000001,
    UNSIGNED_INT = 0x00000002,
    FLAG         = 0x00000004,
    SIGNED_INT   = 0x00000008,
};
#ifdef DEBUG
#define MAX_TYPES (uint32_t)3
#endif

union data {
    std::string* text;
    union intdata{
        uint64_t number_u64;
        int64_t  number_i64;
    } intdata;
    bool flag;
};

struct APTableEntry {
    std::string abbr_form{};
    std::string full_form{};
    union data data{};
    APDataType data_type{};
    bool required{};
    bool initialized{};

    APTableEntry(std::string in_abbr_form, std::string in_full_form, APDataType in_data_type, bool in_required) : 
                 abbr_form(in_abbr_form), full_form(in_full_form), data_type(in_data_type), required(in_required), initialized(false) {}

    APTableEntry(std::string in_full_form, APDataType in_data_type, bool in_required) : 
                 abbr_form(""), full_form(in_full_form), data_type(in_data_type), required(in_required), initialized(false) {}

    APTableEntry() : abbr_form(""), full_form(""), data_type(APDataType::TEXT), required(false), initialized(false) {}
    
    ~APTableEntry() {
        if(this->initialized && this->data_type == APDataType::TEXT){
            delete this->data.text;
        }
    }
};
typedef struct APTableEntry APTableEntry;


class ArgParsing{
    private:
    std::vector<APTableEntry> arg_table;
    std::vector<std::string> err_msg_data;
    std::string error_msg;
    std::string prev_argv_element;
    #ifndef DEBUG
    static ArgParsing* ap_ptr;
    #endif
    char** argv;
    int argv_idx;
    int argc;
    APState state;
    APErrRsn reason;
    int eval_arg_idx;
    bool is_table_set;
    
    #ifndef DEBUG
    ArgParsing();
    ~ArgParsing();
    #endif
    ArgParsing(const ArgParsing&) = delete;
    ArgParsing& operator=(const ArgParsing&) = delete;
    int32_t get_index_in_arg_table(std::string&, bool);
    bool is_valid_hex(std::string&);
    bool is_valid_dec(std::string&);
    void arg_begin();
    void arg_abbr_form();
    void arg_full_form();
    void arg_value();
    std::string APErrRsn_to_string(APErrRsn);
    void display_error_msg();
    bool validate_flag_value(std::string&);
    
    public:
    #ifndef DEBUG
    static ArgParsing* ArgParsing_get_instance(){
        if(ap_ptr == nullptr){
            ap_ptr = new ArgParsing();
        }
        return ap_ptr;
    }
    
    static void ArgParsing_end_instance(){
        if(ap_ptr){
            delete ap_ptr;
        }
    }
    #else
    ArgParsing();
    ~ArgParsing();
    void get_arg_table(std::vector<APTableEntry>&);
    void get_error_msg(std::string&);
    void display_arg_table();
    #endif
    void set_input_args(int, char**);
    int set_arg_table(APTableEntry*, size_t);
    int set_arg_table(std::vector<APTableEntry>&);
    int parse();
    
    template<typename T> T get_arg_value(std::string arg_id, bool is_abbr_input){
        int32_t arg_table_idx;
        T ret_value{};

        // Check if argument identifier exists
        arg_table_idx = this->get_index_in_arg_table(arg_id, is_abbr_input);
        if(arg_table_idx == -1){
            return ret_value;
        }

        // Return the appropiate argument value
        if constexpr (std::is_integral<T>::value) {
            if constexpr (std::is_signed<T>::value) {
                if(this->arg_table[arg_table_idx].data_type == APDataType::SIGNED_INT){
                    ret_value = this->arg_table[arg_table_idx].data.intdata.number_i64;
                }
            }
            else if constexpr (std::is_same<T, bool>::value) {
                if(this->arg_table[arg_table_idx].data_type == APDataType::FLAG){
                    ret_value = this->arg_table[arg_table_idx].data.flag;
                }
            }
            else{
                if(this->arg_table[arg_table_idx].data_type == APDataType::UNSIGNED_INT){
                    ret_value = this->arg_table[arg_table_idx].data.intdata.number_u64;
                }
            }
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            if(this->arg_table[arg_table_idx].data.text != nullptr && 
               this->arg_table[arg_table_idx].data_type == APDataType::TEXT){
                ret_value = this->arg_table[arg_table_idx].data.text;
            }
        }
        return ret_value;
    }
};

#endif
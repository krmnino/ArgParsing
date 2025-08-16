#ifndef ARGPARSING
#define ARGPARSING

#include <iostream>
#include <vector>
#include <string.h>

enum class APState {
    DONE,
    ERROR,
    ARGV_BEGIN,
    ARGV_ABBR_ID,
    ARGV_FULL_ID,
    ARGV_VALUE,
};


enum class APErrRsn {
    OK                    =  0x00000001,
    MISSING_FIRST_DASH    =  0x00000002,
    MISSING_REQUIRED_ARG  =  0x00000004,
    UNKNOWN_ARGUMENT      =  0x00000008,
    REPEATED_ARGUMENT     =  0x00000010,
    MUST_BE_FLAG          =  0x00000020,
    BAD_NUMERIC_VALUE     =  0x00000040,
};


enum class APDataType {
    TEXT   = 0x00000001,
    NUMBER = 0x00000002,
    FLAG   = 0x00000004,
};
#ifdef DEBUG
#define MAX_TYPES (uint32_t)3
#endif

struct APTableEntry {
    std::string abbr_form;
    std::string full_form;
    std::string value;
    APDataType data_type;
    bool required;
    bool initialized;
    APTableEntry(std::string in_abbr_form, std::string in_full_form, APDataType in_data_type, bool in_required) : 
                 abbr_form(in_abbr_form), full_form(in_full_form), data_type(in_data_type), required(in_required), initialized(false) {}
    APTableEntry(std::string in_full_form, APDataType in_data_type, bool in_required) : 
                 abbr_form(""), full_form(in_full_form), data_type(in_data_type), required(in_required), initialized(false) {}
    APTableEntry() : abbr_form(""), full_form(""), data_type(APDataType::TEXT), required(false), initialized(false) {}
    ~APTableEntry() {}
};
typedef struct APTableEntry APTableEntry;


class ArgParsing{
    private:
    std::vector<APTableEntry> arg_table;
    std::vector<std::string> err_msg_data;
    #ifndef DEBUG
    static ArgParsing* ap_ptr;
    #else
    std::string dbg_error_msg;
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
    int get_index_in_arg_table(std::string&, bool);
    bool is_valid_hex(std::string&);
    bool is_valid_dec(std::string&);
    void arg_begin();
    void arg_abbr_form();
    void arg_full_form();
    void arg_value();
    void display_error_msg();
    
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
    #endif
    
    void set_input_args(int, char**);
    int set_arg_table(APTableEntry*, size_t);
    int set_arg_table(std::vector<APTableEntry>&);
    int parse();
    std::string get_arg_value(std::string, bool);
    #ifdef DEBUG
    void display_arg_table();
    #endif

};

#endif
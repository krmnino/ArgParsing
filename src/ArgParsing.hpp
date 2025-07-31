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
    BAD_FORMAT,
    MISSING_REQUIRED,
    UNKNOWN_ARGUMENT,
    REPEATED_ARGUMENT,
    MUST_BE_FLAG,
    BAD_NUMERIC_VALUE,
};


enum class APDataType {
    NONE,
    FLAG,
    NUMBER,
    TEXT
};


struct APTableEntry {
    std::string value;
    std::string abbr_form;
    std::string full_form;
    APDataType data_type;
    bool required;
    bool initialized = false;
};
typedef struct APTableEntry APTableEntry;


class ArgParsing{
    private:
    std::vector<APTableEntry> arg_table;
    std::vector<std::string> err_msg_data;
    static ArgParsing* ap_ptr;
    char** argv;
    size_t argv_idx;
    APState state;
    APErrRsn reason;
    int argc;
    int eval_arg_idx;
    bool is_table_set;

    ArgParsing();
    ArgParsing(const ArgParsing&) = delete;
    ArgParsing& operator=(const ArgParsing&) = delete;
    int get_index_in_arg_table(std::string&, bool);
    bool is_valid_hex(std::string&);
    bool is_valid_dec(std::string&);
    void display_error_msg();

    public:
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

    void set_input_args(int, char**);
    int set_arg_table(APTableEntry*, size_t);
    int parse();
    void arg_begin();
    void arg_abbr_form();
    void arg_full_form();
    void arg_value();
    #ifdef DEBUG
    void display_arg_table();
    #endif

};

#endif
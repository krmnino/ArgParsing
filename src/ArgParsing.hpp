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

enum class APDataType {
    NONE,
    FLAG,
    NUMBER,
    STRING
};

struct APTableEntry {
    std::string value;
    std::string abbr_form;
    std::string full_form;
    APDataType data_type;
    bool is_required;
};
typedef struct APTableEntry APTableEntry;



class ArgParsing{
    private:
    std::vector<APTableEntry> arg_table;
    static ArgParsing* ap_ptr;
    char** argv;
    size_t argc_idx;
    size_t eval_arg_idx;
    APState state;
    int argc;
    bool is_table_set;

    ArgParsing();
    ArgParsing(const ArgParsing&) = delete;
    ArgParsing& operator=(const ArgParsing&) = delete;
    int get_index_in_arg_table(std::string&, bool);

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
    void parse();
    void arg_begin();
    void arg_abbr_form();
    void arg_full_form();
    void display_arg_table();

};

#endif
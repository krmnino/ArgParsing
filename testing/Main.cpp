#include "../src/ArgParsing.hpp"

ArgParsing* ArgParsing::ap_ptr = nullptr;

int main(int argc, char* argv[]){
    ArgParsing* ap = ArgParsing::ArgParsing_get_instance();
    
    APTableEntry arg_table[] = {
        { "", "a", "arg1", APDataType::FLAG, false },
        { "", "b", "arg2", APDataType::FLAG, false },
        { "", "c", "arg3", APDataType::FLAG, false },
    };
    ap->set_arg_table(arg_table, sizeof(arg_table) / sizeof(APTableEntry));
    ap->set_input_args(argc, argv);
    ap->parse();
    ap->display_arg_table();
    ap->ArgParsing_end_instance();
    return 0;
}
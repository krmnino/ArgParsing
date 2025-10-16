#include "../src/ArgParsing.hpp"

int main(int argc, char* argv[]){
    int ret;

    ArgParsing& ap = ArgParsing::get_instance();

    APTableEntry arg_table[] = {
        { "a", "arg1", APDataType::FLAG        , true  },
        { "x", "arg2", APDataType::TEXT        , false },
        {      "arg3", APDataType::UNSIGNED_INT, false },
        { "9", "arg4", APDataType::FLAG        , false },
        { "z", "arg5", APDataType::TEXT        , true  },
    };
    
    ret = ap.set_arg_table(arg_table, sizeof(arg_table) / sizeof(APTableEntry));
    if(ret != 0){
        std::cerr << "ERROR: ArgParsing::set_arg_table() return code -> " << ret << std::endl;
        return -1;
    }

    ap.set_input_args(argc, argv);
    
    ret = ap.parse();
    if(ret != 0){
        std::cerr << "ERROR: ArgParsing::set_input_args() return code -> " << ret << std::endl;
        return -1;
    }

    std::cout << "a/arg1: " << ap.get_arg_value<bool>("arg1", false) << std::endl;
    std::cout << "x/arg2: " << ap.get_arg_value<std::string>("x", true) << std::endl;
    std::cout << "  arg3: " << ap.get_arg_value<uint64_t>("arg3", false) << std::endl;
    std::cout << "9/arg4: " << ap.get_arg_value<bool>("9", true) << std::endl;
    std::cout << "z/arg5: " << ap.get_arg_value<std::string>("z", true) << std::endl;
    
    return 0;
}
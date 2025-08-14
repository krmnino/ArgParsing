#include "ArgParsingTesting.hpp"

std::string space_padding(std::string input_str, size_t width, std::string pad){
    std::string ret = input_str;
    if(input_str.size() >= width){
        return ret;
    }
    size_t padding = width - input_str.size();
    for(size_t i = 0; i < padding; i++){
        ret += pad;
    }
    return ret;
}

std::string describe_argv(int input_argc, char** input_argv){
    if(input_argv == nullptr){
        return "";
    }
    std::stringstream buffer;
    buffer << " ";
    buffer << space_padding("INDEX", PRT_IDX_STR_WIDTH, " ") << " | ";
    buffer << space_padding("VALUE", PRT_VALUE_STR_WIDTH, " ") << "\n";
    buffer << "-";
    buffer << space_padding("-", PRT_IDX_STR_WIDTH, "-") << "-+-";
    buffer << space_padding("-", PRT_VALUE_STR_WIDTH, "-") << "\n";
    for(int i = 0; i < input_argc; i++){
        buffer << " ";
        buffer << space_padding(std::to_string(i), PRT_IDX_STR_WIDTH, " ") << " | ";
        buffer << space_padding(input_argv[i], PRT_VALUE_STR_WIDTH, " ") << "\n";
    }
    return buffer.str();
}
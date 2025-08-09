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
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

std::string ScenarioType_to_string(ScenarioType st){
    std::string ret = "ScenarioType::";
    switch (st){
    case ScenarioType::OK:
        ret += "OK";
        break;
    case ScenarioType::MISSING_FIRST_DASH:
        ret += "MISSING_FIRST_DASH";
        break;
    case ScenarioType::MISSING_REQUIRED_ARG:
        ret += "MISSING_REQUIRED_ARG";
        break;
    case ScenarioType::UNKNOWN_ARGUMENT:
        ret += "UNKNOWN_ARGUMENT";
        break;
    case ScenarioType::REPEATED_ARGUMENT:
        ret += "REPEATED_ARGUMENT";
        break;
    case ScenarioType::MUST_BE_FLAG:
        ret += "MUST_BE_FLAG";
        break;
    case ScenarioType::BAD_NUMERIC_VALUE:
        ret += "BAD_NUMERIC_VALUE";
        break;
    case ScenarioType::EMPTY_ARG_LIST:
        ret += "EMPTY_ARG_LIST";
        break;
    case ScenarioType::VALID_FLAG_GROUP:
        ret += "VALID_FLAG_GROUP";
        break;
    case ScenarioType::INVALID_FLAG_GROUP:
        ret += "INVALID_FLAG_GROUP";
        break;
    default:
        ret += "UNDEFINED";
        break;
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

std::string APDataType_to_string(APDataType apdt){
    std::string ret = "APDataType::";
    switch (apdt){
    case APDataType::TEXT:
        ret += "TEXT";
        break;
    case APDataType::NUMBER:
        ret += "NUMBER";
        break;
    case APDataType::FLAG:
        ret += "FLAG";
        break;
    default:
        ret += "UNDEFINED";
        break;
    }
    return ret;
}

std::string bool_to_string(bool data){
    return (data) ? "true" : "false";
}

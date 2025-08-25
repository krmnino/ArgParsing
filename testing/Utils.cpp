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


std::string arg_table_to_string(std::vector<APTableEntry>& arg_table){
    std::stringstream buffer;
    std::string idx_str;
    std::string abbr_form_str;
    std::string full_form_str;
    std::string data_type_str;
    std::string required_str;
    std::string initialized_str;
    std::string value_str;
    buffer << " ";
    buffer << space_padding("INDEX", PRT_IDX_STR_WIDTH, " ") << " | ";
    buffer << space_padding("ABBR. FORM", PRT_ABBR_FORM_STR_WIDTH, " ") << " | ";
    buffer << space_padding("FULL FORM", PRT_FULL_FORM_STR_WIDTH, " ") << " | ";
    buffer << space_padding("DATA TYPE", PRT_DATA_TYPE_STR_WIDTH, " ") << " | ";
    buffer << space_padding("REQUIRED", PRT_REQUIRED_STR_WIDTH, " ") << " | ";
    buffer << space_padding("INITIALIZED", PRT_INITIALIZED_STR_WIDTH, " ") << " | ";
    buffer << space_padding("VALUE", PRT_VALUE_STR_WIDTH, " ") << "\n";
    buffer << "-";
    buffer << space_padding("-", PRT_IDX_STR_WIDTH, "-") << "-+-";
    buffer << space_padding("-", PRT_ABBR_FORM_STR_WIDTH, "-") << "-+-";
    buffer << space_padding("-", PRT_FULL_FORM_STR_WIDTH, "-") << "-+-";
    buffer << space_padding("-", PRT_DATA_TYPE_STR_WIDTH, "-") << "-+-";
    buffer << space_padding("-", PRT_REQUIRED_STR_WIDTH, "-") << "-+-";
    buffer << space_padding("-", PRT_INITIALIZED_STR_WIDTH, "-") << "-+-";
    buffer << space_padding("-", PRT_VALUE_STR_WIDTH, "-") << "\n";
    for(size_t i = 0; i < arg_table.size(); i++){
        idx_str = std::to_string(i);
        abbr_form_str = arg_table[i].abbr_form;
        full_form_str = arg_table[i].full_form;
        value_str = arg_table[i].value;
        switch (arg_table[i].data_type){
            case APDataType::FLAG:
            data_type_str = "FLAG";
            break;        
            case APDataType::NUMBER:
            data_type_str = "NUMBER";
            break;        
            case APDataType::TEXT:
            data_type_str = "TEXT";
            break;        
            default:
            data_type_str = "NONE";
            break;
        }
        if(arg_table[i].required){
            required_str = "TRUE";
        }
        else{
            required_str = "FALSE";
        }
        if(arg_table[i].initialized){
            initialized_str = "TRUE";
        }
        else{
            initialized_str = "FALSE";
        }
        buffer << " ";
        buffer << space_padding(idx_str, PRT_IDX_STR_WIDTH, " ") << " | ";
        buffer << space_padding(abbr_form_str, PRT_ABBR_FORM_STR_WIDTH, " ") << " | ";
        buffer << space_padding(full_form_str, PRT_FULL_FORM_STR_WIDTH, " ") << " | ";
        buffer << space_padding(data_type_str, PRT_DATA_TYPE_STR_WIDTH, " ") << " | ";
        buffer << space_padding(required_str, PRT_REQUIRED_STR_WIDTH, " ") << " | ";
        buffer << space_padding(initialized_str, PRT_INITIALIZED_STR_WIDTH, " ") << " | ";
        buffer << space_padding(value_str, PRT_VALUE_STR_WIDTH, " ") << "\n";
    }
    return buffer.str();
}
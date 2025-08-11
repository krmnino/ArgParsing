#include "ArgParsingTesting.hpp"

static const char* alphanum_dict = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";


int build_initial_arg_table(Randomizer* rnd, TestcaseData& tdata){
    uint32_t attempt_counter;
    uint32_t n_args = rnd->gen_integral_range<uint32_t>(0, 100);
    uint32_t allowed_dtypes = (uint32_t)APDataType::TEXT   |
                              (uint32_t)APDataType::FLAG   |
                              (uint32_t)APDataType::NUMBER;
                                  
    // Empty the vector and reserve space
    tdata.ini_argtab.clear();
    tdata.ini_argtab.reserve(n_args);

    // Allowed datatypes for testcase
    tdata.allowed_data_types = allowed_dtypes;

    for(size_t i = 0; i < n_args; i++){
        attempt_counter = 0;
        while(true){
            if(attempt_counter > BUILD_MAX_ATTEMPTS){
                return -1;
            }
            if(build_entry(rnd, tdata.ini_argtab, allowed_dtypes) == 0){
                break;
            }
            attempt_counter++;
        }
    }

    return 0;
}

int build_entry(Randomizer* rnd, std::vector<APTableEntry>& arg_table, uint32_t enabled_data_types){
    APTableEntry new_entry;
    std::string result_str;
    uint32_t result_u32;
    uint32_t attempt_counter;
    uint32_t shifter;
    bool result_bool;
    bool invalid;

    // APTableEntry.abbr_form -> Make it 50% of the time
    result_bool = rnd->gen_bool();
    if(result_bool){
        invalid = false;
        attempt_counter = 0;
        while (true){
            // If counter expired, stop trying and return to the caller
            if(attempt_counter > BUILD_MAX_ATTEMPTS){
                return -1;
            }
            result_str = rnd->gen_string(MAX_ABBR_FORM_ID_LEN, alphanum_dict);
            // Prevent duplicates
            for(size_t j = 0; j < arg_table.size(); j++){
                if(result_str == arg_table[j].abbr_form){
                    invalid = true;
                    break;
                }
            }
            if(!invalid){
                new_entry.abbr_form = result_str;
                break;
            }
            // Loop back and try again
            invalid = false;
            attempt_counter++;
        }
    }

    // APTableEntry.full_form
    invalid = false;
    attempt_counter = 0;
    while (true){
        // If counter expired, stop trying and return to the caller
        if(attempt_counter > BUILD_MAX_ATTEMPTS){
            return -1;
        }
        result_u32 = rnd->gen_integral_range<uint32_t>(2, MAX_FULL_FORM_ID_LEN);
        result_str = rnd->gen_string(result_u32, alphanum_dict);
        // Prevent duplicates
        for(size_t i = 0; i < arg_table.size(); i++){
            if(result_str == arg_table[i].full_form || result_str.size() < 2){
                invalid = true;
                break;
            }
        }
        if(!invalid){
            new_entry.full_form = result_str;
            break;
        }
        // Loop back and try again
        invalid = false;
        attempt_counter++;
    }

    // APTableEntry.data_type
    attempt_counter = 0;
    while(true){
        if(attempt_counter > BUILD_MAX_ATTEMPTS){
            return -1;
        }
        shifter = rnd->gen_integral_range<uint32_t>(0, MAX_TYPES);
        if((enabled_data_types & (1 << shifter)) != 0){
            break;
        }
        attempt_counter++;
    }
    new_entry.data_type = (APDataType)(1 << shifter);

    // APTableEntry.required -> Make it not required 60% of the time
    result_u32 = rnd->gen_integral_range<uint32_t>(1, 10);
    if(result_u32 <= 6){
        new_entry.required = false;
    }
    else{
        new_entry.required = true;
    }
    arg_table.push_back(new_entry);

    return 0;
}

bool contains_data_type(std::vector<APTableEntry>& arg_table, APDataType input_data_type){
    for(size_t i = 0; i < arg_table.size(); i++){
        if(arg_table[i].data_type == input_data_type){
            return true;
        }
    }
    return false;
}

bool contains_required(std::vector<APTableEntry>& arg_table){
    for(size_t i = 0; i < arg_table.size(); i++){
        if(arg_table[i].required){
            return true;
        }
    }
    return false;
}

size_t count_args_by_type(std::vector<APTableEntry>& arg_table, APDataType input_data_type){
    size_t accumulator = 0;
    for(size_t i = 0; i < arg_table.size(); i++){
        if(arg_table[i].data_type == input_data_type){
            accumulator++;
        }
    }
    return accumulator;
}


bool abbr_form_available(std::vector<APTableEntry>& arg_table, size_t idx){
    return (arg_table[idx].abbr_form.size() == 0) ? false : true;
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
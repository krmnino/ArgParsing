#include "ArgParsingTesting.hpp"

int32_t build_arg_table(Randomizer* rnd, std::vector<APTableEntry>& table,  uint32_t n_args){
    uint32_t attempt_counter;
                                  
    // Empty the vector and reserve space
    table.clear();
    table.reserve(n_args);

    // Attempt uilding entries one by one
    for(size_t i = 0; i < n_args; i++){
        attempt_counter = 0;
        while(true){
            if(attempt_counter > BUILD_MAX_ATTEMPTS){
                return -1;
            }
            if(build_entry(rnd, table) == 0){
                break;
            }
            attempt_counter++;
        }
    }

    return 0;
}

int32_t build_entry(Randomizer* rnd, std::vector<APTableEntry>& arg_table){
    const char* alphanum_dict = ALPHANUM_DICT;
    const char* valid_flag_values[] = VALID_FLAG_VALUES;
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
        attempt_counter = 0;
        while (true){
            // If counter expired, stop trying and return to the caller
            if(attempt_counter > BUILD_MAX_ATTEMPTS){
                return -1;
            }
            result_str = rnd->gen_string(MAX_ABBR_FORM_ID_LEN, alphanum_dict);
            invalid = false;
            // Prevent duplicates
            for(size_t j = 0; j < arg_table.size(); j++){
                if(result_str == arg_table[j].abbr_form){
                    invalid = true;
                    break;
                }
            }
            // No restricted identifier values
            for(size_t i = 0; i < sizeof(valid_flag_values) / sizeof(valid_flag_values[0]); i++){
                if(result_str == valid_flag_values[i]){
                    invalid = true;
                    break;
                }
            }
            if(!invalid){
                new_entry.abbr_form = result_str;
                break;
            }
            // Loop back and try again
            attempt_counter++;
        }
    }

    // APTableEntry.full_form
    attempt_counter = 0;
    while (true){
        // If counter expired, stop trying and return to the caller
        if(attempt_counter > BUILD_MAX_ATTEMPTS){
            return -1;
        }
        result_u32 = rnd->gen_integral_range<uint32_t>(2, MAX_FULL_FORM_ID_LEN);
        result_str = rnd->gen_string(result_u32, alphanum_dict);
        invalid = false;
        // Prevent duplicates
        for(size_t i = 0; i < arg_table.size(); i++){
            if(result_str == arg_table[i].full_form || result_str.size() < 2){
                invalid = true;
                break;
            }
        }
        // No restricted identifier values
        for(size_t i = 0; i < sizeof(valid_flag_values) / sizeof(valid_flag_values[0]); i++){
            if(result_str == valid_flag_values[i]){
                invalid = true;
                break;
            }
        }
        if(!invalid){
            new_entry.full_form = result_str;
            break;
        }
        // Loop back and try again
        attempt_counter++;
    }

    // APTableEntry.data_type
    shifter = rnd->gen_integral_range<uint32_t>(0, MAX_TYPES - 1);
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
    new_entry = {};
    return 0;
}

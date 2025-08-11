#include "ArgParsingTesting.hpp"

Randomizer* Randomizer::rnd_ptr = nullptr;

int run_testcase(Randomizer* rnd){
    //ArgParsing* ap_ptr = new ArgParsing();
    //std::vector<APTableEntry> arg_table;
    TestcaseData tdata;
    std::string scenario_argv;
    size_t n_scenarios;
    uint32_t scenario_type_pool;
    uint32_t picked_scenario_type;
    uint32_t attempt_counter;
    uint32_t user_allowed_scenario_types;
    uint32_t shifter;
    ScenarioData sdata;
    int ret;
    
    user_allowed_scenario_types = (uint32_t)ScenarioType::OK                     ;//|
                                  //(uint32_t)ScenarioType::MISSING_FIRST_DASH     |
                                  //(uint32_t)ScenarioType::MISSING_REQUIRED_ARG   |
                                  //(uint32_t)ScenarioType::UNKNOWN_ARGUMENT       |
                                  //(uint32_t)ScenarioType::REPEATED_ARGUMENT      |
                                  //(uint32_t)ScenarioType::MUST_BE_FLAG           |
                                  //(uint32_t)ScenarioType::BAD_NUMERIC_VALUE      |
                                  //(uint32_t)ScenarioType::EMPTY_ARG_LIST         |
                                  //(uint32_t)ScenarioType::VALID_FLAG_GROUP       |
                                  //(uint32_t)ScenarioType::INVALID_FLAG_GROUP     ;
    
    // Attempt building a argument table and check scenarios that can be tested
    attempt_counter = 0;
    while(true){
        if(attempt_counter > BUILD_MAX_ATTEMPTS){
            return -1;
        }
        ret = build_initial_arg_table(rnd, tdata);
        // If returned -1, try build another argument table
        if(ret == -1){
            continue;
        }
        scenario_type_pool = check_allowed_scenarios(tdata.ini_argtab, user_allowed_scenario_types);
        // If returned scenario pool is zero/empty, try build another argument table
        if(scenario_type_pool != 0x00000000){
            break;
        }
        attempt_counter++;
    }

    // Generate scenarios based on argument table and allowed scenario types
    n_scenarios = 100;
    tdata.scenario.reserve(n_scenarios);
    for(size_t i = 0; i < n_scenarios; i++){
        // Pick a single scenario from the pool
        attempt_counter = 0;
        while(true){
            if(attempt_counter > BUILD_MAX_ATTEMPTS){
                return -1;
            }
            shifter = rnd->gen_integral_range<uint32_t>(0, MAX_TYPES);
            picked_scenario_type = (1 << shifter);
            if((scenario_type_pool & picked_scenario_type) != 0){
                break;
            }
            attempt_counter++;
        }
        sdata.type = (ScenarioType)picked_scenario_type;

        // Copy initial arg table to scenario expected table data 
        sdata.exp_argtab = tdata.ini_argtab;

        // Build the argv for scenario
        build_scenario(rnd, sdata);
        std::cout << arg_table_to_string(sdata.exp_argtab) << std::endl;
        
        //std::cout << scenario_argv << std::endl;
    }

    
    //delete ap_ptr;
    return 0;
}

int main(int argc, char* argv[]){
    uint32_t init_seed;
    uint32_t n_tests;
    
    init_seed = 1;
    Randomizer* rnd = Randomizer::get_instance(init_seed);
    if(rnd == nullptr){
        return -1;
    }

    n_tests = 1;
    for(size_t i = 0; i < n_tests; i++){
        run_testcase(rnd);
    }

    Randomizer::end_instance();
    return 0;
}
#include "ArgParsingTesting.hpp"

int build_testcase(Randomizer* rnd, TestcaseData& tdata, uint32_t n_scenarios, uint32_t user_allowed_scenario_types){
    uint32_t scenario_type_pool;
    uint32_t picked_scenario_type;
    uint32_t attempt_counter;
    uint32_t shifter;
    int ret;
    
    // Set the number of scenarios for this testcase
    tdata.n_scenarios = n_scenarios;

    // Attempt building a argument table and check scenarios that can be tested
    attempt_counter = 0;
    while(true){
        if(attempt_counter > BUILD_MAX_ATTEMPTS){
            return -1;
        }
        ret = build_initial_arg_table(rnd, tdata.ini_argtab);
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
    tdata.s_arr = new ScenarioData[n_scenarios];
    for(size_t i = 0; i < n_scenarios; i++){
        // Pick a single scenario from the pool
        attempt_counter = 0;
        while(true){
            if(attempt_counter > BUILD_MAX_ATTEMPTS){
                return -1;
            }
            shifter = rnd->gen_integral_range<uint32_t>(0, MAX_SCENARIO_TYPES - 1);
            picked_scenario_type = (1 << shifter);
            if((scenario_type_pool & picked_scenario_type) != 0){
                break;
            }
            attempt_counter++;
        }
        tdata.s_arr[i].type = (ScenarioType)picked_scenario_type;
        // Copy initial arg table to scenario expected table data 
        tdata.s_arr[i].exp_argtab.reserve(tdata.ini_argtab.size());
        tdata.s_arr[i].exp_argtab = tdata.ini_argtab;
        // Build the scenario: expected table and argv
        build_scenario(rnd, tdata.s_arr[i]);
    }

    return 0;
}
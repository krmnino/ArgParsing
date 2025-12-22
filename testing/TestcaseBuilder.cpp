/*
Copyright (c) 2025 Kurt Manrique-Nino

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "ArgParsingTesting.hpp"


int32_t build_testcase(Randomizer* rnd, TestcaseData& tdata, uint32_t n_scenarios, uint32_t user_allowed_scenario_types){
    uint32_t scenario_type_pool;
    uint32_t picked_scenario_type;
    uint32_t attempt_counter;
    uint32_t shifter;
    uint32_t n_args;
    int32_t ret;
    bool invalid;
    
    // Set the number of scenarios for this testcase
    tdata.n_scenarios = n_scenarios;

    // Attempt building a argument table and check scenarios that can be tested
    attempt_counter = 0;
    while(true){
        invalid = false;
        if(attempt_counter > BUILD_MAX_ATTEMPTS){
            return -1;
        }
        n_args = rnd->gen_integral_range<uint32_t>(0, MAX_ARGS);
        ret = build_arg_table(rnd, tdata.ini_argtab, n_args);
        // If returned -1, set invalid on
        if(ret != 0){
            invalid = true;
        }
        scenario_type_pool = check_allowed_scenarios(tdata.ini_argtab, user_allowed_scenario_types);
        // If returned scenario pool is zero/empty, set invalid on
        if(scenario_type_pool == 0x00000000){
            invalid = true;
        }
        // If invalid is not set, then stop right now
        if(!invalid){
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
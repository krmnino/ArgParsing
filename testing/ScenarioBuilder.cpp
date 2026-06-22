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
#include <algorithm>


int build_scenario(Randomizer* rnd, ScenarioData& scenario){
    switch(scenario.type){
    case ScenarioType::OK:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(arg_table_count_required(scenario.exp_argtab), scenario.exp_argtab.size());
        build_OK_scenario(rnd, scenario);
        break;
    case ScenarioType::MISSING_FIRST_DASH:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(1, scenario.exp_argtab.size());
        build_MISSING_FIRST_DASH_scenario(rnd, scenario);
        break;
    case ScenarioType::MISSING_REQUIRED_ARG:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(arg_table_count_required(scenario.exp_argtab), scenario.exp_argtab.size());
        build_MISSING_REQUIRED_ARG_scenario(rnd, scenario);
        break;
    case ScenarioType::UNKNOWN_ARGUMENT:
        // Extra room (+1) for unknown argument
        scenario.n_args = rnd->gen_integral_range<uint32_t>(arg_table_count_required(scenario.exp_argtab), scenario.exp_argtab.size()) + 1;
        build_UNKNOWN_ARGUMENT_scenario(rnd, scenario);
        break;
    case ScenarioType::REPEATED_ARGUMENT:
        // We need at least 1 argument to repeat it
        // Extra room (+1) for unknown argument
        scenario.n_args = rnd->gen_integral_range<uint32_t>(arg_table_count_required(scenario.exp_argtab) + 1, scenario.exp_argtab.size()) + 1;
        build_REPEATED_ARGUMENT_scenario(rnd, scenario);
        break;
    case ScenarioType::MUST_BE_FLAG:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(arg_table_count_required(scenario.exp_argtab), scenario.exp_argtab.size());
        build_MUST_BE_FLAG_scenario(rnd, scenario);
        break;
    case ScenarioType::BAD_NUMERIC_VALUE:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(arg_table_count_required(scenario.exp_argtab), scenario.exp_argtab.size());
        build_BAD_NUMERIC_VALUE_scenario(rnd, scenario);
        break;
    case ScenarioType::EMPTY_ARG_LIST:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(1, MAX_ARGS);
        build_EMPTY_ARG_LIST_scenario(rnd, scenario);
        break;
    case ScenarioType::VALID_FLAG_GROUP:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(2, scenario.exp_argtab.size());
        build_VALID_FLAG_GROUP_scenario(rnd, scenario);
        break;
    case ScenarioType::INVALID_FLAG_GROUP:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(2, scenario.exp_argtab.size());
        build_INVALID_FLAG_GROUP_scenario(rnd, scenario);
        break;
    case ScenarioType::EXPECTING_VALUE:
        scenario.n_args = rnd->gen_integral_range<uint32_t>(2, scenario.exp_argtab.size());
        build_EXPECTING_VALUE_scenario(rnd, scenario);
        break;
    default:
        std::cerr << "ERROR: Invalid ScenarioType provided to build_scenario(): " << (int)scenario.type << std::endl;
        return -1;
    }
    return 0;
}

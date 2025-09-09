#include "ArgParsingTesting.hpp"

void validate(ErrorReporter* er, uint32_t seed, size_t tc_counter, TestcaseData& tc){
    std::string buffer;
    for(size_t i = 0; i < tc.n_scenarios; i++){
        buffer = "ArgParsingTesting - " + ScenarioType_to_string(tc.s_arr[i].type);
        er->begin_test(buffer);
        buffer = "SEED             : " + std::to_string(seed);
        er->log_it(buffer);
        buffer = "TESTCASE COUNTER : " + std::to_string(tc_counter);
        er->log_it(buffer);
        er->log_it(">>> START OF INITIAL ARGUMENT TABLE <<<");
        buffer = arg_table_to_string(tc.ini_argtab);
        er->log_it(buffer);
        er->log_it(">>> END OF INITIAL ARGUMENT TABLE <<<");
        switch (tc.s_arr[i].type){
        case ScenarioType::OK:
            validate_OK_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::MISSING_FIRST_DASH:
            validate_MISSING_FIRST_DASH_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::MISSING_REQUIRED_ARG:
            validate_MISSING_REQUIRED_ARG_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::UNKNOWN_ARGUMENT:
            validate_UNKNOWN_ARGUMENT_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::REPEATED_ARGUMENT:
            validate_REPEATED_ARGUMENT_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::MUST_BE_FLAG:
            validate_MUST_BE_FLAG_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::BAD_NUMERIC_VALUE:
            validate_BAD_NUMERIC_VALUE_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::EMPTY_ARG_LIST:
            validate_EMPTY_ARG_LIST_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::VALID_FLAG_GROUP:
            validate_VALID_FLAG_GROUP_scenario(er, tc.s_arr[i]);
            break;
        case ScenarioType::INVALID_FLAG_GROUP:
            validate_INVALID_FLAG_GROUP_scenario(er, tc.s_arr[i]);
            break;
        default:
        break;
        }
        er->end_test();
    }
}

void collect_ap_data(ScenarioData& scenario, ArgParsing* ap){
    ap->get_arg_table(scenario.res_argtab);
    ap->get_error_msg(scenario.res_error_message);
}
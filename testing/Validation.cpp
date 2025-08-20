#include "ArgParsingTesting.hpp"

void validate(ErrorReporter* er, uint32_t seed, size_t tc_counter, TestcaseData& tc){
    std::string buffer;
    for(size_t i = 0; i < tc.n_scenarios; i++){
        buffer = "ArgParsingTesting - " + ScenarioType_to_string(tc.s_arr[i].type);
        er->begin_test(buffer);
        buffer = "SEED:         " + std::to_string(seed);
        er->log_it(buffer);
        buffer = "PASS COUNTER: " + std::to_string(tc_counter);
        er->log_it(buffer);
        er->log_it(">>> START OF INITIAL ARGUMENT TABLE <<<");
        buffer = arg_table_to_string(tc.ini_argtab);
        er->log_it(buffer);
        er->log_it(">>> END OF INITIAL ARGUMENT TABLE <<<");
        switch (tc.s_arr[i].type){
        case ScenarioType::OK:
            validate_OK_scenario(er, tc.s_arr[i]);
            break;
        default:
            break;
        }
        er->end_test();
    }
}

void validate_OK_scenario(ErrorReporter* er, ScenarioData& sc){
    // Result vs. Expected error mesage
    if(sc.res_error_message != sc.exp_error_message){
        er->mark_error();
        std::cout << "TODO: ERROR 1" << std::endl;
        return;
    }
    // Result vs. Expected argument table size
    if(sc.res_argtab.size() != sc.exp_argtab.size()){
        er->mark_error();
        std::cout << "TODO: ERROR 2" << std::endl;
        return;
    }
    // Result vs. Expected argument tables
    for(size_t i = 0; i < sc.exp_argtab.size(); i++){
        // abbr_form field should not be altered
        if(sc.res_argtab[i].abbr_form != sc.exp_argtab[i].abbr_form){
            er->mark_error();
            er->log_it("ERROR: abbr_form FIELD MISMATCH");
        }
        // full_form field should not be altered
        if(sc.res_argtab[i].full_form != sc.exp_argtab[i].full_form){
            er->mark_error();
            er->log_it("ERROR: full_form FIELD MISMATCH");
        }
        // data_type field should not be altered
        if(sc.res_argtab[i].data_type != sc.exp_argtab[i].data_type){
            er->mark_error();
            er->log_it("ERROR: data_type FIELD MISMATCH");
        }
        // required field should not be altered
        if(sc.res_argtab[i].required != sc.exp_argtab[i].required){
            er->mark_error();
            er->log_it("ERROR: required FIELD MISMATCH");
        }
        // initialized should match the expected
        if(sc.res_argtab[i].initialized != sc.exp_argtab[i].initialized){
            er->mark_error();
            er->log_it("ERROR: initialized FIELD MISMATCH");
        }
        // value should match the expected
        if(sc.res_argtab[i].value != sc.exp_argtab[i].value){
            er->mark_error();
            er->log_it("ERROR: value FIELD MISMATCH");
        }
    }
}

void collect_ap_data(ScenarioData& scenario, ArgParsing* ap){
    ap->get_arg_table(scenario.res_argtab);
    ap->get_error_msg(scenario.res_error_message);
}
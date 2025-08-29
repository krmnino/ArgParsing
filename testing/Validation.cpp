#include "ArgParsingTesting.hpp"

void validate(ErrorReporter* er, uint32_t seed, size_t tc_counter, TestcaseData& tc){
    std::string buffer;
    for(size_t i = 0; i < tc.n_scenarios; i++){
        buffer = "ArgParsingTesting - " + ScenarioType_to_string(tc.s_arr[i].type);
        er->begin_test(buffer);
        buffer = "SEED         : " + std::to_string(seed);
        er->log_it(buffer);
        buffer = "PASS COUNTER : " + std::to_string(tc_counter);
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
            validate_UNKNOWN_ARGUMENT(er, tc.s_arr[i]);
            break;
        default:
        break;
        }
        er->end_test();
    }
}

void validate_OK_scenario(ErrorReporter* er, ScenarioData& sc){
    std::string buffer;
    // Result vs. Expected error mesage
    er->log_it("Result   : res_error_message = \"" + sc.res_error_message + "\"");
    er->log_it("Expected : exp_error_message = \"" + sc.exp_error_message + "\"");
    if(sc.res_error_message != sc.exp_error_message){
        er->mark_error();
        er->log_it("!!! ERROR: error_message MISMATCH");
    }
    // Result vs. Expected argument table size
    er->log_it("Result   : size of result argtab = " + std::to_string(sc.res_argtab.size()));
    er->log_it("Expected : size of result argtab = " + std::to_string(sc.exp_argtab.size()));
    if(sc.res_argtab.size() != sc.exp_argtab.size()){
        er->mark_error();
        er->log_it("!!! ERROR: size of argtab MISMATCH");
    }
    // Result vs. Expected argument tables
    for(size_t i = 0; i < sc.exp_argtab.size(); i++){
        // abbr_form field should not be altered
        if(sc.res_argtab[i].abbr_form != sc.exp_argtab[i].abbr_form){
            er->mark_error();
            er->log_it("!!! ERROR: abbr_form FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : abbr_form = " + sc.res_argtab[i].abbr_form);
            er->log_it("Expected : abbr_form = " + sc.exp_argtab[i].abbr_form);
        }
        // full_form field should not be altered
        if(sc.res_argtab[i].full_form != sc.exp_argtab[i].full_form){
            er->mark_error();
            er->log_it("!!! ERROR: full_form FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : full_form = " + sc.res_argtab[i].full_form);
            er->log_it("Expected : full_form = " + sc.exp_argtab[i].full_form);
        }
        // data_type field should not be altered
        if(sc.res_argtab[i].data_type != sc.exp_argtab[i].data_type){
            er->mark_error();
            er->log_it("!!! ERROR: data_type FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : data_type = " + APDataType_to_string(sc.res_argtab[i].data_type));
            er->log_it("Expected : data_type = " + APDataType_to_string(sc.exp_argtab[i].data_type));
        }
        // required field should not be altered
        if(sc.res_argtab[i].required != sc.exp_argtab[i].required){
            er->mark_error();
            er->log_it("!!! ERROR: required FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : required = " + bool_to_string(sc.res_argtab[i].required));
            er->log_it("Expected : required = " + bool_to_string(sc.exp_argtab[i].required));
        }
        // initialized should match the expected
        if(sc.res_argtab[i].initialized != sc.exp_argtab[i].initialized){
            er->mark_error();
            er->log_it("!!! ERROR: initialized FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : initialized = " + bool_to_string(sc.res_argtab[i].initialized));
            er->log_it("Expected : initialized = " + bool_to_string(sc.exp_argtab[i].initialized));
        }
        // value should match the expected
        if(sc.res_argtab[i].value != sc.exp_argtab[i].value){
            er->mark_error();
            er->log_it("!!! ERROR: value FIELD MISMATCH");
            er->log_it("Index    : " + std::to_string(i));
            er->log_it("Result   : value = " + sc.res_argtab[i].value);
            er->log_it("Expected : value = " + sc.exp_argtab[i].value);
        }
    }
    er->log_it(">>> START OF ARGV <<<");
    er->log_it(describe_argv(sc.argc, sc.argv));
    er->log_it(">>> END OF ARGV <<<");
    er->log_it(">>> START OF RESULT ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.res_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF RESULT ARGUMENT TABLE <<<");
    er->log_it(">>> START OF EXPECTED ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.exp_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF EXPECTED ARGUMENT TABLE <<<");
}

void validate_MISSING_FIRST_DASH_scenario(ErrorReporter* er, ScenarioData& sc){
    std::string buffer;
    // Result vs. Expected error mesage
    er->log_it("Result   : res_error_message = \"" + sc.res_error_message + "\"");
    er->log_it("Expected : exp_error_message = \"" + sc.exp_error_message + "\"");
    if(sc.res_error_message != sc.exp_error_message){
        er->mark_error();
        er->log_it("!!! ERROR: error_message MISMATCH");
    }
    er->log_it(">>> START OF ARGV <<<");
    er->log_it(describe_argv(sc.argc, sc.argv));
    er->log_it(">>> END OF ARGV <<<");
    // Result vs. Expected argument table size
    er->log_it("Result   : size of result argtab = " + std::to_string(sc.res_argtab.size()));
    er->log_it("Expected : size of result argtab = " + std::to_string(sc.exp_argtab.size()));
    if(sc.res_argtab.size() != sc.exp_argtab.size()){
        er->mark_error();
        er->log_it("!!! ERROR: size of argtab MISMATCH");
    }
    er->log_it(">>> START OF RESULT ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.res_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF RESULT ARGUMENT TABLE <<<");
    er->log_it(">>> START OF EXPECTED ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.exp_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF EXPECTED ARGUMENT TABLE <<<");
}

void validate_MISSING_REQUIRED_ARG_scenario(ErrorReporter* er, ScenarioData& sc){
    std::string buffer;
    // Result vs. Expected error mesage
    er->log_it("Result   : res_error_message = \"" + sc.res_error_message + "\"");
    er->log_it("Expected : exp_error_message = \"" + sc.exp_error_message + "\"");
    if(sc.res_error_message != sc.exp_error_message){
        er->mark_error();
        er->log_it("!!! ERROR: error_message MISMATCH");
    }
    er->log_it(">>> START OF ARGV <<<");
    er->log_it(describe_argv(sc.argc, sc.argv));
    er->log_it(">>> END OF ARGV <<<");
    // Result vs. Expected argument table size
    er->log_it("Result   : size of result argtab = " + std::to_string(sc.res_argtab.size()));
    er->log_it("Expected : size of result argtab = " + std::to_string(sc.exp_argtab.size()));
    if(sc.res_argtab.size() != sc.exp_argtab.size()){
        er->mark_error();
        er->log_it("!!! ERROR: size of argtab MISMATCH");
    }
    er->log_it(">>> START OF RESULT ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.res_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF RESULT ARGUMENT TABLE <<<");
    er->log_it(">>> START OF EXPECTED ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.exp_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF EXPECTED ARGUMENT TABLE <<<");
}

void validate_UNKNOWN_ARGUMENT(ErrorReporter* er, ScenarioData& sc){
    std::string buffer;
    // Result vs. Expected error mesage
    er->log_it("Result   : res_error_message = \"" + sc.res_error_message + "\"");
    er->log_it("Expected : exp_error_message = \"" + sc.exp_error_message + "\"");
    if(sc.res_error_message != sc.exp_error_message){
        er->mark_error();
        er->log_it("!!! ERROR: error_message MISMATCH");
    }
    er->log_it(">>> START OF ARGV <<<");
    er->log_it(describe_argv(sc.argc, sc.argv));
    er->log_it(">>> END OF ARGV <<<");
    // Result vs. Expected argument table size
    er->log_it("Result   : size of result argtab = " + std::to_string(sc.res_argtab.size()));
    er->log_it("Expected : size of result argtab = " + std::to_string(sc.exp_argtab.size()));
    if(sc.res_argtab.size() != sc.exp_argtab.size()){
        er->mark_error();
        er->log_it("!!! ERROR: size of argtab MISMATCH");
    }
    er->log_it(">>> START OF RESULT ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.res_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF RESULT ARGUMENT TABLE <<<");
    er->log_it(">>> START OF EXPECTED ARGUMENT TABLE <<<");
    buffer = arg_table_to_string(sc.exp_argtab);
    er->log_it(buffer);
    er->log_it(">>> END OF EXPECTED ARGUMENT TABLE <<<");
}

void collect_ap_data(ScenarioData& scenario, ArgParsing* ap){
    ap->get_arg_table(scenario.res_argtab);
    ap->get_error_msg(scenario.res_error_message);
}
#include "ArgParsingTesting.hpp"

#include <atomic>
#include <signal.h>

Randomizer* Randomizer::rnd_ptr = nullptr;
ErrorReporter* ErrorReporter::er_ptr = nullptr;
volatile sig_atomic_t running = true;

void terminating_handler(int s){
    running = false;
}

int main(int argc, char* argv[]){
    struct sigaction sa_struct;
    std::string pgm_err_msg;
    std::string seed_argval;
    std::string trace_argval;
    ArgParsing* pgm_ap;
    ArgParsing* ap_test;
    Randomizer* rnd;
    ErrorReporter* er;
    TestcaseData* testcase;
    uint64_t pass_counter;
    uint64_t n_tests;
    uint64_t n_scenarios;
    uint32_t init_seed;
    uint32_t user_allowed_scenario_types;
    bool infinite_loop;

    // Program argument table 
    APTableEntry arg_table[] = {
        { "s", "seed"       , APDataType::NUMBER  , true  },
        { "t", "n_tests"    , APDataType::NUMBER  , true  },
        { "c", "n_scenarios", APDataType::NUMBER  , true  },
        { "r", "trace"      , APDataType::FLAG    , false },
    };
    
    // Start the argument parser
    pgm_ap = new ArgParsing();
    pgm_ap->set_arg_table(arg_table, sizeof(arg_table) / sizeof(arg_table[0]));
    pgm_ap->set_input_args(argc, argv);
    if(pgm_ap->parse() != 0){
        pgm_ap->get_error_msg(pgm_err_msg);
        std::cerr << pgm_err_msg << std::endl;
        return -1;
    }

    // Start the Randomizer
    init_seed = std::stoi(pgm_ap->get_arg_value("seed", false));
    rnd = Randomizer::get_instance(init_seed);
    if(rnd == nullptr){
        std::cerr << "ERROR: Could not initialize Randomizer." << std::endl;
        return -1;
    }

    // Start the ErrorReporter
    er = ErrorReporter::get_instance();
    if(er == nullptr){
        std::cerr << "ERROR: Could not initialize ErrorReporter." << std::endl;
        return -1;
    }
    
    // Set up signal handler to stop program
    sa_struct.sa_handler = terminating_handler;
    sigemptyset(&sa_struct.sa_mask);
    sa_struct.sa_flags = 0;
    sigaction(SIGINT, &sa_struct, NULL);
    
    n_tests = std::stoi(pgm_ap->get_arg_value("n_tests", false));
    // Ignore pass counter if n_tests is 0
    if(n_tests == 0){
        infinite_loop = true;
    }
    else{
        infinite_loop = false;
    }
    n_scenarios = std::stoi(pgm_ap->get_arg_value("n_scenarios", false));

    user_allowed_scenario_types = (uint32_t)ScenarioType::OK                     |
                                  (uint32_t)ScenarioType::MISSING_FIRST_DASH     ;//|
                                  //(uint32_t)ScenarioType::MISSING_REQUIRED_ARG   |
                                  //(uint32_t)ScenarioType::UNKNOWN_ARGUMENT       |
                                  //(uint32_t)ScenarioType::REPEATED_ARGUMENT      |
                                  //(uint32_t)ScenarioType::MUST_BE_FLAG           |
                                  //(uint32_t)ScenarioType::BAD_NUMERIC_VALUE      |
                                  //(uint32_t)ScenarioType::EMPTY_ARG_LIST         |
                                  //(uint32_t)ScenarioType::VALID_FLAG_GROUP       |
                                  //(uint32_t)ScenarioType::INVALID_FLAG_GROUP     ;

    // Reset pass counter
    pass_counter = 0;

    // Allow tracing?
    if(pgm_ap->get_arg_value("trace", false) == "1"){
        er->log_everything(true);
    }
    else{
        er->log_everything(false);
    }
    
    // Main driver
    while((pass_counter < n_tests || infinite_loop) && running){
        testcase = new TestcaseData();
        // Build a testcase and its multiple scenarios
        build_testcase(rnd, *testcase, n_scenarios, user_allowed_scenario_types);
        for(size_t i = 0; i < n_scenarios; i++){
            ap_test = new ArgParsing();
            ap_test->set_arg_table(testcase->ini_argtab);
            ap_test->set_input_args(testcase->s_arr[i].argc, testcase->s_arr[i].argv);
            ap_test->parse();
            // Collect the data from the ArgParsing object 
            collect_ap_data(testcase->s_arr[i], ap_test);
            delete ap_test;
        }
        validate(er, rnd->get_root_seed(), pass_counter, *testcase);
        delete testcase;
        rnd->root_seed_next();
        pass_counter++;
        if(!running){
            break;
        }
    }

    std::cout << "\nTERMINATING... " << "Pass Counter: " << pass_counter << std::endl;
    er->print_report();

    delete pgm_ap;
    Randomizer::end_instance();
    ErrorReporter::end_instance();

    return 0;
}
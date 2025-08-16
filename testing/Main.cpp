#include "ArgParsingTesting.hpp"

#include <atomic>
#include <signal.h>

Randomizer* Randomizer::rnd_ptr = nullptr;
static std::atomic_bool terminate = false;

void terminating(int s){
    terminate = true;
}

int main(int argc, char* argv[]){
    ArgParsing* pgm_ap;
    ArgParsing* ap_test;
    Randomizer* rnd;
    TestcaseData* testcase;
    uint64_t counter;
    uint64_t n_tests;
    uint64_t n_scenarios;
    uint32_t init_seed;
    uint32_t user_allowed_scenario_types;
    bool infinite_loop;
    bool terminate;

    // Program argument table 
    APTableEntry arg_table[] = {
        { "s", "seed"       , APDataType::NUMBER  , true  },
        { "t", "n_tests"    , APDataType::NUMBER  , true  },
        { "c", "n_scenarios", APDataType::NUMBER  , true  },
        { "r", "trace"      , APDataType::FLAG    , false },
    };
    
    // Start the argument parser
    pgm_ap = new ArgParsing();
    pgm_ap->set_arg_table(arg_table, sizeof(arg_table) / sizeof(APTableEntry));
    pgm_ap->set_input_args(argc, argv);
    if(pgm_ap->parse() != 0){
        return -1;
    }

    // Start the Randomizer
    init_seed = 1;
    rnd = Randomizer::get_instance(init_seed);
    if(rnd == nullptr){
        return -1;
    }

    // Set up signal handler to stop program
    terminate = false;
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = terminating;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    counter = 0;
    n_tests = 1;
    n_scenarios = 1;
    infinite_loop = false;
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
    
    // Main driver
    while((counter < n_tests || infinite_loop) && !terminate){
        testcase = new TestcaseData();
        // Build a testcase and its multiple scenarios
        build_testcase(rnd, *testcase, n_scenarios, user_allowed_scenario_types);
        for(size_t i = 0; i < n_scenarios; i++){
            ap_test = new ArgParsing();
            ap_test->set_arg_table(testcase->ini_argtab);
            ap_test->set_input_args(testcase->s_arr[i].argc, testcase->s_arr[i].argv);
            ap_test->parse();
            delete ap_test;
        }
        delete testcase;
        counter++;
    }

    delete pgm_ap;
    Randomizer::end_instance();
    return 0;
}
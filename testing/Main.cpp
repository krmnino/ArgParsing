#include "ArgParsingTesting.hpp"

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
    std::string types_argval;
    ArgParsing* pgm_ap;
    ArgParsing* ap_test;
    Randomizer* rnd;
    ErrorReporter* er;
    TestcaseData* testcase;
    uint64_t testcase_counter;
    uint32_t n_tests;
    uint32_t n_scenarios;
    uint32_t init_seed;
    uint32_t user_allowed_scenario_types;
    bool infinite_loop;

    // Program argument table 
    APTableEntry arg_table[] = {
        { "s", "seed"       , APDataType::UNSIGNED_INT , true  },
        { "n", "n_tests"    , APDataType::UNSIGNED_INT , true  },
        { "c", "n_scenarios", APDataType::UNSIGNED_INT , true  },
        { "t", "types"      , APDataType::UNSIGNED_INT , true  },
        { "r", "trace"      , APDataType::FLAG         , false },
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

    // Read and validate seed argument
    seed_argval = pgm_ap->get_arg_value("seed", false);
    if(seed_argval.size() > 2 && seed_argval[0] == '0' && seed_argval[1] == 'x'){
        seed_argval = seed_argval.substr(2);
        init_seed = std::stoi(pgm_ap->get_arg_value("seed", false), nullptr, 16);
    }
    else{
        init_seed = std::stoi(pgm_ap->get_arg_value("seed", false));
    }
    if(init_seed == 0){
        std::cerr << "ERROR: seed value cannot be zero." << std::endl;
        return -1;
    }

    // Start the Randomizer
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
    
    // Read n_tests argument
    n_tests = std::stoi(pgm_ap->get_arg_value("n_tests", false));
    // Ignore pass counter if n_tests is 0
    if(n_tests == 0){
        infinite_loop = true;
    }
    else{
        infinite_loop = false;
    }
    
    // Read n_scenarios argument
    n_scenarios = std::stoi(pgm_ap->get_arg_value("n_scenarios", false));
    
    // Read and validate types argument
    types_argval = pgm_ap->get_arg_value("types", false);
    if(types_argval.size() > 2 && types_argval[0] == '0' && types_argval[1] == 'x'){
        types_argval = types_argval.substr(2);
        user_allowed_scenario_types = std::stoi(types_argval, nullptr, 16);
    }
    else{
        user_allowed_scenario_types = std::stoi(types_argval);
    }
    if(user_allowed_scenario_types == 0){
        std::cerr << "ERROR: --types argument cannot be zero." << std::endl;
        return -1;
    }

    // Reset pass counter
    testcase_counter = 0;

    // Allow tracing?
    if(pgm_ap->get_arg_value("trace", false) == "1"){
        er->log_everything(true);
    }
    else{
        er->log_everything(false);
    }
    
    // Main driver
    while((testcase_counter < n_tests || infinite_loop) && running){
        testcase = new TestcaseData();
        // Build a testcase and its multiple scenarios
        build_testcase(rnd, *testcase, n_scenarios, user_allowed_scenario_types);
        for(uint32_t i = 0; i < n_scenarios; i++){
            ap_test = new ArgParsing();
            ap_test->set_arg_table(testcase->ini_argtab);
            ap_test->set_input_args(testcase->s_arr[i].argc, testcase->s_arr[i].argv);
            ap_test->parse();
            // Collect the data from the ArgParsing object and delete ArgParsing object
            collect_ap_data(testcase->s_arr[i], ap_test);
            delete ap_test;
        }
        validate(er, rnd->get_root_seed(), testcase_counter, *testcase);
        delete testcase;
        rnd->root_seed_next();
        testcase_counter++;
        if(!running){
            break;
        }
    }

    std::cout << "\nTERMINATING... " << "Testcase Counter: " << testcase_counter << std::endl;
    er->print_report();

    delete pgm_ap;
    Randomizer::end_instance();
    ErrorReporter::end_instance();

    return 0;
}
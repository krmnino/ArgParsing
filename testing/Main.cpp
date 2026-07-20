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


Randomizer* Randomizer::rnd_ptr = nullptr;
ErrorReporter* ErrorReporter::er_ptr = nullptr;
volatile sig_atomic_t running = true;


void terminating_handler(int s){
    running = false;
}


int main(int argc, char* argv[]){
    struct sigaction sa_struct{};
    std::vector<TestcaseData> tcs_on_error{};
    std::string pgm_err_msg{};
    std::string seed_argval{};
    std::string types_argval{};
    ArgParsing* pgm_ap{};
    ArgParsing* ap_test{};
    Randomizer* rnd{};
    ErrorReporter* er{};
    uint64_t testcase_counter{};
    uint64_t n_tests{};
    uint64_t n_scenarios{};
    uint64_t error_counter{};
    uint64_t max_errors{};
    BuildStatus td_status{};
    uint32_t init_seed{};
    uint32_t user_allowed_scenario_types{};
    bool infinite_loop{};
    bool trace{};

    // Program argument table 
    APTableEntry arg_table[] = {
        { "s", "seed"       , APDataType::UNSIGNED_INT , true  },
        { "n", "n_tests"    , APDataType::UNSIGNED_INT , true  },
        { "" , "n_scenarios", (uint64_t)1                      },
        { "e", "max_errors" , (uint64_t)1                      },
        { "" , "types"      , (uint64_t)0x7FF                  },
        { "t", "trace"      , APDataType::FLAG         , false },
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
    init_seed = pgm_ap->get_arg_value<uint32_t>("seed", false);
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
    
    // Read n_tests argument
    n_tests = pgm_ap->get_arg_value<uint64_t>("n_tests", false);
    // Ignore pass counter if n_tests is 0
    if(n_tests == 0){
        infinite_loop = true;
    }
    else{
        infinite_loop = false;
    }

    // Read max_errors argument
    max_errors = pgm_ap->get_arg_value<uint64_t>("max_errors", false);
    tcs_on_error.reserve(max_errors);
    
    // Read n_scenarios argument
    n_scenarios = pgm_ap->get_arg_value<uint64_t>("n_scenarios", false);
    
    // Read and validate types argument
    user_allowed_scenario_types = pgm_ap->get_arg_value<uint32_t>("types", false);
    if(user_allowed_scenario_types == 0){
        std::cerr << "ERROR: --types argument cannot be zero." << std::endl;
        return -1;
    }

    // Allow tracing?
    trace = pgm_ap->get_arg_value<bool>("trace", false);

    // Set up signal handler to stop program
    sa_struct.sa_handler = terminating_handler;
    sigemptyset(&sa_struct.sa_mask);
    sa_struct.sa_flags = 0;
    sigaction(SIGINT, &sa_struct, NULL);

    std::cout << std::endl << "STARTING TEST MAIN LOOP... " << std::endl;

    // Main loop
    while((testcase_counter < n_tests || infinite_loop) && (error_counter < max_errors) && running){
        // Build a testcase and its multiple scenarios
        TestcaseData testcase(rnd, n_scenarios, user_allowed_scenario_types, testcase_counter);
        td_status = testcase.get_status();
        if(td_status != BuildStatus::OK){
            std::cerr << "Testcase initialization ended with \'" << BuildStatus_to_string(td_status) << "\'." << std::endl;
            break;
        }
        // Run the scenarios on ArgParsing
        for(uint32_t i = 0; i < n_scenarios; i++){
            ScenarioData& loc_sc = testcase.get_scenario(i);
            ap_test = new ArgParsing();
            ap_test->set_arg_table(testcase.get_init_argtab());
            ap_test->set_input_args(loc_sc.get_argc(), loc_sc.get_argv());
            ap_test->parse();
            // Collect the data from the ArgParsing object and delete ArgParsing object
            loc_sc.collect_ap_data(ap_test);
            loc_sc.validate(er, testcase_counter);
            if(loc_sc.get_error_types() != ErrorType::OK){
                tcs_on_error.push_back(testcase);
                error_counter++;
            }
            else if(trace){
                loc_sc.display();
            }
            rnd->root_seed_next();
            delete ap_test;
        }
        testcase_counter++;
        if(!running){
            break;
        }
    }
    std::cout << std::endl << "TERMINATING... " << "Testcase Counter: " << testcase_counter << std::endl;

    // Print any errors that may have been detected
    for(size_t i = 0; i < tcs_on_error.size(); i++){
        std::cout << "================ (START OF ERROR REPORT) ================" << std::endl;
        std::cout << "Initial seed              : " << init_seed << std::endl;
        std::cout << "Total tescases            : " << testcase_counter << std::endl;
        std::cout << "Tescases containin errors : " << error_counter << std::endl;
        tcs_on_error[i].display_errors();
        std::cout << "================= (END OF ERROR REPORT) =================" << std::endl;
    }

    delete pgm_ap;
    Randomizer::end_instance();
    ErrorReporter::end_instance();

    return 0;
}
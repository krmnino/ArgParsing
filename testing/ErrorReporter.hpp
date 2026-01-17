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

#ifndef REPORTER
#define REPORTER

#include <iostream>
#include <vector>


enum class ERExceptionCode {
	OK,
	TEST_ALREADY_STARTED,
	TEST_NOT_STARTED,
};


class ERException : public std::exception {
protected:
	ERExceptionCode err_code;
	std::string message;

public:
	explicit ERException(){
        this->err_code = ERExceptionCode::OK;
    }


	explicit ERException(ERExceptionCode error_code){
        switch (this->err_code) {
	    case ERExceptionCode::OK:
		    break;
	    case ERExceptionCode::TEST_ALREADY_STARTED:
            this->message = "A test session has been started already.";
		    break;
	    case ERExceptionCode::TEST_NOT_STARTED:
            this->message = "A test session has not been started.";
		    break;
       	default:
		    this->message = "Undefinded error.";
		    break;
	    }
    }


	virtual ~ERException() noexcept {}


	virtual const char* what() const noexcept{
        return this->message.c_str();
    }


	ERExceptionCode get_error_code(){
        return this->err_code;
    }
};


struct Log{
    std::string text;
    std::string test_id;
    bool is_error;
};


class ErrorReporter{
private:
    std::vector<Log> logs;
    static ErrorReporter* er_ptr;
    size_t logs_size;
    unsigned long test_counter;
    unsigned long success_counter;
    unsigned long error_counter;
    bool log_everything_flag;
    bool started_test;
    bool marked_error;
    std::string test_id_buffer;
    std::string test_text_buffer;

    ErrorReporter(){
        this->logs_size = 0;
        this->test_counter = 0;
        this->success_counter = 0;
        this->error_counter = 0;
        this->log_everything_flag = false;
        this->started_test = false;
        this->marked_error = false;
    }

    std::string print_is_error(bool data){
        return (data) ? "FAIL" : "PASS";
    }    

public:
    static ErrorReporter* get_instance(){
        if(er_ptr == nullptr){
            er_ptr = new ErrorReporter();
        }
        return er_ptr;
    }


    static void end_instance(){
        delete er_ptr;
    }


    size_t get_error_counter(){
        return this->error_counter;
    }


    void log_everything(bool flag){
        this->log_everything_flag = flag;
    }
   
    
    ~ErrorReporter() {}


    void log_it(std::string input_text){
        if(test_text_buffer.size() != 0){
            test_text_buffer += "\n";
        }
        test_text_buffer += input_text;
    }


    void begin_test(const char* input_id){
        if(this->started_test){
            throw ERException(ERExceptionCode::TEST_ALREADY_STARTED);
        }
        this->test_text_buffer = "";
        this->test_id_buffer = input_id;
        this->marked_error = false;
        this->started_test = true;
    }


    void begin_test(std::string& input_id){
        if(this->started_test){
            throw ERException(ERExceptionCode::TEST_ALREADY_STARTED);
        }
        this->test_text_buffer = "";
        this->test_id_buffer = input_id;
        this->marked_error = false;
        this->started_test = true;
    }
    
    
    void end_test(){
        if(!this->started_test){
            throw ERException(ERExceptionCode::TEST_NOT_STARTED);
        }
        if(this->log_everything_flag || this->marked_error){
            this->logs.push_back(Log());
            this->logs[logs_size].text = this->test_text_buffer;
            this->logs[logs_size].test_id = this->test_id_buffer;
            this->logs[logs_size].is_error = this->marked_error;
            this->logs_size++;
            if(this->marked_error){
                this->error_counter++;
            }
        }
        if(!marked_error){
            this->success_counter++;
        }
        this->test_counter++;
        this->started_test = false;
    }


    void mark_error(){
        if(!this->started_test){
            throw ERException(ERExceptionCode::TEST_NOT_STARTED);
        }
        this->marked_error = true;
    }


    void print_report(){
        std::cout << "================ (START OF REPORT) ================" << std::endl;
        std::cout << "Total passes: " << this->test_counter << std::endl;
        std::cout << "Successful passes: " << this->success_counter << std::endl;
        std::cout << "Failed passes: " << this->error_counter << std::endl;
        std::cout << std::endl;
        for(size_t i = 0; i < this->logs_size; i++){
            std::cout << "START TEST       : " << this->logs[i].test_id << std::endl;
            std::cout << "TEST RESULT      : " << this->print_is_error(this->logs[i].is_error) << std::endl;
            std::cout << this->logs[i].text << std::endl;
            std::cout << "END TEST         : " << this->logs[i].test_id << std::endl;
            std::cout << "---------------------------------------------------" << std::endl;
        }
        std::cout << "================= (END OF REPORT) =================" << std::endl;
    }
};

#endif
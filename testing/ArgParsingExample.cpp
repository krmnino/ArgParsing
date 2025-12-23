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


#include "../src/ArgParsing.hpp"


int main(int argc, char* argv[]){
    int ret;

    ArgParsing& ap = ArgParsing::get_instance();

    APTableEntry arg_table[] = {
        { "a", "arg1", APDataType::FLAG        , true  },
        { "x", "arg2", APDataType::TEXT        , false },
        {      "arg3", APDataType::UNSIGNED_INT, false },
        { "9", "arg4", APDataType::FLAG        , false },
        { "z", "arg5", APDataType::TEXT        , true  },
    };
    
    ret = ap.set_arg_table(arg_table, sizeof(arg_table) / sizeof(APTableEntry));
    if(ret != 0){
        std::cerr << "ERROR: ArgParsing::set_arg_table() return code -> " << ret << std::endl;
        return -1;
    }

    ap.set_input_args(argc, argv);
    
    ret = ap.parse();
    if(ret != 0){
        std::cerr << "ERROR: ArgParsing::set_input_args() return code -> " << ret << std::endl;
        return -1;
    }

    std::cout << "a/arg1: " << ap.get_arg_value<bool>("arg1", false) << std::endl;
    std::cout << "a/arg1 byte_size: " << ap.get_arg_value_bytesize("arg1", false) << std::endl;
    std::cout << "x/arg2: " << ap.get_arg_value<std::string>("x", true) << std::endl;
    std::cout << "x/arg2 byte_size: " << ap.get_arg_value_bytesize("x", true) << std::endl;
    std::cout << "  arg3: " << ap.get_arg_value<uint64_t>("arg3", false) << std::endl;
    std::cout << "  arg3 byte_size: " << ap.get_arg_value_bytesize("arg3", false) << std::endl;
    std::cout << "9/arg4: " << ap.get_arg_value<bool>("9", true) << std::endl;
    std::cout << "9/arg4 byte_size: " << ap.get_arg_value_bytesize("9", true) << std::endl;
    std::cout << "z/arg5: " << ap.get_arg_value<std::string>("z", true) << std::endl;
    std::cout << "z/arg5 byte_size: " << ap.get_arg_value_bytesize("z", true) << std::endl;
    
    return 0;
}
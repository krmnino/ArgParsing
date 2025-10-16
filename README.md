# ArgParsing

A simple command-line argument parser library for C/C++ programs.

## Repository Structure

- `src`: contains all the source files, Makefile, and bash script required to build the `libArgParsing.so` file.
- `testing`: test driver program to validate operation of the ArgParsing library.

## Requirements and Dependencies

- C++17
- Make

## Build

1. Navigate into `src`.
2. Issue the command `sh Initialize.sh` to prepare the environment before creating the library file.
3. Issue the command `make`. This will generate and place the library file in `res/libArgParsing.so`.
4. You can now copy the shared object file `res/libArgParsing.so` to a desired location for your project.
5. If you intend to integrate `ArgParsing` to a C++ project, you should copy the header file `src/ArgParsing.hpp` to a desired location for your project.
6. If you intend to integrate `ArgParsing` to a C project, you should copy the header file `src/ArgParsing_C.h` to a desired location for your project.

## Usage for C++ Integration

The file `testing/ArgParsingExample.cpp` is an integration example on how the `ArgParsing` library can be used within a C++ program.

Additionally, the file `testing/ArgParsingCExample.c` is an integration example on how the `ArgParsing` library can be used within a C program.

Below you will find the publicly exposed methods belonging to the `ArgParsing` class.

### `static ArgParsing& get_instance()`

- **Input arguments**: none.
- **Output**: a static reference to the singleton ArgParsing object.
- Only one instance of the `ArgParsing` class is allowed to exist throughout the execution of the program.

### `void set_input_args(int input_argc, char** input_argv)`

- **Input arguments**:
  - `input_argc`: the number of C strings in program's argv to be passed to the `ArgParsing` object.
  - `input_argv`: an array of pointers to C strings to be passed to the `ArgParsing` object.
- **Output**: none.
- This method will assign the pointer to the array of strings containing the program's argument list (argv) and how many of them are in the list (argc) to the `ArgParsing` instance.

### `int set_arg_table(APTableEntry* arg_table_ptr, size_t n_entries)`

- **Input arguments**:
  - `arg_table_ptr`: a pointer to an array of `APTableEntry` objects that will be used to define `ArgParsing`'s argument table.
  - `n_entries`: the number of elements in the array of `APTableEntry` objects.
- **Output**: return 0 if no errors occurred. Return 1 if an error was encountred plus an informational message is reported.
- This method will initialize an argument table in the `ArgParsing` object. This table of arguments will be used to validate the program's command line arguments and hold their values once parsed and validated.
- This method performs validation to prevent duplicate argument identifiers, use of reserved keywords, etc.

### `int set_arg_table(std::vector<APTableEntry>& arg_table)`

- **Input arguments**:
  - `arg_table`: a reference to a vector of `APTableEntry` objects that will be used to define `ArgParsing`'s argument table.
- **Output**: return 0 if no errors occurred. Return 1 if an error was encountred plus an informational message.
- Method overloading, calls `set_arg_table(APTableEntry* arg_table_ptr, size_t n_entries)`.

### `int parse()`

- **Input arguments**: none.
- **Output**: return 0 if no errors occurred. Return 1 if an error was encountred plus an informational message is reported.
- This method will parse the program's command line arguments set by `set_input_args()` method and and validate them against the argument table established `set_arg_table()` method.
- When an error is encontered, the `ArgParsing` object will switch to an error state and reason for error is set.
- An informational message is reported describing the cause of the error.

### `template<typename T> get_arg_value(std::string arg_key, bool is_abbr_input)`

- **Input arguments**:
  - `arg_key`: a string containing the argument identifier
  - `is_abbr_input`: a flag indicating whether the argument identifier string is in abbreviated or full form.
- **Output**: the argument value on its corresponding data type that `APDataType` maps will be returned if `ArgParsing`'s argument table has an entry with an argument identifier that matches the input argument.
- `APDataType` mappings described below:

|`APDataType`   |C/C++ data type  |
|---------------|-----------------|
|`TEXT`         |`std::string`    |
|`FLAG`         |`bool`           |
|`UNSIGNED_INT` |`uint64_t`       |
|`SIGNED_INT`   |`int64_t`        |

## Usage for C Integration

The file `testing/ArgParsingCExample.c` is an integration example on how the `ArgParsing` library can be used within a C program.

Below you will find the C interface functions to interact with the `ArgParsing` class.

### `ArgParsing_C* ArgParsing_C_get_instance()`

- **Input arguments**: none.
- **Output**: a pointer to `ArgParsing_C` object.
- The returned `ArgParsing_C` pointer points to a statically allocated `ArgParsing` object.

### `void ArgParsing_C_set_input_args(ArgParsing_C* apc, int input_argc, char** input_argv)`

- **Input arguments**:
  - `apc`: a pointer to a `ArgParsing_C` object.
  - `input_argc`: the number of C strings in program's argv to be passed to the `ArgParsing` object.
  - `input_argv`: an array of pointers to C strings to be passed to the `ArgParsing` object.
- **Output**: none.
- This function calls the `ArgParsing` method `void set_input_args(int input_argc, char** input_argv)`.

### `int ArgParsing_C_set_arg_table(ArgParsing_C* apc, APTableEntry_C* input_arg_table, size_t n_entries)`

- **Input arguments**:
  - `apc`: a pointer to a `ArgParsing_C` object.
  - `input_arg_table`: a pointer to an array of `APTableEntry_C` objects that will be used to define `ArgParsing`'s argument table.
  - `n_entries`: the number of elements in the array of `APTableEntry_C` objects.
- **Output**: return 0 if no errors occurred. Return 1 if an error was encountred plus an informational message is reported.
- Iterates through each element in the input array, converting them into `APTableEntry` objects, and placing these new objects into a vector.
- This function calls the `ArgParsing` method `int ArgParsing::set_arg_table(std::vector<APTableEntry>& arg_table)`.

### `int ArgParsing_C_parse(ArgParsing_C*)`

- **Input arguments**:
  - `apc`: a pointer to a `ArgParsing_C` object.
- **Output**: return 0 if no errors occurred. Return 1 if an error was encountred plus an informational message is reported.
- This function calls the `ArgParsing` method `int parse()`.

### `const char* ArgParsing_C_get_value_TEXT(ArgParsing_C* apc, const char* arg_key, bool is_abbr_input)`

- **Input arguments**:
  - `apc`: a pointer to a `ArgParsing_C` object.
  - `arg_key`: a string containing the argument identifier
  - `is_abbr_input`: a flag indicating whether the argument identifier string is in abbreviated or full form.
- **Output**: if the argument exists, the `const char*` value associated with the argument is returned.
- This function calls the `ArgParsing` method `template<typename T> get_arg_value(std::string arg_key, bool is_abbr_input)` with `T` being `std::string`.

### `bool ArgParsing_C_get_value_FLAG(ArgParsing_C* apc, const char* arg_key, bool is_abbr_input)`

- **Input arguments**:
  - `apc`: a pointer to a `ArgParsing_C` object.
  - `arg_key`: a string containing the argument identifier
  - `is_abbr_input`: a flag indicating whether the argument identifier string is in abbreviated or full form.
- **Output**: if the argument exists, the `bool` value associated with the argument is returned.
- This function calls the `ArgParsing` method `template<typename T> get_arg_value(std::string arg_key, bool is_abbr_input)` with `T` being `bool`.

### `uint64_t ArgParsing_C_get_value_UNSIGNED_INT(ArgParsing_C* apc, const char* arg_key, bool is_abbr_input)`

- **Input arguments**:
  - `apc`: a pointer to a `ArgParsing_C` object.
  - `arg_key`: a string containing the argument identifier
  - `is_abbr_input`: a flag indicating whether the argument identifier string is in abbreviated or full form.
- **Output**: if the argument exists, the `uint64_t` value associated with the argument is returned.
- This function calls the `ArgParsing` method `template<typename T> get_arg_value(std::string arg_key, bool is_abbr_input)` with `T` being `uint64_t`.

### `int64_t ArgParsing_C_get_value_SIGNED_INT(ArgParsing_C* apc, const char* arg_key, bool is_abbr_input)`

- **Input arguments**:
  - `apc`: a pointer to a `ArgParsing_C` object.
  - `arg_key`: a string containing the argument identifier
  - `is_abbr_input`: a flag indicating whether the argument identifier string is in abbreviated or full form.
- **Output**: if the argument exists, the `int64_t` value associated with the argument is returned.
- This function calls the `ArgParsing` method `template<typename T> get_arg_value(std::string arg_key, bool is_abbr_input)` with `T` being `int64_t`.

## Changelog

### v1.2

- Implemented Meyers' Singleton for `ArgParsing` class.
- Updates to `ArgParsingExample.cpp` reflecting changes to support Meyers' Singleton implementation.
- Definition of API for integration with C projects, including the following functions:
  - `ArgParsing_C* ArgParsing_C_get_instance()`
  - `void ArgParsing_C_set_input_args(ArgParsing_C*, int, char**)`
  - `int ArgParsing_C_set_arg_table(ArgParsing_C*, APTableEntry_C*, size_t)`
  - `int ArgParsing_C_parse(ArgParsing_C*)`
  - `const char* ArgParsing_C_get_value_TEXT(ArgParsing_C*, const char*, bool)`
  - `bool ArgParsing_C_get_value_FLAG(ArgParsing_C*, const char*, bool)`
  - `uint64_t ArgParsing_C_get_value_UNSIGNED_INT(ArgParsing_C*, const char*, bool)`
  - `int64_t ArgParsing_C_get_value_SIGNED_INT(ArgParsing_C*, const char*, bool)`
- Miscellaneous fixes and code clean up.

### v1.1

- Definiton of data types for argument values:
  - `APDataType::TEXT`
  - `APDataType::FLAG`
  - `APDataType::UNSIGNED_INT`
  - `APDataType::SIGNED_INT`
- Store argument values in their equivalent C++ data type rather than just `std::string`.
- Refactored getter method using templates: `T get_arg_value(std::string, bool)`.

### v1.0

- Initial release.

# ArgParsing

A simple command-line argument parser library for C++ programs.

## Repository Structure

- `src`: contains all the source files, Makefile, and bash script required to build the `libArgParsing.so` file.
- `testing`: test driver program to validate operation of the ArgParsing library.

## Requirements and Dependencies

- C++17
- Make

## Build

1. Navigate into `src`.
2. Issue the command `sh Initialize.sh` to prepare the environment before creating the library file.
3. Issue the command `make`. This will generate and place the library file in `../res/libArgParsing.so`.
4. You may now copy the header file `src/ArgParsing.hpp` and `res/libArgParsing.so` to a desired location for your project.

## Usage

The file `testing/ArgParsingExample.cpp` is an integration example on how the `ArgParsing` library can be used within a C++ program.

Below you will find the publicly exposed methods belonging to the `ArgParsing` class.

### `ArgParsing* ArgParsing_get_instance()`

- **Input arguments**: none.
- **Output**: a pointer to the allocated singleton ArgParsing object.
- This method will allocate a single instance of the `ArgParsing` class. Only one instance of the `ArgParsing` class is allowed to exist throughout the execution of the program.

### `void set_input_args(int input_argc, char** input_argv)`

- **Input arguments**:
  - `input_argc`: the number of C strings in program's argv to be passed to the `ArgParsing` object.
  - `input_argv`: an array of pointers to C strings to be passed to the `ArgParsing` object.
- **Output**: none.
- This method will assign the pointer to the array of strings containing the program's argument list (argv) and how many of them are in the list (argc) to the `ArgParsing` instance.

### `int set_arg_table(APTableEntry* arg_table_ptr, size_t n_entries)`

- **Input arguments**: Pointer to an array of `APTableEntry` objects that will be used to define `ArgParsing`'s argument table.
- **Output**: return 0 if no errors occurred. Return 1 if an error was encountred plus an informational message is reported.
- This method will initialize an argument table in the `ArgParsing` object. This table of arguments will be used to validate the program's command line arguments and hold their values once parsed and validated.
- This method performs validation to prevent duplicate argument identifiers, use of reserved keywords, etc.

### `int set_arg_table(std::vector<APTableEntry>& arg_table)`

- **Input arguments**: Reference to a vector of `APTableEntry` objects that will be used to define `ArgParsing`'s argument table.
- **Output**: return 0 if no errors occurred. Return 1 if an error was encountred plus an informational message.
- Method overloading, calls `set_arg_table(APTableEntry* arg_table_ptr, size_t n_entries)`.

### `int parse()`

- **Input arguments**: none.
- **Output**: return 0 if no errors occurred. Return 1 if an error was encountred plus an informational message is reported.
- This method will parse the program's command line arguments set by `set_input_args()` method and and validate them against the argument table established `set_arg_table()` method.
- When an error is encontered, the `ArgParsing` object will switch to an error state and reason for error is set.
- An informational message is reported describing the cause of the error.

### `std::string get_arg_value(std::string arg_key, bool is_abbr_input)`

- **Input arguments**: a string containing the argument identifier plus a flag indicating whether the argument identifier string is in abbreviated or full form.
- **Output**: a string representing the argument value will be returned if `ArgParsing`'s argument table has an entry with an argument identifier that matches the input argument.

## Changelog

### v1.0

- Initial release.

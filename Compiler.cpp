#include <iostream> // syx compiler =)
#include "easystring.h"
#include <string>
#include <fstream>
#include <limits>
#include "randomnumbergen.hpp"
#include <cmath>
#include <thread>
#include <chrono>
#include <sstream>
#include <cstdlib>
#include <unordered_map>
#include <variant>
#include <algorithm>  // std::transform (used by str to upper/lower)

// ============================================================
// COMMAND ID ENUM  – every recognised command gets one value
// ============================================================
enum class Cmd {
    UNKNOWN,
    PRINT, NO_NEWLINE_PRINT,
    INT_VAR, PRINT_INT, PRINT_STR, STR_VAR,
    START_TIMER, END_TIMER,
    ADD_AND_SET, MINUS_AND_SET,
    CHANGE_INT, CHANGE_INT_TO_INT,
    SET_AND_RANDOMGENNUM, SET_VAR_TO_SQRT,
    EXECUTE, EXECUTE2,
    MAKE_INT_LIST, ADD_TO_INT_LIST,
    SET_INT_TO_INT_FROM_LIST, REMOVE_FROM_INT_LIST,
    REPEAT,
    WHILE_INT_LESS, WHILE_INT_MORE,
    IF_EQUAL, IF_INT_EQUAL, IF_INT_LESS, IF_INT_MORE,
    IF_STR_EQUAL, IF_STR_NOT_EQUAL,
    PAUSE,
    GIVE_ME_SECRET,
    SET_INT_TO_INT, SET_INT,
    RUST_IS_BETTER,   // the important one
    INC, DEC,
    STR_TO_INT, SET_INT_TO_INPUT,
    CHANGE_STR, CHANGE_STR_TO_STR,
    SET_STR_TO_INPUT,
    STR_TO_UPPER, STR_TO_LOWER,
    APPEND_STR_BY_VAR, APPEND_STR,
    INT_TO_STR,
    MULTIPLY_AND_SET, DIVIDE_AND_SET,
    MAKE_STR_LIST, ADD_TO_STR_LIST,
    SET_STR_FROM_LIST, SET_STR_FROM_LIST_BY_INT,
    SET_STR_TO_CHAR_FROM_STR,
    MODULO_INT_AND_SET,
    SET_INT_TO_STR_LENGTH,
    REMOVE_FROM_STR_LIST, PRINT_STR_LIST_ITEM,
    APPEND_FILE, ADD_NEWLINE_TO_FILE,
    STORE_FILE_IN_LIST, OVERWRITE_FILE_WITH_LIST,
    CREATE_STRUCT, ADD_INT_VAR_TO_STRUCT,
    SET_INT_TO_STRUCT_ELEMENT,
    SET_STRUCT_SLOT_TO_INT, SET_STRUCT_SLOT_TO_INT_VAR,
    ADD_INT_TO_STRUCT, ADD_STR_VAR_TO_STRUCT,
    ADD_STR_TO_STRUCT,
    SET_STRUCT_SLOT_TO_STR_VAR, SET_STRUCT_SLOT_TO_STR,
    SET_STR_TO_STRUCT_ELEMENT,
    PRINT_STRUCT_ELEMENT, PRINT_STRUCT,
    STRUCT_SIZE_TO_INT, REMOVE_STRUCT_SLOT,
    CLEAR_STRUCT, COPY_STRUCT,
    IF_STRUCT_SLOT_IS_INT, IF_STRUCT_SLOT_IS_STR,
    FINISH, CLEAR_SCREEN,
    // ---- library / import support ----
    IMPORT,
};

// ============================================================
// COMMAND LOOKUP TABLE  – maps command strings ? Cmd enum
// Using an unordered_map for O(1) dispatch instead of a chain
// of if-else comparisons.
// ============================================================
static const std::unordered_map<std::string, Cmd> CMD_MAP = {
    {"print",                       Cmd::PRINT},
    {"no new line print",           Cmd::NO_NEWLINE_PRINT},
    {"int var",                     Cmd::INT_VAR},
    {"print int",                   Cmd::PRINT_INT},
    {"print str",                   Cmd::PRINT_STR},
    {"str var",                     Cmd::STR_VAR},
    {"start timer",                 Cmd::START_TIMER},
    {"end timer",                   Cmd::END_TIMER},
    {"add and set",                 Cmd::ADD_AND_SET},
    {"minus and set",               Cmd::MINUS_AND_SET},
    {"Change int",                  Cmd::CHANGE_INT},
    {"Change int to int",           Cmd::CHANGE_INT_TO_INT},
    {"set and randomgennum",        Cmd::SET_AND_RANDOMGENNUM},
    {"set var to sqrt",             Cmd::SET_VAR_TO_SQRT},
    {"execute",                     Cmd::EXECUTE},
    {"execute2",                    Cmd::EXECUTE2},
    {"make int list",               Cmd::MAKE_INT_LIST},
    {"add to int list",             Cmd::ADD_TO_INT_LIST},
    {"set int to int from list",    Cmd::SET_INT_TO_INT_FROM_LIST},
    {"remove from int list",        Cmd::REMOVE_FROM_INT_LIST},
    {"repeat",                      Cmd::REPEAT},
    {"while int less",              Cmd::WHILE_INT_LESS},
    {"while int more",              Cmd::WHILE_INT_MORE},
    {"if equal",                    Cmd::IF_EQUAL},
    {"if int equal",                Cmd::IF_INT_EQUAL},
    {"if int less",                 Cmd::IF_INT_LESS},
    {"if int more",                 Cmd::IF_INT_MORE},
    {"if str equal",                Cmd::IF_STR_EQUAL},
    {"if str not equal",            Cmd::IF_STR_NOT_EQUAL},
    {"pause",                       Cmd::PAUSE},
    {"give me secret",              Cmd::GIVE_ME_SECRET},
    {"set int to int",              Cmd::SET_INT_TO_INT},
    {"set int",                     Cmd::SET_INT},
    {"Rust is better than c++",     Cmd::RUST_IS_BETTER},
    {"inc",                         Cmd::INC},
    {"dec",                         Cmd::DEC},
    {"str to int",                  Cmd::STR_TO_INT},
    {"set int to input",            Cmd::SET_INT_TO_INPUT},
    {"Change str",                  Cmd::CHANGE_STR},
    {"Change str to str",           Cmd::CHANGE_STR_TO_STR},
    {"set str to input",            Cmd::SET_STR_TO_INPUT},
    {"str to upper",                Cmd::STR_TO_UPPER},
    {"str to lower",                Cmd::STR_TO_LOWER},
    {"append str by var",           Cmd::APPEND_STR_BY_VAR},
    {"append str",                  Cmd::APPEND_STR},
    {"int to str",                  Cmd::INT_TO_STR},
    {"multiply and set",            Cmd::MULTIPLY_AND_SET},
    {"divide and set",              Cmd::DIVIDE_AND_SET},
    {"make str list",               Cmd::MAKE_STR_LIST},
    {"add to str list",             Cmd::ADD_TO_STR_LIST},
    {"set str to str from list",    Cmd::SET_STR_FROM_LIST},
    {"set str to str from list by int", Cmd::SET_STR_FROM_LIST_BY_INT},
    {"set str to char from str",    Cmd::SET_STR_TO_CHAR_FROM_STR},
    {"mudolo int and set",          Cmd::MODULO_INT_AND_SET},
    {"set int to str length",       Cmd::SET_INT_TO_STR_LENGTH},
    {"remove from str list",        Cmd::REMOVE_FROM_STR_LIST},
    {"print str list item",         Cmd::PRINT_STR_LIST_ITEM},
    {"append file",                 Cmd::APPEND_FILE},
    {"add newline to file",         Cmd::ADD_NEWLINE_TO_FILE},
    {"store file in list",          Cmd::STORE_FILE_IN_LIST},
    {"overwrite file with list",    Cmd::OVERWRITE_FILE_WITH_LIST},
    {"create struct",               Cmd::CREATE_STRUCT},
    {"add int var to struct",       Cmd::ADD_INT_VAR_TO_STRUCT},
    {"set int to struct element",   Cmd::SET_INT_TO_STRUCT_ELEMENT},
    {"set struct slot to int",      Cmd::SET_STRUCT_SLOT_TO_INT},
    {"set struct slot to int var",  Cmd::SET_STRUCT_SLOT_TO_INT_VAR},
    {"add int to struct",           Cmd::ADD_INT_TO_STRUCT},
    {"add str var to struct",       Cmd::ADD_STR_VAR_TO_STRUCT},
    {"add str to struct",           Cmd::ADD_STR_TO_STRUCT},
    {"set struct slot to str var",  Cmd::SET_STRUCT_SLOT_TO_STR_VAR},
    {"set struct slot to str",      Cmd::SET_STRUCT_SLOT_TO_STR},
    {"set str to struct element",   Cmd::SET_STR_TO_STRUCT_ELEMENT},
    {"print struct element",        Cmd::PRINT_STRUCT_ELEMENT},
    {"print struct",                Cmd::PRINT_STRUCT},
    {"struct size to int",          Cmd::STRUCT_SIZE_TO_INT},
    {"remove struct slot",          Cmd::REMOVE_STRUCT_SLOT},
    {"clear struct",                Cmd::CLEAR_STRUCT},
    {"copy struct",                 Cmd::COPY_STRUCT},
    {"if struct slot is int",       Cmd::IF_STRUCT_SLOT_IS_INT},
    {"if struct slot is str",       Cmd::IF_STRUCT_SLOT_IS_STR},
    {"finish",                      Cmd::FINISH},
    {"clear screen",                Cmd::CLEAR_SCREEN},
    // library support
    {"import",                      Cmd::IMPORT},
};

// ============================================================
// HELPERS
// ============================================================

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

int clean_stoi(const std::string& input, int fallback_value = 0) {
    std::string cleaned = "";
    for (char c : input) {
        if (std::isdigit(c) || (cleaned.empty() && (c == '-' || c == '+')))
            cleaned += c;
    }
    if (cleaned.empty() || cleaned == "-" || cleaned == "+")
        return fallback_value;
    try {
        return std::stoi(cleaned);
    } catch (const std::exception&) {
        return fallback_value;
    }
}

// Safe integer index into a list – prints an error and returns false on bad index
template<typename Vec>
bool checkIndex(const Vec& v, int idx, const std::string& listname) {
    if (idx < 0 || idx >= (int)v.size()) {
        std::cerr << "[error] index " << idx << " is out of bounds for list '"
                  << listname << "' (size " << v.size() << ")\n";
        return false;
    }
    return true;
}

// Grow a struct vector to at least slotindex+1 slots
using Multitype = std::variant<int, double, std::string>;
void growStruct(std::vector<Multitype>& s, int slotindex) {
    while ((int)s.size() <= slotindex)
        s.push_back(0);
}

// Run a saved function body line-by-line (forward declaration)
// defined after processLine is constructed.

// ============================================================
// MAIN
// ============================================================
int main() {
    std::cout << "type file name for compiling: ";
    easystring UserText;
    std::string userfile;
    std::string temp;

    std::unordered_map<std::string, std::string>            string;
    std::unordered_map<std::string, std::string>            function;
    std::unordered_map<std::string, int>                    integer;
    std::unordered_map<std::string, std::vector<int>>       int_lists;
    std::unordered_map<std::string, std::vector<std::string>> str_lists;
    std::unordered_map<std::string, std::vector<Multitype>> structs;

    std::cin >> userfile;
    std::fstream script(userfile);
    if (!script.is_open()) {
        std::cerr << "[error] could not open file: " << userfile << "\n";
        return 1;
    }

    std::chrono::high_resolution_clock::time_point timerStart;

    // Read all lines up-front so save blocks can consume multiple lines
    std::vector<std::string> allLines;
    while (std::getline(script, temp))
        allLines.push_back(temp);

    // ---- library loader ---------------------------------------------------
    // Reads a .syxlib file and injects its lines into allLines at a given pos.
    // Called when "import,libname" is encountered during the pre-pass.
    auto loadLibrary = [&](const std::string& libname, size_t insertAfter) {
        std::string filename = libname + ".syxlib";
        std::ifstream lib(filename);
        if (!lib.is_open()) {
            // Also try libname directly (user might give full filename)
            lib.open(libname);
        }
        if (!lib.is_open()) {
            std::cerr << "[error] could not import library '" << libname
                      << "' (tried '" << libname << ".syxlib' and '" << libname << "')\n";
            return;
        }
        std::vector<std::string> libLines;
        std::string ll;
        while (std::getline(lib, ll))
            libLines.push_back(ll);
        allLines.insert(allLines.begin() + (long long)insertAfter + 1,
                        libLines.begin(), libLines.end());
        std::cout << "[info] imported library: " << libname << " (" << libLines.size() << " lines)\n";
    };

    // ---- processLine lambda ----------------------------------------------
    // Uses the CMD_MAP lookup to dispatch to the right handler.
    auto processLine = [&](std::string line, auto& self) -> void {
        UserText = line;
        int commandpos = UserText.findindex(",");
        if (commandpos == -1) {
            // No comma – could be a blank line or a comment (lines starting with #)
            std::string tl = trim(line);
            if (!tl.empty() && tl[0] != '#')
                std::cerr << "[warning] unrecognised line (no comma): " << line << "\n";
            return;
        }
        easystring beforecoma = UserText.getrange(0, commandpos - 1);

        // Look up command
        auto it = CMD_MAP.find(beforecoma.get());
        Cmd cmd = (it != CMD_MAP.end()) ? it->second : Cmd::UNKNOWN;

        // Helper: run a saved function block line-by-line
        auto runBlock = [&](const std::string& funcName) {
            if (function.find(funcName) == function.end()) {
                std::cerr << "[error] function '" << funcName << "' is not defined\n";
                return;
            }
            std::string block = function[funcName];
            std::istringstream ss(block);
            std::string subline;
            while (std::getline(ss, subline)) {
                if (!subline.empty())
                    self(subline, self);
            }
        };

        switch (cmd) {

        // ---- output -------------------------------------------------------
        case Cmd::PRINT: {
            std::string toprint = UserText.from(commandpos + 1);
            std::cout << toprint << "\n";
            break;
        }
        case Cmd::NO_NEWLINE_PRINT: {
            std::string toprint = UserText.from(commandpos + 1);
            std::cout << toprint;
            break;
        }
        case Cmd::PRINT_INT: {
            std::string variable = UserText.from(commandpos + 1);
            if (integer.find(variable) == integer.end())
                std::cerr << "[error] print int: integer variable '" << variable << "' does not exist\n";
            else
                std::cout << integer[variable];
            break;
        }
        case Cmd::PRINT_STR: {
            std::string variable = UserText.from(commandpos + 1);
            if (string.find(variable) == string.end())
                std::cerr << "[error] print str: string variable '" << variable << "' does not exist\n";
            else
                std::cout << string[variable];
            break;
        }

        // ---- variables ----------------------------------------------------
        case Cmd::INT_VAR: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] int var: missing '.'\n"; break; }
            std::string VarName = UserText.getrange(commandpos + 1, dotpos - 1).get();
            if (VarName.empty()) { std::cerr << "[error] int var: variable name is empty\n"; break; }
            int VarValue = clean_stoi(UserText.from(dotpos + 1));
            integer[VarName] = VarValue;
            break;
        }
        case Cmd::STR_VAR: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] str var: missing '.'\n"; break; }
            std::string strname = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string value   = UserText.from(dotpos + 1);
            string[strname] = value;
            break;
        }
        case Cmd::CHANGE_INT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] Change int: missing '.'\n"; break; }
            std::string varname = UserText.getrange(commandpos + 1, dotpos - 1);
            if (integer.find(varname) == integer.end())
                std::cerr << "[error] Change int: variable '" << varname << "' not found\n";
            else {
                try {
                    integer[varname] = std::stoi(UserText.from(dotpos + 1));
                } catch (std::invalid_argument&) {
                    std::cerr << "[error] Change int: value is not a valid integer\n";
                } catch (std::out_of_range&) {
                    std::cerr << "[error] Change int: value is out of 32-bit integer range\n";
                }
            }
            break;
        }
        case Cmd::CHANGE_INT_TO_INT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] Change int to int: missing '.'\n"; break; }
            std::string varname  = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string newvalue = UserText.from(dotpos + 1);
            if (integer.find(varname)  == integer.end()) std::cerr << "[error] Change int to int: destination '" << varname  << "' not found\n";
            else if (integer.find(newvalue) == integer.end()) std::cerr << "[error] Change int to int: source '"      << newvalue << "' not found\n";
            else integer[varname] = integer[newvalue];
            break;
        }
        case Cmd::SET_INT_TO_INT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] set int to int: missing '.'\n"; break; }
            std::string var1 = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string var2 = UserText.from(dotpos + 1);
            if (integer.find(var2) == integer.end()) std::cerr << "[error] set int to int: source '" << var2 << "' not found\n";
            else integer[var1] = integer[var2];
            break;
        }
        case Cmd::SET_INT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] set int: missing '.'\n"; break; }
            std::string var1 = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                integer[var1] = std::stoi(UserText.from(dotpos + 1));
            } catch (std::invalid_argument&) {
                std::cerr << "[error] set int: value is not a valid integer\n";
            } catch (std::out_of_range&) {
                std::cerr << "[error] set int: value is out of 32-bit integer range\n";
            }
            break;
        }
        case Cmd::INC: {
            std::string varname = UserText.from(commandpos + 1);
            if (integer.find(varname) == integer.end()) std::cerr << "[error] inc: variable '" << varname << "' not found\n";
            else integer[varname]++;
            break;
        }
        case Cmd::DEC: {
            std::string varname = UserText.from(commandpos + 1);
            if (integer.find(varname) == integer.end()) std::cerr << "[error] dec: variable '" << varname << "' not found\n";
            else integer[varname]--;
            break;
        }
        case Cmd::CHANGE_STR: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] Change str: missing '.'\n"; break; }
            std::string varname  = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string newvalue = UserText.from(dotpos + 1);
            string[varname] = newvalue;
            break;
        }
        case Cmd::CHANGE_STR_TO_STR: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] Change str to str: missing '.'\n"; break; }
            std::string var1 = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string var2 = UserText.from(dotpos + 1);
            if (string.find(var2) == string.end()) std::cerr << "[error] Change str to str: source '" << var2 << "' not found\n";
            else string[var1] = string[var2];
            break;
        }

        // ---- arithmetic ---------------------------------------------------
        case Cmd::ADD_AND_SET: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] add and set: missing '.'\n"; break; }
            std::string destVar = UserText.getrange(commandpos + 1, dotpos - 1).get();
            easystring remainder = UserText.from(dotpos + 1);
            int total = 0;
            int pluspos;
            while ((pluspos = remainder.findindex("+")) != -1) {
                std::string varName = remainder.getrange(0, pluspos - 1).get();
                if (integer.find(varName) == integer.end())
                    std::cerr << "[warning] add and set: variable '" << varName << "' not found, treating as 0\n";
                else total += integer[varName];
                remainder = remainder.from(pluspos + 1);
            }
            if (integer.find(remainder.get()) == integer.end())
                std::cerr << "[warning] add and set: variable '" << remainder.get() << "' not found, treating as 0\n";
            else total += integer[remainder.get()];
            integer[destVar] = total;
            break;
        }
        case Cmd::MINUS_AND_SET: {
            easystring afterFirstComma = UserText.from(commandpos + 1);
            int secondCommaPos = afterFirstComma.findindex(",");
            if (secondCommaPos == -1) { std::cerr << "[error] minus and set: missing second ','\n"; break; }
            std::string destVar = afterFirstComma.getrange(0, secondCommaPos - 1).get();
            easystring remainder = afterFirstComma.from(secondCommaPos + 1);
            int firstMinusPos = remainder.findindex("-");
            if (firstMinusPos == -1) { std::cerr << "[error] minus and set: missing '-'\n"; break; }
            std::string firstVar = remainder.getrange(0, firstMinusPos - 1).get();
            int total = (integer.find(firstVar) != integer.end()) ? integer[firstVar] : 0;
            remainder = remainder.from(firstMinusPos + 1);
            int minuspos;
            while ((minuspos = remainder.findindex("-")) != -1) {
                std::string varName = remainder.getrange(0, minuspos - 1).get();
                total -= (integer.find(varName) != integer.end()) ? integer[varName] : 0;
                remainder = remainder.from(minuspos + 1);
            }
            total -= (integer.find(remainder.get()) != integer.end()) ? integer[remainder.get()] : 0;
            integer[destVar] = total;
            break;
        }
        case Cmd::MULTIPLY_AND_SET: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] multiply and set: missing '.'\n"; break; }
            easystring afterComma = UserText.from(commandpos + 1);
            int timespos = afterComma.findindex("*");
            if (timespos == -1) { std::cerr << "[error] multiply and set: missing '*'\n"; break; }
            std::string var1 = afterComma.getrange(0, timespos - 1).get();
            easystring remainder = afterComma.from(timespos + 1);
            int total = (integer.find(var1) != integer.end()) ? integer[var1] : 0;
            int mpos;
            while ((mpos = remainder.findindex("*")) != -1) {
                std::string varName = remainder.getrange(0, mpos - 1).get();
                total *= (integer.find(varName) != integer.end()) ? integer[varName] : 0;
                remainder = remainder.from(mpos + 1);
            }
            total *= (integer.find(remainder.get()) != integer.end()) ? integer[remainder.get()] : 0;
            std::string destVar = UserText.getrange(commandpos + 1, dotpos - 1).get();
            integer[destVar] = total;
            break;
        }
        case Cmd::DIVIDE_AND_SET: {
            easystring afterFirstComma = UserText.from(commandpos + 1);
            int secondCommaPos = afterFirstComma.findindex(",");
            if (secondCommaPos == -1) { std::cerr << "[error] divide and set: missing second ','\n"; break; }
            std::string destVar = afterFirstComma.getrange(0, secondCommaPos - 1).get();
            easystring remainder = afterFirstComma.from(secondCommaPos + 1);
            int slashpos = remainder.findindex("/");
            if (slashpos == -1) { std::cerr << "[error] divide and set: missing '/'\n"; break; }
            std::string firstVar = remainder.getrange(0, slashpos - 1).get();
            int total = (integer.find(firstVar) != integer.end()) ? integer[firstVar] : 0;
            remainder = remainder.from(slashpos + 1);
            int dpos;
            while ((dpos = remainder.findindex("/")) != -1) {
                std::string varName = remainder.getrange(0, dpos - 1).get();
                if (integer.find(varName) == integer.end() || integer[varName] == 0) {
                    std::cerr << "[error] divide and set: division by zero (variable '" << varName << "')\n";
                    return;
                }
                total /= integer[varName];
                remainder = remainder.from(dpos + 1);
            }
            if (integer.find(remainder.get()) == integer.end() || integer[remainder.get()] == 0) {
                std::cerr << "[error] divide and set: division by zero (variable '" << remainder.get() << "')\n";
                return;
            }
            total /= integer[remainder.get()];
            integer[destVar] = total;
            break;
        }
        case Cmd::MODULO_INT_AND_SET: {
            int dotpos       = UserText.findindex(".");
            int somethingpos = UserText.findindex("-");
            if (dotpos == -1 || somethingpos == -1) { std::cerr << "[error] mudolo int and set: missing '.' or '-'\n"; break; }
            std::string target = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string var1   = UserText.getrange(dotpos + 1, somethingpos - 1);
            std::string var2   = UserText.from(somethingpos + 1);
            if (integer.find(var2) == integer.end() || integer[var2] == 0) {
                std::cerr << "[error] mudolo int and set: modulo by zero\n"; break;
            }
            integer[target] = integer[var1] % integer[var2];
            break;
        }
        case Cmd::SET_VAR_TO_SQRT: {
            std::string var = UserText.from(commandpos + 1);
            if (integer.find(var) == integer.end()) { std::cerr << "[error] set var to sqrt: variable '" << var << "' not found\n"; break; }
            if (integer[var] < 0) { std::cerr << "[error] set var to sqrt: cannot take sqrt of negative number\n"; break; }
            integer[var] = static_cast<int>(std::sqrt(integer[var]));
            break;
        }
        case Cmd::SET_AND_RANDOMGENNUM: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] set and randomgennum: missing '.'\n"; break; }
            std::string VarName = UserText.getrange(commandpos + 1, dotpos - 1).get();
            int numberpos = UserText.findindex("-");
            if (numberpos == -1) { std::cerr << "[error] set and randomgennum: missing '-'\n"; break; }
            int number1 = clean_stoi(UserText.getrange(commandpos, numberpos - 1));
            int number2 = clean_stoi(UserText.from(numberpos + 1));
            if (number1 > number2) std::swap(number1, number2);
            integer[VarName] = randomnumbergen(number1, number2);
            break;
        }

        // ---- string operations --------------------------------------------
        case Cmd::STR_TO_INT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] str to int: missing '.'\n"; break; }
            std::string val1 = UserText.getrange(commandpos + 1, dotpos - 1);
            if (string.find(val1) == string.end()) { std::cerr << "[error] str to int: string variable '" << val1 << "' not found\n"; break; }
            try {
                integer[val1] = std::stoi(string[val1]);
                string[val1].erase();
            } catch (const std::invalid_argument&) {
                std::cerr << "[error] str to int: '" << string[val1] << "' is not a valid integer\n";
            } catch (const std::out_of_range&) {
                std::cerr << "[error] str to int: value '" << string[val1] << "' is too large for a 32-bit integer\n";
            }
            break;
        }
        case Cmd::INT_TO_STR: {
            std::string varname = UserText.from(commandpos + 1);
            if (integer.find(varname) == integer.end()) { std::cerr << "[error] int to str: variable '" << varname << "' not found\n"; break; }
            string[varname] = std::to_string(integer[varname]);
            break;
        }
        case Cmd::SET_INT_TO_STR_LENGTH: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] set int to str length: missing '.'\n"; break; }
            std::string integername = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string stringname  = UserText.from(dotpos + 1);
            if (string.find(stringname) == string.end()) { std::cerr << "[error] set int to str length: string '" << stringname << "' not found\n"; break; }
            integer[integername] = (int)string[stringname].length();
            break;
        }
        case Cmd::STR_TO_UPPER: {
            std::string varname = UserText.from(commandpos + 1);
            if (string.find(varname) == string.end()) { std::cerr << "[error] str to upper: variable '" << varname << "' not found\n"; break; }
            for (char& c : string[varname]) c = (char)std::toupper((unsigned char)c);
            break;
        }
        case Cmd::STR_TO_LOWER: {
            std::string varname = UserText.from(commandpos + 1);
            if (string.find(varname) == string.end()) { std::cerr << "[error] str to lower: variable '" << varname << "' not found\n"; break; }
            for (char& c : string[varname]) c = (char)std::tolower((unsigned char)c);
            break;
        }
        case Cmd::APPEND_STR_BY_VAR: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] append str by var: missing '.'\n"; break; }
            std::string var1 = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string var2 = UserText.from(dotpos + 1);
            if (string.find(var2) == string.end()) { std::cerr << "[error] append str by var: source '" << var2 << "' not found\n"; break; }
            string[var1] += string[var2];
            break;
        }
        case Cmd::APPEND_STR: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] append str: missing '.'\n"; break; }
            std::string var1 = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string var2 = UserText.from(dotpos + 1);
            string[var1] += var2;
            break;
        }
        case Cmd::SET_STR_TO_CHAR_FROM_STR: {
            int dotpos       = UserText.findindex(".");
            int somethingpos = UserText.findindex("-");
            if (dotpos == -1 || somethingpos == -1) { std::cerr << "[error] set str to char from str: missing '.' or '-'\n"; break; }
            std::string toedit  = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string orginal = UserText.getrange(dotpos + 1, somethingpos - 1);
            if (string.find(orginal) == string.end()) { std::cerr << "[error] set str to char from str: source string '" << orginal << "' not found\n"; break; }
            try {
                int index = std::stoi(UserText.from(somethingpos + 1));
                if (index < 0 || index >= (int)string[orginal].size()) {
                    std::cerr << "[error] set str to char from str: index " << index << " out of bounds\n"; break;
                }
                string[toedit] = string[orginal][index];
            } catch (...) {
                std::cerr << "[error] set str to char from str: invalid index\n";
            }
            break;
        }

        // ---- input --------------------------------------------------------
        case Cmd::SET_INT_TO_INPUT: {
            std::string valname = UserText.from(commandpos + 1);
            int newusval;
            if (std::cin >> newusval) {
                integer[valname] = newusval;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                std::cerr << "[error] set int to input: invalid integer entered\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            break;
        }
        case Cmd::SET_STR_TO_INPUT: {
            std::string varname = UserText.from(commandpos + 1);
            std::string userinput;
            std::getline(std::cin, userinput);
            string[varname] = userinput;
            break;
        }

        // ---- functions / control flow ------------------------------------
        case Cmd::EXECUTE: {
            std::string funcName = UserText.from(commandpos + 1);
            runBlock(funcName);
            break;
        }
        case Cmd::EXECUTE2: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] execute2: missing '.'\n"; break; }
            std::string funcName = UserText.getrange(commandpos + 1, dotpos - 1);
            runBlock(funcName);
            break;
        }
        case Cmd::REPEAT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] repeat: missing '.'\n"; break; }
            int repeattimes = clean_stoi(UserText.getrange(commandpos + 1, dotpos - 1));
            if (repeattimes < 0) { std::cerr << "[error] repeat: negative repeat count\n"; break; }
            std::string commandname = UserText.from(dotpos + 1);
            if (function.find(commandname) == function.end()) { std::cerr << "[error] repeat: function '" << commandname << "' not defined\n"; break; }
            for (int i = 0; i < repeattimes; i++)
                runBlock(commandname);
            break;
        }
        case Cmd::WHILE_INT_LESS: {
            int dotpos  = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            if (dotpos == -1 || funcpos == -1) { std::cerr << "[error] while int less: missing '.' or '!'\n"; break; }
            std::string var1name = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string var2name = UserText.getrange(dotpos + 1, funcpos - 1);
            std::string funcName = UserText.from(funcpos + 1);
            if (function.find(funcName) == function.end()) { std::cerr << "[error] while int less: function '" << funcName << "' not defined\n"; break; }
            while (integer[var1name] < integer[var2name])
                runBlock(funcName);
            break;
        }
        case Cmd::WHILE_INT_MORE: {
            int dotpos  = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            if (dotpos == -1 || funcpos == -1) { std::cerr << "[error] while int more: missing '.' or '!'\n"; break; }
            std::string var1name = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string var2name = UserText.getrange(dotpos + 1, funcpos - 1);
            std::string funcName = UserText.from(funcpos + 1);
            if (function.find(funcName) == function.end()) { std::cerr << "[error] while int more: function '" << funcName << "' not defined\n"; break; }
            while (integer[var1name] > integer[var2name])
                runBlock(funcName);
            break;
        }
        case Cmd::IF_EQUAL: {
            int dotpos  = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            if (dotpos == -1 || funcpos == -1) { std::cerr << "[error] if equal: missing '.' or '!'\n"; break; }
            int var1 = clean_stoi(UserText.getrange(commandpos + 1, dotpos - 1));
            int var2 = clean_stoi(UserText.getrange(dotpos + 1, funcpos - 1));
            std::string funccall = UserText.from(funcpos + 1);
            if (var1 == var2) runBlock(funccall);
            break;
        }
        case Cmd::IF_INT_EQUAL: {
            int dotpos  = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            if (dotpos == -1 || funcpos == -1) { std::cerr << "[error] if int equal: missing '.' or '!'\n"; break; }
            std::string var1 = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string var2 = UserText.getrange(dotpos + 1, funcpos - 1);
            std::string funccall = UserText.from(funcpos + 1);
            if (integer[var1] == integer[var2]) runBlock(funccall);
            break;
        }
        case Cmd::IF_INT_LESS: {
            int dotpos  = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            if (dotpos == -1 || funcpos == -1) { std::cerr << "[error] if int less: missing '.' or '!'\n"; break; }
            std::string var1 = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string var2 = UserText.getrange(dotpos + 1, funcpos - 1);
            std::string funccall = UserText.from(funcpos + 1);
            if (integer[var1] < integer[var2]) runBlock(funccall);
            break;
        }
        case Cmd::IF_INT_MORE: {
            int dotpos  = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            if (dotpos == -1 || funcpos == -1) { std::cerr << "[error] if int more: missing '.' or '!'\n"; break; }
            std::string var1 = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string var2 = UserText.getrange(dotpos + 1, funcpos - 1);
            std::string funccall = UserText.from(funcpos + 1);
            if (integer[var1] > integer[var2]) runBlock(funccall);
            break;
        }
        case Cmd::IF_STR_EQUAL: {
            int dotpos  = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            int elsepos = UserText.findindex("E");
            if (dotpos == -1 || funcpos == -1 || elsepos == -1) { std::cerr << "[error] if str equal: missing '.', '!', or 'E'\n"; break; }
            std::string var1     = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string var2     = UserText.getrange(dotpos + 1, funcpos - 1);
            std::string funccall = UserText.getrange(funcpos + 1, elsepos - 1);
            std::string elsecall = UserText.from(elsepos + 1);
            if (string[var1] == string[var2]) runBlock(funccall);
            else                              runBlock(elsecall);
            break;
        }
        case Cmd::IF_STR_NOT_EQUAL: {
            int dotpos  = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            int elsepos = UserText.findindex("E");
            if (dotpos == -1 || funcpos == -1 || elsepos == -1) { std::cerr << "[error] if str not equal: missing '.', '!', or 'E'\n"; break; }
            std::string var1     = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string var2     = UserText.getrange(dotpos + 1, funcpos - 1);
            std::string funccall = UserText.getrange(funcpos + 1, elsepos - 1);
            std::string elsecall = UserText.from(elsepos + 1);
            if (string[var1] != string[var2]) runBlock(funccall);
            else                              runBlock(elsecall);
            break;
        }

        // ---- lists --------------------------------------------------------
        case Cmd::MAKE_INT_LIST: {
            std::string vecname = UserText.from(commandpos + 1);
            int_lists[vecname]; // default-init
            break;
        }
        case Cmd::ADD_TO_INT_LIST: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] add to int list: missing '.'\n"; break; }
            std::string valname = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                int toadd = std::stoi(UserText.from(dotpos + 1));
                int_lists[valname].push_back(toadd);
            } catch (std::invalid_argument&) {
                std::cerr << "[error] add to int list: value is not a valid integer\n";
            }
            break;
        }
        case Cmd::SET_INT_TO_INT_FROM_LIST: {
            int dotpos       = UserText.findindex(".");
            int somethingpos = UserText.findindex("-");
            if (dotpos == -1 || somethingpos == -1) { std::cerr << "[error] set int to int from list: missing '.' or '-'\n"; break; }
            std::string valname  = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string listname = UserText.getrange(dotpos + 1, somethingpos - 1);
            try {
                int index = std::stoi(UserText.from(somethingpos + 1));
                if (int_lists.find(listname) == int_lists.end()) { std::cerr << "[error] set int to int from list: list '" << listname << "' not found\n"; break; }
                if (!checkIndex(int_lists[listname], index, listname)) break;
                integer[valname] = int_lists[listname][index];
            } catch (...) {
                std::cerr << "[error] set int to int from list: invalid index\n";
            }
            break;
        }
        case Cmd::REMOVE_FROM_INT_LIST: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] remove from int list: missing '.'\n"; break; }
            std::string listname = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                int toremove = std::stoi(UserText.from(dotpos + 1));
                if (int_lists.find(listname) == int_lists.end()) { std::cerr << "[error] remove from int list: list '" << listname << "' not found\n"; break; }
                if (!checkIndex(int_lists[listname], toremove, listname)) break;
                int_lists[listname].erase(int_lists[listname].begin() + toremove);
            } catch (...) {
                std::cerr << "[error] remove from int list: invalid index\n";
            }
            break;
        }
        case Cmd::MAKE_STR_LIST: {
            std::string vecname = UserText.from(commandpos + 1);
            str_lists[vecname];
            break;
        }
        case Cmd::ADD_TO_STR_LIST: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] add to str list: missing '.'\n"; break; }
            std::string listname = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string toadd    = UserText.from(dotpos + 1);
            str_lists[listname].push_back(toadd);
            break;
        }
        case Cmd::SET_STR_FROM_LIST: {
            int dotpos       = UserText.findindex(".");
            int somethingpos = UserText.findindex("-");
            if (dotpos == -1 || somethingpos == -1) { std::cerr << "[error] set str to str from list: missing '.' or '-'\n"; break; }
            std::string varname  = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string listname = UserText.getrange(dotpos + 1, somethingpos - 1);
            try {
                int index = std::stoi(UserText.from(somethingpos + 1));
                if (str_lists.find(listname) == str_lists.end()) { std::cerr << "[error] set str to str from list: list '" << listname << "' not found\n"; break; }
                if (!checkIndex(str_lists[listname], index, listname)) break;
                string[varname] = str_lists[listname][index];
            } catch (...) {
                std::cerr << "[error] set str to str from list: invalid index\n";
            }
            break;
        }
        case Cmd::SET_STR_FROM_LIST_BY_INT: {
            int dotpos       = UserText.findindex(".");
            int somethingpos = UserText.findindex("-");
            if (dotpos == -1 || somethingpos == -1) { std::cerr << "[error] set str to str from list by int: missing '.' or '-'\n"; break; }
            std::string varname  = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string listname = UserText.getrange(dotpos + 1, somethingpos - 1);
            std::string intname  = UserText.from(somethingpos + 1);
            if (integer.find(intname) == integer.end()) { std::cerr << "[error] set str to str from list by int: index variable '" << intname << "' not found\n"; break; }
            int index = integer[intname];
            if (str_lists.find(listname) == str_lists.end()) { std::cerr << "[error] set str to str from list by int: list '" << listname << "' not found\n"; break; }
            if (!checkIndex(str_lists[listname], index, listname)) break;
            string[varname] = str_lists[listname][index];
            break;
        }
        case Cmd::REMOVE_FROM_STR_LIST: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] remove from str list: missing '.'\n"; break; }
            std::string listname = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                int toremove = std::stoi(UserText.from(dotpos + 1));
                if (str_lists.find(listname) == str_lists.end()) { std::cerr << "[error] remove from str list: list '" << listname << "' not found\n"; break; }
                if (!checkIndex(str_lists[listname], toremove, listname)) break;
                str_lists[listname].erase(str_lists[listname].begin() + toremove);
            } catch (...) {
                std::cerr << "[error] remove from str list: invalid index\n";
            }
            break;
        }
        case Cmd::PRINT_STR_LIST_ITEM: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] print str list item: missing '.'\n"; break; }
            std::string listname = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                int index = std::stoi(UserText.from(dotpos + 1));
                if (str_lists.find(listname) == str_lists.end()) { std::cerr << "[error] print str list item: list '" << listname << "' not found\n"; break; }
                if (!checkIndex(str_lists[listname], index, listname)) break;
                std::cout << str_lists[listname][index];
            } catch (...) {
                std::cerr << "[error] print str list item: invalid index\n";
            }
            break;
        }

        // ---- file I/O -----------------------------------------------------
        case Cmd::APPEND_FILE: {
            int dotpos = UserText.findindex("-");
            if (dotpos == -1) { std::cerr << "[error] append file: missing '-'\n"; break; }
            std::string filename = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string to_add   = UserText.from(dotpos + 1);
            std::ofstream file(filename, std::ios::app);
            if (!file.is_open()) std::cerr << "[error] append file: could not open '" << filename << "'\n";
            else file << to_add;
            break;
        }
        case Cmd::ADD_NEWLINE_TO_FILE: {
            std::string filename = UserText.from(commandpos + 1);
            std::ofstream file(filename, std::ios::app);
            if (!file.is_open()) std::cerr << "[error] add newline to file: could not open '" << filename << "'\n";
            else file << "\n";
            break;
        }
        case Cmd::STORE_FILE_IN_LIST: {
            int dotpos = UserText.findindex("-");
            if (dotpos == -1) { std::cerr << "[error] store file in list: missing '-'\n"; break; }
            std::string filename = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string listname = UserText.from(dotpos + 1);
            std::ifstream file(filename);
            if (!file.is_open()) { std::cerr << "[error] store file in list: could not open '" << filename << "'\n"; break; }
            std::string currentline;
            while (std::getline(file, currentline))
                str_lists[listname].push_back(currentline);
            break;
        }
        case Cmd::OVERWRITE_FILE_WITH_LIST: {
            int dotpos = UserText.findindex("-");
            if (dotpos == -1) { std::cerr << "[error] overwrite file with list: missing '-'\n"; break; }
            std::string filename = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string listname = UserText.from(dotpos + 1);
            if (str_lists.find(listname) == str_lists.end()) { std::cerr << "[error] overwrite file with list: list '" << listname << "' not found\n"; break; }
            std::ofstream file(filename);
            if (!file.is_open()) { std::cerr << "[error] overwrite file with list: could not open '" << filename << "'\n"; break; }
            for (const auto& cl : str_lists[listname])
                file << cl << '\n';
            break;
        }

        // ---- structs ------------------------------------------------------
        case Cmd::CREATE_STRUCT: {
            std::string structname = UserText.from(commandpos + 1);
            structs[structname];
            break;
        }
        case Cmd::ADD_INT_VAR_TO_STRUCT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] add int var to struct: missing '.'\n"; break; }
            std::string structname = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string intname    = UserText.from(dotpos + 1);
            if (integer.find(intname) == integer.end()) { std::cerr << "[error] add int var to struct: integer '" << intname << "' not found\n"; break; }
            structs[structname].push_back(integer[intname]);
            break;
        }
        case Cmd::SET_INT_TO_STRUCT_ELEMENT: {
            int dotpos       = UserText.findindex(".");
            int somethingpos = UserText.findindex("-");
            if (dotpos == -1 || somethingpos == -1) { std::cerr << "[error] set int to struct element: missing '.' or '-'\n"; break; }
            std::string intname    = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string structname = UserText.getrange(dotpos + 1, somethingpos - 1);
            try {
                int element = std::stoi(UserText.from(somethingpos + 1));
                if (structs.find(structname) == structs.end()) { std::cerr << "[error] set int to struct element: struct '" << structname << "' not found\n"; break; }
                if (!checkIndex(structs[structname], element, structname)) break;
                integer[intname] = std::get<int>(structs[structname][element]);
            } catch (std::bad_variant_access&) {
                std::cerr << "[error] set int to struct element: slot is not an integer\n";
            } catch (...) {
                std::cerr << "[error] set int to struct element: invalid element index\n";
            }
            break;
        }
        case Cmd::SET_STRUCT_SLOT_TO_INT: {
            int dotpos  = UserText.findindex(".");
            int dashpos = UserText.findindex("-");
            if (dotpos == -1 || dashpos == -1) { std::cerr << "[error] set struct slot to int: missing '.' or '-'\n"; break; }
            std::string structname = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                int slotindex = std::stoi(UserText.getrange(dotpos + 1, dashpos - 1));
                int value     = std::stoi(UserText.from(dashpos + 1));
                growStruct(structs[structname], slotindex);
                structs[structname][slotindex] = value;
            } catch (...) {
                std::cerr << "[error] set struct slot to int: invalid slot or value\n";
            }
            break;
        }
        case Cmd::SET_STRUCT_SLOT_TO_INT_VAR: {
            int dotpos  = UserText.findindex(".");
            int dashpos = UserText.findindex("-");
            if (dotpos == -1 || dashpos == -1) { std::cerr << "[error] set struct slot to int var: missing '.' or '-'\n"; break; }
            std::string structname = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                int slotindex = std::stoi(UserText.getrange(dotpos + 1, dashpos - 1));
                std::string varname = UserText.from(dashpos + 1);
                if (integer.find(varname) == integer.end()) { std::cerr << "[error] set struct slot to int var: variable '" << varname << "' not found\n"; break; }
                growStruct(structs[structname], slotindex);
                structs[structname][slotindex] = integer[varname];
            } catch (...) {
                std::cerr << "[error] set struct slot to int var: invalid slot index\n";
            }
            break;
        }
        case Cmd::ADD_INT_TO_STRUCT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] add int to struct: missing '.'\n"; break; }
            std::string structname = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                int value = std::stoi(UserText.from(dotpos + 1));
                structs[structname].push_back(value);
            } catch (...) {
                std::cerr << "[error] add int to struct: value is not a valid integer\n";
            }
            break;
        }
        case Cmd::ADD_STR_VAR_TO_STRUCT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] add str var to struct: missing '.'\n"; break; }
            std::string structname = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string strname    = UserText.from(dotpos + 1);
            if (string.find(strname) == string.end()) { std::cerr << "[error] add str var to struct: string '" << strname << "' not found\n"; break; }
            structs[structname].push_back(string[strname]);
            break;
        }
        case Cmd::ADD_STR_TO_STRUCT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] add str to struct: missing '.'\n"; break; }
            std::string structname = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string value      = UserText.from(dotpos + 1);
            structs[structname].push_back(value);
            break;
        }
        case Cmd::SET_STRUCT_SLOT_TO_STR_VAR: {
            int dotpos  = UserText.findindex(".");
            int dashpos = UserText.findindex("-");
            if (dotpos == -1 || dashpos == -1) { std::cerr << "[error] set struct slot to str var: missing '.' or '-'\n"; break; }
            std::string structname = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                int slotindex = std::stoi(UserText.getrange(dotpos + 1, dashpos - 1));
                std::string varname = UserText.from(dashpos + 1);
                if (string.find(varname) == string.end()) { std::cerr << "[error] set struct slot to str var: string '" << varname << "' not found\n"; break; }
                growStruct(structs[structname], slotindex);
                structs[structname][slotindex] = string[varname];
            } catch (...) {
                std::cerr << "[error] set struct slot to str var: invalid slot index\n";
            }
            break;
        }
        case Cmd::SET_STRUCT_SLOT_TO_STR: {
            int dotpos  = UserText.findindex(".");
            int dashpos = UserText.findindex("-");
            if (dotpos == -1 || dashpos == -1) { std::cerr << "[error] set struct slot to str: missing '.' or '-'\n"; break; }
            std::string structname = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                int slotindex = std::stoi(UserText.getrange(dotpos + 1, dashpos - 1));
                std::string value = UserText.from(dashpos + 1);
                growStruct(structs[structname], slotindex);
                structs[structname][slotindex] = value;
            } catch (...) {
                std::cerr << "[error] set struct slot to str: invalid slot index\n";
            }
            break;
        }
        case Cmd::SET_STR_TO_STRUCT_ELEMENT: {
            int dotpos  = UserText.findindex(".");
            int dashpos = UserText.findindex("-");
            if (dotpos == -1 || dashpos == -1) { std::cerr << "[error] set str to struct element: missing '.' or '-'\n"; break; }
            std::string strname    = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string structname = UserText.getrange(dotpos + 1, dashpos - 1);
            try {
                int element = std::stoi(UserText.from(dashpos + 1));
                if (structs.find(structname) == structs.end()) { std::cerr << "[error] set str to struct element: struct '" << structname << "' not found\n"; break; }
                if (!checkIndex(structs[structname], element, structname)) break;
                string[strname] = std::get<std::string>(structs[structname][element]);
            } catch (std::bad_variant_access&) {
                std::cerr << "[error] set str to struct element: slot is not a string\n";
            } catch (...) {
                std::cerr << "[error] set str to struct element: invalid element index\n";
            }
            break;
        }
        case Cmd::PRINT_STRUCT_ELEMENT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] print struct element: missing '.'\n"; break; }
            std::string structname = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                int element = std::stoi(UserText.from(dotpos + 1));
                if (structs.find(structname) == structs.end()) { std::cerr << "[error] print struct element: struct '" << structname << "' not found\n"; break; }
                if (!checkIndex(structs[structname], element, structname)) break;
                Multitype& slot = structs[structname][element];
                if      (std::holds_alternative<int>(slot))         std::cout << std::get<int>(slot);
                else if (std::holds_alternative<std::string>(slot)) std::cout << std::get<std::string>(slot);
                else if (std::holds_alternative<double>(slot))      std::cout << std::get<double>(slot);
            } catch (...) {
                std::cerr << "[error] print struct element: invalid element index\n";
            }
            break;
        }
        case Cmd::PRINT_STRUCT: {
            std::string structname = UserText.from(commandpos + 1);
            if (structs.find(structname) == structs.end()) { std::cerr << "[error] print struct: struct '" << structname << "' not found\n"; break; }
            for (size_t si = 0; si < structs[structname].size(); si++) {
                std::cout << "[" << si << "] ";
                Multitype& slot = structs[structname][si];
                if      (std::holds_alternative<int>(slot))         std::cout << std::get<int>(slot);
                else if (std::holds_alternative<std::string>(slot)) std::cout << std::get<std::string>(slot);
                else if (std::holds_alternative<double>(slot))      std::cout << std::get<double>(slot);
                std::cout << "\n";
            }
            break;
        }
        case Cmd::STRUCT_SIZE_TO_INT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] struct size to int: missing '.'\n"; break; }
            std::string varname    = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string structname = UserText.from(dotpos + 1);
            if (structs.find(structname) == structs.end()) { std::cerr << "[error] struct size to int: struct '" << structname << "' not found\n"; break; }
            integer[varname] = (int)structs[structname].size();
            break;
        }
        case Cmd::REMOVE_STRUCT_SLOT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] remove struct slot: missing '.'\n"; break; }
            std::string structname = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                int slotindex = std::stoi(UserText.from(dotpos + 1));
                if (structs.find(structname) == structs.end()) { std::cerr << "[error] remove struct slot: struct '" << structname << "' not found\n"; break; }
                if (!checkIndex(structs[structname], slotindex, structname)) break;
                structs[structname].erase(structs[structname].begin() + slotindex);
            } catch (...) {
                std::cerr << "[error] remove struct slot: invalid slot index\n";
            }
            break;
        }
        case Cmd::CLEAR_STRUCT: {
            std::string structname = UserText.from(commandpos + 1);
            structs[structname].clear();
            break;
        }
        case Cmd::COPY_STRUCT: {
            int dotpos = UserText.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] copy struct: missing '.'\n"; break; }
            std::string src  = UserText.getrange(commandpos + 1, dotpos - 1);
            std::string dest = UserText.from(dotpos + 1);
            if (structs.find(src) == structs.end()) { std::cerr << "[error] copy struct: source struct '" << src << "' not found\n"; break; }
            structs[dest] = structs[src];
            break;
        }
        case Cmd::IF_STRUCT_SLOT_IS_INT: {
            int dotpos  = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            if (dotpos == -1 || funcpos == -1) { std::cerr << "[error] if struct slot is int: missing '.' or '!'\n"; break; }
            std::string structname = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                int slotindex = std::stoi(UserText.getrange(dotpos + 1, funcpos - 1));
                std::string funcName = UserText.from(funcpos + 1);
                if (!checkIndex(structs[structname], slotindex, structname)) break;
                if (std::holds_alternative<int>(structs[structname][slotindex]))
                    runBlock(funcName);
            } catch (...) {
                std::cerr << "[error] if struct slot is int: invalid slot index\n";
            }
            break;
        }
        case Cmd::IF_STRUCT_SLOT_IS_STR: {
            int dotpos  = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            if (dotpos == -1 || funcpos == -1) { std::cerr << "[error] if struct slot is str: missing '.' or '!'\n"; break; }
            std::string structname = UserText.getrange(commandpos + 1, dotpos - 1);
            try {
                int slotindex = std::stoi(UserText.getrange(dotpos + 1, funcpos - 1));
                std::string funcName = UserText.from(funcpos + 1);
                if (!checkIndex(structs[structname], slotindex, structname)) break;
                if (std::holds_alternative<std::string>(structs[structname][slotindex]))
                    runBlock(funcName);
            } catch (...) {
                std::cerr << "[error] if struct slot is str: invalid slot index\n";
            }
            break;
        }

        // ---- timer --------------------------------------------------------
        case Cmd::START_TIMER:
            timerStart = std::chrono::high_resolution_clock::now();
            break;
        case Cmd::END_TIMER: {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - timerStart;
            std::cout << " took: " << elapsed.count() << " ms\n";
            break;
        }

        // ---- misc ---------------------------------------------------------
        case Cmd::PAUSE: {
            int pauseamount = clean_stoi(UserText.from(commandpos + 1));
            if (pauseamount < 0) { std::cerr << "[error] pause: negative pause amount\n"; break; }
            std::this_thread::sleep_for(std::chrono::seconds(pauseamount));
            break;
        }
        case Cmd::GIVE_ME_SECRET: {
            std::ofstream suprise("suprise.txt");
            if (!suprise.is_open()) { std::cerr << "[error] give me secret: could not create suprise.txt\n"; break; }
            suprise << "here is your secret: https://www.youtube.com/watch?v=dQw4w9WgXcQ";
            break;
        }
        case Cmd::RUST_IS_BETTER: {
            std::ofstream NEVER("NEVER.txt");
            if (!NEVER.is_open()) { std::cerr << "[error] Rust is better than c++: could not create NEVER.txt\n"; break; }
            NEVER << "you are just coping because no one uses rust, c++ is king";
            break;
        }
        case Cmd::FINISH:
            std::exit(0);
        case Cmd::CLEAR_SCREEN:
#ifdef _WIN32
            std::system("cls");
#else
            std::system("clear");
#endif
            break;

        // ---- library import (handled in main loop but kept here for safety)
        case Cmd::IMPORT:
            // Imports are resolved during the pre-pass in the main loop below.
            // If one somehow reaches processLine, just silently skip it.
            break;

        // ---- unknown command ----------------------------------------------
        case Cmd::UNKNOWN:
        default:
            std::cerr << "[error] unknown command: '" << beforecoma.get() << "'\n";
            break;
        }
    };

    // ---- Main execution loop ---------------------------------------------
    // Handles "save/end" blocks specially, and resolves "import" statements.
    size_t i = 0;
    while (i < allLines.size()) {
        std::string line = allLines[i];
        easystring el    = line;
        int cp           = el.findindex(",");
        easystring before = el.getrange(0, cp - 1);

        if (before.get() == "save") {
            // --- multi-line function save ---
            if (cp == -1) { std::cerr << "[error] save: missing ','\n"; i++; continue; }
            int dotpos = el.findindex(".");
            if (dotpos == -1) { std::cerr << "[error] save: missing '.'\n"; i++; continue; }
            std::string funcname  = el.getrange(cp + 1, dotpos - 1);
            std::string firstLine = el.from(dotpos + 1);
            std::string body      = "";
            if (!trim(firstLine).empty())
                body += firstLine;
            i++;
            while (i < allLines.size() && trim(allLines[i]) != "end") {
                if (!body.empty()) body += "\n";
                body += allLines[i];
                i++;
            }
            if (i >= allLines.size())
                std::cerr << "[warning] save '" << funcname << "': reached end of file without 'end'\n";
            function[funcname] = body;

        } else if (before.get() == "import") {
            // --- library import ---
            if (cp == -1) { std::cerr << "[error] import: missing ','\n"; i++; continue; }
            std::string libname = trim(el.from(cp + 1));
            loadLibrary(libname, i);
            // loadLibrary inserts the lib lines right after position i,
            // so we just advance and the new lines will be processed next.

        } else {
            processLine(line, processLine);
        }
        i++;
    }

    return 0;
}
// hi :)

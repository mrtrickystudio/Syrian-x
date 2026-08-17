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


std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

int clean_stoi(const std::string& input, int fallback_value = 0) {
    std::string cleaned = "";
    for (char c : input) {
        if (std::isdigit(c) || (cleaned.empty() && (c == '-' || c == '+'))) {
            cleaned += c;
        }
    }
    if (cleaned.empty() || cleaned == "-" || cleaned == "+") {
        return fallback_value;
    }
    try {
        return std::stoi(cleaned);
    } catch (const std::exception&) {
        return fallback_value;
    }
}

int main(){
	using Multitype = std::variant<int, double, std::string>;
    std::cout << "type file name for compiling:";
    easystring UserText;
    std::string userfile;
    std::string temp;
    std::unordered_map<std::string, std::string> string;
    std::unordered_map<std::string, std::string> function;
    std::unordered_map<std::string, int> integer;
    std::unordered_map<std::string, std::vector<int>> int_lists;
    std::unordered_map<std::string, std::vector<std::string>> str_lists;
    using Multitype = std::variant<int, double, std::string>;
    std::unordered_map<std::string, std::vector<Multitype>> structs;
    
    std::cin >> userfile;
    std::fstream script(userfile);
    
    std::chrono::high_resolution_clock::time_point start;
    std::cout << "dont forget to remove the timer";
    
    // Read ALL lines upfront so save blocks can consume multiple lines
    std::vector<std::string> allLines;
    while (std::getline(script, temp)) {
        allLines.push_back(temp);
    }

    auto processBlock = [&](const std::string& block, auto& self) -> void{};

    auto processLine = [&](std::string line, auto& self) -> void {
        UserText = line;
        int commandpos = UserText.findindex(",");
        easystring beforecoma = UserText.getrange(0, commandpos - 1);

        if(beforecoma.get() == "print"){
            std::string toprint = UserText.from(commandpos + 1);
            std::cout << toprint + "\n";
        }
        if(beforecoma.get() == "no new line print"){
            std::string toprint = UserText.from(commandpos + 1);
            std::cout << toprint;
        }
        if(beforecoma.get() == "int var"){
            int dotpos = UserText.findindex(".");
            std::string VarName = UserText.getrange(commandpos + 1, dotpos - 1).get();
            int VarValue = clean_stoi(UserText.from(dotpos + 1));
            integer[VarName] = VarValue;
        }
        if(beforecoma.get() == "print int"){
            std::string variable = UserText.from(commandpos + 1);
            std::cout << integer[variable];
        }
        if(beforecoma.get() == "print str"){
            std::string variable = UserText.from(commandpos + 1);
            std::cout << string[variable];
        }
        if(beforecoma.get() == "str var"){
            int dotpos = UserText.findindex(".");
            std::string strname = UserText.getrange(commandpos+1, dotpos-1);
            std::string value = UserText.from(dotpos+1);
            string[strname] = value;
        }
        if(beforecoma.get() == "start timer"){
			start = std::chrono::high_resolution_clock::now();
		}
		if(beforecoma.get() == "end timer"){
			
			auto end = std::chrono::high_resolution_clock::now();
			    
			std::chrono::duration<double, std::milli> elapsed = end - start;
			std::cout << " took: " << elapsed.count() << " ms\n";
			
		}
		if(beforecoma.get() == "add and set"){
		    int dotpos = UserText.findindex(".");
		    std::string destVar = UserText.getrange(commandpos + 1, dotpos - 1).get();
		    easystring remainder = UserText.from(dotpos + 1);
		    int total = 0;
		    int pluspos;
		    while((pluspos = remainder.findindex("+")) != -1){
		        std::string varName = remainder.getrange(0, pluspos - 1).get();
		        total += integer[varName];
		        remainder = remainder.from(pluspos + 1);
		    }
		    total += integer[remainder.get()];
		    integer[destVar] = total;
		}
        if(beforecoma.get() == "minus and set"){
            easystring afterFirstComma = UserText.from(commandpos + 1);
            int secondCommaPos = afterFirstComma.findindex(",");
            std::string destVar = afterFirstComma.getrange(0, secondCommaPos - 1).get();
            easystring remainder = afterFirstComma.from(secondCommaPos + 1);
            int firstMinusPos = remainder.findindex("-");
            int total = integer[remainder.getrange(0, firstMinusPos - 1).get()];
            remainder = remainder.from(firstMinusPos + 1);
            int minuspos;
            while((minuspos = remainder.findindex("-")) != -1){
                std::string varName = remainder.getrange(0, minuspos - 1).get();
                total -= integer[varName];
                remainder = remainder.from(minuspos + 1);
            }
            total -= integer[remainder.get()];
            integer[destVar] = total;
        }
        if(beforecoma.get() == "Change int"){
            int dotpos = UserText.findindex(".");
            std::string varaiblename = UserText.getrange(commandpos+1, dotpos-1);
            try{
            	int newvalue = std::stoi(UserText.from(dotpos+1));
				integer[varaiblename] = newvalue;
			} catch(std::invalid_argument){
				std::cout << "nuh uh, your so called integer has letters or symbols in it";
			}
        }
        if(beforecoma.get() == "finish"){
			std::exit(0);
		}
		if(beforecoma.get() == "clear screen"){
			std::system("cls"); 
		}
        if(beforecoma.get() == "Change int to int"){
            int dotpos = UserText.findindex(".");
            std::string varaiblename = UserText.getrange(commandpos+1, dotpos-1);
            std::string newvalue = UserText.from(dotpos+1);
            integer[varaiblename] = integer[newvalue];
        }
        if(beforecoma.get() == "set and randomgennum"){
            int dotpos = UserText.findindex(".");
            std::string VarName = UserText.getrange(commandpos + 1, dotpos - 1).get();
            int numberpos = UserText.findindex("-");
            int number1 = clean_stoi(UserText.getrange(commandpos, numberpos-1));
            int number2 = clean_stoi(UserText.from(numberpos+1));
            int VarValue = randomnumbergen(number1, number2);
            integer[VarName] = VarValue;
        }
        if(beforecoma.get() == "set var to sqrt"){
            std::string var = UserText.from(commandpos+1);
            int valueofinteger = integer[var];
            integer[var] = static_cast<int>(std::sqrt(valueofinteger));
        }

        // --- FIXED: execute now runs multi-line blocks line by line ---
        if(beforecoma.get() == "execute"){
            std::string funcName = UserText.from(commandpos+1);
            std::string block = function[funcName];
            std::istringstream ss(block);
            std::string subline;
            while(std::getline(ss, subline)){
                if(!subline.empty())
                    self(subline, self);
            }
        }
        if(beforecoma.get() == "execute2"){
            int dotpos = UserText.findindex(".");
            std::string funcName = UserText.getrange(commandpos+1, dotpos-1);
            std::string block = function[funcName];
            std::istringstream ss(block);
            std::string subline;
            while(std::getline(ss, subline)){
                if(!subline.empty())
                    self(subline, self);
            }
        }
        if(beforecoma.get() == "make int list"){
			std::string vecname = UserText.from(commandpos+1);
			int_lists[vecname];
		}
		if(beforecoma.get() == "add to int list"){
			int dotpos = UserText.findindex(".");
			try{
			std::string valname = UserText.getrange(commandpos+1,dotpos-1);
			int toadd = std::stoi(UserText.from(dotpos+1));
			int_lists[valname].push_back(toadd);
			} catch(std::invalid_argument){
				std::cout << "nuh uh your so called integer has letters/symbols in it";
			}
		}
		if(beforecoma.get() == "set int to int from list"){
			int dotpos = UserText.findindex(".");
			int somethingpos = UserText.findindex("-");
			std::string valname = UserText.getrange(commandpos+1,dotpos-1);
			std::string listname = UserText.getrange(dotpos+1,somethingpos-1);
			int index = std::stoi(UserText.from(somethingpos+1));
			
			integer[valname] = int_lists[listname][index];
		}
		if(beforecoma.get() == "remove from int list"){
			int dotpos = UserText.findindex(".");
			std::string listname = UserText.getrange(commandpos+1,dotpos-1);
			int toremove = std::stoi(UserText.from(dotpos+1));
			int_lists[listname].erase(int_lists[listname].begin() + toremove);
		}

        // --- FIXED: repeat also works with multi-line blocks now ---
        if(beforecoma.get() == "repeat"){
            int dotpos = UserText.findindex(".");
            int repeattimes = clean_stoi(UserText.getrange(commandpos+1, dotpos-1));
            std::string commandname = UserText.from(dotpos+1);
            std::string block = function[commandname];
            for(int i = 0; i < repeattimes; i++){
                std::istringstream ss(block);
                std::string subline;
                while(std::getline(ss, subline)){
                    if(!subline.empty())
                        self(subline, self);
                }
            }
        }

        // --- NEW: while int less (loop while var1 < var2) ---
        // syntax: while int less,var1.var2!funcname
        if(beforecoma.get() == "while int less"){
            int dotpos = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            std::string var1name = UserText.getrange(commandpos+1, dotpos-1);
            std::string var2name = UserText.getrange(dotpos+1, funcpos-1);
            std::string funcName = UserText.from(funcpos+1);
            while(integer[var1name] < integer[var2name]){
                std::string block = function[funcName];
                std::istringstream ss(block);
                std::string subline;
                while(std::getline(ss, subline)){
                    if(!subline.empty())
                        self(subline, self);
                }
            }
        }

        // --- NEW: while int more (loop while var1 > var2) ---
        // syntax: while int more,var1.var2!funcname
        if(beforecoma.get() == "while int more"){
            int dotpos = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            std::string var1name = UserText.getrange(commandpos+1, dotpos-1);
            std::string var2name = UserText.getrange(dotpos+1, funcpos-1);
            std::string funcName = UserText.from(funcpos+1);
            while(integer[var1name] > integer[var2name]){
                std::string block = function[funcName];
                std::istringstream ss(block);
                std::string subline;
                while(std::getline(ss, subline)){
                    if(!subline.empty())
                        self(subline, self);
                }
            }
        }

        if(beforecoma.get() == "if equal"){
            int dotpos = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            int var1 = clean_stoi(UserText.getrange(commandpos+1, dotpos-1));
            int var2 = clean_stoi(UserText.getrange(dotpos+1, funcpos-1));
            std::string funccall = UserText.from(funcpos+1);
            if(var1 == var2){
                std::string block = function[funccall];
                std::istringstream ss(block);
                std::string subline;
                while(std::getline(ss, subline)){
                    if(!subline.empty())
                        self(subline, self);
                }
            }
        }
        if(beforecoma.get() == "if int equal"){
            int dotpos = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            std::string var1 = UserText.getrange(commandpos+1, dotpos-1);
            std::string var2 = UserText.getrange(dotpos+1, funcpos-1);
            std::string funccall = UserText.from(funcpos+1);
            if(integer[var1] == integer[var2]){
                std::string block = function[funccall];
                std::istringstream ss(block);
                std::string subline;
                while(std::getline(ss, subline)){
                    if(!subline.empty())
                        self(subline, self);
                }
            }
        }
        if(beforecoma.get() == "if int less"){
            int dotpos = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            std::string var1 = UserText.getrange(commandpos+1, dotpos-1);
            std::string var2 = UserText.getrange(dotpos+1, funcpos-1);
            std::string funccall = UserText.from(funcpos+1);
            if(integer[var1] < integer[var2]){
                std::string block = function[funccall];
                std::istringstream ss(block);
                std::string subline;
                while(std::getline(ss, subline)){
                    if(!subline.empty())
                        self(subline, self);
                }
            }
        }
        if(beforecoma.get() == "if int more"){
            int dotpos = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            std::string var1 = UserText.getrange(commandpos+1, dotpos-1);
            std::string var2 = UserText.getrange(dotpos+1, funcpos-1);
            std::string funccall = UserText.from(funcpos+1);
            if(integer[var1] > integer[var2]){
                std::string block = function[funccall];
                std::istringstream ss(block);
                std::string subline;
                while(std::getline(ss, subline)){
                    if(!subline.empty())
                        self(subline, self);
                }
            }
        }
        if(beforecoma.get() == "if str equal"){
            int dotpos = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            int elsepos = UserText.findindex("E");
            std::string var1 = UserText.getrange(commandpos+1, dotpos-1);
            std::string var2 = UserText.getrange(dotpos+1, funcpos-1);
            std::string funccall = UserText.getrange(funcpos+1,elsepos-1);
            std::string elsecall = UserText.from(elsepos+1);
            if(string[var1] == string[var2]){
                std::string block = function[funccall];
                std::istringstream ss(block);
                std::string subline;
                while(std::getline(ss, subline)){
                    if(!subline.empty())
                        self(subline, self);
                }
            } else{
                std::string block = function[elsecall];
                std::istringstream ss(block);
                std::string subline;
                while(std::getline(ss, subline)){
                    if(!subline.empty())
                        self(subline, self);
                }
			}
        }
        if(beforecoma.get() == "if str not equal"){
            int dotpos = UserText.findindex(".");
            int funcpos = UserText.findindex("!");
            int elsepos = UserText.findindex("E");
            std::string var1 = UserText.getrange(commandpos+1, dotpos-1);
            std::string var2 = UserText.getrange(dotpos+1, funcpos-1);
            std::string funccall = UserText.getrange(funcpos+1,elsepos-1);
            std::string elsecall = UserText.from(elsepos+1);
            if(string[var1] != string[var2]){
                std::string block = function[funccall];
                std::istringstream ss(block);
                std::string subline;
                while(std::getline(ss, subline)){
                    if(!subline.empty())
                        self(subline, self);
                }
            } else{
                std::string block = function[elsecall];
                std::istringstream ss(block);
                std::string subline;
                while(std::getline(ss, subline)){
                    if(!subline.empty())
                        self(subline, self);
                }
			}
        }
        if(beforecoma.get() == "pause"){
            int pauseamount = clean_stoi(UserText.from(commandpos+1));
            std::this_thread::sleep_for(std::chrono::seconds(pauseamount));
        }
        if(beforecoma.get() == "give me secret"){
            std::ofstream suprise("suprise.txt");
            suprise << "here is your secret:https://www.youtube.com/watch?v=dQw4w9WgXcQ&list=RDdQw4w9WgXcQ&start_radio=1";
        }
        if(beforecoma.get() == "set int to int"){
			int dotpos = UserText.findindex(".");
			std::string var1 = UserText.getrange(commandpos+1,dotpos-1);
			std::string var2 = UserText.from(dotpos+1);
			integer[var1] = integer[var2];
		}
        if(beforecoma.get() == "set int"){
			int dotpos = UserText.findindex(".");
			std::string var1 = UserText.getrange(commandpos+1,dotpos-1);
			int newvar = std::stoi(UserText.from(dotpos+1));
			integer[var1] = newvar;
		}
		if(beforecoma.get() == "Rust is better than c++"){
			std::ofstream NEVER("NEVER.txt");
			NEVER << "you are just coping becuase no one uses rust c++ is king";
		}
		if(beforecoma.get() == "inc"){
			std::string varname = UserText.from(commandpos+1);
			int varval = integer[varname];
			integer[varname] =varval + 1;
		}
		if(beforecoma.get() == "dec"){
			std::string varname = UserText.from(commandpos+1);
			int varval = integer[varname];
			integer[varname] = varval - 1;
		}
		if(beforecoma.get() == "str to int"){
			int dotpos = UserText.findindex(".");
			std::string val1 = UserText.getrange(commandpos+1,dotpos-1);
			//std::string val2 = UserText.from(dotpos+1);
			try{
				int number = std::stoi(string[val1]);
				integer[val1] = number;
				string[val1].erase();
			} catch(const std::invalid_argument& e){
				std::cout << "nuh uh, your so called integer has letters or symbols in it"; //oh wow my first actual error messages!
			} catch(const std::out_of_range& e){
				std::cout << "your number wich is" << val1 << "is bigger than the 32 bit integer limit";
			}
		}
		if(beforecoma.get() == "set int to input"){
			std::string valname = UserText.from(commandpos+1);
			int newusval;
			if(std::cin >> newusval){
				integer[valname] = newusval;
			} else{std::cout << "it seem like you tried entering an invalid character....";}
		}
        if(beforecoma.get() == "Change str"){
            int dotpos = UserText.findindex(".");
            std::string varname = UserText.getrange(commandpos+1, dotpos-1);
            std::string newvalue = UserText.from(dotpos+1);
            string[varname] = newvalue;
        }
        if(beforecoma.get() == "Change str to str"){
            int dotpos = UserText.findindex(".");
            std::string var1 = UserText.getrange(commandpos+1, dotpos-1);
            std::string var2 = UserText.from(dotpos+1);
            string[var1] = string[var2];
        }
        if(beforecoma.get() == "set str to input"){
            std::string varname = UserText.from(commandpos+1);
            std::string userinput;
            std::getline(std::cin, userinput);
            string[varname] = userinput;
        }
        if(beforecoma.get() == "str to upper"){
            std::string varname = UserText.from(commandpos+1);
            for(char& c : string[varname]) c = std::toupper(c);
        }
        if(beforecoma.get() == "str to lower"){
            std::string varname = UserText.from(commandpos+1);
            for(char& c : string[varname]) c = std::tolower(c);
        }
        if(beforecoma.get() == "append str by var"){
            int dotpos = UserText.findindex(".");
            std::string var1 = UserText.getrange(commandpos+1, dotpos-1);
            std::string var2 = UserText.from(dotpos+1);
            string[var1] += string[var2];
        }
        if(beforecoma.get() == "append str"){
            int dotpos = UserText.findindex(".");
            std::string var1 = UserText.getrange(commandpos+1, dotpos-1);
            std::string var2 = UserText.from(dotpos+1);
            string[var1] += var2;
        }
        if(beforecoma.get() == "int to str"){
            std::string varname = UserText.from(commandpos+1);
            string[varname] = std::to_string(integer[varname]);
        }
        if(beforecoma.get() == "multiply and set"){
		            int dotpos = UserText.findindex(".");
		            easystring afterComma = UserText.from(commandpos+1);
		            int timespos = afterComma.findindex("*");
		            std::string var1 = afterComma.getrange(0, timespos-1).get();
		            easystring remainder = afterComma.from(timespos+1);
		            int total = integer[var1];
		            int mpos;
		            while((mpos = remainder.findindex("*")) != -1){
		                std::string varName = remainder.getrange(0, mpos-1).get();
		                total *= integer[varName];
		                remainder = remainder.from(mpos+1);
		            }
		            total *= integer[remainder.get()];
		            // result goes into destVar parsed before the first *
		            std::string destVar = UserText.getrange(commandpos+1, dotpos-1).get();
		            integer[destVar] = total;
		        }
		        if(beforecoma.get() == "divide and set"){
		            easystring afterFirstComma = UserText.from(commandpos+1);
		            int secondCommaPos = afterFirstComma.findindex(",");
		            std::string destVar = afterFirstComma.getrange(0, secondCommaPos-1).get();
		            easystring remainder = afterFirstComma.from(secondCommaPos+1);
		            int slashpos = remainder.findindex("/");
		            int total = integer[remainder.getrange(0, slashpos-1).get()];
		            remainder = remainder.from(slashpos+1);
		            int dpos;
		            while((dpos = remainder.findindex("/")) != -1){
		                std::string varName = remainder.getrange(0, dpos-1).get();
		                if(integer[varName] == 0){ std::cout << "cant divide by zero\n"; return; }
		                total /= integer[varName];
		                remainder = remainder.from(dpos+1);
		            }
		            if(integer[remainder.get()] == 0){ std::cout << "cant divide by zero\n"; return; }
		            total /= integer[remainder.get()];
		            integer[destVar] = total;
		        }
		        if(beforecoma.get() == "make str list"){
				            std::string vecname = UserText.from(commandpos+1);
				            str_lists[vecname];
				        }
				        if(beforecoma.get() == "add to str list"){
				            int dotpos = UserText.findindex(".");
				            std::string listname = UserText.getrange(commandpos+1, dotpos-1);
				            std::string toadd = UserText.from(dotpos+1);
				            str_lists[listname].push_back(toadd);
				        }
				        if(beforecoma.get() == "set str to str from list"){
				            int dotpos = UserText.findindex(".");
				            int somethingpos = UserText.findindex("-");
				            std::string varname = UserText.getrange(commandpos+1, dotpos-1);
				            std::string listname = UserText.getrange(dotpos+1, somethingpos-1);
				            int index = std::stoi(UserText.from(somethingpos+1));
				            string[varname] = str_lists[listname][index];
				        }
				        if(beforecoma.get() == "set str to str from list by int"){
				            int dotpos = UserText.findindex(".");
				            int somethingpos = UserText.findindex("-");
				            std::string varname = UserText.getrange(commandpos+1, dotpos-1);
				            std::string listname = UserText.getrange(dotpos+1, somethingpos-1);
				            std::string intname = UserText.from(somethingpos+1);
				            int index = integer[intname];
				            string[varname] = str_lists[listname][index];
				        }
				        if(beforecoma.get() == "set str to char from str"){
				            int dotpos = UserText.findindex(".");
				            int somethingpos = UserText.findindex("-");
				            std::string toedit = UserText.getrange(commandpos+1, dotpos-1);
				            std::string orginal = UserText.getrange(dotpos+1, somethingpos-1);
				            int index = std::stoi(UserText.from(somethingpos+1));
							char letter = string[orginal][index];
							string[toedit] = letter;	
						}
						if(beforecoma.get() == "mudolo int and set"){
							int dotpos = UserText.findindex(".");
							int somethingpos = UserText.findindex("-");
							std::string target = UserText.getrange(commandpos+1,dotpos-1);
							std::string var1 = UserText.getrange(dotpos+1,somethingpos-1);
							std::string var2 = UserText.from(somethingpos+1);
							integer[target] = integer[var1] % integer[var2];
						}
						
				        if(beforecoma.get() == "set int to str length"){
							int dotpos = UserText.findindex(".");
							std::string integername = UserText.getrange(commandpos+1,dotpos-1);
							std::string stringname = UserText.from(dotpos+1);
							integer[integername] = string[stringname].length();
						}
				        if(beforecoma.get() == "remove from str list"){
				            int dotpos = UserText.findindex(".");
				            std::string listname = UserText.getrange(commandpos+1, dotpos-1);
				            int toremove = std::stoi(UserText.from(dotpos+1));
				            str_lists[listname].erase(str_lists[listname].begin() + toremove);
				        }
				        if(beforecoma.get() == "print str list item"){
				            int dotpos = UserText.findindex(".");
				            int somethingpos = UserText.findindex("-");
				            std::string listname = UserText.getrange(commandpos+1, dotpos-1);
				            int index = std::stoi(UserText.from(dotpos+1));
				            std::cout << str_lists[listname][index];
				        }
				        if(beforecoma.get() == "append file"){
				        	int dotpos = UserText.findindex("-");
				        	std::string filename = UserText.getrange(commandpos+1,dotpos-1);
				        	std::string to_add = UserText.from(dotpos+1);
							std::ofstream file(filename, std::ios::app);
							file << to_add;
						}
				        if(beforecoma.get() == "add newline to file"){
				        	std::string filename = UserText.from(commandpos+1);
							std::ofstream file(filename, std::ios::app);
							file << "\n";
						}
						if(beforecoma.get() == "store file in list"){
							int dotpos = UserText.findindex("-");
							std::string filename = UserText.getrange(commandpos+1,dotpos-1);
							std::string listname = UserText.from(dotpos+1);
							std::string currentline;
							std::ifstream file(filename);
							while (std::getline(file, currentline)) {
							        str_lists[listname].push_back(currentline);
							    }
						}
						if(beforecoma.get() == "overwrite file with list"){
							int dotpos = UserText.findindex("-");
							std::string filename = UserText.getrange(commandpos+1,dotpos-1);
							std::string listname = UserText.from(dotpos+1);
							std::string currentline;
							
							std::ofstream file(filename);
							
							for (const auto& currentline : str_lists[listname]) {
							        file << currentline << '\n';
							    }
						}
						if(beforecoma.get() == "create struct"){
							std::string structname = UserText.from(commandpos+1);
							structs[structname];
						}
						if(beforecoma.get() == "add int var to struct"){
							int dotpos = UserText.findindex(".");
							std::string structname = UserText.getrange(commandpos+1,dotpos-1);
							std::string intname = UserText.from(dotpos+1);
							structs[structname].push_back(integer[intname]);
						}
						if(beforecoma.get() == "set int to struct element"){
							int dotpos = UserText.findindex(".");
							int somethingpos = UserText.findindex("-");
							std::string intname = UserText.getrange(commandpos+1,dotpos-1);
							std::string structname = UserText.getrange(dotpos+1,somethingpos-1);
							int element = std::stoi(UserText.from(somethingpos+1));
							integer[intname] = std::get<int>(structs[structname][element]);
						}
						

// ============================================================
// STRUCT ADDITIONS - paste these inside processLine, right
// before the closing   };   (the one at line ~559)
// ============================================================

// --- set a struct slot to a plain integer literal ---
// syntax: set struct slot to int,structname.slotindex-value
// example: set struct slot to int,player.0-42
if(beforecoma.get() == "set struct slot to int"){
    int dotpos    = UserText.findindex(".");
    int dashpos   = UserText.findindex("-");
    std::string structname = UserText.getrange(commandpos+1, dotpos-1);
    int slotindex = std::stoi(UserText.getrange(dotpos+1, dashpos-1));
    int value     = std::stoi(UserText.from(dashpos+1));
    if(slotindex < (int)structs[structname].size()){
        structs[structname][slotindex] = value;
    } else {
        // grow the struct to fit if needed
        while((int)structs[structname].size() <= slotindex)
            structs[structname].push_back(0);
        structs[structname][slotindex] = value;
    }
}

// --- set a struct slot to an integer variable ---
// syntax: set struct slot to int var,structname.slotindex-varname
// example: set struct slot to int var,player.0-score
if(beforecoma.get() == "set struct slot to int var"){
    int dotpos    = UserText.findindex(".");
    int dashpos   = UserText.findindex("-");
    std::string structname = UserText.getrange(commandpos+1, dotpos-1);
    int slotindex = std::stoi(UserText.getrange(dotpos+1, dashpos-1));
    std::string varname = UserText.from(dashpos+1);
    if(slotindex < (int)structs[structname].size()){
        structs[structname][slotindex] = integer[varname];
    } else {
        while((int)structs[structname].size() <= slotindex)
            structs[structname].push_back(0);
        structs[structname][slotindex] = integer[varname];
    }
}

// --- add a plain integer literal to struct (no variable needed) ---
// syntax: add int to struct,structname.value
// example: add int to struct,player.100
if(beforecoma.get() == "add int to struct"){
    int dotpos = UserText.findindex(".");
    std::string structname = UserText.getrange(commandpos+1, dotpos-1);
    int value = std::stoi(UserText.from(dotpos+1));
    structs[structname].push_back(value);
}

// --- add a string variable to struct ---
// syntax: add str var to struct,structname.varname
// example: add str var to struct,player.username
if(beforecoma.get() == "add str var to struct"){
    int dotpos = UserText.findindex(".");
    std::string structname = UserText.getrange(commandpos+1, dotpos-1);
    std::string strname = UserText.from(dotpos+1);
    structs[structname].push_back(string[strname]);
}

// --- add a plain string literal to struct ---
// syntax: add str to struct,structname.hello world
// example: add str to struct,player.unnamed
if(beforecoma.get() == "add str to struct"){
    int dotpos = UserText.findindex(".");
    std::string structname = UserText.getrange(commandpos+1, dotpos-1);
    std::string value = UserText.from(dotpos+1);
    structs[structname].push_back(value);
}

// --- set a struct slot to a string variable ---
// syntax: set struct slot to str var,structname.slotindex-varname
// example: set struct slot to str var,player.1-username
if(beforecoma.get() == "set struct slot to str var"){
    int dotpos  = UserText.findindex(".");
    int dashpos = UserText.findindex("-");
    std::string structname = UserText.getrange(commandpos+1, dotpos-1);
    int slotindex = std::stoi(UserText.getrange(dotpos+1, dashpos-1));
    std::string varname = UserText.from(dashpos+1);
    if(slotindex < (int)structs[structname].size()){
        structs[structname][slotindex] = string[varname];
    } else {
        while((int)structs[structname].size() <= slotindex)
            structs[structname].push_back(std::string(""));
        structs[structname][slotindex] = string[varname];
    }
}

// --- set a struct slot to a plain string literal ---
// syntax: set struct slot to str,structname.slotindex-hello world
// example: set struct slot to str,player.1-unnamed
if(beforecoma.get() == "set struct slot to str"){
    int dotpos  = UserText.findindex(".");
    int dashpos = UserText.findindex("-");
    std::string structname = UserText.getrange(commandpos+1, dotpos-1);
    int slotindex = std::stoi(UserText.getrange(dotpos+1, dashpos-1));
    std::string value = UserText.from(dashpos+1);
    if(slotindex < (int)structs[structname].size()){
        structs[structname][slotindex] = value;
    } else {
        while((int)structs[structname].size() <= slotindex)
            structs[structname].push_back(std::string(""));
        structs[structname][slotindex] = value;
    }
}

// --- get string element out of a struct into a string variable ---
// syntax: set str to struct element,varname.structname-slotindex
// example: set str to struct element,mystr.player-1
if(beforecoma.get() == "set str to struct element"){
    int dotpos  = UserText.findindex(".");
    int dashpos = UserText.findindex("-");
    std::string strname    = UserText.getrange(commandpos+1, dotpos-1);
    std::string structname = UserText.getrange(dotpos+1, dashpos-1);
    int element = std::stoi(UserText.from(dashpos+1));
    string[strname] = std::get<std::string>(structs[structname][element]);
}


if(beforecoma.get() == "print struct element"){
    int dotpos = UserText.findindex(".");
    std::string structname = UserText.getrange(commandpos+1, dotpos-1);
    int element = std::stoi(UserText.from(dotpos+1));
    Multitype& slot = structs[structname][element];
    if(std::holds_alternative<int>(slot))
        std::cout << std::get<int>(slot);
    else if(std::holds_alternative<std::string>(slot))
        std::cout << std::get<std::string>(slot);
    else if(std::holds_alternative<double>(slot))
        std::cout << std::get<double>(slot);
}


if(beforecoma.get() == "print struct"){
    std::string structname = UserText.from(commandpos+1);
    for(size_t si = 0; si < structs[structname].size(); si++){
        std::cout << "[" << si << "] ";
        Multitype& slot = structs[structname][si];
        if(std::holds_alternative<int>(slot))
            std::cout << std::get<int>(slot);
        else if(std::holds_alternative<std::string>(slot))
            std::cout << std::get<std::string>(slot);
        else if(std::holds_alternative<double>(slot))
            std::cout << std::get<double>(slot);
        std::cout << "\n";
    }
}


if(beforecoma.get() == "struct size to int"){
    int dotpos = UserText.findindex(".");
    std::string varname    = UserText.getrange(commandpos+1, dotpos-1);
    std::string structname = UserText.from(dotpos+1);
    integer[varname] = (int)structs[structname].size();
}


if(beforecoma.get() == "remove struct slot"){
    int dotpos = UserText.findindex(".");
    std::string structname = UserText.getrange(commandpos+1, dotpos-1);
    int slotindex = std::stoi(UserText.from(dotpos+1));
    if(slotindex < (int)structs[structname].size())
        structs[structname].erase(structs[structname].begin() + slotindex);
}


if(beforecoma.get() == "clear struct"){
    std::string structname = UserText.from(commandpos+1);
    structs[structname].clear();
}

//big copy
if(beforecoma.get() == "copy struct"){
    int dotpos = UserText.findindex(".");
    std::string src  = UserText.getrange(commandpos+1, dotpos-1);
    std::string dest = UserText.from(dotpos+1);
    structs[dest] = structs[src];
}


if(beforecoma.get() == "if struct slot is int"){
    int dotpos  = UserText.findindex(".");
    int funcpos = UserText.findindex("!");
    std::string structname = UserText.getrange(commandpos+1, dotpos-1);
    int slotindex = std::stoi(UserText.getrange(dotpos+1, funcpos-1));
    std::string funcName = UserText.from(funcpos+1);
    if(std::holds_alternative<int>(structs[structname][slotindex])){
        std::string block = function[funcName];
        std::istringstream ss(block);
        std::string subline;
        while(std::getline(ss, subline)){
            if(!subline.empty()) self(subline, self);
        }
    }
}

if(beforecoma.get() == "if struct slot is str"){
    int dotpos  = UserText.findindex(".");
    int funcpos = UserText.findindex("!");
    std::string structname = UserText.getrange(commandpos+1, dotpos-1);
    int slotindex = std::stoi(UserText.getrange(dotpos+1, funcpos-1));
    std::string funcName = UserText.from(funcpos+1);
    if(std::holds_alternative<std::string>(structs[structname][slotindex])){
        std::string block = function[funcName];
        std::istringstream ss(block);
        std::string subline;
        while(std::getline(ss, subline)){
            if(!subline.empty()) self(subline, self);
        }
    }
}


		        
    };

    // Main execution loop ? handles save/end blocks specially
    size_t i = 0;
    while(i < allLines.size()){
        std::string line = allLines[i];
        easystring el = line;
        int cp = el.findindex(",");
        easystring before = el.getrange(0, cp - 1);

        if(before.get() == "save"){
            int dotpos = el.findindex(".");
            std::string funcname = el.getrange(cp+1, dotpos-1);
            std::string firstLine = el.from(dotpos+1); 
            std::string body = "";
            if(!trim(firstLine).empty()){
                body += firstLine;
            }
            i++;
            while(i < allLines.size() && trim(allLines[i]) != "end"){
                if(!body.empty()) body += "\n";
                body += allLines[i];
                i++;
            }
            // i now points at "end", skip it
            function[funcname] = body;
        } else {
            processLine(line, processLine);
        }
        i++;
    }
}
// hi :)

// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


#include <fstream>
#include <iomanip>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>
#include "emulator/kyiv.h"
#include "emulator/asm_disasm.h"

#define COMMAND_SIZE 4

std::map <std::string, std::string> en_instructions = {
        {"add",  "01"},
        {"sub",  "02"},
        {"addcmd",  "03"},
        {"suba", "06"},
        {"addcyc",  "07"},
        {"mul",  "10"},
        {"rmul", "11"},
        {"div",  "12"},
        {"sh",  "13"},
        {"and",  "15"},
        {"or",  "14"},
        {"xor",  "17"},
        {"norm", "35"},
        {"jle",  "04"},
        {"jlea",  "05"},
        {"je", "16"},
        {"nfork",  "30"},
        {"ncall",  "31"},
        {"ret", "32"},
        {"gob",  "26"},
        {"goe",  "27"},
        {"fop", "34"},
        {"rdt",  "20"},
        {"rbn",  "21"},
        {"wbn", "22"},
        {"wmd",  "23"},
        {"rmd",  "24"},
        {"imd", "25"},
        {"ostanov",  "33"}
};

std::map <std::string, std::string> ua_instructions = {
        {"дод",  "01"},
        {"від",  "02"},
        {"обк",  "03"},
        {"авід", "06"},
        {"цдод",  "07"},
        {"множ",  "10"},
        {"замнож", "11"},
        {"діл",  "12"},
        {"зсв",  "13"},
        {"і",  "15"},
        {"або",  "14"},
        {"вабо",  "17"},
        {"норм", "35"},
        {"кмр",  "04"},
        {"кмра",  "05"},
        {"кр", "16"},
        {"упп",  "30"},
        {"упч",  "31"},
        {"прп", "32"},
        {"пго",  "26"},
        {"кго",  "27"},
        {"фікс", "34"},
        {"чдн",  "20"},
        {"чбн",  "21"},
        {"дру", "22"},
        {"мбз",  "23"},
        {"мбч",  "24"},
        {"мбп", "25"},
        {"останов",  "33"}
};

//int disassembly(const uint64_t & command_oct, Kyiv_memory & kmem) {
//
////    std::cout << command_oct << std::endl;
//    std::ostringstream str;
//    str << std::oct << command_oct;
//    std::string command = str.str();
//
//    if (command.size() != 13 && command.size() != 14) {
//        return -1;
//    }
//    if (command.size() != 14) {
//        command.insert(0, "0");
//    }
//    std::string result;
//    for (const auto & it : en_instructions) {
//        if ( it.second == command.substr(0, 2) )
//            result.append(it.first);
//    }
//    if (result.empty()) {
//        for (const auto & it : ua_instructions) {
//            if ( it.second == command.substr(0, 2) )
//                result.append(it.first);
//        }
//    }
//    if (result.empty()) {
//        assert("Shouldn't be here!");
//    }
//    result.append(" " + command.substr(2, 4) + " " + command.substr(6, 4) + " " + command.substr(10, 4));
//    word_t Addr_1_mask_shift = (40-6-11)+1;
//    word_t Addr_1_mask = 0b11'111'111'111ULL << (Addr_1_mask_shift-1);
//    word_t Addr_2_mask_shift = (40-6-12-11)+1;
//    word_t Addr_2_mask = 0b11'111'111'111ULL << (Addr_2_mask_shift-1);
//    std::string val1 = std::to_string((word_to_number(kmem[(command_oct & Addr_1_mask) >> Addr_1_mask_shift]) ));  //* std::pow(2, -40)
//    std::string val2 = std::to_string(word_to_number(kmem[(command_oct & Addr_2_mask) >> Addr_2_mask_shift]));
//    result.append("\t;; " + val1 + " " + val2);
//    std::cout << result << std::endl;
//    return 0;
//}

int disassembly(const uint64_t & command_oct, Kyiv_memory_t & kmem, const addr3_t &addr3) {

//    std::cout << command_oct << std::endl;
    std::ostringstream str;
    str << std::oct << command_oct;
    std::string command = str.str();

    if (command.size() != 13 && command.size() != 14) {
        return -1;
    }
    if (command.size() != 14) {
        command.insert(0, "0");
    }
    std::string result;
    for (const auto & it : en_instructions) {
        if ( it.second == command.substr(0, 2) )
            result.append(it.first);
    }
    if (result.empty()) {
        for (const auto & it : ua_instructions) {
            if ( it.second == command.substr(0, 2) )
                result.append(it.first);
        }
    }
    if (result.empty()) {
        assert("Shouldn't be here!");
    }
    result.append(" " + command.substr(2, 4) + " " + command.substr(6, 4) + " " + command.substr(10, 4));
    word_t Addr_1_mask_shift = (40-6-11)+1;
    word_t Addr_1_mask = 0b11'111'111'111ULL << (Addr_1_mask_shift);
    word_t Addr_2_mask_shift = (40-6-12-11)+1;
    word_t Addr_2_mask = 0b11'111'111'111ULL << (Addr_2_mask_shift);
//    std::cout << "NUM : " << std::bitset<41>(command_oct) << std::endl;
//    std::cout << "TRUE : " << std::bitset<41>(0'11'0002'3067'0002ULL) << std::endl;
    std::string val1 = std::to_string((word_to_number(kmem.read_memory(addr3.source_1)) )) + "  " +
                       std::to_string(word_to_number(kmem.read_memory(addr3.source_1)) * std::pow(2, -40));  //* std::pow(2, -40)
    std::string val2 = std::to_string(word_to_number(kmem.read_memory(addr3.source_2))) + "  " +
                       std::to_string(word_to_number(kmem.read_memory(addr3.source_2)) * std::pow(2, -40) );
    result.append("\t;; " + val1 + "\t" + val2);
    std::cout << result << std::endl;
    addr_t a = (command_oct & Addr_2_mask) >> Addr_2_mask_shift;
//    std::cout << "TRUE : " << std::bitset<41>(a) << std::endl;
//    std::cout << "val2 : " << kmem.read_memory((command_oct & Addr_2_mask) >> Addr_2_mask_shift) << std::endl;
    return 0;
}

int disassembly_text(const std::string file_from, const std::string file_to) {
    word_t Addr_1_mask_shift = (40-6-11)+1;
    word_t Addr_1_mask = 0b11'111'111'111ULL << (Addr_1_mask_shift-1);
    word_t Addr_2_mask_shift = (40-6-12-11)+1;
    word_t Addr_2_mask = 0b11'111'111'111ULL << (Addr_2_mask_shift-1);
    word_t oct_command;
    std::map<std::string, std::string> program;
    std::map<std::string, std::string> jumps;
    size_t jump_counter = 0;

    std::ifstream infile(file_from);

    std::string line;

    while (std::getline(infile, line)) {
        line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
        std::ostringstream str;
        str << std::oct << line;
        std::string command = str.str();
        std::string address = command.substr(0, 4);
        command = command.substr(4);

        if (command.size() != 13 && command.size() != 14) {
            return -1;
        }
        if (command.size() != 14) {
            command.insert(0, "0");
        }
        std::string result;

        if (command.substr(0, 2) == "00") {
            program[address] = command;
            continue;
        }

        for (const auto &it: en_instructions) {
            if (it.second == command.substr(0, 2)) {
                result.append(it.first + " ");
                std::string addr_1 = command.substr(2, 4) + " ";
                std::string addr_2 = command.substr(6, 4) + " ";
                std::string addr_3 = command.substr(10, 4) + " ";
                std::string mod = mod_comment(addr_1, addr_2, addr_3);

                if (it.first == "jle" || it.first == "jlea" || it.first == "je" || it.first == "ncall" ||
                    it.first == "gob") {
                    std::string label = "jump" + std::to_string(jump_counter++);
                    jumps[addr_3.substr(0, 4)] = label + ":";

                    result.append(addr_1);
                    result.append(addr_2);
                    result.append(label + " ");
                } else if (it.first == "nfork" || it.first == "goe") {
                    std::string label_1 = "jump" + std::to_string(jump_counter++);
                    std::string label_2 = "jump" + std::to_string(jump_counter++);
                    jumps[addr_2.substr(0, 4)] = label_1 + ":";
                    jumps[addr_3.substr(0, 4)] = label_2 + ":";

                    result.append(addr_1);
                    result.append(label_1 + " ");
                    result.append(label_2 + " ");
                } else {
                    result.append(addr_1);
                    result.append(addr_2);
                    result.append(addr_3);
                }

                if (mod != ";") {
                    result.append("\t" + mod);
                }
            }
//                if (result.empty()) {
//                    for (const auto &it: ua_instructions) {
//                        if (it.second == command.substr(0, 2))
//                            result.append(it.first);
//                    }
//                }
            if (result.empty()) {
                assert("Shouldn't be here!");
            }
            program[address] = result;
        }
    }
    disassembler_second_pass(program, jumps, file_to);
    return 0;
}

int disassembler_second_pass(std::map<std::string, std::string> program, std::map<std::string, std::string> jumps, const std::string& file_to) {
    std::string prog_to_file = "";
    int last_address = std::stoi(program.begin()->first, 0, 8);
    int origin_counter = 1;
    prog_to_file.append("org0 " + program.begin()->first + "\n");

    for(const auto & it : program) {
        if(jumps.find(it.first) != jumps.end()) {
            prog_to_file.append(jumps.find(it.first)->second + "\n");
        }
        if (std::stoi(it.first, 0, 8) - last_address > 1) {
            prog_to_file.append("org" + std::to_string(origin_counter++) + " " + it.first + "\n");
        }
        last_address = std::stoi(it.first, 0, 8);   // not sure if I should initialize a new variable for this or leave it like that

        prog_to_file.append("\t" + it.second + "\n");
    }

    std::ofstream infile(file_to);
    if (!infile.is_open())
        return -1;
    infile << prog_to_file;
    infile.close();

    return 0;
}


int check_modification(std::string addr) {
    word_t mod_bit = 0b100'000'000'000ULL;
    int num = std::stoi(addr, 0, 8);

    if (num & mod_bit) {
        return num^mod_bit;
    }
    return -1;
}

std::string mod_comment(std::string addr_1, std::string addr_2, std::string addr_3) {
    std::string comment = ";";
    int check = check_modification(addr_1);

    if (check != -1) {
        std::ostringstream str;
        str << std::oct << check;
        std::string to_print_1 = str.str();
        comment.append(" Addr 1: " + to_print_1 + " + A; ");
    }

    check = check_modification(addr_2);

    if (check != -1) {
        std::ostringstream str;
        str << std::oct << check;
        std::string to_print_1 = str.str();
        comment.append(" Addr 2: " + to_print_1 + " + A; ");
    }

    check = check_modification(addr_3);

    if (check != -1) {
        std::ostringstream str;
        str << std::oct << check;
        std::string to_print_1 = str.str();
        comment.append(" Addr 3: " + to_print_1 + " + A; ");
    }
    return comment;
}

/*
 * Read file in assembly code, convert it into Kyiv code and write into output file.
 * If numerate set in true -- output file will contain commands numeration.
 * @param input filename as string, flag to indicate if we address commands as bool
 * @return 0 if success
 */
int Assembly::read_file(const std::string& filename, bool numerate=false) {
    references = {};
    readers = {};
    command_count = 0;
    org_counter = 0;
    numer = numerate;
    std::cout << "reading1" << std::endl;
    std::ifstream infile(filename);
    std::string line;
    std::vector<std::string> commands;
    std::cout << "reading2" << std::endl;

    while (std::getline(infile, line)) {
        std::cout << line << std::endl;

        if (line.find(';') != std::string::npos)
            line.erase(line.find_first_of(';')); // remove comment
        if (!line.empty() && find_special_bts(line) == 0) {
            commands.push_back(line);
            if (numer) {
                std::ostringstream oct;
                oct << std::setw(4) << std::setfill('0') << std::oct << command_count;
                lines_cout.insert(lines_cout.begin(), oct.str());
                command_count++;
            }
            end++;
        }
    }
    std::cout << "reading4" << std::endl;

    std::ostringstream oct;
    oct << std::setw(4) << std::setfill('0') << std::oct << end;
    std::cout << "reading5" << std::endl;

    references["org" + std::to_string(org_counter-1)] += oct.str() + "0000";
    infile.close();
    std::string res;
    std::cout << "reading6" << std::endl;

    for (auto & command : commands) {
        std::vector<std::string> argv;
        boost::split(argv,command,boost::is_any_of(" "), boost::algorithm::token_compress_off);
        if (references.find(argv[0]) != references.end()) {
            readers.push_back(references[argv[0]]);
        } else {
            if (assembly_command(command, res) == -2) {
                res += (numer) ? lines_cout.back()+ "\t" : "";
                res += command + "\n";
            }
            if (numer)
                lines_cout.pop_back();
        }
    }
    std::cout << "reading7" << std::endl;

    write_file(outputf, res);
    return 0;
}

/*
 * Check if we have any helper in line, such as origin, label, text or data section
 * @param input line as string
 * @return 0 if success
 */
int Assembly::find_special_bts(std::string &line) {
    std::vector<std::string> argv;
    boost::algorithm::trim(line);
    boost::split(argv,line,boost::is_any_of(" "), boost::algorithm::token_compress_off);
    if (argv[0] == ".text"){
        text = true;
        return 1;
    }
    else if (argv[0] == ".data") {
        text = false;
        return 1;
    } else if (argv[0] == "org") { // 20/21 start end 0000
        if (org_counter != 0) {
            std::ostringstream oct;
            oct << std::setw(4) << std::setfill('0') << std::oct << end;
            references["org" + std::to_string(org_counter-1)] += oct.str() + "0000";
        }
        end = std::stoi(argv[1], 0, 8) - 1;
        std::string com = text ? "21" : "20";
        com += argv[1];
        if (numer)
            command_count = std::stoi(argv[1]);
        references["org" + std::to_string(org_counter)] = com;
        line = "org" + std::to_string(org_counter++);
    } else if (!text && argv.size() == 2) {
        std::ostringstream oct;
        oct << std::setw(4) << std::setfill('0') << std::oct << end;
        references[argv[0]] = oct.str();
        line = argv[1];
    } else if (!text) {
        if (stol(argv[0]) == 1)
            line = "1099511627775";
        else {
            if (argv[0].find('d') != std::string::npos) {
                argv[0].pop_back();
                line = std::to_string((int64_t) (stol(argv[0])));     // in case user already converted
            }
            else
                line = std::to_string((int64_t) (stof(argv[0]) * std::pow(2, 40)));
        }

    } else if (text && argv.size() == 1) {
        std::ostringstream oct;
        oct << std::setw(4) << std::setfill('0') << std::oct << end;
        argv[0].pop_back();
        references[argv[0]] = oct.str();
        return 1;
    }
    return 0;
}

/*
 * Check if given line contains command, if yes - convert it into Kyiv code and write into result line
 * @param input line as string, result with Kyiv codes as string
 * @return 0 if success, -2 if contains data
 */
int Assembly::assembly_command(std::string& command, std::string &result) {
    std::vector<std::string> argv;
    boost::split(argv,command,boost::is_any_of(" "), boost::algorithm::token_compress_off);
    if (argv.size() != COMMAND_SIZE)
        return -2;
    for (uint8_t i = 1; i < COMMAND_SIZE; i++) {
        if (argv[i].find_first_not_of("01234567") != std::string::npos) {
            if (references.find(argv[i]) != references.end()) {
                std::cout << command << "\t" << argv[i] << std::endl;
                argv[i] = references.find(argv[i])->second;
            } else {
                return -1;
            }
        }
        if (argv[i].size() != COMMAND_SIZE)
            return -1;
    }
    if (en_instructions.find(argv[0]) != en_instructions.end())
        argv[0] = en_instructions[argv[0]];
    else if (ua_instructions.find(argv[0]) != ua_instructions.end())
        argv[0] = ua_instructions[argv[0]];
    else
        return -1;
    result += (numer && !lines_cout.empty()) ? lines_cout.back() + "\t": "";
    result += boost::algorithm::join(argv, "") + "\n";
    return 0;
}

/*
 * Write result to given file (punched tape, or perfocard)
 * @param file name as pointer to char array, result as string
 * @return 0 if success
 */
int Assembly::write_file(const char* filename, std::string &res) {
    std::ofstream infile(filename);
    if (!infile.is_open())
        return -1;
    infile << res;
    infile.close();
    return 0;
}

/*
 * Execute all commands that we convert from assembly into Kyiv codes
 * @param struct Kyiv_t, address from which we start execution
 * @return 0 if success
 */
void Assembly::execute() {
    machine.h = 0;
    std::cout << std::setprecision(10);
    std::cout << "asm" << std::endl;
    for (auto & i : readers) {
        std::cout << "asmloop" << std::endl;
        machine.kmem.write_memory(0001, stol(i, 0, 8));
        machine.C_reg = 1;
        machine.execute_opcode();
    }

//    machine.kmem.write_rom(03000, 219902325555); // 0.2 == 2h  549755813
//    machine.kmem.write_rom(03001, 0); // 0 == x0
//    machine.kmem.write_rom(03002, 1098412116148); // 0.999 == x_max
//    machine.kmem.write_rom(03003, 0'04'0000'0000'0101ULL); // start of program
//    machine.kmem.write_rom(03004, 0'00'0000'0004'0000ULL); // m
//    machine.kmem.write_rom(03005, 0'00'0000'0116'0000ULL); // a

//    machine.kmem.write_memory(00071, 0);
//    machine.kmem.write_memory(00101, 0'10'0104'4000'0122ULL);   // mul x*0.3 => 122
//    machine.kmem.write_memory(00102, 0'10'0122'3000'0122ULL);   // mul 122*0.2(2h) => 122
//    machine.kmem.write_memory(00103, 0'05'0000'0000'3634ULL);   // jlea 3643
//
//    machine.kmem.write_memory(00104, 1110406787451); // -radon
//
//    machine.kmem.write_memory(00130, 109951162777); // 0.1
//    machine.kmem.write_memory(00131, 54975581388); // 0.05
//    machine.kmem.write_memory(00065, 68719476748);
//    machine.kmem.write_memory(00115, 329853488332); // 0.3
//    machine.kmem.write_memory(00116, 4096); // a
//    machine.kmem.write_memory(00117, 230897441832); // y1  n    120 - y_i 121 - x_i 122 - 2*h*K
//    machine.kmem.write_memory(00075, 16384); // 4 0000

    machine.C_reg = 03600;
    machine.T_reg = false;

    std::cout << "\t 3000: " << ( word_to_number(machine.kmem.read_memory(03000)));
    std::cout << "\t 3001: " << ( word_to_number(machine.kmem.read_memory(03001)));
    std::cout << "\t 3002: " << ( word_to_number(machine.kmem.read_memory(03002)));
    std::cout << "\t 3003: " << ( word_to_number(machine.kmem.read_memory(03003)));
    std::cout << "\t 3004: " << ( word_to_number(machine.kmem.read_memory(03004)));
    std::cout << "\t 3005: " << ( word_to_number(machine.kmem.read_memory(03004)));

    while (machine.execute_opcode()) {
        std::cout << "\t X: " << ( word_to_number(machine.kmem.read_memory(00031))) * std::pow(2, -40);
        std::cout << "\t 117: " << ( word_to_number(machine.kmem.read_memory(00117))) * std::pow(2, -40);
        std::cout << "\t 120: " << ( word_to_number(machine.kmem.read_memory(00120))) * std::pow(2, -40);
        std::cout << "\t 121: " << ( word_to_number(machine.kmem.read_memory(00121))) * std::pow(2, -40);
        std::cout << "\t 122: " << ( word_to_number(machine.kmem.read_memory(00122))) * std::pow(2, -40) << std::endl;
    }
    machine.kmem.write_memory(0001, 0'01'0117'0000'0117ULL);
    machine.T_reg = false;
    machine.C_reg = 0001;
    machine.execute_opcode();
    std::cout << "RES: " << machine.kmem.read_memory(0002) << " - " << machine.kmem.read_memory(0002) * std::pow(2, -40) <<  std::endl;
    return;
};
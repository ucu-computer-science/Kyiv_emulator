// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


#include <fstream>
#include <iomanip>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>
#include "kyiv.h"
#include "asm_disasm.h"

#define COMMAND_SIZE 4

constexpr char outputf[] = "../punched_tape.txt";

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
        {"and",  "14"},
        {"or",  "15"},
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
        {"і",  "14"},
        {"або",  "15"},
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

int disassembly(const uint64_t & command_oct, Kyiv_memory & kmem) {

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
    word_t Addr_1_mask = 0b11'111'111'111ULL << (Addr_1_mask_shift-1);
    word_t Addr_2_mask_shift = (40-6-12-11)+1;
    word_t Addr_2_mask = 0b11'111'111'111ULL << (Addr_2_mask_shift-1);
    std::string val1 = std::to_string((word_to_number(kmem[(command_oct & Addr_1_mask) >> Addr_1_mask_shift]) ));  //* std::pow(2, -40)
    std::string val2 = std::to_string(word_to_number(kmem[(command_oct & Addr_2_mask) >> Addr_2_mask_shift]));
    result.append("\t;; " + val1 + " " + val2);
    std::cout << result << std::endl;
    return 0;
}

int assembly(std::string& command, std::string &result) {
    std::vector<std::string> argv;
    boost::split(argv,command,boost::is_any_of(" "), boost::algorithm::token_compress_off);
    if (argv.size() != COMMAND_SIZE)
        return -1;
    for (uint8_t i = 1; i < COMMAND_SIZE; i++) {
        if ((argv[i].size() != COMMAND_SIZE) || (argv[i].find_first_not_of("01234567") != std::string::npos))
            return -1;
    }
    if (en_instructions.find(argv[0]) != en_instructions.end())
        argv[0] = en_instructions[argv[0]];
    else if (ua_instructions.find(argv[0]) != ua_instructions.end())
        argv[0] = ua_instructions[argv[0]];
    else
        return -1;
    result += boost::algorithm::join(argv, "") + "\n";
    return 0;
}

int read_file(const std::string& filename, std::string &result) {
    std::ifstream infile(filename);
    std::string line;
    size_t count = 1;
    while (std::getline(infile, line)) { // Добавити для запису даних через тумблер
        if (assembly(line, result) < 0) {
            std::cerr << "Error in line " << count << ": " << line << std::endl;
            return -1;
        }
        count++;
    }
    infile.close();
    return 0;
}

int write_file(const char* filename, std::string &res) {
    std::ofstream infile(filename);
    if (!infile.is_open())
        return -1;
    infile << res;
    infile.close();
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Wrong arg" << std::endl;
        return -1;
    }
    std::string result;
    if (read_file(argv[1], result) < 0)
        return -1;
    write_file(outputf, result);

    Kyiv_t machine;
    // machine.kmem[0001] = 0'12'0016'0003'0016ULL;
    machine.kmem[0001] = 0'21'0002'0010'0000ULL;
    machine.kmem[0012] = 549755813888;
    machine.kmem[0013] = 16;
    // machine.kmem[0016] = 9;
    machine.C_reg = 1;

    std::cout << std::setprecision(15);

    while (machine.execute_opcode()) {
        std::cout << "\tRES: " << machine.kmem[0014] << " - " << machine.kmem[0014] * std::pow(2, -40) <<  std::endl;
    }
    std::cout << "RES: " << machine.kmem[0014] << " - " << machine.kmem[0014] * std::pow(2, -40) <<  std::endl;
    return 0;
}

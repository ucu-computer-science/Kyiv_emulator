#ifndef KYIVEMU_ASM_DISASM_H
#define KYIVEMU_ASM_DISASM_H
#include <QObject>

constexpr char outputf[] = "../punched_tape.txt";

int disassembly(const uint64_t & command_oct, Kyiv_memory_t & kmem, const addr3_t &addr3);
int check_modification(std::string addr);
int disassembly_text(const std::string file_from, const std::string file_to);
std::string mod_comment(std::string addr_1, std::string addr_2, std::string addr_3);
int disassembler_second_pass(std::map<std::string, std::string> program, std::map<std::string, std::string> jumps, const std::string& file_to);
int write_file(const char* filename, std::string &res);

class Assembly {
    /*
     * Class for reading and executing assembly code on Kyiv.
     */
private:
    Kyiv_t & machine;
    std::map<std::string, std::string> references;          // contains origins, labels
    std::vector<std::string> readers;                       // helper for executing final code on Kyiv
    std::vector<std::string> lines_cout;                    // Optional - saves command address
    size_t command_count;                                   // helper to save current command address
    size_t org_counter;                                     // helper to numerate origin
    size_t end;                                             // helper to find address of last command
    bool text;                                              // check if we input command or value
    bool numer;                                             // Optional - if we want to address commands

public:
    Assembly(Kyiv_t & machine_) : machine(machine_) {};
    int read_file(const std::string& filename, bool numerate);
    int find_special_bts(std::string &line);
    int assembly_command(std::string& command, std::string &result);
    static int write_file(const char* filename, std::string &res);
    void execute();
};

#endif //KYIVEMU_ASM_DISASM_H

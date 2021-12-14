#ifndef KYIVEMU_ASM_DISASM_H
#define KYIVEMU_ASM_DISASM_H

int disassembly(const uint64_t & command, Kyiv_memory_t & kmem);
int check_modification(std::string addr);
int disassembly_text(const std::string file_from, const std::string file_to);
std::string mod_comment(std::string addr_1, std::string addr_2, std::string addr_3);
int disassembler_second_pass(std::map<std::string, std::string> program, std::map<std::string, std::string> jumps, const std::string& file_to);
int write_file(const char* filename, std::string &res);
#endif //KYIVEMU_ASM_DISASM_H
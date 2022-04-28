// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <stdio.h>
#include <iostream>
#include <cstdint>
#include <cmath>
#include <cassert>
#include <fstream>
#include <regex>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/float128.hpp>
#include "emulator/kyiv.h"
#include "emulator/asm_disasm.h"
typedef uint64_t addr_t;
typedef uint64_t word_t;
typedef int64_t  signed_word_t;
typedef uint32_t opcode_t;
typedef boost::multiprecision::int128_t mul_word_t;


constexpr addr3_t word_to_addr3(word_t w){
    constexpr word_t Addr_1_mask_shift = (40-6-11)+1;
    constexpr word_t Addr_1_mask = 0b11'111'111'111ULL << (Addr_1_mask_shift);  // also was -1
    constexpr word_t Addr_2_mask_shift = (40-6-12-11)+1;
    constexpr word_t Addr_2_mask = 0b11'111'111'111ULL << (Addr_2_mask_shift);  // also was -1
    constexpr word_t Addr_3_mask_shift = 0; // Для одноманітності
    constexpr word_t Addr_3_mask = 0b11'111'111'111ULL;

    addr3_t res;
    res.source_1    = (w & Addr_1_mask) >> Addr_1_mask_shift;
    res.source_2    = (w & Addr_2_mask) >> Addr_2_mask_shift;
    res.destination = (w & Addr_3_mask);
    return res;
}

constexpr opcode_t word_to_opcode(word_t w) {
    constexpr word_t op_code_shift = (40-5)+1;
    constexpr word_t op_code_mask = 0b11'111ULL << (op_code_shift);
    opcode_t opcode = (w & op_code_mask) >> op_code_shift;
    return opcode;
}

static constexpr word_t mask_40_bits = (1ULL << 40) - 1; // 0b111...11 -- 40 1-bits, std::pow(2, 41) === 1 << 41
static constexpr word_t mask_41_bit = (1ULL << 40);      // 0b1000...00 -- 40 zeros after the 1


constexpr bool is_negative(word_t w){
    return w & mask_41_bit; // 0 - додатнє, не нуль -- від'ємне
}

constexpr word_t to_negative(word_t w){
    return w | mask_41_bit;
}

constexpr word_t to_positive(word_t w){
    return w & (~mask_41_bit);
}

constexpr uint16_t leftmost_one(word_t w){
    uint16_t ct = 0;
    while (w > 1) {
        ct++;
        w = w >> 1;
    }
    return ct;
}

//hz looks like kostyl but whatever
uint16_t leftmost_one(mul_word_t w){
    uint16_t ct = 0;
    while (w > 1) {
        ct++;
        w = w >> 1;
    }
    return ct;
}

signed_word_t get_absolute(word_t w){
    return static_cast<signed_word_t>(w & mask_40_bits);
}

signed_word_t word_to_number(word_t w){
    signed_word_t sign1 = (is_negative(w) ? -1 : 1);
    signed_word_t abs_val1 = get_absolute(w);
    return sign1 * abs_val1;
}

constexpr bool get_A1(word_t w){
    constexpr word_t A_1_mask = 1ULL << (40-5);
    return w & A_1_mask;
}

constexpr bool get_A2(word_t w){
    constexpr word_t A_2_mask = 1ULL << (40-5-12);
    return w & A_2_mask;
}

constexpr bool get_A3(word_t w){
    constexpr uint64_t A_3_mask = 1ULL << (40-5-24);
    return w & A_3_mask;
}

constexpr addr3_t shift_addr3_byA(addr3_t addr3, uint64_t offset, word_t w){
    if(get_A1(w))
        addr3.source_1 += offset;
    if(get_A2(w))
        addr3.source_2 += offset;
    if(get_A3(w))
        addr3.destination += offset;
    return  addr3;
}

int *decimal_to_octal(word_t decimal) {
    int octal[14];
    int i = 13;
    while (decimal != 0 && i >= 0) {
        octal[i] = decimal % 8;
        decimal = decimal / 8;
        i--;
    }
    for (i; i >= 0; i--) {

    }
    return octal;
}

opcode_t Kyiv_t::get_opcode(bool norm_work) {
    if (norm_work) {
        K_reg = kmem.read_memory(C_reg);
    }
    std::cout << K_reg << std::endl;
    opcode_t opcode = word_to_opcode(K_reg);
    if (opcode == 0 && norm_work) {
        if (!C_block) { ++C_reg; }
    }

    // fill in address register on signalization panel


    return opcode;
}

addr_t Kyiv_t::get_addr1() {
    addr3_t addr3 = word_to_addr3(K_reg);
    return addr3.source_1;
}

addr_t Kyiv_t::get_addr2() {
    addr3_t addr3 = word_to_addr3(K_reg);
    return addr3.source_2;
}

addr_t Kyiv_t::get_addr3() {
    addr3_t addr3 = word_to_addr3(K_reg);
    return addr3.destination;
}

//void Kyiv_t::fill_address(addr_t addr) {
//
//}

//! Returns: True -- continue, false -- ситуація останову.
bool Kyiv_t::execute_opcode(bool norm_work){
    std::cout << "T reg: " << T_reg << std::endl;
    QRadioButton* iva1 = signalization[2][29];
    QRadioButton* iva2 = signalization[2][31];
    QRadioButton* iva3 = signalization[2][34];

    QRadioButton *btn_st = signalization[6][32];
    btn_st -> setChecked(true);
    QTimer::singleShot(400,[btn_st]()->void{btn_st->setChecked(0);});

    std::cout << "normw" << norm_work << std::endl;

    if (norm_work) {
        iva1 -> setChecked(true);
        iva2 -> setChecked(true);
        iva3 -> setChecked(true);
        K_reg = kmem.read_memory(C_reg);
        QTimer::singleShot(400,[iva1]()->void{iva1->setChecked(false);});
        QTimer::singleShot(400,[iva2]()->void{iva2->setChecked(false);});
        QTimer::singleShot(400,[iva3]()->void{iva3->setChecked(false);});
        std::bitset<11> b_c_reg(C_reg);
        std::string c_reg_str = b_c_reg.to_string();
        for (size_t i = 0; i < 11; i++) {
            bool val = (c_reg_str[i] - '0');
            signalization[3][i+27]->setChecked(val);
        }
    } else {
        for (size_t i = 0; i < 11; i++) {
            signalization[3][i+27]->setChecked(0);
        }
    }
    std::cout << "T reg1: " << T_reg << std::endl;

    QRadioButton *btn_iva4 = signalization[2][36];
    btn_iva4 -> setChecked(true);
    QTimer::singleShot(400,[btn_iva4]()->void{btn_iva4->setChecked(false);});

    std::stringstream str_stream;
    str_stream << std::oct << K_reg;

    std::string command_str = str_stream.str();
    std::cout << "Command str: " << command_str << std::endl;
//    std::cout << command_str.size() << std::endl;
    std::cout << "T reg2: " << T_reg << std::endl;
    for (size_t i = 0; i < 41; i++) {
        signalization[4][i]->setChecked(false);
    }
//    std::cout << "Is it here" << std::endl;
    for (int i = command_str.size() - 1; i >= 0; i--) {
//        std::cout << "Is it here21" << std::endl;
        int digit = command_str[i] - '0';
        signalization[4][40 - 3*(command_str.size() - 1 - i)]->setChecked(digit % 2);
//        std::cout << "Is it here23" << std::endl;
        int next_digit = digit / 2;
        signalization[4][39 - 3*(command_str.size() - 1 - i)]->setChecked(next_digit % 2);
//        std::cout << "Is it here24" << std::endl;
        int index = 38 - 3*(command_str.size() - 1 - i);
//        std::cout << "ind: " << index << std::endl;
        if (index > 0) {
//            std::cout << "Is it here26" << std::endl;
            int third_digit = next_digit / 2;
            signalization[4][index]->setChecked(third_digit % 2);
        }
//        std::cout << "Is it here25" << std::endl;
    }
//    std::cout << "Is it here2" << std::endl;
    std::cout << "T reg3: " << T_reg << std::endl;
    QRadioButton *btn_vz4 = signalization[2][37];
    btn_vz4 -> setChecked(true);
    QTimer::singleShot(400,[btn_vz4]()->void{btn_vz4->setChecked(false);});

    std::cout << "RRRRRRRRR: " << K_reg << std::endl;

    opcode_t opcode = word_to_opcode(K_reg);
    std::cout << "T reg4: " << T_reg << std::endl;

    if (opcode == 0 && norm_work) {
        if (!C_block) { ++C_reg; }
    }
    if (opcode > 0 && opcode <= 29) {

        if (opcode <= 16) {
            QRadioButton *btn = signalization[2][opcode - 1];
            btn -> setChecked(true);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(false);});
        } else {
            QRadioButton *btn = signalization[3][opcode - 17];
            btn -> setChecked(true);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(false);});
        }
    }
    std::cout << "opcode: " << opcode << std::endl;
    addr3_t addr3 = word_to_addr3(K_reg); // Парі команд потрібна
    std::cout << "T reg5: " << T_reg << std::endl;
    if (((ostanov_state == 2 && K_reg == ostanovCommand) || (ostanov_state == 1 && addr3.destination == ostanovAdrr_t)) && !Ostanov_block) {
        std::cout << "T reg6: " << T_reg << std::endl;
        if (T_tr) {
            T_reg = true;
            if (norm_work && !C_block) { ++C_reg; }
        } else {
            if (norm_work && !C_block) { C_reg += 2; }
        }
    } else {

        std::cout << "A_reg_2: " << A_reg << std::endl;
        std::cout << "Cycle_reg" << Loop_reg << std::endl;
        addr3_t addr3_shifted = shift_addr3_byA(addr3, A_reg, K_reg); // Решта використовують цю змінну

        std::cout << "source 1: " << addr3_shifted.source_1 << std::endl;
        std::cout << "source 2: " << addr3_shifted.source_2 << std::endl;

        disassembly(K_reg, kmem, addr3_shifted);
        //! Ймовірно, потім це діло треба буде відрефакторити -- відчуваю, но де буде проблема - поки не знаю :+)
        switch (opcode) {

            //TODO: Тестував лише opcode_add !!! -- решта вважайте невірними, поки не буде тестів. opcode_div
            case arythm_operations_t::opcode_div:
                [[fallthrough]];
            case arythm_operations_t::opcode_norm:
                [[fallthrough]];
            case arythm_operations_t::opcode_add:
                [[fallthrough]];
            case arythm_operations_t::opcode_sub:
                [[fallthrough]];
            case arythm_operations_t::opcode_addcmd:
                [[fallthrough]];
            case arythm_operations_t::opcode_subabs:
                [[fallthrough]];
            case arythm_operations_t::opcode_mul:
                [[fallthrough]];
            case arythm_operations_t::opcode_addcyc:
                [[fallthrough]];
            case arythm_operations_t::opcode_mul_round:
                opcode_arythm(addr3_shifted, opcode, norm_work);
                break;
                //==========================================================================================================
            case flow_control_operations_t::opcode_jmp_less_or_equal:
                [[fallthrough]];
            case flow_control_operations_t::opcode_jmp_abs_less_or_equal:
                [[fallthrough]];
            case flow_control_operations_t::opcode_jmp_equal:
                [[fallthrough]];
            case flow_control_operations_t::opcode_fork_negative:
                [[fallthrough]];
            case flow_control_operations_t::opcode_call_negative:
                [[fallthrough]];
            case flow_control_operations_t::opcode_ret:
                [[fallthrough]];
            case flow_control_operations_t::opcode_group_op_begin:
                [[fallthrough]];
            case flow_control_operations_t::opcode_group_op_end:
                [[fallthrough]];
            case flow_control_operations_t::opcode_F:
                [[fallthrough]];
            case flow_control_operations_t::opcode_stop:
                opcode_flow_control(addr3_shifted, opcode, addr3, norm_work);
                break;
                //==========================================================================================================
            case logic_operations_t::opcode_log_shift: {
                //! TODO: Мені не повністю зрозуміло з обох книг, величина зсуву береться із RAM за адресою,
                //! чи закодована в команді? Швидше перше -- але перевірити!
                word_t shift = kmem.read_memory(
                        addr3_shifted.source_1); // Глушко-Ющенко, стор 12, сверджує: "на число разрядов,
                // равное абсолютной величине константы сдвига, размещаемой в шести младших разрядах ячейки а1"
                // 2^6 -- 64, тому решта бітів справді просто дадуть нуль на виході, але все рівно маскую, щоб
                // не було невизначеної поведінки С. Та й зразу знак викидаємо
                std::cout << "shift: " << shift << std::endl;
                shift &= 0b111'111;
                std::cout << "shift after some and: " << shift << std::endl;
                if (is_negative(kmem.read_memory(addr3_shifted.source_1))) {
                    kmem.write_memory(addr3_shifted.destination, kmem.read_memory(addr3_shifted.source_2) >> shift);
                } else {
                    kmem.write_memory(addr3_shifted.destination, kmem.read_memory(addr3_shifted.source_2) << shift);
                    kmem.write_memory(addr3_shifted.destination, kmem.read_memory(addr3_shifted.destination) &
                                                                 (mask_40_bits |
                                                                  mask_41_bit)); // Зануляємо зайві біти
                }
                if (norm_work && !C_block) { ++C_reg; }
            }
                break;
            case logic_operations_t::opcode_log_or: {
                kmem.write_memory(addr3_shifted.destination, kmem.read_memory(addr3_shifted.source_1) |
                                                             kmem.read_memory(addr3_shifted.source_2));
                if (norm_work && !C_block) { ++C_reg; }
            }
                break;
            case logic_operations_t::opcode_log_and: {

                kmem.write_memory(addr3_shifted.destination, kmem.read_memory(addr3_shifted.source_1) &
                                                             kmem.read_memory(addr3_shifted.source_2));

                if (norm_work && !C_block) { ++C_reg; }
            }
                break;
            case logic_operations_t::opcode_log_xor: {
                kmem.write_memory(addr3_shifted.destination, kmem.read_memory(addr3_shifted.source_1) ^
                                                             kmem.read_memory(addr3_shifted.source_2));
                if (norm_work && !C_block) { ++C_reg; }
            }
                break;
                //==========================================================================================================
            case IO_operations_t::opcode_read_perfo_data:{
                std::ifstream punch_cards;
                std::ifstream heads;
                std::string head;
                std::string line;
                std::string perfo;
                std::vector<std::string> argv;
                signed_word_t number;

                punch_cards.open("../punched_tape.txt");
                heads.open("../heads.txt");

                std::getline(heads, head);
//            int num = std::stoi(head);
                size_t num = h;
                int counter = 0;
                int num_counter = 0;
                bool flag = false;

                while(punch_cards){
                    std::getline(punch_cards, line);
                    if(counter == num){
                        perfo = line.substr(kmem.read_memory(addr3_shifted.destination), line.size());
                        boost::split(argv, perfo, boost::is_any_of(" "), boost::algorithm::token_compress_off);
                        for(auto & i : argv){
                            if(num_counter == addr3_shifted.source_2 - addr3_shifted.source_1){
                                flag = true;
                                break;
                            }
                            number = std::stoll(i);
                            if(number >= 0){
                                std::cout << (kmem.read_memory(addr3_shifted.source_1 + num_counter)) << " num " << number << std::endl;
                                kmem.write_memory(addr3_shifted.source_1 + num_counter, number);
                                std::cout << (kmem.read_memory(addr3_shifted.source_1 + num_counter)) << std::endl;
                            }else{
                                kmem.write_memory(addr3_shifted.source_1 + num_counter, to_negative(std::abs(number)));
                            }
                            num_counter ++;
                        }
                    }else if(counter > num){
                        perfo = line;
                        boost::split(argv, perfo, boost::is_any_of(" "), boost::algorithm::token_compress_off);
                        for(auto & i : argv){
                            if(num_counter == addr3_shifted.source_2 - addr3_shifted.source_1){
                                flag = true;
                                break;
                            }
                            number = std::stoll(i);
                            if(number >= 0){
                                kmem.write_memory(addr3_shifted.source_1 + num_counter, number);
                            }else{
                                kmem.write_memory(addr3_shifted.source_1 + num_counter, to_negative(std::abs(number)));
                            }
                            num_counter ++;
                        }
                    }
                    if(flag){
                        break;
                    }
                    counter ++;
                }

                h = counter;
                punch_cards.close();
                heads.close();

                C_reg ++;
                K_reg = kmem.read_memory(C_reg);
            }
                break;

            case IO_operations_t::opcode_read_perfo_binary:{
                std::ifstream punch_cards;
                std::ifstream heads;
                std::string head;
                std::string line;

                punch_cards.open("../punched_tape.txt");
                heads.open("../heads.txt");

                std::getline(heads, head);
                std::getline(heads, head);
                // int num = std::stoi(head);
                size_t num = h;
                int counter = 0;
                int com_counter = 0;
                bool flag = false;

                while(punch_cards){
                    std::getline(punch_cards, line);
                    int pos = 0;
                    if(counter == num){
                        if(com_counter < addr3_shifted.source_2 - addr3_shifted.source_1){
                            if (!line.empty())
                                kmem.write_memory(addr3_shifted.source_1 + com_counter, std::stol(line, 0, 8));
                            com_counter++;
                        }else{
                            flag = true;
                            break;
                        }
                    }else if(counter > num){
                        if(com_counter < addr3_shifted.source_2 - addr3_shifted.source_1){
                            kmem.write_memory(addr3_shifted.source_1 + com_counter, std::stol(line, 0, 8));
                            com_counter ++;
                        }else{
                            flag = true;
                            break;
                        }

                    }
                    if(flag){
                        break;
                    }
                }
                h += com_counter;
                ++C_reg;
            }
                break;
            case IO_operations_t::opcode_read_magnetic_drum: {
                QRadioButton* btn = signalization[2][16];
                QRadioButton* read = signalization[6][21];
                QRadioButton* signBut = signalization[1][1];
                QRadioButton* signBut2 = signalization[5][0];

                read ->setChecked(1);
                btn -> setChecked(1);
                addr_t j = drum_zone_read;
                bool sign;
                word_t abs_val;
                if (drum_num_read == 1) {
                    for (addr_t i = addr3_shifted.source_1; i <= addr3_shifted.source_2; i++) {
                        word_t num = drum1[j] -> text().toLongLong();
                        sign = is_negative(num);
                        signBut -> setChecked(sign);
                        signBut2 -> setChecked(sign);
                        abs_val = static_cast<signed_word_t>(num & mask_40_bits);
                        std::bitset<40> b(abs_val);
                        std::string butts = b.to_string();
                        for (int i = 0; i < 40; i++) {
                            bool val = butts[i] - '0';
                            signalization[1][2+i] -> setChecked(val);
                            signalization[5][1+i] -> setChecked(val);
                        }
                        kmem.write_memory(i, num);
                        j++;
                    }
                } else if (drum_num_read == 2) {
                    for (addr_t i = addr3_shifted.source_1; i <= addr3_shifted.source_2; i++) {
                        word_t num = drum2[j] -> text().toLongLong();
                        sign = is_negative(num);
                        signBut -> setChecked(sign);
                        abs_val = static_cast<signed_word_t>(num & mask_40_bits);
                        std::bitset<40> b(abs_val);
                        std::string butts = b.to_string();
                        for (int i = 0; i < 40; i++) {
                            bool val = butts[i] - '0';
                            signalization[1][2+i] -> setChecked(val);
                            signalization[5][1+i] -> setChecked(val);
                        }
                        kmem.write_memory(i, num);
                        j++;
                    }
                } else {
                    for (addr_t i = addr3_shifted.source_1; i <= addr3_shifted.source_2; i++) {
                        word_t num = drum3[j] -> text().toLongLong();
                        sign = is_negative(num);
                        signBut -> setChecked(sign);
                        abs_val = static_cast<signed_word_t>(num & mask_40_bits);
                        std::bitset<40> b(abs_val);
                        std::string butts = b.to_string();
                        for (int i = 0; i < 40; i++) {
                            bool val = butts[i] - '0';
                            signalization[1][2+i] -> setChecked(val);
                            signalization[5][1+i] -> setChecked(val);
                        }
                        kmem.write_memory(i, num);
                        j++;
                    }
                }
                if (norm_work && !C_block) { C_reg = addr3_shifted.destination; }
                if (norm_work) { K_reg = kmem.read_memory(C_reg); }
                QTimer::singleShot(400,[btn]()->void{btn->setChecked(0);});
                QTimer::singleShot(400,[read]()->void{read->setChecked(0);});
                for (int i = 1; i <= 41; i++) {
                    signalization[1][i] -> setChecked(0);
                    signalization[5][i] -> setChecked(0);
                }
            }
                break;

            case IO_operations_t::opcode_write_perfo_binary: {
                QRadioButton* write = signalization[6][20];
                QRadioButton* btn = signalization[2][16];
                write -> setChecked(1);
                btn -> setChecked(1);
                std::ofstream myfile;
                myfile.open("../punc_cards_out.txt");
                if (myfile.is_open()) {
                    for (uint64_t i = 0; i <= addr3_shifted.source_2; i++) {
                        myfile << word_to_number(kmem.read_memory(addr3_shifted.source_1 + i));
                        myfile << ' ';
                    }
                    myfile.close();
                } else {
                    std::cout << "Unable to open file";
                }
                if (norm_work && !C_block) { C_reg = addr3_shifted.destination; }
                if (norm_work) { K_reg = kmem.read_memory(C_reg); }
                QTimer::singleShot(400,[btn]()->void{btn->setChecked(0);});
                QTimer::singleShot(400,[write]()->void{write->setChecked(0);});
            }
                break;

            case IO_operations_t::opcode_write_magnetic_drum: {
                QRadioButton* btn = signalization[2][16];
                QRadioButton* write = signalization[6][20];
                QRadioButton* signBut = signalization[1][1];
                QRadioButton* signBut2 = signalization[5][0];

                write -> setChecked(1);
                btn -> setChecked(1);
                addr_t j = drum_zone_write;
                word_t num;
                bool sign;
                word_t abs_val;
                if (drum_num_write == 1) {
                    for (addr_t i = addr3_shifted.source_1; i <= addr3_shifted.source_2; i++) {
                        num = kmem.read_memory(i);
                        drum1[j] -> setText(QString::number(num));
                        sign = is_negative(num);
                        signBut -> setChecked(sign);
                        signBut2 -> setChecked(sign);
                        abs_val = static_cast<signed_word_t>(num & mask_40_bits);
                        std::bitset<40> b(abs_val);
                        std::string butts = b.to_string();
                        for (int i = 0; i < 40; i++) {
                            bool val = butts[i] - '0';
                            signalization[1][2+i] -> setChecked(val);
                            signalization[5][1+i] -> setChecked(val);
                        }
                        j++;
                    }
                } else if (drum_num_write == 2) {
                    for (addr_t i = addr3_shifted.source_1; i <= addr3_shifted.source_2; i++) {
                        num = kmem.read_memory(i);
                        drum2[j] -> setText(QString::number(num));
                        sign = is_negative(num);
                        signBut -> setChecked(sign);
                        abs_val = static_cast<signed_word_t>(num & mask_40_bits);
                        std::bitset<40> b(abs_val);
                        std::string butts = b.to_string();
                        for (int i = 0; i < 40; i++) {
                            bool val = butts[i] - '0';
                            signalization[1][2+i] -> setChecked(val);
                            signalization[5][1+i] -> setChecked(val);
                        }
                        j++;
                    }
                } else {
                    for (addr_t i = addr3_shifted.source_1; i <= addr3_shifted.source_2; i++) {
                        num = kmem.read_memory(i);
                        drum2[j] -> setText(QString::number(num));
                        sign = is_negative(num);
                        signBut -> setChecked(sign);
                        abs_val = static_cast<signed_word_t>(num & mask_40_bits);
                        std::bitset<40> b(abs_val);
                        std::string butts = b.to_string();
                        for (int i = 0; i < 40; i++) {
                            bool val = butts[i] - '0';
                            signalization[1][2+i] -> setChecked(val);
                            signalization[5][1+i] -> setChecked(val);
                        }
                        j++;
                    }
                }
                for (int i = 1; i <= 41; i++) {
                    signalization[1][i] -> setChecked(0);
                    signalization[5][i] -> setChecked(0);
                }
                if (norm_work && !C_block) { C_reg = addr3_shifted.destination; }
                if (norm_work) { K_reg = kmem.read_memory(C_reg); }
                QTimer::singleShot(400,[write]()->void{write->setChecked(0);});
                QTimer::singleShot(400,[btn]()->void{btn->setChecked(0);});
            }
                break;
                //==========================================================================================================
            default:
                T_reg = true; // ! TODO: Не пам'ятаю, яка там точно реакція на невідому команду
            }
        }

    std::bitset<11> b(P_reg);
    std::string p_reg_str = b.to_string();
    for (size_t i = 0; i < 11; i++) {
        bool val = (p_reg_str[i] - '0');
        signalization[3][i+16]->setChecked(val);
    }

    std::bitset<10> b_a(A_reg);
    std::string a_reg = b_a.to_string();
    for (size_t i = 0; i < 10; i++) {
        bool val = (a_reg[i] - '0');
        signalization[6][i+10]->setChecked(val);
    }

    if (work_state > 1) {
        std::bitset<11> b_a(addr3.destination);
        std::string addr_str = b_a.to_string();
        for (size_t i = 0; i < 11; i++) {
            bool val = (addr_str[i] - '0');
            signalization[2][i + 17]->setChecked(val);
        }
    }

    std::bitset<10> b_c(Loop_reg);
    std::string loop_reg = b_c.to_string();
    for (size_t i = 0; i < 10; i++) {
        bool val = (loop_reg[i] - '0');
        signalization[6][i+22]->setChecked(val);
    }

    std::cout << "T_reg: " << T_reg << std::endl;

    QRadioButton *btn_end = signalization[6][34];
    btn_end -> setChecked(1);
    QTimer::singleShot(400,[btn_end]()->void{btn_end->setChecked(0);});

    return !T_reg;
}

void Kyiv_t::opcode_arythm(const addr3_t& addr3, opcode_t opcode, bool norm_work){
    //! TODO: Додати перевірку на можливість запису. Що робила машина при спробі запису в ПЗП?
    //! TODO: Додати перевірку на вихід за границю пам'яті -- воно ніби зациклювалося при тому
    //! (зверталося до байта add mod 2^11, в сенсі), але точно не знаю.

    signed_word_t sign1 = (is_negative(kmem.read_memory(addr3.source_1)) ? -1 : 1);
    signed_word_t sign2 = (is_negative(kmem.read_memory(addr3.source_2)) ? -1 : 1);

    QRadioButton* vz1 = signalization[2][30];
    QRadioButton* vz2 = signalization[2][32];
    QRadioButton* vz3 = signalization[2][35];
    vz1->setChecked(true);
    word_t abs_val1 = static_cast<signed_word_t>(kmem.read_memory(addr3.source_1) & mask_40_bits);
    QTimer::singleShot(400,[vz1]()->void{vz1->setChecked(false);});
    vz2->setChecked(true);
    word_t abs_val2 = static_cast<signed_word_t>(kmem.read_memory(addr3.source_2) & mask_40_bits);
    QTimer::singleShot(400,[vz2]()->void{vz2->setChecked(false);});
    signed_word_t res = sign1 * (signed_word_t) abs_val1;

    signed_word_t res_for_norm;
    mul_word_t res_mul;
    uint16_t power = 40 - leftmost_one(abs_val1) -1;
    QRadioButton* btn = signalization[2][33];
    QRadioButton* zn = signalization[5][0];


    std::cout << "C_block" << C_block << std::endl;
    switch(opcode){
        case arythm_operations_t::opcode_add: {
            res += sign2 * (signed_word_t) abs_val2;
            btn -> setChecked(true);
            QTimer::singleShot(40000,[btn]()->void{btn->setChecked(0);});
        }
            break;
        case arythm_operations_t::opcode_sub: {
            res -= sign2 * (signed_word_t) abs_val2;
            btn -> setChecked(true);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(0);});
        }
            break;
        case arythm_operations_t::opcode_addcmd: {
            res += (signed_word_t) abs_val2;
            btn -> setChecked(true);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(false);});
        }
            break;
        case arythm_operations_t::opcode_subabs: {
            res = (signed_word_t) abs_val1 - (signed_word_t) abs_val2;
            btn -> setChecked(true);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(false);});
        }
            break;
        case arythm_operations_t::opcode_addcyc: {
            res += sign2 * (signed_word_t) abs_val2; // Те ж, що і для opcode_add, але подальша обробка інша
            btn -> setChecked(true);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(false);});
        }
            break;
        case arythm_operations_t::opcode_mul: [[fallthrough]];
        case arythm_operations_t::opcode_mul_round: {
            res_mul = sign1 * (mul_word_t) abs_val1 * sign2 * (mul_word_t) abs_val2;
            btn -> setChecked(true);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(false);});
        }
            break;
        case arythm_operations_t::opcode_norm: {
            res_for_norm = sign1 * (abs_val1 << power);
            btn -> setChecked(true);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(false);});
        }
            break;
        case arythm_operations_t::opcode_div: {
            if (((abs_val2 == 0) || (abs_val2 < abs_val1)) && !Ostanov_block) {
                T_reg = true;
                zn -> setChecked(true);
                QTimer::singleShot(400,[zn]()->void{zn->setChecked(false);});
                if (norm_work && !C_block) { ++C_reg; }
                return;
            }
            res_mul = ((mul_word_t) abs_val1 << 40) / (mul_word_t) abs_val2;
            btn -> setChecked(true);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(false);});
        }
            break;

        default:
            assert(false && "Should never been here!");
    }

    if(opcode == arythm_operations_t::opcode_add ||
       opcode == arythm_operations_t::opcode_sub ||
       opcode == arythm_operations_t::opcode_subabs     //! TODO: Я не плутаю, результат може мати знак?
            ) {
        //! TODO: До речі, а якщо переповнення, воно кінцевий регістр змінювало до останову, чи ні?
        // Тут я зробив, ніби ні -- але ХЗ, могло. Щоб точно знати -- треба моделювати на рівні схем ;=) --
        // як ви і поривалися. Але це не має бути важливим.

        bool negative = (sign2 == -1);
        signalization[0][0]->setChecked(negative);

        bool is_negative = (res < 0);
        if (is_negative)
            res = -res;
        assert(res >= 0);
        if (res & mask_41_bit) { // if sum & CPU1.mask_41_bit == 1 -- overflow to sign bit
            if (is_negative) {
                signalization[0][1]->setChecked(false);
                signalization[0][2]->setChecked(true);
            } else {
                signalization[0][1]->setChecked(true);
                signalization[0][2]->setChecked(false);
            }
            if (!Ostanov_block) {
                T_reg = true;
                zn -> setChecked(true);
                QTimer::singleShot(400,[zn]()->void{zn->setChecked(false);});
            }
            if (norm_work && !C_block) { ++C_reg; }
            return;
        } else {
            signalization[0][1]->setChecked(is_negative);
            signalization[0][2]->setChecked(is_negative);
        }
        vz3 -> setChecked(true);
        kmem.write_memory(addr3.destination, static_cast<uint64_t>(res) & mask_40_bits);
        std::cout << "Written value: " << kmem.read_memory(addr3.destination) << std::endl;
        std::bitset<40> b(kmem.read_memory(addr3.destination));
        std::string res_str = b.to_string();
        for (int i = 0; i < 40; i++) {
            bool val = (res_str[i] - '0');
            if (is_negative) { // one's complement
                val = !val;
            }
            signalization[0][i+3]->setChecked(val);
        }

        if (is_negative)
            kmem.write_memory(addr3.destination, kmem.read_memory(addr3.destination) | mask_41_bit);
        //! "Нуль, получаемый как разность двух равных чисел, имеет отрицательный знак" -- стор. 13 Глушко-Ющенко, опис УПЧ
        if(opcode == arythm_operations_t::opcode_sub && res == 0
           && abs_val2 == 0 //! TODO: Моє припущення -- перевірити!
                ){
            kmem.write_memory(addr3.destination, res | mask_41_bit);
            std::cout << "NEGATIVE 0" << (res | mask_41_bit) << std::endl;
        }
        QTimer::singleShot(400,[vz3]()->void{vz3->setChecked(false);});

    } else if(opcode == arythm_operations_t::opcode_addcmd){
        vz3 -> setChecked(true);
        kmem.write_memory(addr3.destination, static_cast<uint64_t>(res) & mask_40_bits);
        kmem.write_memory(addr3.destination, kmem.read_memory(addr3.destination) | (kmem.read_memory(addr3.source_2) & mask_41_bit)); // Копіюємо біт знаку з source_2 // edited тут наче так має бути
        QTimer::singleShot(400,[vz3]()->void{vz3->setChecked(0);});
    } else if(opcode == arythm_operations_t::opcode_addcyc){
        //! TODO: Вияснити, а як ця команда функціонує.
        // "Отличается от обычного сложения лишь тем, что  в нем отсутствует блокировка при выходе
        // из разполагаемого числа разрядов. Перенос из знакового разряда поступает в младший разряд
        // сумматора".
        // Питання (нумеруючи біти з 1 до 41):
        // 1. Перенос із 40 в 41 біт тут можливий? З фрази виглядає, що так.
        // 2. Якщо додавання переносу до молодшого біту виникло переповнення, що далі?
        //    Так виглядає, що воно не може виникнути, але чи я не помилився? -- не може, десь через переніс буде 0
        bool is_negative = (res < 0);
        // std::cout << (res) << std::endl;
        if (is_negative)
            res = -res;
        assert(res >= 0);

        // std::cout << std::bitset<41> (res) << std::endl;
        if(res & mask_41_bit){
            res += 1; // Маємо перенос із знакового біту
        }
        vz1 -> setChecked(1);
        kmem.write_memory(addr3.destination, static_cast<uint64_t>(res) & mask_40_bits);
        if (is_negative)
            kmem.write_memory(addr3.destination, kmem.read_memory(addr3.destination) | mask_41_bit);
        QTimer::singleShot(400,[vz3]()->void{vz3->setChecked(0);});

    } else if(opcode == arythm_operations_t::opcode_mul ||
              opcode == arythm_operations_t::opcode_mul_round
            ) {
        bool is_negative = (res_mul < 0);
        if (is_negative)
            res_mul = -res_mul;
        assert(res_mul >= 0);

        uint16_t leftmost = leftmost_one(res_mul);

        if (opcode == arythm_operations_t::opcode_mul_round) {
            res_mul += 1ULL << 39;
        }
        res_mul = res_mul >> 40;

        vz3 -> setChecked(1);
        kmem.write_memory(addr3.destination, static_cast<uint64_t>(res_mul) & mask_40_bits);
        if (is_negative)
            kmem.write_memory(addr3.destination, kmem.read_memory(addr3.destination) | mask_41_bit);
        QTimer::singleShot(400,[vz3]()->void{vz3->setChecked(0);});

    } else if (opcode == arythm_operations_t::opcode_norm) {
        bool is_negative = (res_for_norm < 0);

        if (is_negative)
            res_for_norm = -res_for_norm;
        assert(res_for_norm >= 0);

        vz3 -> setChecked(1);
        kmem.write_memory(addr3.source_2, power);
        kmem.write_memory(addr3.destination, static_cast<uint64_t>(res_for_norm) & mask_40_bits);
        if (is_negative)
            kmem.write_memory(addr3.destination, kmem.read_memory(addr3.destination) | mask_41_bit);
        QTimer::singleShot(400,[vz3]()->void{vz3->setChecked(0);});
    } else if (opcode == arythm_operations_t::opcode_div) {
        vz3->setChecked(1);
        kmem.write_memory(addr3.destination, static_cast<uint64_t>(res_mul) & mask_40_bits);
        if ((sign1 * sign2) == -1)
            kmem.write_memory(addr3.destination, kmem.read_memory(addr3.destination) | mask_41_bit);
        QTimer::singleShot(400, [vz3]() -> void { vz3->setChecked(0); });
    }
    if (norm_work && !C_block) { ++C_reg; }
}


void Kyiv_t::opcode_flow_control(const addr3_t& addr3_shifted, opcode_t opcode, const addr3_t &addr3, bool norm_work){
    signed_word_t sign1 = (is_negative(kmem.read_memory(addr3_shifted.source_1)) ? -1 : 1);
    signed_word_t sign2 = (is_negative(kmem.read_memory(addr3_shifted.source_2)) ? -1 : 1);
    signed_word_t abs_val1 = static_cast<signed_word_t>(kmem.read_memory(addr3_shifted.source_1) & mask_40_bits);
    signed_word_t abs_val2 = static_cast<signed_word_t>(kmem.read_memory(addr3_shifted.source_2) & mask_40_bits);
    QRadioButton* btn = signalization[2][28];

    switch (opcode) {
        case flow_control_operations_t::opcode_jmp_less_or_equal: {
            btn -> setChecked(1);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(0);});
            if(norm_work && ((sign1 * abs_val1) <= (sign2 * abs_val2))){
                if (!C_block) { C_reg = addr3_shifted.destination; }
            } else {
                if (!C_block) { ++C_reg; }
            }
        }
            break;
        case flow_control_operations_t::opcode_jmp_abs_less_or_equal: {
            btn -> setChecked(1);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(0);});
            if (norm_work && (abs_val1 <= abs_val2)) {
                if (!C_block) { C_reg = addr3_shifted.destination; }
            } else {
                if (!C_block) { ++C_reg; }
            }
        }
            break;
        case flow_control_operations_t::opcode_jmp_equal: {
            btn -> setChecked(1);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(0);});
            if (norm_work && ((sign1 * abs_val1) == (sign2 * abs_val2))){
                if (!C_block) { C_reg = addr3_shifted.destination; }
            } else {
                if (!C_block) { ++C_reg; }
            }
        }
            break;
        case flow_control_operations_t::opcode_fork_negative: {
            btn -> setChecked(1);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(0);});
            if (norm_work && (is_negative(kmem.read_memory(addr3_shifted.source_1))) ){
                if (!C_block) { C_reg = addr3_shifted.destination; }
            }else{
                if (!C_block) { C_reg = addr3_shifted.source_2; }
            }
        }
            break;
        case flow_control_operations_t::opcode_call_negative:{
            btn -> setChecked(1);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(0);});
            if( is_negative(kmem.read_memory(addr3_shifted.source_1)) ){
                if (!A_block) { P_reg = addr3_shifted.source_2; } //! TODO: Згідно тексту стор 342 (пункт 18) Гнеденко-Королюк-Ющенко-1961
                //! Глушков-Ющенко, стор 13, УПП не до кінця однозначна -- a1 без штриха, це зрозуміло,
                //! але з врахуванням A і біта модифікатора, чи без?
                //! Виглядає, що в таблиці на стор 180 -- помилка ('A2 => P -- зайвий штрих точно помилка,
                //! чи помилка, що, немає зсуву на А?).
                //! Однак, в  Гнеденко-Королюк-Ющенко-1961 опкод (32) суперечить опкоду в Глушко-Ющенко.
                //! Перевірити!
                if (norm_work && !C_block) { C_reg = addr3_shifted.destination; }
            }else{
                if (norm_work && !C_block) { ++C_reg; } //! Тут P_reg не мала б змінювати
            }
        }
            break;
        case flow_control_operations_t::opcode_ret:{
            btn -> setChecked(1);
            QTimer::singleShot(400,[btn]()->void{btn->setChecked(0);});
            for (size_t i = 0; i < 11; i++) {
                signalization[3][16+i]->setChecked(0);
            }
            if (norm_work && !C_block) { C_reg = P_reg; P_reg = 0; }
        }
            break;
        case flow_control_operations_t::opcode_group_op_begin:{
            // У книжці Глушков-Ющенко на ст. 14, ймовірно, помилка, бо навіть словами пояснено,
            // що береться значення а1 і а2, але разом із тим наголошено, що береться не 'а1 чи 'а2,
            // а саме а1 і а2. В кінці цієї книжки та у Гнеденко-Королюк-Ющенко пише,
            // ніби беруться значення, тому тут реалізовано саме так.
            Loop_reg = addr3_shifted.source_1; //word_to_number(kmem.read_memory(addr3_shifted.source_1));
            if (!A_block) {
                A_reg = addr3_shifted.source_2; // word_to_number(kmem.read_memory(addr3_shifted.source_2));
            }
            if (norm_work && (A_reg == Loop_reg)) {
                if (!C_block) { C_reg = addr3_shifted.destination; }
            } else {
                if (!C_block) { ++C_reg; }
            }

            std::cout << "A_reg: " << A_reg << std::endl;
        }
            break;
        case flow_control_operations_t::opcode_group_op_end:{
            // Такий самий прикол, як з НГО
            // not a value
            if (!A_block) { A_reg += addr3.source_1; } // word_to_number(kmem.read_memory(addr3_shifted.source_1));
            if (norm_work && (A_reg == Loop_reg)) {
                if (!C_block) { C_reg = word_to_number(addr3_shifted.destination); }
            } else {
                if (!C_block) { C_reg = (addr3_shifted.source_2); }
            }
        }
            break;
        case flow_control_operations_t::opcode_F:{
            // Якщо я правильно розібралася з 2 попередними командами, то тут все зрозуміло і немає суперечностей
            A_reg = word_to_addr3(kmem.read_memory(addr3_shifted.source_1)).source_2;
            kmem.write_memory(addr3_shifted.destination, kmem.read_memory(A_reg));
            if (norm_work && !C_block) { ++C_reg; }
        }
            break;
        case flow_control_operations_t::opcode_stop:{ //! TODO: Вона враховує стан кнопки на пульті?
            // From Glushkov-Iushchenko p. 55
            // If B_tumb == 0 -> neutral mode -> full stop
            // If B_tumb > 0 -> just skip one command without full stop
            // From Glushkov-Iushchenko pp. 163-164
            // If B_tumb == 1 -> stop by 3d address
            // If B_tumb == 2 -> stop by command number
            // I'm not sure what to do with 1st and 2nd B_tumb (maybe that should be handled in main???)
            if (!Ostanov_block) {
                if (T_tr) {
                    T_reg = true;
                    if (norm_work && !C_block) { ++C_reg; }
                } else {
                    if (norm_work && !C_block) { C_reg += 2; }
                }
            }
        }
            break;
    }
}

void Kyiv_t::null_registers() {
    C_reg = 0, K_reg = 0, P_reg = 0, Loop_reg = 0, A_reg = 0, B_tumb = 0;
}

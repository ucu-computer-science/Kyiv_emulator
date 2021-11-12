// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <stdlib.h>
#include <cstdint>
#include <cmath>
#include <cassert>
#include "k_memory.h"
#include <fstream>
#include <string>

typedef uint64_t addr_t;
typedef uint64_t word_t;
typedef int64_t  signed_word_t;
typedef uint32_t opcode_t;

struct addr3_t{
    addr_t source_1 = 0, source_2 = 0, destination = 0;
    constexpr addr3_t() = default;
};

constexpr addr3_t word_to_addr3(word_t w){
    constexpr word_t Addr_1_mask_shift = (40-6-11)+1;
    constexpr word_t Addr_1_mask = 0b11'111'111'111ULL << (Addr_1_mask_shift-1);
    constexpr word_t Addr_2_mask_shift = (40-6-12-11)+1;
    constexpr word_t Addr_2_mask = 0b11'111'111'111ULL << (Addr_2_mask_shift-1);
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
    constexpr word_t op_code_mask = 0b11'111ULL << (op_code_shift-1);
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

signed_word_t get_absolute(word_t w){
    return static_cast<signed_word_t>(w & mask_40_bits);
}

word_t word_to_number(word_t w){
    signed_word_t sign1 = (w ? -1 : 1);
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

class Kyiv_memory {
private:
    word_t k[04000] = {0, 0,
                       to_negative(3), to_negative(10), 5,
//                            CPU1.to_negative(6), 7, 8}; // 0AAAA -- octal constant
                       7, 5, 8}; // 0AAAA -- octal constant};
    const word_t krom[01000] = {0};

public:
    word_t & operator[](addr_t addres) {
        if (addres > 04000)
            throw ;
        if (addres < 01777) {
            return k[addres];
        }
        k[addres] = krom[04000 - addres];
        return k[addres];
    }

};


// Контроль виходу за розмір додати потім. Це дуже груба реалізація.
struct Kyiv_t{
    // bitfield-и тут мають один нюанс, тому не юзаю
    //! Увага, ми біти нумеруємо з нуля, вони - з 1, тому 40 = 41-1 і т.д.

    //! TODO: Які вони там значення при включенні мали? Як це керувалося?
    uint64_t C_reg = 0, K_reg = 0, P_reg = 0, Loop_reg = 0, A_reg = 0, B_tumb = 0;
    // B_tumb is not bool because according to p. 163 Glushkov-Iushchenko it has 3 modes.
    // Maybe there is a better way to handle this?
    bool T_reg = false;


    //! ! TODO: Пам'ять певне потрібно виділити в нову сутність -- клас, який зразу відповідатиме і за
    //! захист пам'яті, поведінку при переповненні і т.д. -- я поки тим не заморочувався.
    //! Нульова адреса має завжди містити 0.
    //! Найпростіше, певне, буде створити свій клас, який перевизначає оператор [], і все це реалізовує.
    //! Тоді решта коду не потребуватиме зміни.
    Kyiv_memory kmem;
//    word_t kmem[04000] = {0, 0'02'0003'0004'0005ULL, 0'02'0006'0007'0010ULL,
//                          3, 4, 5,
////                            CPU1.to_negative(6), 7, 8}; // 0AAAA -- octal constant
//                          to_negative(7), 5, 8}; // 0AAAA -- octal constant

    //! TODO: Придумати адекватні мнемоніки
    enum arythm_operations_t{
        opcode_add = 0'01,
        opcode_sub = 0'02,
        opcode_addcmd = 0'03,
        opcode_subabs = 0'06,
        opcode_addcyc = 0'07,
        //! Ймовірно, ці три операції враховують fixed-point природу чисел "Києва"
        opcode_mul = 0'10,       //! УВАГА -- це нетривіальна операція!
        opcode_mul_round = 0'11, //! УВАГА -- це нетривіальна операція!
        opcode_div = 0'12,       //! УВАГА -- це нетривіальна операція!
        opcode_norm = 0'35       // Іноді відносять до логічних операцій
    };
    // Логічні операції працюють над всіма 41 бітами
    enum logic_operations_t{
        opcode_log_shift = 0'13,
        opcode_log_or = 0'14, // Логічне додавання в тексті
        opcode_log_and = 0'15, // Логічне множення в тексті
        opcode_log_xor = 0'17, // Нерівнозначність в тексті
    };
    enum flow_control_operations_t{
        opcode_jmp_less_or_equal = 0'04,
        opcode_jmp_abs_less_or_equal = 0'05,
        opcode_jmp_equal = 0'16,
        opcode_fork_negative = 0'31, // <=-0, дві гілки
        opcode_call_negative = 0'30, // <=-0 -- не забудьте, що +0 -- немає стрибка
        opcode_ret = 0'32,
        // Операції над адресами
        opcode_group_op_begin = 0'26,
        opcode_group_op_end = 0'27,
        opcode_F = 0'34,
        opcode_stop = 0'33, // TODO: Він ніби в десятковому у таблиці -- перевірити
    };

    enum IO_operations_t{
        opcode_read_perfo_data = 0'20,
        opcode_read_perfo_binary = 0'21,
        opcode_write_perfo_binary = 0'22,
        opcode_init_magnetic_drum = 0'25,
        opcode_read_magnetic_drum = 0'24, // В Глушко-Ющенко помилка в табл. на стор 180.
        opcode_write_magnetic_drum = 0'23,
    };
    //! Returns: True -- continue, false -- ситуація останову.
    bool execute_opcode(){
        K_reg = kmem[C_reg];
        opcode_t opcode = word_to_opcode(K_reg);
        addr3_t addr3 = word_to_addr3(K_reg); // Парі команд потрібна
        addr3_t addr3_shifted = shift_addr3_byA(addr3, A_reg, K_reg); // Решта використовують цю змінну

//        signed_word_t sign1 = (is_negative(kmem[addr3_shifted.source_1]) ? -1 : 1);
//        signed_word_t sign2 = (is_negative(kmem[addr3_shifted.source_2]) ? -1 : 1);
//        signed_word_t sign3 = (is_negative(kmem[addr3_shifted.destination]) ? -1 : 1);
//        signed_word_t abs_val1 = static_cast<signed_word_t>(kmem[addr3_shifted.source_1] & mask_40_bits);
//        signed_word_t abs_val2 = static_cast<signed_word_t>(kmem[addr3_shifted.source_2] & mask_40_bits);
//        signed_word_t abs_val3 = static_cast<signed_word_t>(kmem[addr3_shifted.destination] & mask_40_bits);;
//        signed_word_t num1 = sign1 * abs_val1;
//        signed_word_t num2 = sign2 * abs_val2;
//        signed_word_t num3 = sign3 * abs_val3;

        //! Ймовірно, потім це діло треба буде відрефакторити -- відчуваю, но де буде проблема - поки не знаю :+)
        switch(opcode){
            //TODO: Тестував лише opcode_add !!! -- решта вважайте невірними, поки не буде тестів.
            case arythm_operations_t::opcode_add: [[fallthrough]];
            case arythm_operations_t::opcode_sub: [[fallthrough]];
            case arythm_operations_t::opcode_addcmd: [[fallthrough]];
            case arythm_operations_t::opcode_subabs: [[fallthrough]];
            case arythm_operations_t::opcode_addcyc: // [[fallthrough]];
                opcode_arythm(addr3_shifted, opcode);
                break;
                //==========================================================================================================
            case flow_control_operations_t::opcode_jmp_less_or_equal: {
//                std::cout << addr3_shifted.source_1 << std::endl;
//                std::cout << addr3_shifted.source_2 << std::endl;
//                std::cout << C_reg << std::endl;
                if( addr3_shifted.source_1 <= addr3_shifted.source_2){
                    C_reg = addr3_shifted.destination;
                } else {
                    ++C_reg;
                }
//                std::cout << C_reg << std::endl;
            }
                break;
            case flow_control_operations_t::opcode_jmp_abs_less_or_equal: {
                if (get_absolute(addr3_shifted.source_1) <= get_absolute(addr3_shifted.source_2)) {
                    C_reg = addr3_shifted.destination;
                } else {
                    ++C_reg;
                }
            }
                break;
            case flow_control_operations_t::opcode_jmp_equal: {
                if( word_to_number(addr3_shifted.source_1) == word_to_number(addr3_shifted.source_2)){
                    C_reg = addr3_shifted.destination;
                } else {
                    ++C_reg;
                }
            }
                break;
            case flow_control_operations_t::opcode_fork_negative: {
                if( is_negative(kmem[addr3_shifted.source_1]) ){
                    C_reg = addr3_shifted.destination;
                }else{
                    C_reg = addr3_shifted.source_2;
                }
            }
                break;
            case flow_control_operations_t::opcode_call_negative:{
                if( is_negative(kmem[addr3_shifted.source_1]) ){
                    P_reg = addr3_shifted.source_2; //! TODO: Згідно тексту стор 342 (пункт 18) Гнеденко-Королюк-Ющенко-1961
                    //! Глушков-Ющенко, стор 13, УПП не до кінця однозначна -- a1 без штриха, це зрозуміло,
                    //! але з врахуванням A і біта модифікатора, чи без?
                    //! Виглядає, що в таблиці на стор 180 -- помилка ('A2 => P -- зайвий штрих точно помилка,
                    //! чи помилка, що, немає зсуву на А?).
                    //! Однак, в  Гнеденко-Королюк-Ющенко-1961 опкод (32) суперечить опкоду в Глушко-Ющенко.
                    //! Перевірити!
                    C_reg = addr3_shifted.destination;
                }else{
                    ++C_reg; //! Тут P_reg не мала б змінювати
                }
            }
                break;
            case flow_control_operations_t::opcode_ret:{
                C_reg = P_reg;
            }
                break;
            case flow_control_operations_t::opcode_group_op_begin:{
                // У книжці Глушков-Ющенко на ст. 14, ймовірно, помилка, бо навіть словами пояснено,
                // що береться значення а1 і а2, але разом із тим наголошено, що береться не 'а1 чи 'а2,
                // а саме а1 і а2. В кінці цієї книжки та у Гнеденко-Королюк-Ющенко пише,
                // ніби беруться значення, тому тут реалізовано саме так.
                Loop_reg = word_to_number(addr3_shifted.source_1);
                A_reg = word_to_number(addr3_shifted.source_2);
                if (A_reg == Loop_reg) {
                    C_reg = addr3_shifted.destination;
                } else {
                    ++C_reg;
                }
            }
                break;
            case flow_control_operations_t::opcode_group_op_end:{
                // Такий самий прикол, як з НГО
                A_reg += word_to_number(addr3_shifted.source_1);
                if (A_reg == Loop_reg) {
                    C_reg = addr3_shifted.destination;
                } else {
                    C_reg = addr3_shifted.source_2;
                }
            }
                break;
            case flow_control_operations_t::opcode_F:{
                // Якщо я правильно розібралася з 2 попередними командами, то тут все зрозуміло і немає суперечностей
                A_reg = addr3_shifted.source_2;
                addr3_shifted.destination = word_to_number(A_reg);
                ++C_reg;
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
                if (!B_tumb) {
                    T_reg = true;
                    ++C_reg;
                }
                else {
                    C_reg += 2;
                }
            }
                break;
//==========================================================================================================
            case logic_operations_t::opcode_log_shift:{
                //! TODO: Мені не повністю зрозуміло з обох книг, величина зсуву береться із RAM за адресою,
                //! чи закодована в команді? Швидше перше -- але перевірити!
                word_t shift = kmem[addr3_shifted.source_1] ; // Глушко-Ющенко, стор 12, сверджує: "на число разрядов,
                // равное абсолютной величине константы сдвига, размещаемой в шести младших разрядах ячейки а1"
                // 2^6 -- 64, тому решта бітів справді просто дадуть нуль на виході, але все рівно маскую, щоб
                // не було невизначеної поведінки С. Та й зразу знак викидаємо
                shift &= 0b111'111;

                if(is_negative(kmem[addr3_shifted.source_1])) {
                    kmem[addr3_shifted.destination] = kmem[addr3_shifted.source_2] >> shift;
                } else {
                    kmem[addr3_shifted.destination] = kmem[addr3_shifted.source_2] << shift;
                    kmem[addr3_shifted.destination] &= (mask_40_bits | mask_41_bit); // Зануляємо зайві біти
                }
                ++C_reg;
            }
                break;
            case logic_operations_t::opcode_log_or:{
                kmem[addr3_shifted.destination] = kmem[addr3_shifted.source_1] | kmem[addr3_shifted.source_2];
                ++C_reg;
            }
                break;
            case logic_operations_t::opcode_log_and:{
                kmem[addr3_shifted.destination] = kmem[addr3_shifted.source_1] & kmem[addr3_shifted.source_2];
                ++C_reg;
            }
                break;
            case logic_operations_t::opcode_log_xor:{
                kmem[addr3_shifted.destination] = kmem[addr3_shifted.source_1] ^ kmem[addr3_shifted.source_2];
                ++C_reg;
            }
                break;
//==========================================================================================================
                //! Тут буде IO
            case IO_operations_t::opcode_read_perfo_data:{
                std::ifstream myfile;
                myfile.open("input.txt");
                std::string myline;
                int count = 1;
                if ( myfile.is_open() ) {
                    while ( myfile ) {
                        std::getline (myfile, myline);
                        if(count >= addr3_shifted.destination && count <= addr3_shifted.destination + addr3_shifted.source_2 - addr3_shifted.source_1){
                            kmem[addr3_shifted.source_1 + count - 1] = stoi(myline);
                            //std::cout << myline << '\n';
                        }
                        count ++;
                    }
                }
                else {
//                    std::cout << "Couldn't open file\n";
                    T_reg = true;
                }
            }
            case IO_operations_t::opcode_read_perfo_binary:{}
            case IO_operations_t::opcode_read_magnetic_drum:{}
            case IO_operations_t::opcode_write_perfo_binary:{
                //!воно ще якось має передавати управління
                //! команді з третьої адреси, але я хз як саме це має відбуватись
                std::ofstream myfile;
                myfile.open("output.txt");
                if (myfile.is_open())
                {
                    for(uint64_t i = 0; i <= addr3_shifted.source_2; i++){
                        myfile << kmem[addr3_shifted.source_1 + i];
                        myfile << "\n";
                    }
                    myfile.close();
                }else {
                    //std::cout << "Unable to open file";
                    T_reg = true;
                }
                return 0;
            }
            case IO_operations_t::opcode_write_magnetic_drum:{}
            case IO_operations_t::opcode_init_magnetic_drum:{}
//==========================================================================================================
            default:
                T_reg = true; // ! TODO: Не пам'ятаю, яка там точно реакція на невідому команду
        }
        return !T_reg;
    }

    void opcode_arythm(const addr3_t& addr3, opcode_t opcode){
        //! TODO: Додати перевірку на можливість запису. Що робила машина при спробі запису в ПЗП?
        //! TODO: Додати перевірку на вихід за границю пам'яті -- воно ніби зациклювалося при тому
        //! (зверталося до байта add mod 2^11, в сенсі), але точно не знаю.
        signed_word_t sign1 = (is_negative(kmem[addr3.source_1]) ? -1 : 1);
        signed_word_t sign2 = (is_negative(kmem[addr3.source_2]) ? -1 : 1);
        signed_word_t abs_val1 = static_cast<signed_word_t>(kmem[addr3.source_1] & mask_40_bits);
        signed_word_t abs_val2 = static_cast<signed_word_t>(kmem[addr3.source_2] & mask_40_bits);;
        signed_word_t res = sign1 * abs_val1;
//        std::cout << sign1 * abs_val1 << "\t" << sign2 * abs_val2 << std::endl;
        switch(opcode){
            case arythm_operations_t::opcode_add:
                res += sign2 * abs_val2;
                break;
            case arythm_operations_t::opcode_sub:
                res -= sign2 * abs_val2;
                break;
            case arythm_operations_t::opcode_addcmd:
                res += abs_val2;
                break;
            case arythm_operations_t::opcode_subabs:
                res = abs_val1 - abs_val2;
                break;
            case arythm_operations_t::opcode_addcyc:
                res += sign2 * abs_val2; // Те ж, що і для opcode_add, але подальша обробка інша
                break;
            default:
                assert(false && "Should never been here!");
        }
//        std::cout << res << std::endl;
        if(opcode == arythm_operations_t::opcode_add ||
           opcode == arythm_operations_t::opcode_sub ||
           opcode == arythm_operations_t::opcode_subabs     //! TODO: Я не плутаю, результат може мати знак?
                ) {
            //! TODO: До речі, а якщо переповнення, воно кінцевий регістр змінювало до останову, чи ні?
            // Тут я зробив, ніби ні -- але ХЗ, могло. Щоб точно знати -- треба моделювати на рівні схем ;=) --
            // як ви і поривалися. Але це не має бути важливим.
            bool is_negative = (res < 0);
            if (is_negative)
                res = -res;
            assert(res >= 0);
            if (res & mask_41_bit) { // if sum & CPU1.mask_41_bit == 1 -- overflow to sign bit
                T_reg = true;
                ++C_reg;
                return;
            }
            kmem[addr3.destination] = static_cast<uint64_t>(res) & mask_40_bits;
            if (is_negative)
                kmem[addr3.destination] |= mask_41_bit;
            //! "Нуль, получаемый как разность двух равных чисел, имеет отрицательный знак" -- стор. 13 Глушко-Ющенко, опис УПЧ
            if(opcode == arythm_operations_t::opcode_sub && res == 0
               && abs_val1 != 0 //! TODO: Моє припущення -- перевірити!
                    ){
                kmem[addr3.destination] |= mask_41_bit;
            }
        } else if(opcode == arythm_operations_t::opcode_addcmd){
            kmem[addr3.destination] = static_cast<uint64_t>(res) & mask_40_bits;
            kmem[addr3.destination] |= is_negative(kmem[addr3.source_2]); // Копіюємо біт знаку з source_2
        } else if(opcode == arythm_operations_t::opcode_addcyc){
            //! TODO: Вияснити, а як ця команда функціонує.
            // "Отличается от обычного сложения лишь тем, что  в нем отсутствует блокировка при выходе
            // из разполагаемого числа разрядов. Перенос из знакового разряда поступает в младший разряд
            // сумматора".
            // Питання (нумеруючи біти з 1 до 41):
            // 1. Перенос із 40 в 41 біт тут можливий? З фрази виглядає, що так.
            // 2. Якщо додавання переносу до молодшого біту виникло переповнення, що далі?
            //          Так виглядає, що воно не може виникнути, але чи я не помилився?
            if(res | (mask_41_bit<<1)){
                res += 1; // Маємо перенос із знакового біту
            }
            kmem[addr3.destination] = static_cast<uint64_t>(res) & ( mask_40_bits | mask_41_bit);
        }

        ++C_reg;
    }

} machine1;


int main() {
    machine1.C_reg = 1; //! TODO: Звідки вона в реальності починала?
    //! TODO: Додати тести всіх команд!
    //! TODO: Ще тут буде потім перевірка, чи зупиняти машину при виникненні ситуації переповнення -- керується кнопкою на пульт
    machine1.kmem[1] = 0'01'0005'0006'0005ULL;
    machine1.kmem[2] = 0'04'0004'0005'0006ULL;
    machine1.kmem[3] = 0'04'0004'0005'0006ULL;
    machine1.kmem[6] = 0'05'0004'0003'0010ULL;

    while(machine1.execute_opcode()){
        std::cout << machine1.kmem[5] << std::endl;
        // 1) 7 + 5 = 12
        // 2) 5 <= 12 -> jumps to 6
        // 3)

        std::cout << machine1.C_reg << " <- C register" << std::endl;


        // Ще тут буде потім перевірка, чи зупиняти машину при виникненні ситуації переповнення -- керується кнопкою на пульті
    }

    return 0;
}

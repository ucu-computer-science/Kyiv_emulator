#ifndef KYIVEMU_KYIV_H
#define KYIVEMU_KYIV_H

#include <boost/multiprecision/cpp_int.hpp>

typedef uint64_t addr_t;
typedef uint64_t word_t;
typedef int64_t  signed_word_t;
typedef uint32_t opcode_t;
typedef boost::multiprecision::int128_t mul_word_t;

struct addr3_t{
    addr_t source_1 = 0, source_2 = 0, destination = 0;
    constexpr addr3_t() = default;
};

constexpr addr3_t word_to_addr3(word_t w);
constexpr opcode_t word_to_opcode(word_t w);
constexpr bool is_negative(word_t w);
constexpr word_t to_negative(word_t w);
constexpr word_t to_positive(word_t w);
constexpr uint16_t leftmost_one(word_t w);
uint16_t leftmost_one(mul_word_t w);
signed_word_t get_absolute(word_t w);
signed_word_t word_to_number(word_t w);
constexpr bool get_A1(word_t w);
constexpr bool get_A2(word_t w);
constexpr bool get_A3(word_t w);
constexpr addr3_t shift_addr3_byA(addr3_t addr3, uint64_t offset, word_t w);


struct aproxy {
    aproxy(word_t& r, addr_t addres) : mPtr(&r), ref(addres) {}
    void operator = (word_t n) const {
        if (ref > 03000) {
            throw "ROM!";
        }
        *mPtr = n;
    }
    operator word_t() {
        return *mPtr;
    }
    void operator |=(word_t x) const {
        *mPtr =  *mPtr | x;
    }
    void operator &=(word_t x) const {
        *mPtr = *mPtr & x;
    }
    addr_t ref;
    word_t *mPtr;
};

class Kyiv_memory {
private:
    word_t k[04000] = {0, 0'12'0004'0005'0007ULL, 0'02'0004'0005'0007ULL,
                       100, 4, 5, 6, 7, 8, 1099511627775, 549755813888, 16, 2, 16, 43980465111};
public:
    auto operator[](addr_t addres) {
        return aproxy(k[addres], addres);
    }

};

class Kyiv_magnetic_drum {
private:
    std::string file_name = "../magnetic_drum.txt";
    std::pair<size_t, size_t> read_d, write_d = {0, 0};
public:
    word_t read_drum(addr3_t & addr3_shifted, Kyiv_memory & kmem) {
        std::ifstream magnetic_drum;
        magnetic_drum.open(file_name);

        std::vector<std::string> data;
        signed_word_t number;
        std::string line;
        int counter = 0;
        int num_counter = 0;
        bool flag;

        while(magnetic_drum) {
            std::getline(magnetic_drum, line);
            if (counter == read_d.first) {
                boost::split(data, line, boost::is_any_of(" "), boost::algorithm::token_compress_off);
                for (size_t i = read_d.second; i < data.size(); i++) {
                    if(num_counter == addr3_shifted.source_2 - addr3_shifted.source_1){
                        flag = true;
                        break;
                    }
                }
            }

        }
//
//        while(magnetic_drum){
//            std::getline(magnetic_drum, line);
//            if(counter == read_d.first){
//                data = line.substr(read_d.second, line.size());
//                boost::split(argv, data, boost::is_any_of(" "), boost::algorithm::token_compress_off);
//                for(const auto& num : argv){
//                    if(num_counter == addr3_shifted.source_2 - addr3_shifted.source_1){
//                        flag = true;
//                        break;
//                    }
//                    number = std::stoi(num);
//                    if(number >= 0){
//                        kmem[addr3_shifted.source_1 + num_counter] = number;
//                    }else{
//                        kmem[addr3_shifted.source_1 + num_counter] = to_negative(std::abs(number));
//                    }
//                    num_counter ++;
//                }
//            }else if(counter > read_d.first){
//                data = line;
//                boost::split(argv, data, boost::is_any_of(" "), boost::algorithm::token_compress_off);
//                for(const auto& num : argv){
//                    if(num_counter == addr3_shifted.source_2 - addr3_shifted.source_1){
//                        flag = true;
//                        break;
//                    }
//                    number = std::stoi(num);
//                    if(number >= 0){
//                        kmem[addr3_shifted.source_1 + num_counter] = number;
//                    }else{
//                        kmem[addr3_shifted.source_1 + num_counter] = to_negative(std::abs(number));
//                    }
//                    num_counter ++;
//                }
//            }
//            if(flag){
//                break;
//            }
//            counter ++;
//
//        }
        magnetic_drum.close();
        return 0;
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
        opcode_norm = 0'35,       // Іноді відносять до логічних операцій
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

    bool execute_opcode();

    void opcode_flow_control(const addr3_t &addr3, opcode_t opcode);

    void opcode_arythm(const addr3_t &addr3, opcode_t opcode);
};


#endif //KYIVEMU_KYIV_H

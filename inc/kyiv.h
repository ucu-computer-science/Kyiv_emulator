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

class memory_error: public std::runtime_error{
    addr_t err_addr;
public:
    memory_error(const std::string& err_descr_, addr_t addr_ ): std::runtime_error{err_descr_}, err_addr(addr_)
    {}
};

class out_of_range_error: public memory_error {
public:
    out_of_range_error(const std::string& err_descr_, addr_t addr_ ): memory_error{err_descr_, addr_}
    {}
};

class ROW_write_error: public memory_error {
public:
    ROW_write_error(const std::string& err_descr_, addr_t addr_ ): memory_error{err_descr_, addr_}
    {}
};


class Kyiv_memory_t {
private:
    word_t mem_array_m[04000] = {0}; // 0AAAA -- octal constant};


    //! Оператор може змінювати деякі значення  ROM
    void internal_set_ROM_values(addr_t addr, word_t val){
        // Перевірки додати, чи що? З пульта не всі змінюються. Або хай тим реалізація пульта займається?
//        if (addr >= max_RAM_addr){
//            throw out_of_range_error("Writing to ROM", addr);
//        }
        mem_array_m[addr] = val;
    }

public:
    static constexpr addr_t min_addr =     0;
    static constexpr addr_t min_RAM_addr = 0;
    static constexpr addr_t min_ROM_addr = 03000;
    static constexpr addr_t max_RAM_addr = min_ROM_addr; //! Напіввідкритий інтервал, як із вказівниками:
    //! Коректні адреси: [min_RAM_addr, max_RAM_addr)
    static constexpr addr_t max_ROM_addr = 04000;
    static constexpr addr_t max_addr =     04000;

    constexpr word_t read_memory(addr_t addr) const {
        if(addr >= max_ROM_addr){
            throw out_of_range_error("Wrong address while reading", addr);
        }
        return mem_array_m[addr];
    }
    void write_memory(addr_t addr, word_t val){
        if(addr >= max_ROM_addr){
            throw out_of_range_error("Wrong address while writing", addr);
        } else if (addr >= max_RAM_addr){
            throw out_of_range_error("Writing to ROM", addr);
        }
        mem_array_m[addr] = val;
    }

    void write_rom(addr_t addr, word_t val) {
        if(addr >= max_ROM_addr){
            throw out_of_range_error("Wrong address while writing", addr);
        } else if (addr < min_ROM_addr){
            throw out_of_range_error("Writing to RAM", addr);
        }
        mem_array_m[addr] = val;
    }

};


class Kyiv_memory {
private:
    word_t k[04000] = {0, 0'12'0004'0005'0007ULL, 0'02'0004'0005'0007ULL,
                       100, 4, 5, 6, 7, 8, 1099511627775, 549755813888, 16, 219902328832, 16, 549755813888
    };
public:
    auto operator[](addr_t addres) {
        return aproxy(k[addres], addres);
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
    size_t h = 0;
    
    int perfo_num = 0;
    int drum_num_read = 0;
    int drum_zone_read = 0;
    int drum_num_write = 0;
    int drum_zone_write = 0;


    //! ! TODO: Пам'ять певне потрібно виділити в нову сутність -- клас, який зразу відповідатиме і за
    //! захист пам'яті, поведінку при переповненні і т.д. -- я поки тим не заморочувався.
    //! Нульова адреса має завжди містити 0.
    //! Найпростіше, певне, буде створити свій клас, який перевизначає оператор [], і все це реалізовує.
    //! Тоді решта коду не потребуватиме зміни.
    Kyiv_memory_t kmem;

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

    Kyiv_t() {
        std::string rom_file = "../ROM.txt";
        std::ifstream infile(rom_file);
        std::string line;
        while (std::getline(infile, line)) {
            line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
            std::ostringstream str;
            str << std::oct << line;
            std::string data = str.str();
            std::string address = data.substr(0, 4);
            data = data.substr(4);
            if (data.size() != 13 && data.size() != 14) {
                continue;
            }
            if (data.size() != 14) {
                data.insert(0, "0");
            }
            kmem.write_rom(std::stoi(address, 0, 8), std::stol(data, 0, 8));
        }
    }

    bool execute_opcode();

    void opcode_flow_control(const addr3_t& addr3_shifted, opcode_t opcode, const addr3_t &addr3);

    void opcode_arythm(const addr3_t &addr3, opcode_t opcode);
};


#endif //KYIVEMU_KYIV_H

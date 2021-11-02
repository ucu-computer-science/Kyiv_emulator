#ifndef KYIVEMU_K_MEMORY_H
#define KYIVEMU_K_MEMORY_H

typedef uint64_t addr_t;
typedef uint64_t word_t;
typedef int64_t  signed_word_t;
typedef uint32_t opcode_t;


//class Kyiv_memory {
//private:
//    word_t k[04000] = {0, 0'01'0003'0004'0005ULL, 0'02'0006'0007'0010ULL,
//                        3, 4, 5,
////                            CPU1.to_negative(6), 7, 8}; // 0AAAA -- octal constant
//                        7, 5, 8}; // 0AAAA -- octal constant};
//    const word_t krom[01000] = {0};
//
//public:
//    word_t & operator[](addr_t addres) {
//        if (addres > 04000)
//            throw ;
//        if (addres < 01777) {
//            return k[addres];
//        }
//        k[addres] = krom[04000 - addres];
//        return k[addres];
//    }
//
//};


#endif //KYIVEMU_K_MEMORY_H

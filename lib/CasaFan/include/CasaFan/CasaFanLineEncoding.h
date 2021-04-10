#pragma once
#include <bitset>

class CasaFanLineEncoding {
public:
    /**
     * Encode bits to line coding
     * @param bits
     * @param num_bits
     * @return
     */
    template<const size_t BITS>
    static std::bitset<BITS * 3 + 1> encode(std::bitset<BITS> input_bits)
    {
        std::bitset<BITS * 3 + 1> result;
        for (unsigned int bit = 0; bit < BITS; ++bit)
        {
            result[bit * 3] = true;
            result[bit * 3 + 1] = false;
            result[bit * 3 + 2] = input_bits.test(bit);
        }

        result[BITS * 3] = true;

        return result;
    }
};

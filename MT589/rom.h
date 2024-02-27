#ifndef ROM_H
#define ROM_H

#include "mt_global.h"

struct microcommand {

    std::bitset<7> AC;
    BYTE FC : 4;
    std::bitset<7> F;
    WORD K;
    BYTE ED : 1;
    BYTE EA : 1;
    BYTE LD : 1;
    // RAMC
    BYTE CS : 1;
    BYTE RW : 1;
    [[maybe_unused]] WORD M;
    [[maybe_unused]] WORD I;
    //  UI
    int index_F;
    int index_REG;
    int index_FIC;
    int index_FOC;
    int index_Jump;
    std::string address_control = "0000000";

    // For naming
    std::string tag = ""; // -->v
    // For programming mode -->^
    bool is_command_entrypoint = false;
    size_t command_code = 0;

    bool is_empty() const;
    void set_empty(bool flag);
    microcommand& operator=(const microcommand& mc) = default;
private:
    bool empty = true;
};

struct ROM
{
    ROM();
    ROM& operator=(const ROM& rom);
    void write(size_t row, size_t col, microcommand cmd);
    microcommand getMicrocommand(size_t row, size_t col) const;
    bool is_nop(size_t row, size_t col) const;

    std::vector<std::vector<microcommand>> memory;
    const size_t _rows = 0x20; // 32
    const size_t _cols = 0x10; // 16

    // for upgrade
    using Args = std::pair<uint16_t, uint16_t>;
    using Code = size_t;
    std::vector<std::pair<Code, Args>> program_as_commands_codes_and_args_order;
};

#endif // ROM_H

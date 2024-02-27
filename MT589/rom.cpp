#include "rom.h"
#include <iostream>

ROM::ROM() {
   memory.resize(_rows);
   for(auto& row : memory) {
       row.resize(_cols);
   }
   for (auto& row : memory) {
       for (auto& cell : row) {
           cell.F = 0b1100000;
           cell.K = 0b00000000;
           cell.FC = 0b0011;
           cell.AC = 0b0000000;
           cell.ED = 0;
           cell.EA = 0;
           cell.LD = 0;
           cell.RW = 0;
           cell.CS = 0;

       }
   }
}

bool ROM::is_nop(size_t row, size_t col) const {
    return memory[row][col].is_empty();
}

void ROM::write(size_t row, size_t col, microcommand cmd) {
    memory[row][col] = cmd;
    if (memory[row][col].is_command_entrypoint == true) {
        std::cerr<<"memory["<<row<<"]["<<col<<"]=iscmdentr"<<memory[row][col].is_command_entrypoint<<std::endl;
    }
}

microcommand ROM::getMicrocommand(size_t row, size_t col) const {
    return memory[row][col];
}

ROM& ROM::operator=(const ROM& rom) {
    // Guard self assignment
    if (this == &rom) {
        return *this;
    }
    // std::vector<std::vector<microcommand>> memory;
    // std::vector<std::pair<Code, Args>> program_as_commands_codes_and_args_order;

    memory.clear();
    memory.resize(rom.memory.size());
    std::cerr<<"ROM::operator=:"<<std::endl;
    for (size_t row = 0; row < 32; ++row) {
        memory[row].resize(rom.memory[row].size());
        for (size_t col = 0; col < 16; ++col) {
            this->write(row, col, rom.memory[row][col]);
        }
    }

    program_as_commands_codes_and_args_order.clear();
    for (size_t i =0; i<rom.program_as_commands_codes_and_args_order.size(); ++i){
        auto node = rom.program_as_commands_codes_and_args_order[i];
        program_as_commands_codes_and_args_order.push_back({node.first, {node.second.first, node.second.second}});
    }
}

bool microcommand::is_empty() const{
    return this->empty;
}

void microcommand::set_empty(bool flag) {
    this->empty = flag;
}

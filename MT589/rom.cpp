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

const microcommand& ROM::getMicrocommand(size_t row, size_t col) const {
    return memory[row][col];
}

const ROM& ROM::get() const {
    return *this;
}

ROM& ROM::operator=(const ROM& rom) {
    // Guard self assignment
    if (this == &rom) {
        return *this;
    }
    memory = rom.memory;
    program = rom.program;
    addresses_args_pairs = rom.addresses_args_pairs;
    return *this;
}

bool microcommand::is_empty() const{
    return this->empty;
}

void microcommand::set_empty(bool flag) {
    this->empty = flag;
}

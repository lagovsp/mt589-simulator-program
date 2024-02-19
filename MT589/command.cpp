#include "command.h"

// Command::Command(QObject *parent)
//     : QObject{parent}
// {}

void Command::set_name(const std::string& name) {
    command_name = name;
}

void Command::set_call_address_x(const size_t ax) {
    call_address_x = ax;
}

void Command::set_call_address_y(const size_t ay) {
    call_address_y = ay;
}

void Command::set_code(const size_t command_code) {
    code = command_code;
}


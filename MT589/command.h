#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>

class Command
{
private:
    std::string command_name = "unknown";
    size_t call_address_x = 0;
    size_t call_address_y = 0;
    size_t code = 0;

public:
    Command() = default;

    Command(const Command&) = default;

    void set_name(const std::string& name);
    void set_call_address_x(const size_t ax);
    void set_call_address_y(const size_t ay);
    void set_code(const size_t command_code);

//signals:
};

#endif // COMMAND_H

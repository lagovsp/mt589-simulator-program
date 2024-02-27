#include <filemanager.h>
#include <iostream>

std::vector<std::string> fm::get_ram(const std::string& filename) {
    json data = json::parse(read_from_file(filename));
    std::vector<std::string> ram;
    for (size_t i = 0; i < 256; ++i) {
        ram.push_back(data[i].get<std::string>());
    }
    return ram;
}

void fm::save_ram(const std::string& filename,
                                  const std::vector<std::string>& data) {
    json json_data;
    for (size_t i = 0; i < 256; ++i) { json_data[i] = data[i]; }
    fm::write_to_file(filename, json_data);
}

void fm::save_isa(const std::string& filename, const fm::isa_data& data) {
    json json_data;

    json_data["regs"] = {};
    json_data["commands"] = {};

    for (const auto& [key, value] : data.isa_regs) {
        json_data["regs"][key] = value;
    }

    for (const auto& [key, value] : data.isa_commands) {
        json_data["commands"][key] = value;
    }

    fm::write_to_file(filename, json_data);
}

fm::isa_data fm::get_isa_data(const std::string& filename) {
    json data = json::parse(read_from_file(filename));

    fm::isa_data isa_data;

    for (const auto& item: data["regs"].items()) {
        isa_data.isa_regs[item.key()] = item.value().get<std::string>();
    }

    for (const auto& item: data["commands"].items()) {
        isa_data.isa_commands[item.key()] = item.value().get<int>();
    }
    return isa_data;
}

fm::programm_data fm::get_data(const std::string& filename) {
    std::cerr<<"entering get_data"<<std::endl;
    json data = json::parse(read_from_file(filename));

    int start_row = -1;
    int start_col = -1;
    MK589 mk;

    start_row = data["start"]["row"].get<int>();
    start_col = data["start"]["col"].get<int>();

    size_t commands = 0;
    for (size_t col = 0; col < 16; ++col) {
        for (size_t row = 0; row < 32; ++row) {
            microcommand command;
            json command_data = data["matrix"][std::to_string(row) + "-" + std::to_string(col)];

            command.set_empty(command_data["empty"].get<bool>());
            command.F = std::bitset<7>(command_data["F"].get<std::string>());
            command.I = command_data["I"].get<int>();
            command.index_F = command_data["index_F"].get<int>();
            command.index_FIC = command_data["index_FIC"].get<int>();
            command.index_FOC = command_data["index_FOC"].get<int>();
            command.index_Jump = command_data["index_Jump"].get<int>();
            command.address_control = command_data["address_control"].get<std::string>();

            command.RW  = command_data["RW"].get<int>();
            command.LD  = command_data["LD"].get<int>();
            command.CS  = command_data["CS"].get<int>();
            command.EA  = command_data["EA"].get<int>();
            command.ED  = command_data["ED"].get<int>();

            command.FC = command_data["FC"].get<int>();
            command.AC = std::bitset<7>(command_data["AC"].get<std::string>());
            command.K = command_data["K"].get<int>();

            // Upgrade to command mode
            command.tag = command_data["Tag"].get<std::string>();
            command.is_command_entrypoint = command_data["is_command_entrypoint"].get<bool>();
            if (command_data["is_command_entrypoint"].get<bool>() == true) { ++commands; }
            // std::cerr<<"READ TAG"<<command.tag<<" ";
            command.command_code = command_data["command_code"].get<size_t>();

            mk.rom.write(row, col, command);
        }
    }
    std::cerr<<"ENTRYPOINTS READ "<<commands<<std::endl;

    if (data.contains("program")) {
        json commands_infos = data["program"];

        for (auto it = commands_infos.cbegin(); it != commands_infos.cend(); ++it) {
            auto command_code = (*it)["command_code"].get<size_t>();
            auto arg1 = (*it)["arg1"].get<uint16_t>();
            auto arg2 = (*it)["arg2"].get<uint16_t>();
            std::cerr<<"gonna add command "<<command_code<<" "<<arg1<<" "<<arg2<<";;;;";
            mk.rom.program_as_commands_codes_and_args_order.push_back({command_code, {arg1, arg2}});
            auto it1 = mk.rom.program_as_commands_codes_and_args_order.back();
            std::cerr<<"added ccode "<<it1.first<<" arg1 "<<it1.second.first<<" arg2 "<<it1.second.second<<std::endl;
        }
    }

    fm::programm_data prog_data;
    prog_data.start_row = start_row;
    prog_data.start_col = start_col;
    prog_data.mk = mk;

    std::cerr<<"exiting get_data"<<std::endl;
    return prog_data;
}

void fm::save(const std::string& filename, MK589& mk, int startCol, int startRow) {
    json data;
    data["start"] = { { "row", startRow }, { "col", startCol }};
    data["matrix"] =  {};

    for (size_t col = 0; col < 16; ++col) {
        for (size_t row = 0; row < 32; ++row) {
            microcommand command = mk.rom.getMicrocommand(row, col);
            json command_data;

            command_data["empty"] = command.is_empty();
            command_data["F"] = command.F.to_string();
            command_data["I"] = command.I;
            command_data["index_F"] = command.index_F;
            command_data["index_FIC"] = command.index_FIC;
            command_data["index_FOC"] = command.index_FOC;
            command_data["index_Jump"] = command.index_Jump;
            command_data["address_control"] = command.address_control;
            command_data["RW"] = int(command.RW);
            command_data["LD"] = int(command.LD);
            command_data["CS"] = int(command.CS);
            command_data["FC"] = int(command.FC);
            command_data["EA"] = int(command.EA);
            command_data["ED"] = int(command.ED);
            command_data["AC"] = command.AC.to_string();
            command_data["K"] = command.K;

            // for command name
            command_data["Tag"] = command.tag;
            command_data["is_command_entrypoint"] = command.is_command_entrypoint;
            command_data["command_code"] = command.command_code;

            data["matrix"][std::to_string(row) + "-" + std::to_string(col)] = command_data;
        }
    }

    json commands_array = {};
    for (auto it = mk.rom.program_as_commands_codes_and_args_order.cbegin(); it != mk.rom.program_as_commands_codes_and_args_order.cend(); ++it) {
        json command;
        command["command_code"] = (*it).first;
        command["arg1"] = (*it).second.first;
        command["arg2"] = (*it).second.second;
        commands_array.push_back(command);
    }
    data["program"] = commands_array;
    fm::write_to_file(filename, data);
}

void fm::write_to_file(const std::string& filename, const json& data) {
    std::ofstream file(filename);
    file << data.dump(4);
    file.close();
}

std::string fm::read_from_file(const std::string& filename) {
    std::ifstream file(filename);
    std::string content( (std::istreambuf_iterator<char>(file) ),
                           (std::istreambuf_iterator<char>()) );

    file.close();
    return content;
}

#include "commandmodewindow.h"
#include "ui_commandmodewindow.h"
#include <filemanager.h>
#include <mainwindow.h>
#include <QFileDialog>
#include <QBrush>
#include <QString>
#include <createisa.h>
#include <iostream>
#include <unordered_set>


void CommandModeWindow::scanProgram() {
    program.clear();

    std::cerr << "SCANPR"<<mk.rom.program_as_commands_codes_and_args_order.size()<< std::endl;

    for(const auto& entry: mk.rom.program_as_commands_codes_and_args_order) {
        size_t code = entry.first;
        uint16_t arg1 = entry.second.first;
        uint16_t arg2 = entry.second.second;

        // std::vector<std::shared_ptr<QTableWidgetItem>> program_command;

        // QTableWidgetItem* item1 = new QTableWidgetItem();
        // program_command.push_back(std::shared_ptr<QTableWidgetItem>(item1));
        // std::stringstream ss1;
        // ss1 << code;
        // item1->setText(QString(ss1.str().c_str()));

        // QTableWidgetItem* item2 = new QTableWidgetItem();
        // program_command.push_back(std::shared_ptr<QTableWidgetItem>(item2));
        // std::string name = code_to_name_and_address.at(code).first;
        // item2->setText(QString(name.c_str()));

        // QTableWidgetItem* item3 = new QTableWidgetItem();
        // program_command.push_back(std::shared_ptr<QTableWidgetItem>(item3));
        // std::stringstream ss2;
        // ss2 << arg1;
        // item3->setText(QString(ss2.str().c_str()));

        // QTableWidgetItem* item4 = new QTableWidgetItem();
        // program_command.push_back(std::shared_ptr<QTableWidgetItem>(item4));
        // std::stringstream ss3;
        // ss3 << arg2;
        // item4->setText(QString(ss3.str().c_str()));

        program.push_back({code, {arg1, arg2}});
    }
}

void CommandModeWindow::displayProgram() {
    ui->programWidget->horizontalHeader()->sectionResizeMode(QHeaderView::Fixed);

    size_t read_commands_counter = 0;
    for (const auto& command_info: program) {
        auto code = command_info.first;
        auto arg1 = command_info.second.first;
        auto arg2 = command_info.second.second;

        std::vector<std::shared_ptr<QTableWidgetItem>> called_command;
        QTableWidgetItem item = QTableWidgetItem();

        std::stringstream ss1;
        ss1 << code;
        item.setText(QString(ss1.str().c_str()));
        called_command.push_back(std::make_shared<QTableWidgetItem>(item));

        std::string name = code_to_name_and_address.at(code).first; // at to code that disappears from the map ? why????
        item.setText(QString(name.c_str()));
        called_command.push_back(std::make_shared<QTableWidgetItem>(item));

        std::stringstream ss2;
        ss2 << arg1;
        item.setText(QString(ss2.str().c_str()));
        called_command.push_back(std::make_shared<QTableWidgetItem>(item));

        std::stringstream ss3;
        ss3 << arg2;
        item.setText(QString(ss3.str().c_str()));
        called_command.push_back(std::make_shared<QTableWidgetItem>(item));

        command_list_widget_matrix.push_back(called_command);
        ui->programWidget->setItem(read_commands_counter, 0, command_list_widget_matrix.back()[0].get());
        ui->programWidget->setItem(read_commands_counter, 1, command_list_widget_matrix.back()[1].get());
        ui->programWidget->setItem(read_commands_counter, 2, command_list_widget_matrix.back()[2].get());
        ui->programWidget->setItem(read_commands_counter, 3, command_list_widget_matrix.back()[3].get());
        ++read_commands_counter;
    }
}

void CommandModeWindow::setupCommandPool() {
    code_to_name_and_address.clear();
    program.clear();
    // std::vector<std::pair<Code, Args>> program_as_commands_codes_and_args_order;
    size_t c= 0;
    std::unordered_set<size_t> codes_set;
    for (size_t col = 0; col < 16; ++col) {
        for (size_t row = 0; row < 32; ++row) {
            auto mc = mk.rom.memory[row][col];
            if (mc.is_command_entrypoint == false) {
                continue;
            }
            std::cerr<<"ccode: "<<mc.command_code<<std::endl;
            ++c;
            codes_set.insert(mc.command_code);
            code_to_name_and_address.insert({mc.command_code, {mc.tag, {row, col}}});
            // std::vector<std::shared_ptr<QTableWidgetItem>> command_description;
            // QTableWidgetItem item = QTableWidgetItem();

            // item.setText(("r" + std::to_string(row) + "-c" + std::to_string(col)).c_str());
            // command_description.push_back(std::make_shared<QTableWidgetItem>(item));

            // std::stringstream ss;
            // ss << std::bitset<8>(mc.command_code);
            // item.setText(ss.str().c_str());
            // command_description.push_back(std::make_shared<QTableWidgetItem>(item));

            // item.setText(mc.tag.c_str());
            // command_description.push_back(std::make_shared<QTableWidgetItem>(item));

            // command_pool.push_back(command_description);
            // std::cerr<<std::to_string(mc.command_code)<<std::endl;
            // code_to_name_and_address.insert({mc.command_code, {mc.tag, {row, col}}});
        }
    }
    std::cerr <<"SETUPCOMPOOL"<< code_to_name_and_address.size()<<" ; c = "<<c<<"; set size = "<<codes_set.size()<< std::endl;
}

void CommandModeWindow::displayCommandPool() {
    ui->commandPoolTableWidget->setHorizontalHeaderLabels({"Адрес вызова", "Код", "Имя"});
    ui->commandPoolTableWidget->horizontalHeader()->sectionResizeMode(QHeaderView::Fixed);
    size_t read_commands_counter = 0;

    for (const auto& [code, value] : code_to_name_and_address) {
        auto name = value.first;
        auto row = value.second.first;
        auto col = value.second.second;

        std::vector<std::shared_ptr<QTableWidgetItem>> command_description;
        QTableWidgetItem item = QTableWidgetItem();

        item.setText(("r" + std::to_string(row) + "-c" + std::to_string(col)).c_str());
        command_description.push_back(std::make_shared<QTableWidgetItem>(item));

        std::stringstream ss;
        ss << std::bitset<8>(code);
        item.setText(ss.str().c_str());
        command_description.push_back(std::make_shared<QTableWidgetItem>(item));

        item.setText(name.c_str());
        command_description.push_back(std::make_shared<QTableWidgetItem>(item));

        command_pool_widget_matrix.push_back(command_description);

        ui->commandPoolTableWidget->setItem(read_commands_counter, 0, command_pool_widget_matrix.back()[0].get());
        ui->commandPoolTableWidget->setItem(read_commands_counter, 1, command_pool_widget_matrix.back()[1].get());
        ui->commandPoolTableWidget->setItem(read_commands_counter, 2, command_pool_widget_matrix.back()[2].get());
        ++read_commands_counter;
    }
}

std::string CommandModeWindow::toHex(unsigned int value) {
    std::stringstream stream;
    stream << std::hex << value;
    std::string str = stream.str();
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    while (str.size() < 4) {
        str = "0" + str;
    }
    str = "0x" + str;
    return str;
}

unsigned int CommandModeWindow::parseHex(const std::string& str) {
    std::stringstream stream;
    stream << std::hex << str;
    unsigned int value = 0;
    stream >> value;
    return value;
}

CommandModeWindow::CommandModeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CommandModeWindow)
{
    ui->setupUi(this);
    setupRegs();
    QStringList verlist;

    for (size_t i = 0; i < mk.ram.size; ++i) {
        verlist << std::to_string(i).c_str();
    }

    ui->programWidget->setVerticalHeaderLabels(verlist);
    ui->programWidget->setHorizontalHeaderLabels({"Адрес", "Команда", "Операнд 1", "Операнд 2"});
    ui->programWidget->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored);
    ui->programWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    for (size_t i = 0; i < mk.ram.size; ++i) {
            QTableWidgetItem* item = new QTableWidgetItem();
            items.push_back(std::shared_ptr<QTableWidgetItem>(item));
            ui->programWidget->setItem(i, 0, item);
            item->setTextAlignment(Qt::AlignCenter);
    }
    loaded = true;
    PC = mk.MEM;

    on_resetButton_clicked();

    reg_labels.push_back(ui->reg0lbl);
    reg_labels.push_back(ui->reg1lbl);
    reg_labels.push_back(ui->reg2lbl);
    reg_labels.push_back(ui->reg3lbl);
    reg_labels.push_back(ui->reg4lbl);
    reg_labels.push_back(ui->reg5lbl);
    reg_labels.push_back(ui->reg6lbl);
    reg_labels.push_back(ui->reg7lbl);
    reg_labels.push_back(ui->reg8lbl);
    reg_labels.push_back(ui->reg9lbl);
}

CommandModeWindow::~CommandModeWindow()
{
    delete ui;
}

void CommandModeWindow::on_open_rom_triggered()
{
    if (!loaded) { return; }
    romWindow.show();
    romWindow.mk = mk;
    romWindow.setupItems();
}

void CommandModeWindow::on_open_triggered()
{
    if (!loaded) { return; }
    std::string filename = QFileDialog::getOpenFileName(this, tr("Open ROM"),
                                                    "~/Desktop/prog.rom",
                                                    tr("*.rom")).toStdString();
    fm::programm_data data = fm::get_data(filename);
    this->mk = data.mk;
    PC = mk.MEM;
}

void CommandModeWindow::setupRegs() {
   regLCDs.push_back(std::shared_ptr<QLCDNumber>(ui->reg0));
   regLCDs.push_back(std::shared_ptr<QLCDNumber>(ui->reg1));
   regLCDs.push_back(std::shared_ptr<QLCDNumber>(ui->reg2));
   regLCDs.push_back(std::shared_ptr<QLCDNumber>(ui->reg3));
   regLCDs.push_back(std::shared_ptr<QLCDNumber>(ui->reg4));
   regLCDs.push_back(std::shared_ptr<QLCDNumber>(ui->reg5));
   regLCDs.push_back(std::shared_ptr<QLCDNumber>(ui->reg6));
   regLCDs.push_back(std::shared_ptr<QLCDNumber>(ui->reg7));
   regLCDs.push_back(std::shared_ptr<QLCDNumber>(ui->reg8));
   regLCDs.push_back(std::shared_ptr<QLCDNumber>(ui->reg9));
}

void CommandModeWindow::update_on_cpu_data() {
  for (size_t i = 0; i < 10; ++i) {
      regLCDs[i]->display(mk.MEM[i]);
  }
  ui->regMAR->display(mk.MAR);
  ui->regT->display(mk.MEM[T]);
  ui->regAC->display(mk.MEM[AC]);

  if (mk.RO) {
      ui->ROlcd->display(mk.RO.value());
  } else {
      ui->ROlcd->display("-");
  }
  if (mk.CO) {
      ui->COlcd->display(mk.CO.value());
  } else {
      ui->COlcd->display("-");
  }
  if (mk.D) {
      ui->Dlcd->display(mk.D.value());
  } else {
      ui->Dlcd->display("-");
  }
  if (mk.A) {
      ui->Alcd->display(mk.A.value());
  } else {
      ui->Alcd->display("-");
  }
}

void CommandModeWindow::on_open_microcommand_mode_triggered()
{
    if (!loaded) { return; }
    MainWindow* window = new MainWindow();
    window->mk = mk;
    window->model = model;
    window->show();
    this->hide();
    window->setupItems();
    window->fillInputs();
}

void CommandModeWindow::on_resetButton_clicked()
{
    if (!loaded) { return; }
    WORD oldRow = *PC;
    mk.reset();
    mk.load(0b10100000);
    update_on_cpu_data();

    changeCurrentRow(oldRow, *PC);
}

void CommandModeWindow::on_stepButton_clicked()
{
    if (!loaded) { return; }
    bool is_loadmem_prog_running = true;
    WORD oldRow = *PC;
    while (is_loadmem_prog_running) {
        size_t r = mk.get_row_adr();
        size_t c = mk.get_col_adr();
        auto command = mk.rom.getMicrocommand(r, c);
        if (command.is_empty()) { return; }
        if (command.CS == 0b1 and command.RW == 0b0 and command.EA == 0b1) {
            // read
            command.M = mk.ram.read(mk.MAR);
        } else {
            command.M = 0x00;
        }

        if (command.LD == 0b1) {
            is_loadmem_prog_running = false;
        }
        mk.do_fetch_decode_execute_cycle(command);
//        if (command.CS == 0b1 and command.RW == 0b1 and mk.EA == 0b1 and mk.ED == 0b1) {
//            // write
//            items[mk.A.value()]->setText(toHex(mk.D.value()).c_str());
//        }
    }
    size_t current_row = mk.get_row_adr();
    size_t current_col = mk.get_col_adr();

    while (current_row != 0 && current_col != 10) {
        auto command = mk.rom.getMicrocommand(current_row, current_col);
        if (command.is_empty()) { break; }
        if (command.CS == 0b1 and command.RW == 0b0 and command.EA == 0b1) {
            // read
            command.M = mk.ram.read(mk.MAR);
        } else {
            command.M = 0x00;
        }
        mk.do_fetch_decode_execute_cycle(command);
        if (command.CS == 0b1 and command.RW == 0b1 and mk.EA == 0b1 and mk.ED == 0b1) {
            // write
            mkwrite = true;
            items[mk.A.value()]->setText(toHex(mk.D.value()).c_str());
            mkwrite = false;
        }
        current_row = mk.get_row_adr();
        current_col = mk.get_col_adr();
    }
    WORD newRow = *PC;
    changeCurrentRow(oldRow, newRow);
    update_on_cpu_data();
}

void CommandModeWindow::on_runButton_clicked()
{
    if (!loaded) { return; }
    while (true) {
        bool is_loadmem_prog_running = true;
        WORD oldRow = *PC;
        while (is_loadmem_prog_running) {
            size_t r = mk.get_row_adr();
            size_t c = mk.get_col_adr();
            auto command = mk.rom.getMicrocommand(r, c);
            if (command.is_empty()) { return; }
            if (command.CS == 0b1 and command.RW == 0b0 and command.EA == 0b1) {
                // read
                command.M = mk.ram.read(mk.MAR);
            } else {
                command.M = 0x00;
            }

            if (command.LD == 0b1) {
                is_loadmem_prog_running = false;
            }
            mk.do_fetch_decode_execute_cycle(command);
    //        if (command.CS == 0b1 and command.RW == 0b1 and mk.EA == 0b1 and mk.ED == 0b1) {
    //            // write
    //            items[mk.A.value()]->setText(toHex(mk.D.value()).c_str());
    //        }
        }
        size_t current_row = mk.get_row_adr();
        size_t current_col = mk.get_col_adr();

        while (current_row != 0 && current_col != 10) {
            auto command = mk.rom.getMicrocommand(current_row, current_col);
            if (command.is_empty()) { break; }
            if (command.CS == 0b1 and command.RW == 0b0 and command.EA == 0b1) {
                // read
                command.M = mk.ram.read(mk.MAR);
            } else {
                command.M = 0x00;
            }
            mk.do_fetch_decode_execute_cycle(command);
            if (command.CS == 0b1 and command.RW == 0b1 and mk.EA == 0b1 and mk.ED == 0b1) {
                // write
                mkwrite = true;
                items[mk.A.value()]->setText(toHex(mk.D.value()).c_str());
                mkwrite = false;
            }
            current_row = mk.get_row_adr();
            current_col = mk.get_col_adr();
        }
        WORD newRow = *PC;
        changeCurrentRow(oldRow, newRow);
        update_on_cpu_data();
    }
}

void CommandModeWindow::on_endButton_clicked()
{
    if (!loaded) { return; }
}

WORD CommandModeWindow::parseCommand(const std::string& str) {
    if (!loaded) {
        return 0;
    }
    std::stringstream ss(str);
    std::string cmd;
    ss >> cmd;

    if (isa_commands.count(cmd) == 0) {
        return 0;
    }
    if (isa_commands[cmd] > 0xFF) {
        return isa_commands[cmd];
    }
    WORD com = isa_commands[cmd];
    std::string arg;
    ss >> arg;
    com = (com << 8) | (arg.empty() ? 0 : parseHex(arg));
    return com;
}

void CommandModeWindow::on_programWidget_cellChanged(int row, int column) {
    if (!loaded) { return; }

    std::string rowContent = items[row]->text().toStdString();
    if (rowContent.empty()) { return; }
    // (mk.EA == 0b1 and mk.ED == 0b1) {
    if (mkwrite) {
        auto data = parseHex(rowContent);
        mk.ram.write(row, data);
        return;
    }
    WORD word = 0;
    if (row < 200) {
        word = parseCommand(rowContent);
        if (!word) {
            items[row]->setText("");
        }
    } else {
        word = parseHex(rowContent);
    }
    mk.ram.write(row, word);
}

void CommandModeWindow::changeCurrentRow(WORD oldRow, WORD newRow) {
    items[oldRow]->setBackground(QBrush(Qt::transparent));
    items[newRow]->setBackground(QBrush(Qt::red));
}

void CommandModeWindow::on_load_isa_triggered()
{
    if (!loaded) { return; }
    // std::string filename = QFileDialog::getOpenFileName(this, tr("Open ISA file"),
    //                                                 "~/Desktop/prog.isad",
    //                                                 tr("*.isad")).toStdString();

    // if (filename.empty()) { return; }
    // fm::isa_data data = fm::get_isa_data(filename);
    // isa_commands = data.isa_commands;
    // isa_regs = data.isa_regs;
    // prepareISAWindowText();
}

// void CommandModeWindow::on_load_rom_triggered()
// {
//     std::string filename = QFileDialog::getOpenFileName(this, tr("Open project"),
//                                                     "~/Desktop/prog.rom",
//                                                     tr("*.rom")).toStdString();

//     if (filename.empty()) { return; }
//     fm::programm_data data = fm::get_data(filename);

//     mk = data.mk;
//     PC = mk.MEM;

//     // add modify command pool here
//     // add modify program listing here
//     setupCommandPool();
//     displayCommandPool();
//     scanProgram();
//     displayProgram();
// }

void CommandModeWindow::on_createISAButton_clicked()
{
    // isaWindow.show();
}

void CommandModeWindow::prepareISAWindowText() {
    // std::string text = "#ISA\n\n";

    // for (const auto& [key, value] : isa_regs) {
    //     text += "# " + key + " - " + value + "\n";
    // }

    // text += "\nCOMMANDS:\n\n";

    // size_t count = 0;
    // for (const auto& [key, value] : isa_commands) {
    //     text += key + " - " + toHex(value) + "\n";
    //     count++;
    // }

    // ui->isaText->setText(text.c_str());

    // for (size_t i = 0; i < reg_labels.size(); ++i) {
    //     std::string default_name = "REG" + std::to_string(i);
    //     if (isa_regs.count(default_name)) {
    //         reg_labels[i]->setText(isa_regs[default_name].c_str());
    //     } else {
    //         reg_labels[i]->setText(default_name.c_str());
    //     }
    // }
}

void CommandModeWindow::on_load_ram_triggered()
{
    if (!loaded) { return; }
    std::string filename = QFileDialog::getOpenFileName(this, tr("Load RAM"),
                                                    "~/Desktop/ram.ramdata",
                                                    tr("*.ramdata")).toStdString();
    if (filename.empty()) {
        return;
    }
    current_filename = filename;

    auto ram = fm::get_ram(filename);
    for (size_t i = 0; i < ram.size(); ++i) {
        items[i]->setText(ram[i].c_str());
        std::string rowContent = ram[i];
        if (rowContent.empty()) { continue; }
        // (mk.EA == 0b1 and mk.ED == 0b1) {
        if (mkwrite) {
            auto data = parseHex(rowContent);
            mk.ram.write(i, data);
            return;
        }
        WORD word = 0;
        if (i < 200) {
            word = parseCommand(rowContent);
            if (!word) {
                items[i]->setText("");
            }
        } else {
            word = parseHex(rowContent);
        }
        mk.ram.write(i, word);
    }
    on_resetButton_clicked();
}

void CommandModeWindow::on_save_triggered()
{
    if (!loaded) { return; }
    if (current_filename.empty()) {
        QString filename = QFileDialog::getSaveFileName(this, tr("Save RAM"),
                                   "~/prog.ramdata",
                                   tr("*.ramdata"));

       current_filename = filename.toStdString();
    }
    std::vector<std::string> vec = {};
    for (auto item: items) {
        vec.push_back(item->text().toStdString());
    }
    fm::save_ram(current_filename, vec);
}

void CommandModeWindow::on_save_as_triggered()
{
    if (!loaded) { return; }
    QString filename = QFileDialog::getSaveFileName(this, tr("Save RAM"),
                                                   "~/prog.ramdata",
                                                   tr("*.ramdata"));

    current_filename = filename.toStdString();
    std::vector<std::string> vec = {};
    for (auto item: items) {
        vec.push_back(item->text().toStdString());
    }
    fm::save_ram(current_filename, vec);
}

void CommandModeWindow::on_addFuncToPool_clicked()
{
    if (!loaded) { return; }
//     Command new_command;
//     new_command.set_name("test1");
//     new_command.set_call_address_x(0);
//     new_command.set_call_address_y(0);
//     new_command.set_code(3);
}


void CommandModeWindow::on_save_rom_as_triggered()
{
    if (!loaded) { return; }
    QString filename = QFileDialog::getSaveFileName(this, tr("Save project"),
                                                      "~/Desktop/prog.rom",
                                                      tr("*.rom"));

    fm::save(filename.toStdString(), this->mk, model.startPoint.row, model.startPoint.col);
    model.current_filename = filename.toStdString();
}


void CommandModeWindow::on_programWidget_cellClicked(int row, int column)
{
    if (!loaded) { return; }
    if (row >= mk.rom.program_as_commands_codes_and_args_order.size()) {
        return;
    }

    auto node = mk.rom.program_as_commands_codes_and_args_order[row];

    std::stringstream ss1;
    ss1 << std::bitset<8>(node.first);
    ui->codeBox->setText(QString(ss1.str().c_str()));

    std::stringstream ss2;
    ss2 << node.second.first;
    ui->op1Box->setText(QString(ss2.str().c_str()));

    std::stringstream ss3;
    ss2 << node.second.second;
    ui->op2Box->setText(QString(ss3.str().c_str()));
}


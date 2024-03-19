#include "commandmodewindow.h"
#include "ui_commandmodewindow.h"
#include <filemanager.h>
#include <mainwindow.h>
#include <QFileDialog>
#include <QBrush>
#include <QString>
#include <createisa.h>
#include <iostream>


void CommandModeWindow::displayCommandPool() {
    auto &cpwm = mainWindow->command_pool_widget_matrix;

    QStringList labels = {"Адрес", "Имя"};
    ui->commandPoolTableWidget->setHorizontalHeaderLabels(labels);
    ui->commandPoolTableWidget->horizontalHeader()->sectionResizeMode(QHeaderView::Fixed);

    ui->commandPoolTableWidget->setRowCount(cpwm.size()); // ?
    ui->commandPoolTableWidget->setColumnCount(labels.size());
    ui->commandPoolTableWidget->setHorizontalHeaderLabels(labels);
    ui->commandPoolTableWidget->horizontalHeader()->sectionResizeMode(QHeaderView::Fixed);
    ui->commandPoolTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    for (size_t i = 0; i < cpwm.size(); ++i) {
        for (size_t j = 0; j < labels.size(); ++j) {
            ui->commandPoolTableWidget->setItem(i, j, cpwm[i][j]);
            ui->commandPoolTableWidget->setItem(i, j, cpwm[i][j]);
        }
    }
}

void CommandModeWindow::displayScannedProgram() {
    ui->programWidget->horizontalHeader()->sectionResizeMode(QHeaderView::Fixed);

    auto &clwm = mainWindow->command_list_widget_matrix;
    for (size_t i = 0; i < mainWindow->command_list_widget_matrix.size(); ++i) {
        ui->programWidget->setItem(i, 0, clwm[i][0]);
        ui->programWidget->setItem(i, 1, clwm[i][1]);
        ui->programWidget->setItem(i, 2, clwm[i][2]);
        ui->programWidget->setItem(i, 3, clwm[i][3]);
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
            item->setTextAlignment(Qt::AlignCenter);
            items.push_back(item);
            ui->programWidget->setItem(i, 0, item);
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
    romWindow.mk = mk;
    romWindow.setupItems();
    romWindow.show();
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
   regLCDs.push_back(ui->reg0);
   regLCDs.push_back(ui->reg1);
   regLCDs.push_back(ui->reg2);
   regLCDs.push_back(ui->reg3);
   regLCDs.push_back(ui->reg4);
   regLCDs.push_back(ui->reg5);
   regLCDs.push_back(ui->reg6);
   regLCDs.push_back(ui->reg7);
   regLCDs.push_back(ui->reg8);
   regLCDs.push_back(ui->reg9);
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
    loaded = false;

    MainWindow* window = new MainWindow();
    window->mk = mk;
    window->model = model;

    window->setupItems();
    window->fillInputs();
    this->hide();
    window->show();

    loaded = true;
}

void CommandModeWindow::on_resetButton_clicked()
{
    if (!loaded) { return; }
    loaded = false;
    //WORD oldRow = *PC;
    mk.reset();
    //mk.load(0b10100000);
    //update_on_cpu_data();

    // changeCurrentRow(oldRow, *PC);
    cur_command_number = 0;
    cur_microcommand_in_cur_command = 0;
    loaded = true;
}

void CommandModeWindow::displayTrackerCommands(int cit) {
    if (cit >= mk.rom.program.size()) {
        return;
    }
    // if (!loaded) { return; }
    // loaded = false;
    std::cerr<<"void CommandModeWindow::displayTrackerCommands:"<<cit<<"\n";
    mainWindow->command_list_widget_matrix[cit][0]->setBackground(mainWindow->currentRunningColor);
    mainWindow->command_list_widget_matrix[cit][1]->setBackground(mainWindow->currentRunningColor);
    mainWindow->command_list_widget_matrix[cit][2]->setBackground(mainWindow->currentRunningColor);
    mainWindow->command_list_widget_matrix[cit][3]->setBackground(mainWindow->currentRunningColor);
    // loaded=true;
}

void CommandModeWindow::undisplayTrackerCommands(int cit) {
    if (cit >= mk.rom.program.size()) {
        return;
    }
    // if (!loaded) { return; }
    // loaded = false;
    std::cerr<<"void CommandModeWindow::undisplayTrackerCommands:"<<cit<<"\n";
    mainWindow->command_list_widget_matrix[cit][0]->setBackground(mainWindow->transparentColor);
    mainWindow->command_list_widget_matrix[cit][1]->setBackground(mainWindow->transparentColor);
    mainWindow->command_list_widget_matrix[cit][2]->setBackground(mainWindow->transparentColor);
    mainWindow->command_list_widget_matrix[cit][3]->setBackground(mainWindow->transparentColor);
    // loaded=true;
}

void CommandModeWindow::undisplayTrackerMicroCommand(int mcit) {
    if (mcit >= selected_command_microcommands.size()) {
        return;
    }
    std::cerr<<"void CommandModeWindow::undisplayTrackerMicroCommand:"<<mcit<<"\n";
    microcommand_list_widget_matrix[mcit][0]->setBackground(mainWindow->transparentColor);
    microcommand_list_widget_matrix[mcit][1]->setBackground(mainWindow->transparentColor);
    microcommand_list_widget_matrix[mcit][2]->setBackground(mainWindow->transparentColor);
    microcommand_list_widget_matrix[mcit][3]->setBackground(mainWindow->transparentColor);
    microcommand_list_widget_matrix[mcit][4]->setBackground(mainWindow->transparentColor);
    microcommand_list_widget_matrix[mcit][5]->setBackground(mainWindow->transparentColor);
}

void CommandModeWindow::displayTrackerMicroCommand(int mcit) {
    if (mcit >= selected_command_microcommands.size()) {
        return;
    }
    std::cerr<<"void CommandModeWindow::displayTrackerMicroCommand:"<<mcit<<"\n";
    microcommand_list_widget_matrix[mcit][0]->setBackground(mainWindow->currentRunningColor);
    microcommand_list_widget_matrix[mcit][1]->setBackground(mainWindow->currentRunningColor);
    microcommand_list_widget_matrix[mcit][2]->setBackground(mainWindow->currentRunningColor);
    microcommand_list_widget_matrix[mcit][3]->setBackground(mainWindow->currentRunningColor);
    microcommand_list_widget_matrix[mcit][4]->setBackground(mainWindow->currentRunningColor);
    microcommand_list_widget_matrix[mcit][5]->setBackground(mainWindow->currentRunningColor);
}

void CommandModeWindow::displayScannedMicroListing() {
    ui->currentCommandMicroListingWidget->horizontalHeader()->sectionResizeMode(QHeaderView::Fixed);

    for (size_t i = 0; i < microcommand_list_widget_matrix.size(); ++i) {
        ui->currentCommandMicroListingWidget->setItem(i, 0, microcommand_list_widget_matrix[i][0]);
        ui->currentCommandMicroListingWidget->setItem(i, 1, microcommand_list_widget_matrix[i][1]);
        ui->currentCommandMicroListingWidget->setItem(i, 2, microcommand_list_widget_matrix[i][2]);
        ui->currentCommandMicroListingWidget->setItem(i, 3, microcommand_list_widget_matrix[i][3]);
        ui->currentCommandMicroListingWidget->setItem(i, 4, microcommand_list_widget_matrix[i][4]);
        ui->currentCommandMicroListingWidget->setItem(i, 5, microcommand_list_widget_matrix[i][5]);
    }
}

void CommandModeWindow::readCurrentExecutedCommandMicroListing(size_t cit) { // do not touch
    auto current_point = Point(mk.rom.program[cit].first, mk.rom.program[cit].second);
    auto current_command = mk.getRom().getMicrocommand(current_point.row, current_point.col);

    const size_t max_jumps = 512; // 32 * 16
    size_t jumps_counter = 0;
    const bool clicked_is_empty = current_command.is_empty();

    selected_command_microcommands.clear();
    while (jumps_counter < max_jumps && !current_command.is_empty()) {
        selected_command_microcommands.push_back({current_point.row, current_point.col});

        ++jumps_counter;

        if (current_command.LD == true) { break; }
        if (current_command.AC.to_string().starts_with("00")) {
            current_point.row = (size_t)(current_command.AC.to_ulong());
        }
        if (current_command.AC.to_string().starts_with("010")) {
            current_point.row = 0;
            current_point.col = (size_t)(current_command.AC.to_ulong()) - 32;
        }
        if (current_command.AC.to_string().starts_with("011")) {
            current_point.col = (size_t)(current_command.AC.to_ulong()) - 48;
        }
        if (current_command.AC.to_string().starts_with("1110")) {
            current_point.row = (size_t)(current_command.AC.to_ulong()) - 112;
        }
        current_command = mk.getRom().getMicrocommand(current_point.row, current_point.col);
    }

    QStringList labels = {"Адрес", "Имя", "F", "K", "I", "AC"};
    ui->currentCommandMicroListingWidget->setRowCount(selected_command_microcommands.size());
    ui->currentCommandMicroListingWidget->setColumnCount(labels.size());
    ui->currentCommandMicroListingWidget->setHorizontalHeaderLabels(labels);
    ui->currentCommandMicroListingWidget->horizontalHeader()->sectionResizeMode(QHeaderView::Fixed);
    ui->currentCommandMicroListingWidget->setSelectionMode(QAbstractItemView::NoSelection);
    // command_list_widget_matrix

    microcommand_list_widget_matrix.clear();
    for (size_t it = 0; it < selected_command_microcommands.size(); ++it) {
        std::vector<QTableWidgetItem*> vectorRow;

        auto [row, column] = selected_command_microcommands[it];
        auto mc = mk.getRom().getMicrocommand(row, column);

        std::cerr << "row " << row << " col " << column << " tag " << mc.tag << " F" << mc.F << " K" << mc.K << " I" << mc.I << " AC" << mc.AC << "\n";

        QString data1 = QString::number(row) + "-" + QString::number(column);
        auto itemPtr = new QTableWidgetItem(data1);
        itemPtr->setTextAlignment(Qt::AlignCenter);
        vectorRow.push_back(itemPtr);

        QString data2 = QString::fromStdString(mc.tag);
        itemPtr = new QTableWidgetItem(data2);
        itemPtr->setTextAlignment(Qt::AlignCenter);
        vectorRow.push_back(itemPtr);

        QString data3 = QString::fromStdString(mc.F.to_string());
        itemPtr = new QTableWidgetItem(data3);
        itemPtr->setTextAlignment(Qt::AlignCenter);
        vectorRow.push_back(itemPtr);

        itemPtr = new QTableWidgetItem;
        itemPtr->setText(QString(toHex(mc.K).c_str()));
        itemPtr->setTextAlignment(Qt::AlignCenter);
        vectorRow.push_back(itemPtr);

        itemPtr = new QTableWidgetItem;
        itemPtr->setText(QString(toHex(mc.I).c_str()));
        itemPtr->setTextAlignment(Qt::AlignCenter);
        vectorRow.push_back(itemPtr);

        itemPtr = new QTableWidgetItem;
        itemPtr->setText(QString(mc.AC.to_string().c_str()));
        itemPtr->setTextAlignment(Qt::AlignCenter);
        vectorRow.push_back(itemPtr);

        microcommand_list_widget_matrix.push_back(vectorRow);
    }

    for (size_t i = 0; i < microcommand_list_widget_matrix.size(); ++i){
        for (size_t j = 0; j < microcommand_list_widget_matrix[i].size(); ++j) {
            ui->currentCommandMicroListingWidget->setItem(i, j, microcommand_list_widget_matrix[i][j]);
        }
    }
}

void CommandModeWindow::showCurrentMicroListing() {
    // QStringList labels = {"Адрес", "Имя", "F", "K", "I", "AC"};
    // ui->currentCommandMicroListingWidget->setRowCount(selected_command_microcommands.size());
    // ui->currentCommandMicroListingWidget->setColumnCount(labels.size());
    // ui->currentCommandMicroListingWidget->setHorizontalHeaderLabels(labels);
    // ui->currentCommandMicroListingWidget->horizontalHeader()->sectionResizeMode(QHeaderView::Fixed);
    // ui->currentCommandMicroListingWidget->setSelectionMode(QAbstractItemView::NoSelection);

    // for (size_t it = 0; it < selected_command_microcommands.size(); ++it) {
    //     std::vector<QTableWidgetItem*> vectorRow;

    //     auto [row, column] = selected_command_microcommands[it];
    //     auto mc = mk.getRom().getMicrocommand(row, column);

    //     std::cerr << "row " << row << " col " << column << " tag " << mc.tag << " F" << mc.F << " K" << mc.K << " I" << mc.I << " AC" << mc.AC << "\n";

    //     QString data1 = QString::number(row) + "-" + QString::number(column);
    //     auto itemPtr = new QTableWidgetItem(data1);
    //     itemPtr->setTextAlignment(Qt::AlignCenter);
    //     vectorRow.push_back(itemPtr);

    //     QString data2 = QString::fromStdString(mc.tag);
    //     itemPtr = new QTableWidgetItem(data2);
    //     itemPtr->setTextAlignment(Qt::AlignCenter);
    //     vectorRow.push_back(itemPtr);

    //     QString data3 = QString::fromStdString(mc.F.to_string());
    //     itemPtr = new QTableWidgetItem(data3);
    //     itemPtr->setTextAlignment(Qt::AlignCenter);
    //     vectorRow.push_back(itemPtr);

    //     itemPtr = new QTableWidgetItem;
    //     itemPtr->setText(QString(toHex(mc.K).c_str()));
    //     itemPtr->setTextAlignment(Qt::AlignCenter);
    //     vectorRow.push_back(itemPtr);

    //     itemPtr = new QTableWidgetItem;
    //     itemPtr->setText(QString(toHex(mc.I).c_str()));
    //     itemPtr->setTextAlignment(Qt::AlignCenter);
    //     vectorRow.push_back(itemPtr);

    //     itemPtr = new QTableWidgetItem;
    //     itemPtr->setText(QString(mc.AC.to_string().c_str()));
    //     itemPtr->setTextAlignment(Qt::AlignCenter);
    //     vectorRow.push_back(itemPtr);

    //     microcommand_list_widget_matrix.push_back(vectorRow);
    // }

    // for (size_t i = 0; i < microcommand_list_widget_matrix.size(); ++i){
    //     for (size_t j = 0; j < microcommand_list_widget_matrix[i].size(); ++j) {
    //         ui->currentCommandMicroListingWidget->setItem(i, j, microcommand_list_widget_matrix[i][j]);
    //     }
    // }
}

void CommandModeWindow::on_stepButton_clicked()
{
    std::cerr<<"void CommandModeWindow::on_stepButton_clicked:\n";

    if (!loaded) { return; }
    loaded = false;

    if (mk.rom.program.empty()) {
        return;
    }

    auto curPoint = model.currentPoint;
    std::cerr << "cur point: " << curPoint.row << "-" << curPoint.col << "\n";

    undisplayTrackerCommands(cur_command_number);
    undisplayTrackerMicroCommand(cur_microcommand_in_cur_command);

    displayScannedMicroListing();
    // undisplayTrackerMicroCommand(cur_microcommand_in_cur_command);

    bool filler = false;
    std::cerr<<"gonna launch wcem:"<<curPoint.row<<"-"<<curPoint.col<<"\n";
    auto [row, column] = with_command_execute_microcommand(curPoint.row, curPoint.col, filler);
    ++cur_microcommand_in_cur_command;

    if (mk.getRom().getMicrocommand(curPoint.row, curPoint.col).LD) { // body is right
        cur_microcommand_in_cur_command = 0;
        if (cur_command_number == mk.rom.program.size() - 1) {
            on_resetButton_clicked();
            cur_command_number = 0;
        } else {
            ++cur_command_number;
        }
        readCurrentExecutedCommandMicroListing(cur_command_number);
        std::cerr<<"will be next command:";
        auto nextCommand = mk.getRom().program[cur_command_number];
        model.currentPoint = Point(nextCommand.first, nextCommand.second);
        std::cerr<<model.currentPoint.row<<"-"<<model.currentPoint.col<<"\n";

        mk.mcu.MA = mainWindow->convertPointToBitset(Point(model.currentPoint.row, model.currentPoint.col));
        mk.mcu.MPAR = mainWindow->convertPointToBitset(Point(model.currentPoint.row, model.currentPoint.col)); // ?
    }

    showCurrentMicroListing();

    displayTrackerCommands(cur_command_number);
    displayTrackerMicroCommand(cur_microcommand_in_cur_command);

    // std::cerr<<"mk.mcu.MA:"<<mk.mcu.MA.to_string()<<"\n";
    // std::cerr<<"mk.mcu.MPAR"<<mk.mcu.MPAR.to_string()<<"\n";

    // displayTrackerMicroCommand(cur_microcommand_in_cur_command);

    loaded = true;
}

std::pair<size_t, size_t> CommandModeWindow::with_command_execute_microcommand(size_t row, size_t col, bool& fin) {
    std::cerr << "std::pair<size_t, size_t> CommandModeWindow::with_command_execute_microcommand:"<<row<<"-"<<col<<"\n";
    std::cerr << (model.getMode() == Mode::editing ? "editing" : "running") << "\n";

    if (model.currentPoint.isNull()) {
        model.currentPoint = model.startPoint;
    }

    Point currentPoint = model.currentPoint;
    auto command = mk.rom.getMicrocommand(currentPoint.row, currentPoint.col);

    if (command.CS == 0b1 and command.RW == 0b0 and command.EA == 0b1) {
        // read
        command.M = mk.ram.read(mk.MAR); // microprogram address register
    } else {
        command.M = 0x00;
    }

    mk.do_fetch_decode_execute_cycle(command);
    if (command.CS == 0b1 and command.RW == 0b1 and mk.ED == 0b1 and mk.EA == 0b1) {
        // write
        // ramItems[mk.A.value()]->setText(std::to_string(mk.D.value()).c_str());
    }
    auto nextPoint = Point(mk.get_row_adr(), mk.get_col_adr());

    model.currentPoint = nextPoint;
    std::cerr << "new point: " << model.currentPoint.row << "-" << model.currentPoint.col << "\n";

    // configUIMode();
    // changeCurrentPoint(currentPoint, nextPoint);
    update_on_cpu_data();

    model.currentPoint = nextPoint;

    //configUIMode();
    // mainWindow->changeCurrentPoint(current_point, nextPoint);
    // update_on_cpu_data();
    fin = command.LD;
    return std::make_pair(nextPoint.row, nextPoint.col);
}
void CommandModeWindow::on_runButton_clicked()
{
    if (!loaded) { return; }
    loaded = false;
    // while (true) {
    //     bool is_loadmem_prog_running = true;
    //     WORD oldRow = *PC;
    //     while (is_loadmem_prog_running) {
    //         size_t row = mk.get_row_adr();
    //         size_t column = mk.get_col_adr();
    //         auto command = mk.rom.getMicrocommand(row, column);
    //         if (command.is_empty()) {
    //             return;
    //         }
    //         if (command.CS == 0b1 and command.RW == 0b0 and command.EA == 0b1) {
    //             // read
    //             command.M = mk.ram.read(mk.MAR);
    //         } else {
    //             command.M = 0x00;
    //         }

    //         if (command.LD == 0b1) {
    //             is_loadmem_prog_running = false;
    //         }
    //         mk.do_fetch_decode_execute_cycle(command);
    //        if (command.CS == 0b1 and command.RW == 0b1 and mk.EA == 0b1 and mk.ED == 0b1) {
    //            // write
    //            items[mk.A.value()]->setText(toHex(mk.D.value()).c_str());
    //        }
        // }
        // size_t current_row = mk.get_row_adr();
        // size_t current_col = mk.get_col_adr();

        // while (current_row != 0 && current_col != 10) {
        //     auto command = mk.rom.getMicrocommand(current_row, current_col);
        //     if (command.is_empty()) { break; }
        //     if (command.CS == 0b1 and command.RW == 0b0 and command.EA == 0b1) {
        //         // read
        //         command.M = mk.ram.read(mk.MAR);
        //     } else {
        //         command.M = 0x00;
        //     }
        //     mk.do_fetch_decode_execute_cycle(command);
        //     if (command.CS == 0b1 and command.RW == 0b1 and mk.EA == 0b1 and mk.ED == 0b1) {
        //         // write
        //         mkwrite = true;
        //         items[mk.A.value()]->setText(toHex(mk.D.value()).c_str());
        //         mkwrite = false;
        //     }
        //     current_row = mk.get_row_adr();
        //     current_col = mk.get_col_adr();
        // }
        // WORD newRow = *PC;
        // changeCurrentRow(oldRow, newRow);
        // update_on_cpu_data();
    // }

    if (mk.rom.program.size() == 0) {
        return;
    }
    for (auto cit = mk.rom.program.begin(); cit != mk.rom.program.end(); ++cit) {
        auto command_address = Point(cit->first, cit->second);
        bool fin_flag = false;
        while (true) {
            auto [row, column] = with_command_execute_microcommand(cit->first, cit->second, fin_flag);
            command_address.row = row;
            command_address.col = column;
            if (fin_flag) {
                break;
            }
        }
    }
    loaded = true;
}

void CommandModeWindow::on_endButton_clicked()
{
    if (!loaded) { return; }
}

// WORD CommandModeWindow::parseCommand(const std::string& str) {
    // if (!loaded) {
        // return 0;
    // }
    // std::stringstream ss(str);
    // std::string cmd;
    // ss >> cmd;

    // if (isa_commands.count(cmd) == 0) {
        // return 0;
    // }
    // if (isa_commands[cmd] > 0xFF) {
        // return isa_commands[cmd];
    // }
    // WORD com = isa_commands[cmd];
    // std::string arg;
    // ss >> arg;
    // com = (com << 8) | (arg.empty() ? 0 : parseHex(arg));
    // return com;
// }

void CommandModeWindow::on_programWidget_cellChanged(int row, int column) {
    if (!loaded) { return; }
    // we dont allow changing program from ui rn

    // std::string rowContent = items[row]->text().toStdString();
    // if (rowContent.empty()) { return; }
    // // (mk.EA == 0b1 and mk.ED == 0b1) {
    // if (mkwrite) {
    //     auto data = parseHex(rowContent);
    //     mk.ram.write(row, data);
    //     return;
    // }
    // WORD word = 0;
    // if (row < 200) {
    //     word = parseCommand(rowContent);
    //     if (!word) {
    //         items[row]->setText("");
    //     }
    // } else {
    //     word = parseHex(rowContent);
    // }
    // mk.ram.write(row, word);
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
//     if (!loaded) { return; }
//     std::string filename = QFileDialog::getOpenFileName(this, tr("Open project"),
//                                                     "~/Desktop/prog.rom",
//                                                     tr("*.rom")).toStdString();

//     fm::programm_data data = fm::get_data(filename);

//     model.startPoint = Point(data.start_row, data.start_col);
//     setItemColor(model.startPoint);
//     mk = data.mk;
//     setupItems();
//     fillInputs();
//     model.current_filename = filename;

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
            // word = parseCommand(rowContent);
            // if (!word) {
            //     items[i]->setText("");
            // }
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
    loaded = false;

    if (row >= mk.rom.program.size()) {
        loaded = true;
        return;
    }

    auto [rowSelected, columnSelected] = mk.rom.program[row];
    auto [arg1, arg2] = mk.rom.addresses_args_pairs[rowSelected][columnSelected];

    QString address = QString::number(rowSelected) + "-" + QString::number(columnSelected);
    ui->codeBox->setText(address);
    ui->codeBox->setAlignment(Qt::AlignCenter);

    QString arg11 = QString::number(arg1);
    ui->op1Box->setText(arg11);
    ui->op1Box->setAlignment(Qt::AlignCenter);

    QString arg22 = QString::number(arg2);
    ui->op2Box->setText(arg22);
    ui->op2Box->setAlignment(Qt::AlignCenter);

    loaded=true;
}


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <util.hpp>
#include <QPalette>
#include <QtGui>
#include <QFileDialog>
#include <filemanager.h>
#include <commandmodewindow.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("MT589");
    clearInputs();
    setupRegs();
    setLCDsColor();
    setupBoxes();
    handleInputState();
    setupMatrix();
    setupRAM();
    configUIMode();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupCommandPool() {
    std::cerr << "void MainWindow::setupCommandPool:\n";
    // command_pool_widget_matrix.clear();

    // size_t c = 0;
    // std::unordered_set<size_t> codes_set;
    // for (size_t row = 0; row < 32; ++row) {
    //     for (size_t col = 0; col < 16; ++col) {
    //         auto mc = mk.rom.memory[row][col];
    //         if (mc.is_command_entrypoint == false) {
    //             continue;
    //         }
    //         std::cerr << row << "-" << col << std::endl;
    //         ++c;
    //         codes_set.insert({row, col});

    //         std::vector<QTableWidgetItem*> command_description;
    //         auto item = new QTableWidgetItem;

    //         item->setText(QString((std::to_string(row) + "-" + std::to_string(col)).c_str()));
    //         command_description.push_back(item);

    //         item = new QTableWidgetItem;
    //         item->setText(QString(mk.getRom().getMicrocommand(row, col).tag.c_str()));
    //         command_description.push_back(item);

    //         item = new QTableWidgetItem;
    //         item->setText(QString(mc.tag.c_str()));
    //         command_description.push_back(item);

    //         command_pool_widget_matrix.push_back(command_description);
    //     }
    // }
    // std::cerr <<"Setup command pool: c = " << c << "; set size = "<<codes_set.size()<< std::endl;
}

void MainWindow::putScannedPoolToItems() {
    std::cerr << "void MainWindow::putScannedPoolToItems:\n";
    command_pool_widget_matrix.clear();

    size_t c = 0;
    std::unordered_set<size_t> codes_set;
    for (size_t row = 0; row < 32; ++row) {
        for (size_t col = 0; col < 16; ++col) {
            auto mc = mk.getRom().getMicrocommand(row, col);
            if (mc.is_command_entrypoint == false) {
                continue;
            }
            std::cerr << row << "-" << col << std::endl;
            ++c;
            codes_set.insert({row, col});

            std::vector<QTableWidgetItem*> command_description;

            auto item = new QTableWidgetItem;
            item->setText(QString::number(row) + "-" + QString::number(col));
            item->setTextAlignment(Qt::AlignCenter);
            command_description.push_back(item);

            item = new QTableWidgetItem;
            item->setText(QString::fromStdString(mk.getRom().getMicrocommand(row, col).tag));
            item->setTextAlignment(Qt::AlignCenter);
            command_description.push_back(item);

            command_pool_widget_matrix.push_back(command_description);
        }
    }
    std::cerr <<"Setup command pool: c = " << c << "; set size = "<<codes_set.size()<< std::endl;
}

void MainWindow::scanProgram() {
    std::cerr << "void MainWindow::scanProgram:\n";
    // mk.rom.program.clear();

    // std::cerr << "SCANPR"<<mk.rom.program.size()<< std::endl;

    // for(const auto& command_code: mk.rom.program) {
    //     auto [arg1, arg2] = mk.rom.code_to_args.at(command_code);
    //     mk.rom.code_to_args.insert({command_code, {arg1, arg2}})
    // }
}

void MainWindow::putScannedProgramToItems() {
    std::cerr << "void MainWindow::putScannedProgramToItems:\n";
    command_list_widget_matrix.clear();

    for (const auto& [row, column]: mk.rom.program) {
        auto [arg1, arg2] = mk.rom.addresses_args_pairs[row][column];

        std::vector<QTableWidgetItem*> called_command;
        auto itemPtr = new QTableWidgetItem;

        itemPtr->setText(QString((std::to_string(row) + "-" + std::to_string(column)).c_str()));
        called_command.push_back(itemPtr);

        itemPtr = new QTableWidgetItem;
        Name name = mk.getRom().getMicrocommand(row, column).tag; // at to code that disappears from the map ? why????
        itemPtr->setText(QString(name.c_str()));
        called_command.push_back(itemPtr);

        itemPtr = new QTableWidgetItem;
        itemPtr->setText(QString((std::to_string(arg1)).c_str()));
        called_command.push_back(itemPtr);

        itemPtr = new QTableWidgetItem;
        itemPtr->setText(QString((std::to_string(arg2)).c_str()));
        called_command.push_back(itemPtr);

        command_list_widget_matrix.push_back(called_command);
    }
}

// void MainWindow::displayScannedProgram() {
//     ui->programWidget->horizontalHeader()->sectionResizeMode(QHeaderView::Fixed);

//     for (size_t i = 0; i < command_list_widget_matrix.size(); ++i) {
//         ui->programWidget->setItem(i, 0, command_list_widget_matrix[i][0]);
//         ui->programWidget->setItem(i, 1, command_list_widget_matrix[i][1]);
//         ui->programWidget->setItem(i, 2, command_list_widget_matrix[i][2]);
//         ui->programWidget->setItem(i, 3, command_list_widget_matrix[i][3]);
//     }
// }

void MainWindow::putSelectedListToSelectedItems() {
    std::cerr << "void MainWindow::putSelectedListToSelectedItems:\n";

    selectedCommandItems.clear();
    for (size_t it = 0; it < selected_commands_addresses.size(); ++it) {
        std::vector<QTableWidgetItem*> vectorRow;

        auto [row, column] = selected_commands_addresses[it];
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

        if (it == 0) {
            for (size_t i = 0; i < vectorRow.size(); ++i) {
                vectorRow[i]->setSelected(true);
            }
        }
        if (it == 1) {
            for (size_t i = 0; i < vectorRow.size(); ++i) {
                vectorRow[i]->setBackground(traceColor);
            }
        }
        selectedCommandItems.push_back(vectorRow);
    }
}
void MainWindow::displaySelectedCommandListing() {
    QStringList labels = {"Адрес", "Имя", "F", "K", "I", "AC"};
    ui->listingWidget->setRowCount(selected_commands_addresses.size());
    ui->listingWidget->setColumnCount(labels.size());
    ui->listingWidget->setHorizontalHeaderLabels(labels);
    ui->listingWidget->horizontalHeader()->sectionResizeMode(QHeaderView::Fixed);
    ui->listingWidget->setSelectionMode(QAbstractItemView::NoSelection);

    for (size_t i = 0; i < selected_commands_addresses.size(); ++i) {
        for (size_t j = 0; j < labels.size(); ++j) {
            ui->listingWidget->setItem(i, j, selectedCommandItems[i][j]);
        }
    }
    ui->listingWidget->resizeColumnsToContents();
}

void MainWindow::setupMatrix() {
    QStringList horlist;
    for (size_t i = 0; i < 16; ++i) {
        horlist << std::to_string(i).c_str();
    }
    QStringList verlist;
    for (size_t i = 0; i < 32; ++i) {
        verlist << std::to_string(i).c_str();
    }

    ui->tableWidget->setHorizontalHeaderLabels(horlist);
    ui->tableWidget->setVerticalHeaderLabels(verlist);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    romItems.clear();
    for (size_t i = 0; i < 32; ++i) {
        // std::vector<QTableWidgetItem*> row;
        std::vector<QTableWidgetItem*> row;
        for (size_t j = 0; j < 16; ++j) {
            auto itemPtr = new QTableWidgetItem;
            row.push_back(itemPtr);
            ui->tableWidget->setItem(i, j, itemPtr);
        }
        romItems.push_back(row);
    }

    loaded = true;
}

void MainWindow::setupRAM() {
    QStringList verlist;

    for (size_t i = 0; i < mk.ram.size; ++i) {
        verlist << std::to_string(i).c_str();
    }

    ui->ramWidget->setHorizontalHeaderLabels({"DATA"});
    ui->ramWidget->setVerticalHeaderLabels(verlist);
    ui->ramWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    for (size_t i = 0; i < mk.ram.size; ++i) {
            QTableWidgetItem* item = new QTableWidgetItem();
            ramItems.push_back(item);
            item->setData(Qt::ItemDataRole::EditRole, 0);
            ui->ramWidget->setItem(i, 0, item);
    }
}

void MainWindow::on_stepButton_clicked()
{
    if (model.getMode() == editing) {
        setMode(running);
    }
    if (model.currentPoint.isNull()) {
        model.currentPoint = model.startPoint;
    }
    Point currentPoint = model.currentPoint;
    microcommand command = mk.rom.getMicrocommand(currentPoint.row, currentPoint.col);
    std::string ac = command.AC.to_string();

    if (command.CS == 0b1 and command.RW == 0b0 and command.EA == 0b1) {
        // read
        command.M = mk.ram.read(mk.MAR);
    } else {
        command.M = 0x00;
    }

    mk.do_fetch_decode_execute_cycle(command);
    if (command.CS == 0b1 and command.RW == 0b1 and mk.ED == 0b1 and mk.EA == 0b1) {
            // write
        ramItems[mk.A.value()]->setText(std::to_string(mk.D.value()).c_str());
    }
    Point nextPoint = Point(mk.get_row_adr(), mk.get_col_adr());

    model.currentPoint = nextPoint;

    configUIMode();
    changeCurrentPoint(currentPoint, nextPoint);
    update_on_cpu_data();
}

void MainWindow::on_runButton_clicked()
{
    if (model.getMode() == editing) {
        setMode(running);
    }
    if (model.currentPoint.isNull()) {
        model.currentPoint = model.startPoint;
    }
    ui->stepButton->setEnabled(false);
    while (model.getMode() == running) {
        Point currentPoint = model.currentPoint;
        microcommand command = mk.rom.getMicrocommand(currentPoint.row, currentPoint.col);

        mk.do_fetch_decode_execute_cycle(command);

        Point nextPoint = Point(mk.get_row_adr(), mk.get_col_adr());

        model.currentPoint = nextPoint;

        configUIMode();
        changeCurrentPoint(currentPoint, nextPoint);
        update_on_cpu_data();
    }
    ui->stepButton->setEnabled(true);
}

// HELP FUNCTIONS

void MainWindow::setupRegs() {
    QRegularExpression reg("[0-9]+");
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(reg, this);

    ui->mLineEdit->setValidator(validator);
    ui->startAddressEdit->setValidator(validator);
    ui->commandAddressEdit->setValidator(validator);

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

void MainWindow::setLCDsColor() {
    QPalette palette = regLCDs[0]->palette();
    palette.setColor(QPalette::Normal, QPalette::WindowText, Qt::green);
    for (size_t i = 0; i < 10; ++i) {
        regLCDs[i]->setPalette(palette);
    }
    ui->regMAR->setPalette(palette);
    ui->regT->setPalette(palette);
    ui->regAC->setPalette(palette);
    ui->COlcd->setPalette(palette);
    ui->ROlcd->setPalette(palette);
    ui->Alcd->setPalette(palette);
    ui->Dlcd->setPalette(palette);
}

void MainWindow::setupBoxes() {
    QStringList list;
    for (std::string& func: model.mnemonics) {
        list << func.c_str();
    }
    ui->boxCPE->addItems(list);

    ui->boxFC1->addItems({
                             "SCZ",
                             "STZ",
                             "STC",
                             "HCZ"
                         });
    ui->boxFC2->addItems({
                             "FF0",
                             "FFC",
                             "FFZ",
                             "FFI"
                         });

    ui->boxJUMP->addItems({
                              "JCC",
                              "JZR",
                              "JCR",
                              "JCE",
                              "JFL",
                              "JCF",
                              "JZF",
                              "JPR",
                              "JLL",
                              "JRL",
                              "JPX"
                          });
    std::vector<std::string> acs = {
        "0000000",
        "0100000",
        "0110000",
        "1110000"
    };
    ui->commandAddressEdit->setText(acs[0].c_str());
}

void MainWindow::clearInputs() {
    ui->mLineEdit->setText("00000000");
    ui->wrCheckBox->setChecked(true);
    ui->loadCheckBox->setChecked(false);
    ui->ceCheckBox->setChecked(true);
    ui->kLineEdit->setText("");
}

void MainWindow::update_on_cpu_data() {
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

void MainWindow::on_boxCPE_currentIndexChanged(int index)
{
    ui->fLineEdit->setText(model.funcs[index].c_str());
    std::string k =  model.ks[index];
    ui->kLineEdit->setText(toHex(std::bitset<16>(k+k+k+k+k+k+k+k).to_ulong()).c_str());
}

void MainWindow::on_boxJUMP_currentIndexChanged(int index)
{
    std::vector<std::string> acs = {
        "0000000",
        "0100000",
        "0110000",
        "1110000",
        "1000000",
        "1010000",
        "1011000",
        "1100000",
        "1101000",
        "1111100",
        "1111000"
    };
    ui->commandAddressEdit->setText(acs[index].c_str());
}

void MainWindow::handleInputState() {

    bool haveEmptyLineEdit = false;
    std::string ac = ui->commandAddressEdit->text().toStdString();
    std::string i = ui->mLineEdit->text().toStdString();
    std::string k = ui->kLineEdit->text().toStdString();
    std::string f = ui->fLineEdit->text().toStdString();

    haveEmptyLineEdit = ac.size() < 7 or f.size() < 7
             or i.size() < 6 or k.size() < 6;
    ui->saveButton->setEnabled(!haveEmptyLineEdit && !model.currentPoint.isNull());
    ui->clearButton->setEnabled(!model.currentPoint.isNull());

    if (ui->startAddressEdit->text().toStdString().size() < 8) {
        ui->loadButton->setEnabled(false);
    } else {
        ui->loadButton->setEnabled(true);
    }
}

void MainWindow::on_commandAddressEdit_textEdited(const QString &arg1)
{
    handleInputState();
}

void MainWindow::on_iLineEdit_textEdited(const QString &arg1)
{
    handleInputState();
}

void MainWindow::on_kLineEdit_textEdited(const QString &arg1)
{
    handleInputState();
}

void MainWindow::on_mLineEdit_textEdited(const QString &arg1)
{
    handleInputState();
}

void MainWindow::on_riLineEdit_textEdited(const QString &arg1)
{
    handleInputState();
}

void MainWindow::on_ciLineEdit_textEdited(const QString &arg1)
{
    handleInputState();
}

void MainWindow::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    auto previousPoint = model.currentPoint;
    if (model.getMode() == editing) {
        model.currentPoint = Point(currentRow, currentColumn);
        changeCurrentPoint(previousPoint, model.currentPoint);
        if (previousPoint.isNull()) {
            ui->tableWidget->setCurrentIndex(QModelIndex(ui->tableWidget->model()->index(currentRow, currentColumn)));
        }
        configUIMode();
    }
}

void MainWindow::on_tableWidget_cellClicked(int row, int col)
{
    std::cerr << "on_tableWidget_cellClicked\n";

    if (listingIsBeingChanged) {
        return;
    }
    listingIsBeingChanged = true;

    if (model.getMode() != editing) {
        listingIsBeingChanged = false;
        return;
    }

    Point current_point(row, col);
    auto previousPoint = current_point;
    auto current_command = mk.getRom().getMicrocommand(current_point.row, current_point.col);

    for (size_t r = 0; r < 32; ++r) {
        for (size_t c = 0; c < 16; ++c) {
            if (r == model.startPoint.row && c == model.startPoint.col) {
                romItems[r][c]->setBackground(startColor);
                continue;
            }
            if (!mk.getRom().getMicrocommand(r, c).is_empty()) {
                romItems[r][c]->setBackground(commandColor);
            }
        }
    }

    if (!current_point.isNull()) {
        ui->tableWidget->setCurrentIndex(QModelIndex(ui->tableWidget->model()->index(current_point.row, current_point.col)));
    }

    const size_t max_jumps = 512; // 32 * 16
    size_t jumps_counter = 0;
    const bool clicked_is_empty = current_command.is_empty();

    selected_commands_addresses.clear();
    while (jumps_counter < max_jumps && !current_command.is_empty()) {
        selected_commands_addresses.push_back({current_point.row, current_point.col});

        if (jumps_counter++ == 1 && !clicked_is_empty) {
            romItems[current_point.row][current_point.col]->setBackground(traceColor);
        }

        if (current_command.LD == true) { break; }
        if (current_command.AC.to_string().starts_with("00")){
            current_point.row = (size_t)(current_command.AC.to_ulong());
        }
        if (current_command.AC.to_string().starts_with("010")){
            current_point.row = 0;
            current_point.col = (size_t)(current_command.AC.to_ulong()) - 32;
        }
        if (current_command.AC.to_string().starts_with("011")){
            current_point.col = (size_t)(current_command.AC.to_ulong()) - 48;
        }
        if (current_command.AC.to_string().starts_with("1110")){
            current_point.row = (size_t)(current_command.AC.to_ulong()) - 112;
        }
        current_command = mk.getRom().getMicrocommand(current_point.row, current_point.col);
    }

    std::cerr << "Enumerated command list:\n";
    for (auto clIt = selected_commands_addresses.cbegin(); clIt != selected_commands_addresses.cend(); ++clIt) {
        std::cerr << clIt->first << "-" << clIt->second << "\n";
    }

    model.currentPoint = current_point;
    changeCurrentPoint(previousPoint, model.currentPoint);

    selectedCommandItems.clear();
    ui->listingWidget->clear();
    putSelectedListToSelectedItems();
    displaySelectedCommandListing();

    configUIMode();
    listingIsBeingChanged = false; //
}

void MainWindow::fillInputs() {
    std::cerr << "void MainWindow::fillInputs:\n";
    if (model.currentPoint.isNull()) {
        ui->boxCPE->setCurrentIndex(0);
        ui->boxFC1->setCurrentIndex(0);
        ui->boxFC2->setCurrentIndex(0);
        ui->boxJUMP->setCurrentIndex(0);

        ui->kLineEdit->setText("0x0000");
        ui->mLineEdit->setText("0x0000");
        ui->fLineEdit->setText("0001001");
        ui->wrCheckBox->setChecked(false);
        ui->loadCheckBox->setChecked(false);
        ui->ceCheckBox->setChecked(false);
        ui->eaCheckBox->setChecked(false);
        ui->edCheckBox->setChecked(false);
        on_boxJUMP_currentIndexChanged(0);
    } else {
        auto point = model.currentPoint;

        microcommand command = mk.rom.getMicrocommand(point.row, point.col);

        if (command.is_empty()) {
            ui->boxCPE->setCurrentIndex(0);
            ui->boxFC1->setCurrentIndex(0);
            ui->boxFC2->setCurrentIndex(0);
            ui->boxJUMP->setCurrentIndex(0);

            ui->kLineEdit->setText("0x0000");
            ui->mLineEdit->setText("0x0000");
            ui->fLineEdit->setText("0001001");
            ui->wrCheckBox->setChecked(false);
            ui->loadCheckBox->setChecked(false);
            ui->ceCheckBox->setChecked(false);
            ui->eaCheckBox->setChecked(false);
            ui->edCheckBox->setChecked(false);
            on_boxJUMP_currentIndexChanged(0);
        } else {
            ui->boxCPE->setCurrentIndex(command.index_F);
            ui->boxFC1->setCurrentIndex(command.index_FIC);
            ui->boxFC2->setCurrentIndex(command.index_FOC);
            ui->boxJUMP->setCurrentIndex(command.index_Jump);

            ui->kLineEdit->setText(toHex(command.K).c_str());
            ui->commandAddressEdit->setText(command.address_control.c_str());
            ui->mLineEdit->setText(toHex(command.I).c_str());
            ui->fLineEdit->setText(command.F.to_string().c_str());
            ui->wrCheckBox->setChecked(command.RW == 1);
            ui->loadCheckBox->setChecked(command.LD == 1);
            ui->ceCheckBox->setChecked(command.CS == 1);
            ui->eaCheckBox->setChecked(command.EA == 1);
            ui->edCheckBox->setChecked(command.ED == 1);
        }
    }
}

void MainWindow::on_saveButton_clicked()
{
    std::bitset<7> f = std::bitset<7>(ui->fLineEdit->text().toStdString().c_str());

    int fic = ui->boxFC1->currentIndex();
    int foc = ui->boxFC2->currentIndex();

    std::bitset<7> jumpMask = getFromJump(ui->boxJUMP->currentText().toStdString());
    std::bitset<7> address_control = std::bitset<7>(ui->commandAddressEdit->text().toStdString());
    WORD i = parseHex(ui->mLineEdit->text().toStdString());
    WORD k = parseHex(ui->kLineEdit->text().toStdString());

    microcommand command;
    command.F = f;
    command.I = i;
    command.K = k;
    command.set_empty(false);

    command.index_F = ui->boxCPE->currentIndex();
    command.index_FIC = ui->boxFC1->currentIndex();
    command.index_FOC = ui->boxFC2->currentIndex();
    command.index_Jump = ui->boxJUMP->currentIndex();
    command.address_control = address_control.to_string();
    command.RW = ui->wrCheckBox->isChecked() ? 1 : 0;
    command.LD = ui->loadCheckBox->isChecked() ? 1 : 0;
    command.CS = ui->ceCheckBox->isChecked() ? 1 : 0;
    command.EA = ui->eaCheckBox->isChecked() ? 1 : 0;
    command.ED = ui->edCheckBox->isChecked() ? 1 : 0;

    BYTE fc_buf = ((foc << 2) + fic) & 0b1111;
    std::string str = std::bitset<4>(fc_buf).to_string();
    command.FC = fc_buf;

    command.AC = jumpMask | address_control;

    Point currentPoint = model.currentPoint;

    if (!currentPoint.isNull()) {
        mk.rom.write(currentPoint.row, currentPoint.col, command);
    }

    setItemColor(currentPoint);
    handleInputState();
}

void MainWindow::on_clearButton_clicked()
{
    microcommand command;
    Point currentPoint = model.currentPoint;

    if (!currentPoint.isNull()) {
        mk.rom.write(currentPoint.row, currentPoint.col, command);
    }

    setItemColor(currentPoint);
    ui->tableWidget->clearSelection();
}

void MainWindow::on_startAddressEdit_textEdited(const QString &arg1)
{
    handleInputState();
}

void MainWindow::on_loadButton_clicked()
{
    std::bitset<8> addr = std::bitset<8>(ui->startAddressEdit->text().toStdString());
    mk.load(addr); // fix load (with X connection, not x from argument);
    auto currentStartPoint = model.startPoint;
    auto newStartPoint = Point(mk.get_row_adr(), mk.get_col_adr());
    model.startPoint = newStartPoint;
    setItemColor(currentStartPoint);
    setItemColor(newStartPoint);
    configUIMode();
}

void MainWindow::on_ramcLineEdit_textEdited(const QString &arg1)
{
    handleInputState();
}

void MainWindow::setItemColor(const Point& point) {
    std::cerr << "void MainWindow::setItemColor:\n";
    if (point.isNull()) {
        return;
    }
    auto mc = mk.getRom().getMicrocommand(point.row, point.col);
    auto item = romItems[point.row][point.col];
    // QTableWidgetItem* item = new QTableWidgetItem;
    if (model.getMode() == running) {
        if (point == model.currentPoint) {
            item->setBackground(currentRunningColor);
        } else if (point == model.startPoint) {
            item->setBackground(startColor);
        } else if (mc.is_empty()) {
            item->setBackground(transparentColor);
        } else {
            item->setBackground(commandColor);
        }
    }
    else {
        if (point == model.startPoint) {
            item->setBackground(startColor);
        } else if (mc.is_empty()) {
            item->setBackground(transparentColor);
        } else {
            //item->setBackground(commandColor); // error from here
        }
    }
    auto t = mc.tag;
    item->setText(QString(t.c_str()));
    item->setForeground(QBrush(Qt::white));
}

void MainWindow::configUIMode() {
    if (model.getMode() == running) {
        ui->inputFrame->setEnabled(false);
        ui->startAddressFrame->setEnabled(false);
        ui->endButton->setEnabled(true);

        Point point = model.currentPoint;
        if (!point.isNull()) {
            microcommand command = mk.rom.getMicrocommand(point.row, point.col);
            if (command.is_empty()) {
                setMode(editing);
                configUIMode();
            }
        }
    } else {
        ui->inputFrame->setEnabled(true);
        ui->startAddressFrame->setEnabled(true);
        ui->endButton->setEnabled(false);

        ui->saveButton->setEnabled(!model.currentPoint.isNull());
        ui->clearButton->setEnabled(!model.currentPoint.isNull());

        ui->stepButton->setEnabled(!model.startPoint.isNull());
        ui->runButton->setEnabled(!model.startPoint.isNull());
    }
}

void MainWindow::on_endButton_clicked()
{
    setMode(editing);
}

void MainWindow::setMode(Mode mode) {
    auto currentPoint = model.currentPoint;
    model.setMode(mode);
    ui->tableWidget->clearSelection();
    changeCurrentPoint(currentPoint, model.currentPoint);
    configUIMode();
}

void MainWindow::changeCurrentPoint(Point last, Point currentPoint) {
    setItemColor(last);
    setItemColor(currentPoint);
    fillInputs();
}

void MainWindow::on_fLineEdit_textEdited(const QString &arg1)
{
    handleInputState();
}

void MainWindow::on_ramWidget_cellChanged(int row, int column)
{
    mk.ram.write(row, ramItems[row]->text().toUInt());
}

void MainWindow::on_resetButton_clicked()
{
    on_endButton_clicked();
    model.currentPoint = Point::nullPoint();
    mk.reset();
}


void MainWindow::on_save_file_as_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save project"),
                               "~/Desktop/prog.rom",
                               tr("*.rom"));
   fm::save(filename.toStdString(), this->mk, model.startPoint.row, model.startPoint.col);
   model.current_filename = filename.toStdString();
}

void MainWindow::on_open_file_triggered()
{
    if (!loaded) { return; }
    std::cerr << "void MainWindow::on_open_file_triggered:\n";
    loaded = false;

    std::string filename = QFileDialog::getOpenFileName(this, tr("Open project"),
                                                    "~/Desktop/prog.rom",
                                                    tr("*.rom")).toStdString();

    if (filename.empty()) { return; }
    fm::programm_data data = fm::get_data(filename);

    model.startPoint = Point(data.start_row, data.start_col);
    setItemColor(model.startPoint);
    mk = data.mk;
    setupItems();
    fillInputs();
    model.current_filename = filename;

    scanProgram();
    putScannedProgramToItems();

    on_tableWidget_cellClicked(model.startPoint.row, model.startPoint.col);

    loaded = true;
}

void MainWindow::on_save_file_triggered()
{
    if (model.current_filename.empty()) {
        QString filename = QFileDialog::getSaveFileName(this, tr("Save project"),
                                   "~/prog.rom",
                                   tr("*.rom"));
       fm::save(filename.toStdString(), this->mk, model.startPoint.col, model.startPoint.row);
       model.current_filename = filename.toStdString();
    } else {
        fm::save(model.current_filename, this->mk, model.startPoint.col, model.startPoint.row);
    }
}

void MainWindow::on_open_command_mode_triggered()
{
    CommandModeWindow* command_window = new CommandModeWindow();
    command_window->loaded=false;

    command_window->mainWindow = this;
    command_window->mk = mk;
    command_window->model = model;

    setupCommandPool();
    putScannedPoolToItems();
    command_window->displayCommandPool();

    // command_window->scanProgram();
    // command_window->putScannedProgramToItems();
    command_window->displayScannedProgram();

    command_window->show();

    command_window->loaded=true;
    this->hide();
}

void MainWindow::setupItems() {
    std::cerr << "void MainWindow::setupItems:\n";
    for (size_t row = 0; row < 32; ++row) {
        for (size_t col = 0; col < 16; ++col) {
            setItemColor(Point(row, col));
        }
    }
}

void MainWindow::on_tableWidget_cellChanged(int row, int column)
{
    if (!loaded) {
        return;
    }
    std::string content = romItems[row][column]->text().toStdString();
    microcommand command = mk.rom.getMicrocommand(row, column);
    if (content == "") {
        command.is_command_entrypoint = false;
    }
    command.tag = content;
    mk.rom.write(row, column, command);
}

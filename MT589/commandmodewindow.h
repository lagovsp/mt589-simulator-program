#ifndef COMMANDMODEWINDOW_H
#define COMMANDMODEWINDOW_H

#include <QLCDNumber>
#include <QMainWindow>
#include <romwindow.h>
#include <QTableWidgetItem>
#include <emulator.hpp>
#include <sstream>
#include <mainwindow.h>
#include <createisa.h>
#include <command.h>
#include <map>
#include <unordered_set>

namespace Ui {
class CommandModeWindow;
}

class CommandModeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CommandModeWindow(QWidget *parent = nullptr);
    ~CommandModeWindow();

    bool loaded = false;

    // heart
    MK589 mk;
    Model model;

    int row = 0;
    int column = 0;

    int cur_command_number = 0;
    int cur_microcommand_in_cur_command = 0;

    MainWindow* mainWindow = nullptr;

    void prepareISAWindowText();

    // void setupCommandPool();
    // void putScannedPoolToItems();
    void displayCommandPool();

    // void scanProgram();
    // void putScannedProgramToItems();
    void displayScannedProgram();

    void displayTrackerCommands(int);
    void undisplayTrackerCommands(int);

    void displayTrackerMicroCommand(int);
    void undisplayTrackerMicroCommand(int);

    void showCurrentMicroListing();

    std::pair<size_t, size_t> with_command_execute_microcommand(size_t row, size_t col, bool&);

private slots:
    void on_open_rom_triggered();

    void on_save_triggered();

    void on_save_as_triggered();

    void on_open_triggered();

    void on_open_microcommand_mode_triggered();

    void on_resetButton_clicked();

    void on_stepButton_clicked();

    void on_runButton_clicked();

    void on_endButton_clicked();

    void on_programWidget_cellChanged(int row, int column);

    void update_on_cpu_data();

    void setupRegs();

    void changeCurrentRow(WORD oldRow, WORD newRow);

    void on_load_isa_triggered();

    // void on_load_rom_triggered();

    void on_createISAButton_clicked();

    void on_load_ram_triggered();

    void on_addFuncToPool_clicked();

    void on_save_rom_as_triggered();

    //void on_programWidget_itemClicked(QTableWidgetItem *item);

    void on_programWidget_cellClicked(int row, int column);

private:
    Ui::CommandModeWindow *ui;

    ROMWindow romWindow;

    CreateISA isaWindow;

    std::vector<QLCDNumber*> regLCDs = {};

    std::vector<QTableWidgetItem*> items;

    std::string toHex(unsigned int value);

    unsigned int parseHex(const std::string& str);

    WORD parseCommand(const std::string& str);

    // std::map<std::string, WORD> isa_commands = {
    //     {"INCA", 0x0100},
    //     {"MOVA", 0x03},
    //     {"ADDA", 0x02},
    //     {"POP", 0x0500},
    //     {"PUSH", 0x04},
    //     {"IS", 0x0600},
    //     {"INVA", 0x0700},
    //     {"SHAR", 0x08},
    //     {"SHAL", 0x09},
    //     {"JMP", 0xA}
    // };

    std::map<std::string, std::string> isa_regs = {
        {"REG0", "PC"},
        {"REG1", "SP"},
        {"REG8", "A"},
    };

    WORD* PC;

    // bool loaded = false;

    bool mkwrite = false;

    std::vector<QLabel*> reg_labels;

    std::string current_filename = "";

public:
    //
    // using Code = size_t;
    // using Name = std::string;
    // using Address = std::pair<size_t, size_t>;
    // using Args = std::pair<size_t, size_t>;
    // using CalledCommand = std::pair<Code, Args>;
    // // Command: Code, X, Y, Tag

    // std::map<Code, std::pair<Name, Address>> code_to_name_and_address = {};
    // std::list<CalledCommand> program = {};  // CalledCommand: Code, Arg1, Arg2

    // std::vector<std::vector<QTableWidgetItem*>> command_pool_widget_matrix = {};
    std::vector<MainWindow::Address> selected_command_microcommands = {};
    std::vector<std::vector<QTableWidgetItem*>> command_list_widget_matrix = {};

    // std::vector<std::vector<QTableWidgetItem*>> currentCommandWidget
};

#endif // COMMANDMODEWINDOW_H

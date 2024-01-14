#include "config.h"
#include "commands.h"

Option_Command::Option_Command(std::string labelIn, std::vector<Command*> optionsIn)
: Command(labelIn, [this]() {this->drain();}), options(optionsIn) {}

// Helper function for the option_command
void Option_Command::drain() {
    std::string user_input;
    size_t selected;

    // print options
    std::cout << "\n" << label <<"\n";
    for (size_t i=0; i<options.size(); i++)
        std::cout << " " << i+1 << ") " << this->options[i]->label << "\n";

    // get input
    printf("choose option: ");
    std::getline(std::cin, user_input);
    if (user_input.empty())
        {printf("none selected\n"); return;}
    selected = std::stoi(user_input);
    if (selected > this->options.size() || selected <= 0)
        {printf("none selected\n"); return;}

    // process option
    options[selected-1]->execute();
}

// Gets input from user and executes it
void Console::drain() {
    std::string user_input;
    
    // get user input
    std::cout << "\033[32m" << "> " << "\033[0m";
    std::getline(std::cin, user_input);

    if (user_input.empty()) return;
    
    // process the input
    try {
        for (auto command : commands)
            if (user_input == command->label) {
                command->execute();
                return;
            }
        printf("unknown command\n");
    } catch(const std::exception& e) {
        std::cerr << e.what() << "\n";
    }
}
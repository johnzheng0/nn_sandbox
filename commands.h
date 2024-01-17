#ifndef COMMANDS_H
#define COMMANDS_H

#include <iostream>
#include <string>
#include <functional>
#include <vector>

// Command structs
struct Command {
    public:
        std::string label;
        std::function<void()> execute;
        Command(std::string labelIn, std::function<void()> executeIn)
            : label(labelIn), execute(executeIn) {}
        virtual ~Command() {}
};
struct Option_Command : public Command {
    private:
        std::vector<Command*> options;
        void drain(); // Helper function
    public:
        Option_Command(std::string labelIn, std::vector<Command*> optionsIn);
        ~Option_Command() override {for (auto& option : options) delete option;}
};

// Console struct
struct Console {
    private:
        std::vector<Command*> commands;
        void drain(); // Gets input from user and executes it
        bool running;
    public:
        Console(std::vector<Command*> commandsIn)
            : commands(commandsIn) {}
        ~Console() {for (auto& command : commands) delete command;}

        void start() {running = true; while(running) drain();}
        void exit() {running = false;}
};

#endif

#include "config.h"
#include "neural_network.h"
#include "commands.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <random>

std::string filename = FILENAME;
std::vector<std::string> features = FEATURES;
std::vector<std::string> classes = CLASSES;


// Command for getting help
void cmd_help() {
    std::cout <<
    "Commands\n"
    " exit - end program\n"
    " quit - same as 'exit'\n"
    " help - this\n"
    " info - show information about the dataset and neural network\n"
    " train - neural network will try to converge\n"
    " test - check the classification of inputted features\n"
    " options - selection of editing the dataset and neural network\n"
    "\n"
    "edit config.h and recompile for other options\n";
}

// Clean whitespace from string
std::string remove_whitespace(const std::string& str) {
    std::string result;
    std::remove_copy_if(str.begin(), str.end(), std::back_inserter(result), ::isspace);
    return result;
}

// Read dataset using filename and class names
std::vector<DataInstance> get_dataset(std::string filename, std::vector<std::string> classes) {
    // Open the file containing the data
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << ("Failed to Read Input File\n");
        exit(-1);
    }

    // Initialize dataset vectors for features and classes
    std::vector<DataInstance> dataset;

    // Populate dataset with features and classes from the file
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (std::getline(iss, token, DELIMITER))
            tokens.push_back(remove_whitespace(token));
        
        DataInstance instance_data;
        for(auto& token : tokens) {
            if (token != tokens.back())
                instance_data.features.push_back(std::stod(token));
            else 
                for (size_t i=0; i<classes.size(); i++)
                    if (classes[i].compare(token) == 0)
                        instance_data.label = i;
        } dataset.push_back(instance_data);
    } file.close(); // Close file

    return dataset;
}

void train_test_split(std::vector<DataInstance>& dataset, std::vector<DataInstance>& train_set, std::vector<DataInstance>& test_set) {
    // Randomly shuffle the dataset to distribute the data
    std::shuffle(dataset.begin(), dataset.end(), std::default_random_engine(RANDOM_SEED));

    // Split into training set and testing set
    int split_index = 0.6 * dataset.size();
    train_set = std::vector<DataInstance>(dataset.begin(), dataset.begin()+split_index);
    test_set = std::vector<DataInstance>(dataset.begin()+split_index, dataset.end());
}
void resample(std::vector<DataInstance>& dataset) {
    std::string user_input;
    double ratio;

    std::cout << "enter resampling ratio (default=1.0): ";
    std::getline(std::cin, user_input);
    ratio = user_input.empty() ? 1.0 : std::stod(user_input);

    std::vector<std::vector<DataInstance>> subsets;
    for (size_t i=0; i<classes.size(); i++) {
        std::vector<DataInstance> subset;
        std::copy_if(dataset.begin(), dataset.end(), std::back_inserter(subset), [i](DataInstance x) {
            return x.label == (int)i;});
        subsets.push_back(subset);
    }

    size_t target = dataset.size()*ratio / subsets.size();

    for (size_t i=0; i<subsets.size(); i++) {
        // oversample minority subsets
        for (size_t j=subsets[i].size(); j<target; j++)
            dataset.push_back(subsets[i][rand()%subsets[i].size()]);
        // undersample majority subsets
        for (size_t j=subsets[i].size(); j>target; j--)
            dataset.erase(std::find_if(dataset.begin(), dataset.end(), [i](DataInstance x) {
                return x.label == (int)i; }));   
    }
}

void cmd_train(NeuralNetwork* nn, std::vector<DataInstance> trainset, std::vector<DataInstance> testset) {
    std::string user_input;
    size_t max_iteration;
    double learn_rate;

    std::cout << "enter max iteration (default=100): ";
    std::getline(std::cin, user_input);
    max_iteration = user_input.empty() ? 100 : std::stoi(user_input);

    std::cout << "enter learn rate: (default=1.0): ";
    std::getline(std::cin, user_input);
    learn_rate = user_input.empty() ? 1.0 : std::stoi(user_input);

    nn->train(trainset, testset, max_iteration, learn_rate);
}
// Command for manually testing a datapoint
void cmd_test(NeuralNetwork* nn) {
    std::vector<double> inputs;

    std::cout << "Input Features\n";
    for (auto feature : features) {
        std::string user_input;

        std::cout << feature << ": ";
        std::getline(std::cin, user_input);

        inputs.push_back(std::stod(user_input));
    }

    std::vector<double> output = nn->calculate(inputs);

    auto maxItr = std::max_element(output.begin(), output.end());
    int maxIdx = std::distance(output.begin(), maxItr);

    std::cout << classes[maxIdx] << "\n";
}
// Command for printing the dataset
void cmd_info(NeuralNetwork* nn, std::vector<DataInstance> dataset) {
    std::cout << "[DATASET INFORMATION]\n";
    for (size_t i=0; i<classes.size(); i++) {
        std::vector<DataInstance> subset;
        std::copy_if(dataset.begin(), dataset.end(), std::back_inserter(subset), [i](DataInstance a) {
            return a.label == (int)i;});
        std::cout << " instances of " << classes[i] << ": " << subset.size() << "\n";
    }
    std::cout << " total instances: " << dataset.size() << "\n";

    std::cout << "\n[NEURAL NETWORK INFORMATION]\n";
    printf(" layer sizes: "); for (auto& layer : nn->layers) printf("%ld ", layer->nodes.size()); printf("\n");
    std::cout << " current accuracy (using whole dataset): " << nn->test(dataset) << "\n";

}
// Command to resize the neural network
void cmd_resize(NeuralNetwork*& nn) {
    std::string user_input;

    printf("enter new architecture (eg. 4 5 3): ");
    std::getline(std::cin, user_input);

    std::istringstream iss(user_input);
    std::vector<size_t> layer_sizes;

    std::string token;
    while (std::getline(iss, token, ' '))
        layer_sizes.push_back((size_t)std::stoi(token));

    delete nn;
    nn = new NeuralNetwork(layer_sizes);
}

int main() {
    std::srand(RANDOM_SEED); // seed random

    // Read dataset from file
    std::vector<DataInstance> dataset = get_dataset(filename, classes);
    std::vector<DataInstance> train_set;
    std::vector<DataInstance> test_set;

    // split dataset for training and testing
    train_test_split(dataset, train_set, test_set);

    // Initialize neural network
    NeuralNetwork* nn = new NeuralNetwork(NEURAL_NETWORK_LAYERS);

    // Initialize command console with its commands
    Console console(
        std::vector<Command*> {
            new Command("info", [&nn, &dataset]() {
                cmd_info(nn, dataset); }),
            new Command("train", [&nn, &train_set, &test_set]() {
                cmd_train(nn, train_set, test_set); }),
            new Command("test", [&nn]() {
                cmd_test(nn); }),
            new Option_Command("options", std::vector<Command*> {
                new Option_Command("dataset options", std::vector<Command*> {
                    new Command("split into training and testing sets", [&dataset, &train_set, &test_set]() {
                        train_test_split(dataset, train_set, test_set); }),
                    new Command("balance with resampling", [&dataset]() {
                        resample(dataset); }),
                    new Command("reimport dataset", [&dataset]() {
                        dataset = get_dataset(filename, classes); }),
                }),
                new Option_Command("neural network options", std::vector<Command*> {
                    new Command("change layer sizes", [&nn]() {
                        cmd_resize(nn); }),
                    new Command("randomize weights and biases", [&nn]() {
                        nn->randomize(); }),
                    new Command("test input", [&nn]() {
                        cmd_test(nn); }),
                    new Command("dump", [&nn]() {
                        nn->show(); }),
                }),
            }),
            new Command("help", []() {
                cmd_help(); }),
            new Command("exit", [&console]() {
                console.exit(); }),
            new Command("quit", [&console]() {
                console.exit(); }),
        }
    );

    // start command console
    std::cout << "type 'exit' to quit; type 'help' for help\n";
    console.start();

    delete nn;
    return 0;
}
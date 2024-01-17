#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <random>

// A datapoint for a dataset that contains features associated with a class
struct DataInstance {
    public:
        std::vector<double> features;
        int label;
};

// Node class that affect the output the network
struct Node {
    public:
        std::vector<double> weights;
        double bias;
        
        Node(const size_t& numAxon);

        void randomize();
        void show(); // display node information
};

// Layer class that contains nodes
struct Layer {
    private:
        double sigmoid(double x) {return 1/(1+std::exp(-x));}
    public:
        size_t numInputs;
        std::vector<Node*> nodes;

        Layer(const size_t& size, const size_t& prevLayerSize);
        ~Layer() {for (auto x : nodes) delete x;}

        // method for calculating the output of a layer
        std::vector<double> calculate(std::vector<double> input);

        void randomize() {for (auto node : nodes) node->randomize();}
        void show(); // display layer information
};

// Neural Network class that contains layers of node
struct NeuralNetwork {
    private:
        // methods for calculating the inefficiency of the network
        double loss(DataInstance dataInstance);
        double loss(std::vector<DataInstance> dataset);

        // methods for driving the change to the network
        void apply_gradient(std::vector<std::vector<std::vector<double>>>&, std::vector<std::vector<double>>& biases);
        void learn(std::vector<DataInstance> dataset, double learnRate);
    public:
        std::vector<Layer*> layers;

        NeuralNetwork(const std::vector<size_t>& layerSizes);
        ~NeuralNetwork() {for (auto x : layers) delete x;}

        // methods used to train and test the network
        void train(std::vector<DataInstance> trainset, std::vector<DataInstance> testset, 
            size_t max_iteration, double learn_rate);
        double test(std::vector<DataInstance> testset);

        // method to get the outputs
        std::vector<double> calculate(std::vector<double> input);
        
        void randomize() {for (auto layer : layers) layer->randomize();}
        void show(); // display neural network
};


#endif

#include "config.h"
#include "neural_network.h"
#include <iostream>
#include <algorithm>
#include <random>

std::default_random_engine rng(RANDOM_SEED);

// Constructor for Nodes
Node::Node(const size_t& numAxon) {
    weights.resize(numAxon);
    randomize();
}
// Constructor for Layer
Layer::Layer(const size_t& size, const size_t& prevLayerSize) {
    nodes.resize(size, nullptr);
    for (auto& node : nodes)
        node = new Node(prevLayerSize);
    numInputs = prevLayerSize;
}
// Constructor for NeuralNetwork
NeuralNetwork::NeuralNetwork(const std::vector<size_t>& layerSizes) {
    layers.push_back(new Layer(layerSizes.front(), 0));
    for (auto it = layerSizes.begin()+1; it != layerSizes.end(); it++)
        layers.push_back(new Layer(*it, *(it-1)));
}

// Calculate the loss of a single data point
double NeuralNetwork::loss(DataInstance dataInstance) {
    std::vector<double> output = calculate(dataInstance.features);
    std::vector<double> expected_output(output.size(), 0.0);
        expected_output[dataInstance.label] = 1.0;
    
    double loss = 0.0;
    for (size_t i=0; i<output.size(); i++) {
        loss += std::pow((output[i] - expected_output[i]), 2);}

    return loss;
}
// Calculate the average loss of a dataset
double NeuralNetwork::loss(std::vector<DataInstance> dataset) {
    // reduce the dataset to generalize and optimize calculation of the loss
    size_t reduced_size = std::min((int)(dataset.size()*0.3), 20);
    std::shuffle(dataset.begin(), dataset.end(), std::default_random_engine(rng));
    std::vector<DataInstance> reduced_set(dataset.begin(), dataset.begin()+reduced_size);

    // add the loss of each data point
    double total_loss = 0.0;
    for (auto& data : reduced_set)
        total_loss += loss(data);

    return total_loss / reduced_size;
}

// Updates the weights and biases
void NeuralNetwork::apply_gradient(std::vector<std::vector<std::vector<double>>>& weights, std::vector<std::vector<double>>& biases) {
    for (size_t l=0; l<layers.size(); l++) {
        Layer* layer = layers[l];
        for (size_t n=0; n<layer->nodes.size(); n++) {
            Node* node = layer->nodes[n];
            for (size_t w=0; w<node->weights.size(); w++) {
                node->weights[w] -= weights[l][n][w];
            }
            node->bias -= biases[l][n];
        }
    }
}
// Nudge the weights and biases toward the right direction
void NeuralNetwork::learn(std::vector<DataInstance> dataset, double learnRate) {
    double original_loss = loss(dataset);
    double nudge = 0.0001;

    // Used for 
    std::vector<std::vector<std::vector<double>>> loss_gradient_weights;
    std::vector<std::vector<double>> loss_gradient_bias;

    // Find the loss gradients for weights and biases
    for (auto& layer : layers) {
        std::vector<std::vector<double>> layer_weight_gradient;
        std::vector<double> layer_bias_gradient;
        for (auto& node : layer->nodes) {
            std::vector<double> node_weight_gradient;
            for (auto& weight : node->weights) {
                weight += nudge;
                node_weight_gradient.push_back(learnRate * ((loss(dataset)-original_loss)/nudge));
                weight -= nudge;
            }
            layer_weight_gradient.push_back(node_weight_gradient);

            node->bias += nudge;
            layer_bias_gradient.push_back(learnRate * ((loss(dataset)-original_loss)/nudge));
            node->bias -= nudge;

        }
        loss_gradient_weights.push_back(layer_weight_gradient);
        loss_gradient_bias.push_back(layer_bias_gradient);
    }

    // Apply the loss gradients
    apply_gradient(loss_gradient_weights, loss_gradient_bias);
}

// Trains using a training set and test with a testing set
void NeuralNetwork::train(std::vector<DataInstance> trainset, std::vector<DataInstance> testset, size_t max_iteration, double learn_rate) {
    for (size_t i=1; i<=max_iteration; i++) {
        // nudge the model towards a "minima"
        learn(trainset, learn_rate);

        // calculate the loss and accuracy and print it
        double iteration_loss = loss(trainset);
        double iteration_accuracy = test(testset);
        printf("%ld: [loss:%f] [accuracy:%f]\n", i, iteration_loss, iteration_accuracy);

        // consider a high accuracy or low loss to be converged
        if (iteration_loss < 0.1 || iteration_accuracy == 1.0)
            break;
    }
}
// Tests the model using a testing set and get its accuracy
double NeuralNetwork::test(std::vector<DataInstance> testset) {
    size_t correct = 0; // track the correct predictions

    for (auto data : testset) {
        // get the predictions
        std::vector<double> output = calculate(data.features);

        // the maximum node is considered to be the predicted class
        auto maxItr = std::max_element(output.begin(), output.end());
        int maxIdx = std::distance(output.begin(), maxItr);

        // compare the predicted class and real class
        if (maxIdx == data.label) correct++;
    }

    // return the accuracy as a percent
    return (double)correct / (double)testset.size();
}

// Claculate the output of a single layer given an input
std::vector<double> Layer::calculate(std::vector<double> input) {
    std::vector<double> output;
    for (size_t i=0; i<nodes.size(); i++) {
        double weighted_output = nodes[i]->bias;

        Node* node = nodes[i];
        std::vector<double> weights = node->weights;
        
        for (size_t j=0; j<numInputs; j++)
            weighted_output += weights[j] * input[j];

        output.push_back(sigmoid(weighted_output));
    }
    return output;
}
// Calculate the output of the neural network
std::vector<double> NeuralNetwork::calculate(std::vector<double> input) {
    for (auto it = layers.begin()+1; it != layers.end(); it++)
        input = (*it)->calculate(input);
    return input;
}

// Node randomize weights and biases
void Node::randomize() {
    std::uniform_real_distribution<double> distribution(-2.0, 2.0);
    for (auto& weight : weights)
        weight = distribution(rng);
    bias = 0.0;
}

// Display neural network
void NeuralNetwork::show() {
    printf("NeuralNetwork {\n");
    for (auto it=layers.begin(); it!=layers.end(); it++)
        (*it)->show();
    printf("}\n");
}
// Display layer information
void Layer::show() {
    printf(" Layer {\n");
    for (auto it=nodes.begin(); it!=nodes.end(); it++)
        (*it)->show();
    printf(" }\n");
}
// Display node information
void Node::show() {
    printf("  Node {\n");
    printf("   weights:\t\n"); for (auto weight: weights) printf("    %f\n", weight);
    printf("   bias: %f\n", bias);
    printf("  }\n");
}


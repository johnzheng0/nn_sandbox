#ifndef CONFIG_H
#define CONFIG_H

/*
This is information about the dataset
  FILENAME - name of the file to be read in
  FEATURES - name of the features
  CLASSES - possible output values
*/
#define FILENAME "datasets/iris.csv"
#define FEATURES {"Sepal-length", "Sepal-width", "Petal-length", "Petal-width"}
#define CLASSES {"Iris-setosa", "Iris-versicolor", "Iris-virginica"}

/*
Default Layer Sizes for the Neural Network
  make sure first layer (input) has the same size as the feature set
  and the last layer (output) has the same size as the class set
*/
#define NEURAL_NETWORK_LAYERS {features.size(), 5, classes.size()}


// Used for reading in CSV files. Probably won't need to edit this.
#define DELIMITER ','

// Randomness
#define RANDOM_SEED 87123401


#endif

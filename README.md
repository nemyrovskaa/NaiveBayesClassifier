# Implementation of Naive Bayes Classifier
05.2022

## Program Description:
A program was developed for classifying headlines into categories: Internet, Space, and Science.

## Information about the Data:
The dataset was prepared independently. It consists of a set of article headlines from popular science magazines and news articles in English. The article headlines are divided into three categories: Space, Science, and Internet. The electronic resources used are listed below:

1. https://phys.org/space-news/
2. https://www.newscientist.com/subject/space/
3. https://www.science.org/journals
4. https://techxplore.com/internet-news/
5. https://www.bbc.com/news
6. https://www.ijis.net/

The data is complete. Each data instance has two attributes: the first is a string attribute, and the second is a nominal attribute.

The string attribute accepts any string as a value, i.e., the article headline.
The nominal attribute accepts the class value (space, science, internet).
The total number of data instances is 498.

### Class Distribution:
- space – 166 [33.333%]
- science – 166 [33.333%]
- internet – 166 [33.333%]

A file with stop words, which do not affect the meaning of the text and could negatively influence classification results, was also created.

### Training the Model:
Two-thirds of the entire dataset (332 instances) are used for training the model, split into 3 files:

- 498internet.txt (111 instances)
- 498science.txt (110 instances)
- 498space.txt (111 instances)

### Testing the Model:
A file with one-third of the data (166 instances) is created for testing the model: 498test.txt.
After the classification is performed, the model’s results are compared with the correct answers in the file 498answers.txt.


## Project Build Instructions
To build and run the project, follow these steps:

Clone the repository: If you haven't already cloned the repository, use the following command:

`git clone https://github.com/nemyrovskaa/NaiveBayesClassifier.git`

Navigate to the project directory: After cloning, navigate to the project folder:

`cd path_to_repository`

Build the project: To compile the main.cpp file and generate an executable, run:

`g++ main.cpp -o naive_bayes`

Run the program: After compiling, you can run the program using the following command:

`naive_bayes`

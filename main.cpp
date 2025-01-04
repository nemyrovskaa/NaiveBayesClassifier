/*
* Author:   Nemyrovska
* Date:     05.2022
*/

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

#define SPACE 0
#define SCIENCE 1
#define INTERNET 2
#define CLASS_ATTRIB_NUM 3

using namespace std;

/*
* structure that describes data about a word
* words_in_class	 - the number of occurrences of the word in each class
* norm_prob_in_class - the normalized probability of meeting the word in each class
*/
struct word_data
{
    int words_in_class[CLASS_ATTRIB_NUM] = {0, 0, 0};
    float norm_prob_in_class[CLASS_ATTRIB_NUM] = {0., 0., 0.};
};


// builds classification model
void training(unordered_map<string, word_data>& );

// tests model
vector<string> testing(unordered_map<string, word_data>& );

// checks results of classification (calculates precentege of correctly classified)
void check_result(vector<string>& );

// calculates normalized probability
float norm_prob_calc(int , int );


int main()
{
    unordered_map<string, word_data> words_info;
    vector<string> result;

	// build model
    training(words_info);

	// test model and print results
    result = testing(words_info);
    check_result(result);

    return 0;
}


/*
* Function that builds classification model
* words_info		- a hash table (key - word, value - information about this word)
* common_words		- words that do not affect the meaning (function words, pronouns, etc.)
* words_in_dataset	- the number of occurrences of a word in the training dataset
*/
void training(unordered_map<string, word_data>& words_info)
{
    unordered_map<string, string> common_words;
    fstream file;
    string this_word;
    string file_name[CLASS_ATTRIB_NUM] = {"sources/498space.txt", "sources/498science.txt", "sources/498internet.txt"};
    word_data temp_data;
    int words_in_dataset;

    //--------------------------------ADDING-WORDS------------------------------
    // reading from a file and adding words that
	// do not affect the meaning to the hash table
    file.open("sources/common_words.txt", ios::in);
    if(!file) // check if file with common words is opening
    {
        cout << "Cannot open the file 'common_words.txt' (((" << endl;
        return;
    }

    do{
        file >> this_word;
        // add words into hash-table
        common_words.insert(make_pair(this_word, this_word));

    }while(!file.eof());
    file.close();

    //----------------------------MAKING-TABLE-WITH-WORDS--------------------------
    // reading from the training dataset file and adding words that
	// affect the meaning and their frequency in each class to the
	// hash-table 'words_info'
    for(int i = 0; i < CLASS_ATTRIB_NUM; i++)
    {
        file.open(file_name[i], ios::in);
        if(!file) // check if file with training data is opening
        {
            cout << "Cannot open the file '" << file_name[i] << "' (((" << endl;
            return;
        }

        do{
            file >> this_word;
            if(common_words.find(this_word) == common_words.end())
            {
                if(words_info.find(this_word) == words_info.end())
                    words_info.insert(make_pair(this_word, temp_data));

                words_info[this_word].words_in_class[i] += 1;
            }
        }while(!file.eof());
        file.close();

    }

    // calculation of the normalized probability of each word's
	// association with a class and adding the resulting value
	// to the 'words_info' hash-table
    for(auto i : words_info)
    {
        words_in_dataset = i.second.words_in_class[SPACE] + i.second.words_in_class[SCIENCE] + i.second.words_in_class[INTERNET];

        words_info[i.first].norm_prob_in_class[SPACE] = norm_prob_calc(words_in_dataset, i.second.words_in_class[SPACE]);
        words_info[i.first].norm_prob_in_class[SCIENCE] = norm_prob_calc(words_in_dataset, i.second.words_in_class[SCIENCE]);
        words_info[i.first].norm_prob_in_class[INTERNET] = norm_prob_calc(words_in_dataset, i.second.words_in_class[INTERNET]);
    }
}


/*
* a function that performs testing of a classification model
* words_info	- a hash-table (key - word, value - information about this word)
* result		- a vector with classification results
*/
vector<string> testing(unordered_map<string, word_data>& words_info)
{
    fstream test_file;
    string str, this_word, res_str;
    vector<string> result;
    int start_pos, end_pos;

    // reading from the test dataset file;
	// extracting words from a string and calculating the instance-to-class ratio
    test_file.open("sources/498test.txt", ios::in);
    if(!test_file)
    {
        cout << "Cannot open the file '498test.txt' (((" << endl;
        return result;
    }

    do{
        vector<float> prob_in_class(3, 1./CLASS_ATTRIB_NUM);
        start_pos = 0;

        getline(test_file, str);
        end_pos = str.find(' ', start_pos);

        while(start_pos < str.length())
        {
            this_word = str.substr(start_pos, end_pos - start_pos);

			// calculating probability belonging sentences to classes
            if(words_info.find(this_word) != words_info.end())
            {
                prob_in_class[SPACE] *= words_info[this_word].norm_prob_in_class[SPACE];
                prob_in_class[SCIENCE] *= words_info[this_word].norm_prob_in_class[SCIENCE];
                prob_in_class[INTERNET] *= words_info[this_word].norm_prob_in_class[INTERNET];
            }

            start_pos = end_pos + 1;
            end_pos = str.find(' ', start_pos);

            if(end_pos == string::npos)
                end_pos = str.length();
        }

        /*
        * definition of the maximum probability among classes and storing the corresponding
		* classification result for the instance in the results vector (result)
		*/
		// determining max probability belonging sentences to classes
        if(prob_in_class[SPACE] > prob_in_class[SCIENCE])
            res_str = prob_in_class[SPACE] > prob_in_class[INTERNET] ? "space" : "internet";
        else
            res_str = prob_in_class[SCIENCE] > prob_in_class[INTERNET] ? "science" : "internet";

        result.push_back(res_str);

    }while(!test_file.eof());
    test_file.close();

    return result;
}

void check_result(vector<string>& result)
{
    fstream answ_file;
    string answer;
    int corr_classif = 0, i = 0;

    answ_file.open("sources/498answers.txt", ios::in);
    if(!answ_file) // check if file with training data is opening
    {
        cout << "Cannot open the file '498answers.txt' (((" << endl;
        return;
    }

    cout << "Classification results:" << endl << endl;
    cout << "No.  classified as:\tcorrect answer:" << endl;
    do
    {
        answ_file >> answer;
        if(result[i] == answer)
            corr_classif++;

        cout << i+1 << ")\t" << result[i] << "   \t" << answer <<  endl;

        i++;

    }while(!answ_file.eof());
    answ_file.close();

    cout << "\nCorrectly classified: " << corr_classif << "\t[ " << 100./result.size()*corr_classif << "% ]" << endl;
    cout << "Incorrectly classified: " << result.size()-corr_classif << "\t[ " << 100./result.size()*(result.size()-corr_classif) << "% ]" << endl;
}

/*
* calculating the normalized probability of the word's relation to a class
* words_in_dataset	- the number of occurrences of the word in the training dataset
* words_in_class	- the number of occurrences of the word in the class
*/
float norm_prob_calc(int words_in_dataset, int words_in_class)
{
    return (words_in_class + 1./CLASS_ATTRIB_NUM)/(words_in_dataset + 1);
}

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

// структура, що описує дані про слово
// words_in_class - к-ть повторення слова у кожному класі
// norm_prob_in_class - нормована ймовірність зустріти слово у кожному класі
struct word_data
{
    int words_in_class[CLASS_ATTRIB_NUM] = {0, 0, 0};           // amount of certain word in each class
    float norm_prob_in_class[CLASS_ATTRIB_NUM] = {0., 0., 0.};  // normalized probability of encountering a word in each class
};


void training(unordered_map<string, word_data>& );         // filling unordered map (key = word, value = word data)
vector<string> testing(unordered_map<string, word_data>& );// testing model
void check_result(vector<string>& );                // checking results of classification
float norm_prob_calc(int , int );                   // calculation of normalized probability


int main()
{
    unordered_map<string, word_data> words_info;
    vector<string> result;

    training(words_info); // ф-ія побудови моделі

    /*for(auto i : words_info)
        cout << i.first << "\nspace: " << i.second.words_in_class[SPACE] << ",  probability: " << i.second.norm_prob_in_class[SPACE] <<
         "\nscience: " << i.second.words_in_class[SCIENCE] << ",  probability: " << i.second.norm_prob_in_class[SCIENCE] <<
         "\ninternet: " << i.second.words_in_class[INTERNET] << ",  probability: " << i.second.norm_prob_in_class[INTERNET] << endl << endl;

    cout << "size: " << words_info.size() << endl;*/

    result = testing(words_info); // ф-ія тестування моделі
    check_result(result);  // ф-ія форматованого виводу результатів класифікації

    return 0;
}


// ф-ія, що виконує побудову моделі класифікації
// words_info - хеш-таблиця (ключ - слово, значення - інформація
// про це слово)
// common_words - слова, що не впливають на зміст (несамостійні
// частини мови, займенники, тощо)
// words_in_dataset - к-ть повторень слова у навчальному наборі даних
void training(unordered_map<string, word_data>& words_info)
{
    unordered_map<string, string> common_words;
    fstream file;
    string this_word;
    string file_name[CLASS_ATTRIB_NUM] = {"sources/498space.txt", "sources/498science.txt", "sources/498internet.txt"};
    word_data temp_data;
    int words_in_dataset;

    //--------------------------------ADDING-WORDS------------------------------
    // читання з файлу та додавання слів, що не впливають
    // на зміст до хеш-таблиці
    file.open("sources/common_words.txt", ios::in);
    if(!file) // check if file with common words is opening
    {
        cout << "Cannot open the file 'common_words.txt' (((" << endl;
        return;
    }

    do{
        file >> this_word;
        // add words in hash-table
        common_words.insert(make_pair(this_word, this_word));

    }while(!file.eof());
    file.close();

    //----------------------------MAKING-TABLE-WITH-WORDS--------------------------
    // читання з файлу навчального набору даних та додавання
    // слів, що впливають на зміст та к-ть їх повторень у
    // кожному класі до хеш-таблиці words_info
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

    // обчислення нормованої ймовірності відношення кожного
    // слова до класу та додавання отриманого значення у
    // хеш-таблицю words_info
    for(auto i : words_info)
    {
        words_in_dataset = i.second.words_in_class[SPACE] + i.second.words_in_class[SCIENCE] + i.second.words_in_class[INTERNET];

        words_info[i.first].norm_prob_in_class[SPACE] = norm_prob_calc(words_in_dataset, i.second.words_in_class[SPACE]);
        words_info[i.first].norm_prob_in_class[SCIENCE] = norm_prob_calc(words_in_dataset, i.second.words_in_class[SCIENCE]);
        words_info[i.first].norm_prob_in_class[INTERNET] = norm_prob_calc(words_in_dataset, i.second.words_in_class[INTERNET]);
    }
}


// ф-ія, що виконує тестування моделі класифікації
// words_info - хеш-таблиця (ключ - слово, значення - інформація
// про це слово)
// result - вектор з результатами класифікації
vector<string> testing(unordered_map<string, word_data>& words_info)
{
    fstream test_file;
    string str, this_word, res_str;
    vector<string> result;
    int start_pos, end_pos;

    // читання з файлу тестового набору даних;
    // виділення слів зі строки та обчислення
    // відношення екземпляру до класу
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

            if(words_info.find(this_word) != words_info.end())  // calculating probability belonging sentences to classes
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

        // визначення максимальної ймовірності серед класів
        // та запис відповідного результату класифікації
        // екземпляру до вектору з результатами (result)
        if(prob_in_class[SPACE] > prob_in_class[SCIENCE])           // determining max probability belonging sentences to classes
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

    answ_file.open("498answers.txt", ios::in);
    if(!answ_file)
    {
        cout << "Cannot open the file '498answers.txt' (((" << endl;    // check if file with training data is opening
        return;
    }

    cout << "Classification results:" << endl << endl;
    cout << "No.  classified as:\tcorrect answer:" << endl;
    do{
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

// обчислення нормованої ймовірності відношення слова до класу
// words_in_dataset - к-ть повторення слова у навчальному наборі даних
// words_in_class - к-ть повторення слова у класі
float norm_prob_calc(int words_in_dataset, int words_in_class)
{
    return (words_in_class + 1./CLASS_ATTRIB_NUM)/(words_in_dataset + 1);
}

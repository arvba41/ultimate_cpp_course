#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

// define the DEBUG macro to enable debug mode
// #define DEBUG

// changing the file path based on the build type
#ifdef DEBUG
#define FILE_PATH "test_text.txt"
#else
#define FILE_PATH "../test_text.txt"
#endif


int main() {
    
    // read file 
    ifstream file_read(FILE_PATH);

    string word, prev_word, largest_word, shortest_word;
    int char_count = 0, char_length = 0;

    if (file_read.is_open()) {
        while (file_read >> word) {
            char_count++;

            // remove punctuation
            for (int i = 0, len = word.size(); i < len; i++) {
                if (ispunct(word[i])) {
                    word.erase(i--, 1);
                    len = word.size();
                }
            } 

            char_length += word.size(); // get the length of the word

            // get the first word
            if (char_count == 1) {
                prev_word = word;
                largest_word = word;
                shortest_word = word;
            }

            // get the largest word
            if (word.size() > largest_word.size()) {
                largest_word = word;
            }

            // get the shortest word
            if (word.size() < shortest_word.size()) {
                shortest_word = word;
            }
            
            // store the previous word
            prev_word = word;
        }
        file_read.close();

        // Print the statistics
        cout << "There are " << char_count << " words in the file." << endl;
        cout << "The shortest word was \"" << shortest_word << "\" with " << shortest_word.size() << " character(s)." << endl;
        cout << "The longest word was \"" << largest_word << "\" with " << largest_word.size() << " character(s)." << endl;
        cout << "The average word length was " << fixed << setprecision(2) << (float)char_length / char_count << " character(s)." << endl;

    } else {
        cout << "Unable to open file" << endl;
    }
    
}
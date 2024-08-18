#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

int main() {

    // ---------------------
    cout << "Enter an integer:" << endl;
    int interger_1;
    cin >> interger_1;
    cin.ignore(1000,'\n');
    cout << "You entered the number :" << interger_1 << endl;
    cout << "\n" << endl;

    // ---------------------
    cout << "Enter a four integers:" << endl;
    int interger_vector[4];
    for (int i = 0; i < 4; i++) {
        cin >> interger_vector[i];
    }
    cin.ignore(1000,'\n');
    cout << "You entered the numbers: ";
    for (int i = 0; i < 4; i++) {
        cout << interger_vector[i] << " ";
    }
    cout << "\n" << endl;

    // ---------------------
    cout << "Enter one integer and one real number:" << endl;
    int interger_2;
    double real_number_1;
    cin >> interger_2 >> real_number_1;
    cin.ignore(1000,'\n');
    cout << "The real number is: " << fixed << setprecision(3) << real_number_1 << "\nThe integer is: " << interger_2 << endl;
    cout << "\n" << endl;

    // ---------------------
    cout << "Enter one real number and integer:" << endl;
    int interger_3;
    double real_number_2;
    cin >> real_number_2 >> interger_3;
    cin.ignore(1000,'\n');
    cout << "The real number is: " << fixed << setprecision(3) << real_number_2 << "\nThe integer is: " << interger_3 << endl;
    cout << "\n" << endl;

    // ---------------------
    cout << "Enter a charector:" << endl;
    char character_1;
    cin >> character_1;
    cin.ignore(1000,'\n');
    cout << "You entered the character: " << character_1 << endl;
    cout << "\n" << endl;

    // ---------------------
    cout << "Enter a word:" << endl;
    string word_1;
    cin >> word_1;
    cin.ignore(1000,'\n');
    cout << "The word: " << word_1 << " has " << word_1.size() << " charecter(s)" << endl;
    cout << "\n" << endl;

    // ---------------------
    cout << "Enter an integer and a word:" << endl;
    int interger_4;
    string word_2;
    cin >> interger_4 >> word_2;
    cin.ignore(1000,'\n');
    cout << "You entered '" << interger_4 << "' and '" << word_2 << "'" << endl;
    cout << "\n" << endl;

    // ---------------------
    cout << "Enter a charecter and a word:" << endl;
    char character_2;
    string word_3;
    cin >> character_2 >> word_3;
    cin.ignore(1000,'\n');
    cout << "You entered the string \"" << word_3 << "\" and the character \"" << character_2 << "\"" << endl;
    cout << "\n" << endl;

    // ---------------------
    cout << "Enter a word and a real number:" << endl;
    string word_4;
    double real_number_3;
    cin >> word_4 >> real_number_3;
    cin.ignore(1000,'\n');
    cout << "You entered \"" << word_4 << "\" and \"" << fixed << setprecision(3) << real_number_3 << "\"" << endl;
    cout << "\n" << endl;

    // ---------------------
    cout << "Enter a text-line:" << endl;
    string text_line_1;
    getline(cin, text_line_1, '\n');
    cin.ignore(1000,'\n');
    cout << "You entered: \"" << text_line_1 << "\""<<  endl;
    cout << "\n" << endl;

    // ---------------------
    cout << "Enter a second line of text:" << endl;
    string text_line_2;
    getline(cin, text_line_2, '\n');
    cin.ignore(1000,'\n');
    cout << "You entered: \"" << text_line_2 << "\""<<  endl;
    cout << "\n" << endl;

    // ---------------------
    cout << "Enter three words:" << endl;
    string word_5, word_6, word_7;
    cin >> word_5 >> word_6 >> word_7;
    cin.ignore(1000,'\n');
    cout << "You entered: '" << word_5 << "'" << endl;

    // exit program
    return 0;
}
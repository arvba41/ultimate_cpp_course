#include<iostream>
#include<iomanip>

using namespace std;

void print_header();

int main()
{   
    float first_price, last_price, tax_percent, stride;

    cout << "INPUT PART" << endl;
    cout << "==========" << endl;
    cout << "Enter first price: ";
    cin >> first_price;
    if (first_price < 0)
    {
        cout << "ERROR: first price must be greater or equal than 0" << endl;
        return 1;
    }
    cout << "Enter last price: ";
    cin >> last_price;
    if (last_price < first_price)
    {
        cout << "ERROR: Last price must be greater or equal than first price" << endl;
        return 1;
    }
    cout << "Enter stride: ";
    cin >> stride;
    if (stride < 0.0099) // 0.01 is the minimum value for the stride but using 0.0099 to avoid floating point errors
    {
        cout << "ERROR: Stride must be greater than or equal to 0.01" << endl;
        return 1;
    }
    cout << "Enter tax percent: ";
    cin >> tax_percent;
    if(tax_percent < 0)
    {
        cout << "ERROR: Tax percent must be greater or equal than 0" << endl;
        return 1;
    }

    // Printing the header 
    cout << "OUTPUT PART" << endl;
    cout << "===========" << endl;
    print_header();

    // Printing the table
    float price = first_price;
    float tax, price_with_tax;
    int total_entries = (int)((last_price - first_price) / stride + 1);
    if (total_entries > 100)
    {
        cout << "WARNING, unreasonable values: Total entries are greater than 100. Are you sure you want to continue? ([y]/n): ";
        char choice;
        cin >> choice;
        if (choice == 'n')
        {
            return 0;
        }
    }
    for (int i = 0; i <= total_entries; i++)
    {
        tax = price * tax_percent / 100;
        price_with_tax = price + tax;
        cout << setfill(' ') << fixed << setprecision(2) << setw(10) << price << setw(10) << tax << setw(20) << price_with_tax << endl;
        price = first_price + i * stride;
    }

}

void print_header()
{
    string headers[3];
    headers[0] = "Price";
    headers[1] = "Tax";
    headers[2] = "Price with tax";
    
    cout << setw(10) << headers[0] << setw(10) << headers[1] << setw(20) << headers[2] << endl;
    cout << setfill('-') << setw(40) << "-" << endl;
}
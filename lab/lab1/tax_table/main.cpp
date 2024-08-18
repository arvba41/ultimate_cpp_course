#include<iostream>
#include<iomanip>

using namespace std;

void print_header();

int main()
{   
    float firt_price, last_price, tax_percent, stride;

    cout << "INPUT PART" << endl;
    cout << "==========" << endl;
    cout << "Enter firt price: ";
    cin >> firt_price;
    if (firt_price < 0)
    {
        cout << "ERROR: Firt price must be greater or equal than 0" << endl;
        return 1;
    }
    cout << "Enter last price: ";
    cin >> last_price;
    if (last_price < firt_price)
    {
        cout << "ERROR: Last price must be greater or equal than firt price" << endl;
        return 1;
    }
    cout << "Enter stride: ";
    cin >> stride;
    cout << stride << endl;
    if (stride < 0.01)
    {
        cout << "ERROR: Stride must be greater than or equal to 0.01" << endl;
        return 1;
    }
    cout << "Enter tax percent: ";
    cin >> tax_percent;
    if (tax_percent < 0)
    {
        cout << "ERROR: Tax percent must be greater or equal than 0" << endl;
        return 1;
    }

    // Printing the header 
    print_header();

    // Printing the table
    float price = firt_price;
    float tax = 0;
    float price_with_tax = 0;
    for (price; price <= last_price; price += stride)
    {
        tax = price * tax_percent / 100;
        price_with_tax = price + tax;
        cout << setfill(' ') << fixed << setprecision(2) << setw(10) << price << setw(10) << tax << setw(20) << price_with_tax << endl;
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
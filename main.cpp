#include <iostream>
#include "contactbook.h"

int main()
{
    ContactBook contactBook;

    if (!contactBook.initDatabase("localhost", "user", "password", "database"))
    {
        std::cerr << "Failed to connect to database." << std::endl;
        return 1;
    }
    contactBook.run();
    return 0;
}
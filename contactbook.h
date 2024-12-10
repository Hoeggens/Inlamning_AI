#ifndef CONTACTBOOK_H
#define CONTACTBOOK_H

#include <vector>
#include <string>
#include <mysql/mysql.h>

class Contact
{
public:
    std::string name;
    std::vector<std::string> emails;
    std::vector<std::string> phoneNumbers;
};

class ContactBook
{
public:
    bool initDatabase(const std::string &host, const std::string &user, const std::string &password, const std::string &dbname);
    void run();

private:
    std::vector<Contact> contacts;
    MYSQL *conn;
    void displayMenu();
    void addContact();
    void searchContact();
    void displayContact(const Contact &contact);
    void saveContactToDatabase(const Contact &contact);
    std::vector<Contact> loadContactsFromDatabase();
};

#endif // CONTACTBOOK_H
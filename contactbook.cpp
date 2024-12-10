#include "contactbook.h"
#include <iostream>
#include <algorithm>

bool ContactBook::initDatabase(const std::string &host, const std::string &user, const std::string &password, const std::string &dbname)
{
    conn = mysql_init(nullptr);
    if (conn == nullptr)
    {
        std::cerr << "mysql_init() failed. Error: " << mysql_error(conn) << std::endl;
        return false;
    }

    if (mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), dbname.c_str(), 0, nullptr, 0) == nullptr)
    {
        std::cerr << "mysql_real_connect() failed. Error: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return false;
    }

    contacts = loadContactsFromDatabase();
    return true;
}

void ContactBook::run()
{
    while (true)
    {
        displayMenu();
        int choice;
        std::cin >> choice;
        switch (choice)
        {
        case 1:
            addContact();
            break;
        case 2:
            searchContact();
            break;
        case 3:
            return;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}

void ContactBook::displayMenu()
{
    std::cout << "1. Add Contact" << std::endl;
    std::cout << "2. Search Contact" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

void ContactBook::addContact()
{
    Contact contact;
    std::cout << "Enter name: ";
    std::cin >> contact.name;

    std::string email;
    std::cout << "Enter emails (type 'done' to finish): ";
    while (std::cin >> email && email != "done")
    {
        contact.emails.push_back(email);
    }

    std::string phoneNumber;
    std::cout << "Enter phone numbers (type 'done' to finish): ";
    while (std::cin >> phoneNumber && phoneNumber != "done")
    {
        contact.phoneNumbers.push_back(phoneNumber);
    }

    contacts.push_back(contact);
    saveContactToDatabase(contact);
    std::cout << "Contact added successfully!" << std::endl;
}

void ContactBook::searchContact()
{
    std::string query;
    std::cout << "Enter name, email, or phone number to search: ";
    std::cin >> query;

    for (const auto &contact : contacts)
    {
        if (contact.name == query ||
            std::find(contact.emails.begin(), contact.emails.end(), query) != contact.emails.end() ||
            std::find(contact.phoneNumbers.begin(), contact.phoneNumbers.end(), query) != contact.phoneNumbers.end())
        {
            displayContact(contact);
            return;
        }
    }
    std::cout << "Contact not found." << std::endl;
}

void ContactBook::displayContact(const Contact &contact)
{
    std::cout << "Name: " << contact.name << std::endl;
    std::cout << "Emails: ";
    for (const auto &email : contact.emails)
    {
        std::cout << email << " ";
    }
    std::cout << std::endl;
    std::cout << "Phone Numbers: ";
    for (const auto &phoneNumber : contact.phoneNumbers)
    {
        std::cout << phoneNumber << " ";
    }
    std::cout << std::endl;
}

void ContactBook::saveContactToDatabase(const Contact &contact)
{
    std::string query = "INSERT INTO contacts (name) VALUES ('" + contact.name + "')";
    if (mysql_query(conn, query.c_str()))
    {
        std::cerr << "INSERT failed. Error: " << mysql_error(conn) << std::endl;
        return;
    }

    int contact_id = mysql_insert_id(conn);

    for (const auto &email : contact.emails)
    {
        query = "INSERT INTO emails (contact_id, email) VALUES (" + std::to_string(contact_id) + ", '" + email + "')";
        if (mysql_query(conn, query.c_str()))
        {
            std::cerr << "INSERT failed. Error: " << mysql_error(conn) << std::endl;
        }
    }

    for (const auto &phoneNumber : contact.phoneNumbers)
    {
        query = "INSERT INTO phone_numbers (contact_id, phone_number) VALUES (" + std::to_string(contact_id) + ", '" + phoneNumber + "')";
        if (mysql_query(conn, query.c_str()))
        {
            std::cerr << "INSERT failed. Error: " << mysql_error(conn) << std::endl;
        }
    }
}

std::vector<Contact> ContactBook::loadContactsFromDatabase()
{
    std::vector<Contact> contacts;
    if (mysql_query(conn, "SELECT id, name FROM contacts"))
    {
        std::cerr << "SELECT failed. Error: " << mysql_error(conn) << std::endl;
        return contacts;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == nullptr)
    {
        std::cerr << "mysql_store_result() failed. Error: " << mysql_error(conn) << std::endl;
        return contacts;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        Contact contact;
        contact.name = row[1];

        std::string query = "SELECT email FROM emails WHERE contact_id = " + std::string(row[0]);
        if (mysql_query(conn, query.c_str()))
        {
            std::cerr << "SELECT failed. Error: " << mysql_error(conn) << std::endl;
            continue;
        }

        MYSQL_RES *email_res = mysql_store_result(conn);
        if (email_res != nullptr)
        {
            MYSQL_ROW email_row;
            while ((email_row = mysql_fetch_row(email_res)) != nullptr)
            {
                contact.emails.push_back(email_row[0]);
            }
            mysql_free_result(email_res);
        }

        query = "SELECT phone_number FROM phone_numbers WHERE contact_id = " + std::string(row[0]);
        if (mysql_query(conn, query.c_str()))
        {
            std::cerr << "SELECT failed. Error: " << mysql_error(conn) << std::endl;
            continue;
        }

        MYSQL_RES *phone_res = mysql_store_result(conn);
        if (phone_res != nullptr)
        {
            MYSQL_ROW phone_row;
            while ((phone_row = mysql_fetch_row(phone_res)) != nullptr)
            {
                contact.phoneNumbers.push_back(phone_row[0]);
            }
            mysql_free_result(phone_res);
        }

        contacts.push_back(contact);
    }

    mysql_free_result(res);
    return contacts;
}
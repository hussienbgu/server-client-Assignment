//
// Created by amr on 10/01/2020.
//

#ifndef BOOST_ECHO_CLIENT_BOOK_H
#define BOOST_ECHO_CLIENT_BOOK_H


#include <boost/algorithm/string.hpp>
#include <string>

class Book {
private:
    std::string bookName;
    std::string bookGenre;
    std::string previousHandler;
    bool hasTheBook;

public:
    Book(std::string name,std::string genre,std::string previousHandler,bool hasTheBook);
    std::string getBookName();
    std::string getBookGenre();
    std::string getPreviousHandler();
    bool getTheBook();
    void setBookStatus(bool status);
};


#endif //BOOST_ECHO_CLIENT_BOOK_H

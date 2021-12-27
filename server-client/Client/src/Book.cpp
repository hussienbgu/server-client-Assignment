//
// Created by amr on 10/01/2020.
//

#include "Book.h"
Book::Book(std::string name, std::string genre,std::string previousHandler,bool hasTheBook) :bookName(name), bookGenre(genre),previousHandler(previousHandler),hasTheBook(true){};


std::string Book::getBookGenre() {
    return bookGenre;
}
std::string Book::getPreviousHandler() {
    return previousHandler;
}

std::string Book::getBookName() {
    return bookName;
}

bool Book::getTheBook() {
    return hasTheBook;
}

void Book::setBookStatus(bool status) {
    hasTheBook= status;
}

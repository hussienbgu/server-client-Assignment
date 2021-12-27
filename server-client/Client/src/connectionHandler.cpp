#include <connectionHandler.h>
#include <boost/algorithm/string.hpp>
using boost::asio::ip::tcp;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
#include <map>
#include <iostream>
#include <cassert>
#include <list>
#include <iterator>
ConnectionHandler::ConnectionHandler(string host, short port):host_(host), port_(port), io_service_(), socket_(io_service_),Username(""),Password(""),RecieptIDcounter(0),SubscriptionIDcounter(0),MessageIDcounter(0),Subscriptions(),ReversedSubscriptions(),WantedBooks(),Inventory(),RecieptsGenres(),RecieptsTypes() {}
    
ConnectionHandler::~ConnectionHandler() {
    close();
}
 
bool ConnectionHandler::connect() {
    std::cout << "Starting connect to " 
        << host_ << ":" << port_ << std::endl;
    try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);
		if (error)
			throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead){
    size_t tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
			tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);			
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, '\0');
}

bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameAscii(line, '\0');
}
 

bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
        do {
            if (!getBytes(&ch, 1)) {
                return false;
            }
            if (ch != '\0')
                frame.append(1, ch);
        } while (delimiter != ch);
    }
    catch (std::exception& e) {
        std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
    }
            std::vector<std::string> commands;
            boost::split(commands, frame, [](char c) { return c == '\n'; });
            if (commands[0] == "CONNECTED") {
                frame = "login successful";
            }
            else if (commands[0] == "RECEIPT") {
                std::string IDR = commands[1].substr(commands[1].find(":")+1);
                if (RecieptsTypes.at(IDR)=="logout"){
                    frame="DISCONNECTED";
                }

                else if (RecieptsTypes.at(IDR)=="join"){
                    std::string genre=RecieptsGenres.at(IDR);
                    frame="Joined club "+genre;
                }
                else {
                    std::string genre = RecieptsGenres.at(IDR);
                    frame = "Exited club " + genre;
                }
            }
            else if (commands[0] == "MESSAGE") {
                std::string Destination = commands[3].substr(commands[3].find(':')+1);
                std::vector<std::string> arguments;
                boost::split(arguments, commands[5], [](char c) { return c == ' '; });
                if (commands[5].find("book status") != std::string::npos) {
                    std::vector<std::string> BooksfromthisGenre;
                    for (unsigned long i = 0; i < Inventory.size(); i++) {
                        if (Inventory.at(i).getBookGenre() == Destination && Inventory.at(i).getTheBook()) {
                            BooksfromthisGenre.push_back(Inventory.at(i).getBookName());
                        }
                    }
                    std::string frame1 = Username + ":";
                    if (BooksfromthisGenre.size() > 0) {
                        for (unsigned long i = 0; i < BooksfromthisGenre.size(); i++) {
                            frame1 = frame1 + BooksfromthisGenre[i] + ",";
                        }
                        frame1.pop_back();
                    }
                    std::string toSend = "SEND\ndestination:" + Destination + "\n\n" + frame1+"\n"+'\0';// to check
                    sendBytes(toSend.c_str(), toSend.length());
                    frame = Destination+":"+commands[5];
                }
                else if (commands[5].find("wish to borrow") != std::string::npos) {
                    if (arguments[0] != Username) {
                        string bookname;
                        for (unsigned long i = 4; i < arguments.size(); i++) {
                            bookname = bookname + arguments[i] + " ";
                        }
                        bookname.pop_back();
                        for (unsigned long i = 0; i < Inventory.size(); i++) {
                            if (Inventory.at(i).getBookName() == bookname && Inventory.at(i).getTheBook()) {
                                std::string genre = commands[3].substr(commands[3].find(':') + 1);
                                std::string toSend =
                                        "SEND\ndestination:" + genre + "\n\n" + Username + " has " + bookname + "\n" +
                                        '\0';
                                sendBytes(toSend.c_str(), toSend.length());
                            }
                        }
                    }
                    frame = Destination + ":" + commands[5];
                }
                else if (commands[5].find("Taking ") != std::string::npos) {
                    string bookname;
                    for (unsigned long i=1; i<arguments.size()-2; i++){
                        bookname=bookname+arguments[i]+" ";
                    }
                    bookname.pop_back();
                    if (Username == arguments[arguments.size()-1]) {
                        for (unsigned long i = 0; i < Inventory.size(); i++) {
                            if (Inventory.at(i).getBookName() == bookname)
                                Inventory.at(i).setBookStatus(false);
                        }
                    }
                    frame = Destination+":"+commands[5];
                }
                else if (commands[5].find("Returning") != std::string::npos) {
                    string bookname;
                    for (unsigned long i=1; i<arguments.size()-2; i++){
                        bookname=bookname+ arguments[i]+" ";
                    }
                    bookname.pop_back();
                    std::cout<<"The book name is "<<bookname<<std::endl;
                    if (arguments[arguments.size()-1] == Username) {
                        for (unsigned long i = 0; i < Inventory.size(); i++) {
                            if (Inventory.at(i).getBookName() == bookname) {
                                Inventory.at(i).setBookStatus(true);
                            }
                        }
                    }
                    frame = Destination+":"+commands[5];
                }
                else if (commands[5].find("has") != std::string::npos && arguments[2]!="added") {
                    if (arguments[0] != Username) {
                        std::string BOOKNAME;
                        for (unsigned long i = 2; i < arguments.size(); i++) {
                            BOOKNAME = BOOKNAME + arguments[i] + " ";
                        }
                        BOOKNAME.pop_back();
                        for (unsigned long j = 0; j < WantedBooks.size(); j++) {
                            if (WantedBooks.at(j) == BOOKNAME) {
                                Book toBeAdded(BOOKNAME, Destination, arguments[0], true);
                                Inventory.push_back(toBeAdded);
                                WantedBooks.erase(WantedBooks.begin()+j);
                                std::string toSend =
                                        "SEND\ndestination:" + Destination + "\n\n" + "Taking " + BOOKNAME + " from " +
                                        arguments[0] + '\n' + '\0';//maybe need to change the destination
                                sendBytes(toSend.c_str(), toSend.length());
                                break;
                            }
                        }
                    }
                    frame = Destination + ":" + commands[5];
                }
                else {
                    frame = Destination+":"+commands[5];
                }
            }
            else if(commands[0]=="ERROR"){
                std::string theErrorMsg=commands[2].substr(commands[2].find(':')+1);
                frame="ERROR:"+theErrorMsg;

            }
	return true;
    };
 
 
bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
    bool result;
    std::vector<std::string> commands;
    boost::split(commands,frame,[](char c){return c==' ';});

    if(commands[0]=="login"){
        Username= commands[2];
        Password= commands[3];
        std::string Frame="CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:" + Username +"\npasscode:"+commands[3]+"\n"+"\n";
        result=sendBytes(Frame.c_str(),Frame.length());
    }

    else if(commands[0]=="join"){
        bool alreadyJoined=false;
        std::map<std::string,std::string>::iterator it;
        for(it=Subscriptions.begin(); it!=Subscriptions.end(); it++){
            if (it->second==commands[1])
                alreadyJoined=true;
        }
        if (!alreadyJoined){
            std::string IDS = std::to_string(SubscriptionIDcounter);
            std::string IDR = std::to_string(RecieptIDcounter);
            RecieptsGenres.insert(std::pair<std::string, std::string>(IDR, commands[1]));
            RecieptsTypes.insert(std::pair<std::string, std::string>(IDR, "join"));
            Subscriptions.insert(std::pair<std::string, std::string>(IDS, commands[1]));
            ReversedSubscriptions.insert(std::pair<std::string, std::string>(commands[1], IDS));
            std::string Frame = "SUBSCRIBE\ndestination:" + commands[1] + "\nid:" + IDS + "\nreceipt:" + IDR + "\n";
            SubscriptionIDcounter++;
            RecieptIDcounter++;
            result = sendBytes(Frame.c_str(), Frame.length());
        }
        else {
            std::cout<<"You are already a member of "+commands[1]+" club!"<<std::endl;
            return true;
        }
    }

    else if(commands[0]=="exit") {
        bool aMember= false;
        std::map<std::string,std::string>::iterator it;
        for(it=Subscriptions.begin(); it!=Subscriptions.end(); it++){
            if (it->second==commands[1])
                aMember= true;
        }
        if (aMember){
            std::string IDR = std::to_string(RecieptIDcounter);
            RecieptIDcounter++;
            RecieptsGenres.insert(std::pair<std::string, std::string>(IDR, commands[1]));
            RecieptsTypes.insert(std::pair<std::string, std::string>(IDR, "exit"));
            std::string IDS = ReversedSubscriptions.at(commands[1]);
            ReversedSubscriptions.at(commands[1]).erase();
            Subscriptions.at(IDS).erase();
            std::string Frame = "UNSUBSCRIBE\nid:" + IDS + "\nreceipt:" + IDR + "\n";
            result = sendBytes(Frame.c_str(), Frame.length());
        }
        else {
            std::cout<<"You are not a member in "+commands[1]+" club!"<<std::endl;
            return true;
        }
    }

    else if(commands[0]=="add"){
        std::string bookname;
        for (unsigned long i=2; i<commands.size(); i++){
            bookname=bookname+commands[i]+" ";
        }
        bookname.pop_back();
        Book toBeAdded(bookname,commands[1], Username,true);
        Inventory.push_back(toBeAdded);
        std::string Frame="SEND\ndestination:"+commands[1]+"\n\n"+Username+" has added the book "+ bookname+"\n";
        result=sendBytes(Frame.c_str(),Frame.length());
    }

    else if(commands[0]=="borrow"){
        std::string bookName;
        for (unsigned long i=2; i<commands.size(); i++){
            bookName=bookName+commands[i]+" ";
        }
        bookName.pop_back();
        std::string Frame="SEND\ndestination:"+commands[1]+"\n\n"+Username+ " wish to borrow "+ bookName+"\n";
        WantedBooks.push_back(bookName);
        result=sendBytes(Frame.c_str(),Frame.length());

    }

    else if(commands[0]=="return"){
        std::string bookname;
        for (unsigned long i=2; i<commands.size(); i++){
            bookname=bookname+commands[i]+" ";
        }
        bookname.pop_back();
        std::string PH;
        bool hasTheBook=false;
        for (unsigned long i=0; i<Inventory.size(); i++){
            if (Inventory.at(i).getBookName()==bookname&&Inventory.at(i).getTheBook()){
                PH=Inventory.at(i).getPreviousHandler();
                if(PH==Username){
                    std::cout<<"The book "+bookname+" is yours!"<<std::endl;
                    return true;
                }
                else {
                    Inventory.erase(Inventory.begin() + i);
                    std::string Frame =
                            "SEND\ndestination:" + commands[1] + "\n\n" + "Returning " + bookname + " " + "to " + PH +
                            "\n";
                    result = sendBytes(Frame.c_str(), Frame.length());
                    hasTheBook = true;
                    break;
                }
            }
        }
        if(!hasTheBook){
            std::cout<<"You don't have the book "+bookname+"!"<<std::endl;
            return true;
        }
    }

    else if(commands[0]=="status"){
        std::string Frame="SEND\ndestination:" + commands[1]+"\n\nbook status\n";
        result=sendBytes(Frame.c_str(),Frame.length());
    }

    else if(commands[0]=="logout"){
        std::string IDR=std::to_string(RecieptIDcounter);
        RecieptsTypes.insert(std::pair<std::string,std::string>(IDR,"logout"));
        RecieptIDcounter++;
        std::string Frame="DISCONNECT\nreceipt:" + IDR+"\n";
        result=sendBytes(Frame.c_str(),Frame.length());

    }
	if(!result) return false;
	return sendBytes(&delimiter,1);
}

// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }

}


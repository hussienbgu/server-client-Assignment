#include <stdlib.h>
#include <thread>
#include "../include/Book.h"
#include <connectionHandler.h>
#include "../include/connectionHandler.h"
/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
void reicever(ConnectionHandler* handler);
void writer(ConnectionHandler* handler);
bool Loggedin=false;
int main (int argc, char *argv[]) {
    while (1) {
        std::string connect;
        getline(std::cin, connect);
        if (connect == "bye") {
            break;
        }
        std::vector<std::string> LoginArguments;
        std::vector<std::string> commands;
        boost::split(commands, connect, [](char c) { return c == ' '; });
        boost::split(LoginArguments, commands[1], [](char c) { return c == ':'; });
        std::string host = LoginArguments[0];
        short port = std::stoi(LoginArguments[1]);
        ConnectionHandler connectionHandler(host, port);
        if (connectionHandler.connect()) {
            std::string output;
            connectionHandler.sendLine(connect);
            connectionHandler.getLine(output);
            if (output != "login successful") {
                std::cout << output << std::endl;
            } else {
                std::cout << output << std::endl;
                Loggedin = true;
                std::thread sender(writer, &connectionHandler);
                std::thread getter(reicever, &connectionHandler);
                getter.join();
                sender.detach();
            }

        }
    }
}


void reicever(ConnectionHandler* handler) {
    while (Loggedin) {
        std::string result;
        if (!(handler->getLine(result))) {
            break;
        }
        std::cout << result << std::endl;
        if (result == "DISCONNECTED" || result.find("ERROR")!=std::string::npos) {
            Loggedin = false;
            return;
        }
    }
}

void writer(ConnectionHandler* handler){
    while(Loggedin){
        std::string input;
        std::getline(std::cin, input);
        if (!Loggedin){ break;}
        if (!(handler->sendLine(input))) {
            break;
        }
        if(input=="logout"){
            return;
        }
    }
}

#include <string>
#include <iostream>
#include <sstream>

#include <sys/socket.h>	
#include <arpa/inet.h>	
#include <unistd.h>

#include "LineInfo.h"

using namespace std;

const int MESSAGE_MAX_SIZE = 1024;
const int ERROR = -1;
const int SOCK_PORT = 8888;
const string LOCAL_HOST_ADDRESS = "127.0.0.1";
const int MAX_GUESS_LIMIT = 100;
const int MAX_NO_TRIES = 12;

int main(int argc, char* argv[]){
	int socketServerId;
	sockaddr_in server; 
	string messageStr;
	try{
		if((socketServerId = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)                    
			throw domain_error(LineInfo("Socket socket create failure ",__FILE__,__LINE__)); 
		server.sin_addr.s_addr = inet_addr(LOCAL_HOST_ADDRESS.c_str());
		server.sin_family = AF_INET;
		server.sin_port = htons(SOCK_PORT);

		if( (connect(socketServerId, (struct sockaddr*) &server, sizeof(server)) ) == ERROR)
			throw domain_error(LineInfo("Socket connect failure ",__FILE__,__LINE__));
		cout << endl << endl << "Try to guess a number between 1 and " << MAX_GUESS_LIMIT << endl << endl;
		int nooftries = 0;
		int guessno;

		do{                                                             
			cout << "Current Try Number : " << ++nooftries << endl;        
			cout << "Enter number guess : " << endl << endl;
			cin >> guessno;

			stringstream gnoss;
			gnoss << guessno;
			string guessNoStr = gnoss.str();

			if(( write(socketServerId, guessNoStr.c_str(), guessNoStr.size() + 1)) == ERROR)
				throw domain_error(LineInfo("Socket write failure ",__FILE__,__LINE__));
			char receivedMessage[MESSAGE_MAX_SIZE] = {0};
			if(( read(socketServerId, receivedMessage, MESSAGE_MAX_SIZE)) == ERROR)
				throw domain_error(LineInfo("Socket read failure ",__FILE__,__LINE__));
			string receiveMessageStr = receivedMessage;
			cout << endl << "Server replt : " << endl << endl << receiveMessageStr << endl << endl;

			if(receiveMessageStr.find("Win") != string::npos){
				break;
			}
			if(nooftries == MAX_NO_TRIES){                                                             
				cout << endl << "The maximum number of tries " << MAX_NO_TRIES << " is done." << endl;    
				if((write(socketServerId, "Send Random Number", sizeof("Send Random Number")+1)) == ERROR)
					throw domain_error(LineInfo("Socket write failure ",__FILE__,__LINE__));
				char randomNoMessage[MESSAGE_MAX_SIZE] = {0};
				if((read(socketServerId,randomNoMessage, MESSAGE_MAX_SIZE)) == ERROR)
					throw domain_error(LineInfo("Socket read failure ",__FILE__,__LINE__));
				int randomno = stoi(randomNoMessage);
				cout << "The random number is : " << randomno << endl << endl;
				cout << "Game is over " << endl << endl;
				break;
			}
		}while(true);
		if (close(socketServerId) == ERROR)                                                  
        		throw domain_error(LineInfo("Socket close failure ", __FILE__, __LINE__));
	}
	catch(exception& e){
		cout << e.what() << endl;
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

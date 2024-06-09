
#include <iostream>
#include <string>
#include <sstream>

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

#include "LineInfo.h"

using namespace std;  

const int MESSAGE_MAX_SIZE = 1024;
const int ERROR = -1;

const int SOCK_PORT  = 8888;
const int LISTEN_QUEUE_SIZE = 3;

const int HIGHEST_NO = 100;

#include "LineInfo.h"

int main(int argc, char* argv[]) {
  int    socket_desc, client_sock, read_size, addrsize, status;
  struct sockaddr_in  server, client;
try{
	if(argc != 1)
		throw domain_error(LineInfo("Usage: ./socketserver & ",__FILE__,__LINE__));   
                                                                                   

		if((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
			throw domain_error(LineInfo("Socket socket create failure ",__FILE__,__LINE__));

		server.sin_family      = AF_INET;
	  server.sin_addr.s_addr = INADDR_ANY;
	  server.sin_port        = htons(SOCK_PORT);

		if(( bind(socket_desc, (struct sockaddr*)&server, sizeof(server))) == ERROR)
		throw domain_error(LineInfo("Socket bind failure ",__FILE__,__LINE__));
	
	if((listen(socket_desc, LISTEN_QUEUE_SIZE)) == ERROR)
		throw domain_error(LineInfo("Socket listen failure ",__FILE__,__LINE__));

	addrsize = sizeof(struct sockaddr_in);

		if((client_sock = accept(socket_desc, (struct sockaddr*) &client, (socklen_t*) &addrsize)) == ERROR)
			throw domain_error(LineInfo("Socket accept failure ",__FILE__,__LINE__));

	time_t t;
	srand((unsigned)time(&t));                     
	int randomno = (rand() % HIGHEST_NO + 1);
	stringstream rnoss; rnoss << randomno;
	string randomnostr = rnoss.str();
	
	string playresultstr;
	int guessno;
	string guessnostr;

		do{                                                                                     
			char clientGuessNoMessages[MESSAGE_MAX_SIZE] = {0};                                   
			if((read_size = read(client_sock, clientGuessNoMessages, MESSAGE_MAX_SIZE)) == ERROR)
				throw domain_error(LineInfo("Socket read failure ",__FILE__,__LINE__));
			guessnostr = clientGuessNoMessages;
			if(guessnostr.find("Random Number") != std::string::npos){
				if((write(client_sock, randomnostr.c_str(), randomnostr.size() + 1)) == ERROR)
					throw domain_error(LineInfo("Socket write failure ",__FILE__,__LINE__));

			}
			else{
				if(read_size > 0){
					string clientGuessNoMessageStr(clientGuessNoMessages);
					guessno = stoi(clientGuessNoMessages);
					guessnostr = clientGuessNoMessages;
					if(randomno == guessno){
						playresultstr = "YouWin !! The number is : " + randomnostr;
          }
					else if(randomno > guessno){
						playresultstr = "The number is greater than : " + guessnostr;
          }
					else if(randomno < guessno){
						playresultstr = "The number is less than : " + guessnostr;
          }

					if(write(client_sock, playresultstr.c_str(), (playresultstr.size() + 1)) == ERROR)
						throw domain_error(LineInfo("Socket write failure ",__FILE__,__LINE__));

				}
				else if(read_size == 0){                            
					cout << "Client disconnected" << endl;
					fflush(stdout);
					break;
				}
				else if(read_size == ERROR){
					cout << "Failure: recieve" << endl;
					exit(EXIT_FAILURE);
				}
			}
		}while(true);
	}//try
	catch(exception& e){                 
		cout << e.what() << endl;
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
		
/*
Program Name: forkexecvp
Date: 4/10/2023
Author: Matthew Basinger
Module Purpose
This program will use forks and the calculate method to create a random list of numbers to have the calculate method performed on them
*/
#include <string>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdexcept>
#include <sstream>
#include "LineInfo.h"
using namespace std;

int const READ = 0;              //Instanciation of the instance variables
int const WRITE = 1;
int const PIPE_ERROR = -1;
int const CHILD_PID = 0;
int const MAX_PIPE_MESSAGE_SIZE = 1000;
int const EXECVP_FAILED = -1;
                                                //Method to create a file filled with random numbers the nubmer
                                                //of random items and the range are given in the method call
void CreateArg1FileWithArg2RandomNumbersArg3RandomRange(string randomFileNameStr, unsigned noRandomNumberUns, unsigned  randomRange){
  ofstream outfileStream(randomFileNameStr);
  if(outfileStream.fail()){
    stringstream s;
    s << "Error opening file Random Numbers File " << randomFileNameStr << endl;
    throw domain_error(LineInfo(s.str(),__FILE__,__LINE__));
  }
  
  for(unsigned i=0; i<noRandomNumberUns;++i)
    outfileStream << ( ( rand() % randomRange) + 1) << endl;
    
  outfileStream.close();
}


int main(int argc, char* argv[]){                    //main method that performs all of the calls
  try{
    if(argc != 4){
      stringstream s;                                            //check to ensure the correct number of arguments is correct
      s << endl << endl << "Wrong number of arguments" << endl << "Usage: " << endl
        << "forkexecvp <file to create> <amount of random numbers> <random number range>" 
        << endl << "Example:" << endl << "forkexecvp randomnumber 10000 1000" << endl << endl;
      throw domain_error(LineInfo(s.str(),__FILE__,__LINE__));
    }
    
    string randomFileNameStr = argv[1];                //creation of range and counter variables as well as the file creation method
    unsigned noRandomNumberUns = atoi(argv[2]),
             randomRange = atoi(argv[3]);
    
    cout << endl << argv[0] << " " << randomFileNameStr << " " << noRandomNumberUns << " " << randomRange << endl;    
    CreateArg1FileWithArg2RandomNumbersArg3RandomRange(randomFileNameStr,noRandomNumberUns,randomRange);
    pid_t pid;
    int pipeParentWriteChildReadfds[2];
    
    string messages[] = {"sum","average", "greatest", "least"};                //the calling of the child process to perform all of the
    int noOfMessages = sizeof(messages) / sizeof(messages[0]);                 //different processes as well as throwing the error if the pipe fails
    for(int childProcessNo=0; childProcessNo < noOfMessages; ++childProcessNo){
      if(pipe(pipeParentWriteChildReadfds) == PIPE_ERROR){
        stringstream s;
        s << "Unable to create pipe pipeParentWriteChildReadfds";
        throw domain_error(LineInfo(s.str(),__FILE__,__LINE__));
      }
      
      pid_t forkpid = fork();
      
      if(forkpid < 0){                                                //closes the pipes and throws the domain errors
        stringstream s;                                               //if the fork, pipe, or pipe close fail
        s << "Fork failed\n";
        throw domain_error(LineInfo(s.str(),__FILE__,__LINE__));
      }
        
      if(forkpid != CHILD_PID){
        close(pipeParentWriteChildReadfds[READ]);
        cout << "Parent pid: " << getpid() << "to Child Process No: " << childProcessNo  
          << endl << "Sent Message: " << messages[childProcessNo] << endl;
        if(write(pipeParentWriteChildReadfds[WRITE], messages[childProcessNo].c_str(),
                 sizeof(messages[childProcessNo].c_str())) == PIPE_ERROR){
          stringstream s;
          s << "pipe write failed";
          throw domain_error(LineInfo(s.str(),__FILE__,__LINE__));
        }
      }
      else{
        close(pipeParentWriteChildReadfds[WRITE]);
        char pipeReadMessage[MAX_PIPE_MESSAGE_SIZE] = {0};                  //more pipe things as well as calling the calculate method
        if(read(pipeParentWriteChildReadfds[READ], pipeReadMessage,         //with the different arguments
                sizeof(pipeReadMessage)) == PIPE_ERROR){
          stringstream s;
          s << "pipe read failed";
          throw domain_error(LineInfo(s.str(),__FILE__,__LINE__));
        }
        cout << "Child pid: " << getpid() << " Child Process No: " << childProcessNo <<
              " Recieved Message: " << pipeReadMessage << endl;
              
        char * arglist[] = { (char *)"./calculate", pipeReadMessage, (char *)randomFileNameStr.c_str(), NULL};
        cout << "Child pid: " << getpid() << " Child Process No: " << childProcessNo <<
                "execvp(" << arglist[0] << ", ./calculate," << randomFileNameStr.c_str() <<",NULL" << endl;
        string answerFileName = "answer"; answerFileName += pipeReadMessage; answerFileName += ".txt";
        freopen(answerFileName.c_str(),"w",stdout);
        close(pipeParentWriteChildReadfds[READ]);
        if(execvp(arglist[0], arglist) == EXECVP_FAILED){
          stringstream s;
          s << "execvp failed";
          throw domain_error(LineInfo(s.str(),__FILE__,__LINE__));      
        }                                                              //prints out the parent pid values and waits until
      }                                                                //all of the children pipes close
    }
    cout << "Parent pid: " << getpid() << " Start - waiting for all display answer file children to finish " << endl;
    while(wait(NULL) > 0);
    cout << "Parent pid: " << getpid() << " Start - waiting for all display answer file children to finish " << endl;
  }
  catch(exception& e){
    cout << e.what() << endl << endl;                                        //exception  handling
    cout << "Press enter key once or twice to leave..." << endl << endl;
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}
        
        
        
        
        
        
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <vector>

#include <unistd.h>
#include <pthread.h>

#include "LineInfo.h"

using namespace std;

pthread_mutex_t lock;
static unsigned totalLineCount;

void GetLyricLinesFromFileToVector(string          lyricsFilenameStr,
                                   vector<string>& lyricLinesVector,
                                   unsigned&       noOfLyricLines) {
	ifstream fileLyricStream(lyricsFilenameStr);                         
	if(fileLyricStream.fail())
		throw domain_error(LineInfo("File Open failure ",__FILE__,__LINE__));
	string line;
	while(getline(fileLyricStream,line)){
		lyricLinesVector.push_back(line);
	}
	noOfLyricLines = lyricLinesVector.size();
}

struct SingLinesThreadInfoStruct {      
    unsigned*      singerNoIdPtr;
    vector<string> lyricLinesVector;
    unsigned       noOfLyricLines;
};

void* SingLinesThread(void* singLinesThreadInfoStructPtr) {
                                                                        
    SingLinesThreadInfoStruct* threadInfoPtr = (SingLinesThreadInfoStruct*)singLinesThreadInfoStructPtr;
    static unsigned previousNoId;

    const unsigned maxSingLineNo    = 10;
   	unsigned*      singersNoIdPtr   = threadInfoPtr->singerNoIdPtr;
    vector<string> lyricLinesVector = threadInfoPtr->lyricLinesVector;
    unsigned       noOfLyricLines   = threadInfoPtr->noOfLyricLines;
	
	  if((pthread_mutex_lock(&lock)) !=0)                                         
		  throw domain_error(LineInfo("Pthread_mutex_lock ",__FILE__,__LINE__));
	  cout << endl << "start thread " << *singersNoIdPtr << endl;
	  if((pthread_mutex_unlock(&lock)) !=0)
		  throw domain_error(LineInfo("Pthread_mutex_unlock ",__FILE__,__LINE__));

	  unsigned singLimit = 0;
    for (unsigned lyricLineNo = 0; lyricLineNo < noOfLyricLines; lyricLineNo+= maxSingLineNo){    
      unsigned currentLineSungNo = lyricLineNo;
      singLimit += maxSingLineNo;
    	for( ; (currentLineSungNo < singLimit) && (currentLineSungNo < noOfLyricLines); currentLineSungNo++){
  	  	if((pthread_mutex_lock(&lock)) !=0)
  		  	throw domain_error(LineInfo("Pthread_mutex_lock ",__FILE__,__LINE__));
  		  if(previousNoId != *singersNoIdPtr)
  			  cout << endl;
  		  cout << setw(3) << *singersNoIdPtr << " : " << setw(3) << currentLineSungNo
  		    << " > " << lyricLinesVector[currentLineSungNo] << endl;
  		  if((pthread_mutex_unlock(&lock)) !=0)
  			  throw domain_error(LineInfo("Pthread_mutex_unlock ",__FILE__,__LINE__));
  		
  		  previousNoId = *singersNoIdPtr;
  		  ++totalLineCount;
      }
      sleep(1);
    }
	  if((pthread_mutex_lock(&lock)) !=0)                                      
		  throw domain_error(LineInfo("Pthread_mutex_lock ",__FILE__,__LINE__));
	  cout << "done 	thread: " << *singersNoIdPtr << endl;
	  if((pthread_mutex_unlock(&lock)) !=0)
		  throw domain_error(LineInfo("Pthread_mutex_unlock ",__FILE__,__LINE__));
	  pthread_exit(NULL);
}







int main(int argc, char* argv[]){
	try{
		if(argc !=3){                                                 
			string errorStr = "Usage : ";
				errorStr = errorStr.append("\n./singers <lyrics filename.txt> <number of singers>\n");
				errorStr = errorStr.append("\nExample : \n./singers PopLife.txt 4\n ");
			throw domain_error(LineInfo(errorStr,__FILE__,__LINE__));
		}
		
		string lyricFilenamesStr(argv[1]);
		unsigned noOfSingers = stoi(argv[2]);
		vector<string> lyricLinesVector;
		unsigned noOfLyricLines = 0;
		GetLyricLinesFromFileToVector(lyricFilenamesStr, lyricLinesVector, noOfLyricLines);
		cout << endl << endl;
		cout << "Song Lyrics File Name is : " << lyricFilenamesStr << endl;
		cout << "Number of lyric lines is : " << noOfLyricLines << endl;
		cout << endl << endl;
		pthread_t* singersThreadIdPtr;
		unsigned* singersNoIdPtr;
		SingLinesThreadInfoStruct* singLinesThreadInfoStructPtr;
		singersThreadIdPtr = new pthread_t[noOfSingers];
		singersNoIdPtr = new unsigned [noOfSingers];
		singLinesThreadInfoStructPtr = new SingLinesThreadInfoStruct[noOfSingers];
		if((pthread_mutex_init(&lock, NULL)) != 0)
			throw domain_error(LineInfo("pthread_mutex_init ",__FILE__,__LINE__));

		for(unsigned singerNo=0; singerNo<noOfSingers; ++singerNo){                        
			unsigned * singerNoIdPtr = new unsigned;
			*singerNoIdPtr = singerNo+1;
			singLinesThreadInfoStructPtr[singerNo].singerNoIdPtr = singerNoIdPtr;
			singLinesThreadInfoStructPtr[singerNo].lyricLinesVector = lyricLinesVector;
			singLinesThreadInfoStructPtr[singerNo].noOfLyricLines = noOfLyricLines;
			if((pthread_mutex_lock(&lock)) !=0)
				throw domain_error(LineInfo("Pthread_mutex_lock ",__FILE__,__LINE__));
			cout << endl << "create thread " << *singerNoIdPtr << endl;
			if((pthread_mutex_unlock(&lock)) !=0)
				throw domain_error(LineInfo("Pthread_mutex_unlock ",__FILE__,__LINE__));
			int threadCreateReturn = pthread_create(
				&(singersThreadIdPtr[singerNo]),NULL,SingLinesThread,(/*void**/ &singLinesThreadInfoStructPtr[singerNo]));
			if(threadCreateReturn !=0)
				throw domain_error(LineInfo("pthread_create ",__FILE__,__LINE__));
		}//for
		
		for(int singerNo=0; singerNo < noOfSingers; ++singerNo){
			pthread_join(singersThreadIdPtr[singerNo],NULL);
		}
		cout << endl;
		cout << "Program done" << endl;
		cout << "Total Line count sung : " << totalLineCount << endl;
		cout << endl;

		if((pthread_mutex_destroy(&lock)) != 0)                                    
			throw domain_error(LineInfo("pthread_mutex_destroy ",__FILE__,__LINE__));	
	}
	catch(exception& e){
		cout << e.what() << endl;
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}


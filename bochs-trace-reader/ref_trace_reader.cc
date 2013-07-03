#include <iostream>
using std::cout;
using std::endl;
using std::cerr;
#include <stdio.h>
#include <stdint.h>

#ifdef USE_32_BIT
typedef uint32_t addressin_t;
typedef uint32_t addressout_t;
#else
typedef uint64_t addressin_t;
typedef uint64_t addressout_t;
#endif 

#include <stdlib.h> 

//NOTE: an outgoing page always means that there is an incoming page
//Incoming: Page number entering the NRU (from instrument.cc); Outgoing: page number leaving the NRU

void recordData(char rw, addressin_t addressin, addressout_t addressout, long long tsc, int casetype) {
  cout << "Read/Write ";
  if(rw == 'Q') cout << "Write";                          //If there is outgoing page, we had output Q/T for W/R
  else if(rw == 'T') cout << "Read";
  else cout << rw;
  cout << " Incoming page Virtual Address: " << std::hex << addressin;         
  cout << " TSC: " << tsc; 
  if(casetype != 0) {                                   //If there is an incoming page
    cout << " Incoming 4KB page "; 
    if(casetype == -1) {                                //If there is also an outgoing page
      cout << " Outgoing page Virtual Address: " << std::hex << addressout;
      cout << " Outgoing 4KB page  " << endl;
    }
    else cout << endl;
  }
  else cout << endl;
}

int main()
{
  FILE * myfile;
  char rw;                     //holds and displays read/write status  
  addressin_t addressin;       //Incoming file linear address
  addressout_t addressout;     //Outgoing file linear address
  long long tsc;               //cpu time
  int casetype;
  myfile = stdin;              //choose where to read from

  if(myfile == NULL) {                      //check for opening error
    fputs("File error",stderr);
    exit(1);
  }

  fread(&rw, 1, 1, myfile);                                 //read read/write identifier into rw       
  fread(&addressin, sizeof(addressin), 1, myfile);          //read incoming page lin add into addressin             
  fread(&tsc, 8, 1, myfile);                                //read timestamp into tsc
  while(!feof(myfile)) {                                    //while the filereader has not reached end of file
    casetype = 0;
    if(rw=='R' || rw=='W' || rw=='Q' || rw=='T') {                     //if we have an incoming page
      casetype = 1;        
      int seek_result = fseek(myfile, 4096, SEEK_CUR);               //jump across the page contents (not displayed)
      if(rw=='Q' || rw=='T') {                                         //if we also have an outgoing page
	casetype = -1;
	fread(&addressout, sizeof(addressout), 1, myfile);             //read in outgoing page lin add
	int seek2 = fseek(myfile, 4096, SEEK_CUR);                     //jump over the contnets of outgoing page too
      }
    }
    recordData(rw, addressin, addressout, tsc, casetype);    
    fread(&rw, 1, 1, myfile);                                         //read the next rw, incoming lin add and tsc
    fread(&addressin, sizeof(addressin), 1, myfile);                         
    fread(&tsc, 8, 1, myfile);
  }

  fclose(myfile);                                                    //close file       
  return 0;                                                          //exit main
}

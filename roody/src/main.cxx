//
// main.cxx
//
// $Id: main.cxx 238 2012-05-24 23:26:16Z olchansk $
//

#include <stdlib.h>
#include <iostream>
#include <string>

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#ifdef OS_UNIX
#include <unistd.h>
#endif

#ifdef OS_WINNT
#include <process.h>
#endif 

#include "Roody.h"

#include "TApplication.h"

using std::cout;
using std::endl;
using std::cerr;

void ShowUsage();

int main(int argc, char *argv[])
{
  setbuf(stdout, 0);
  setbuf(stderr, 0);
#ifndef OS_WINNT
  signal(SIGBUS,  SIG_DFL);
  signal(SIGSEGV, SIG_DFL);
  signal(SIGPIPE, SIG_IGN);
#endif
  char **filenames = NULL;
  if( argc > 1 )filenames = new char*[argc-1];
  int fileCount = 0;
  char *hostname = NULL;
  char *nhostname = NULL;
  char *xmlname = NULL;
  for( int i=1; i<argc; ++i )
  {
    if( argv[i][0] == '-' ) // look for the help switch
    {
      if( argv[i][1]=='?' )
      {
        ShowUsage();
        return 0;
      }
      else if (argv[i][1]=='H')
      {
        if( i+1==argc && strlen(argv[i])==2 )
        {
          ShowUsage();
          return 0;
        }
        else
        {
          strlen(argv[i])>2 ? hostname=argv[i]+2 : hostname=argv[++i];
        }
      }
      else if (argv[i][1]=='P')
      {
        if( i+1==argc && strlen(argv[i])==2 )
        {
          ShowUsage();
          return 0;
        }
        else
        {
	  if (strlen(argv[i])==2)
	    nhostname = argv[++i];
	  else
	    nhostname = argv[i]+2;
        }
      }
      else if( argv[i][1]=='r' || argv[i][1]=='R' )
      {
        std::string temp = strlen(argv[i])>2 ? argv[i]+2 : argv[++i];
        if( temp.find(".xml") == temp.npos )temp += ".xml";
        xmlname = new char[temp.size()+1];
        strcpy( xmlname, temp.c_str() );
      }
    }
    else filenames[fileCount++] = argv[i];
  }
  //
  // Do Not pass &argc to TApplication since this will make the
  // program respond with root info if -? is used
  //
  TApplication* app = new TApplication( "App", 0, 0 );
  //Roody* roody = new Roody(gClient->GetRoot(), 800, 800);
  Roody* roody = new Roody();

  if (xmlname)
    roody->RestoreFile(xmlname);
  else
    roody->RestoreFile(NULL);

  if (hostname)
    roody->ConnectServer(hostname);

  if (nhostname)
    roody->ConnectNetDirectory(nhostname);

  for (int i=0; i<fileCount; i++)
    roody->OpenFile(filenames[i]);

  app->Run(kTRUE);
  return 0;
}

void ShowUsage()
{
  cout << "USAGE:" << endl;
  cout << "roody [-h] [-h host[:port]] [-r file.xml] [file.root ...] [file.hbook ...]" << endl;
  cout << " " << endl;
  cout << "Options :" << endl;
  cout << "   -h             - display usage" << endl;
  cout << "   -H host[:port] - connect to midas server on given host and port" << endl;
  cout << "   -P host[:port] - connect to ROOT NetDirectory server on given host and port" << endl;
  cout << "   -r file.xml    - restore savefile" << endl;
  cout << "   file.root      - load .root files" << endl;
  cout << "   file.hbook     - load .hbook files" << endl;
}

// end of file

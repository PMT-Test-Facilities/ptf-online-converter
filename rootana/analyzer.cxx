//
// ROOT analyzer
//
// K.Olchanski
//
// $Id: analyzer.cxx 118 2012-12-14 07:19:24Z olchansk $
//

#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <assert.h>
#include <signal.h>

#include "TMidasOnline.h"
#include "TMidasEvent.h"
#include "TMidasFile.h"
#include "XmlOdb.h"
#ifdef HAVE_MIDASSERVER
#include "midasServer.h"
#endif
#ifdef HAVE_LIBNETDIRECTORY
#include "libNetDirectory/netDirectoryServer.h"
#endif
#ifdef HAVE_XMLSERVER
#include "libXmlServer/xmlServer.h"
#endif

#include <TSystem.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TTimer.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TFolder.h>

#include "Globals.h"

// Global Variables
int  gRunNumber = 0;
bool gIsRunning = false;
bool gIsPedestalsRun = false;
bool gIsOffline = false;
int  gEventCutoff = 0;

TDirectory* gOnlineHistDir = NULL;
TFile* gOutputFile = NULL;
VirtualOdb* gOdb = NULL;

//TCanvas  *gMainWindow = NULL; 	// the online histogram window

double GetTimeSec()
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

class MyPeriodic : public TTimer
{
public:
  typedef void (*TimerHandler)(void);

  int          fPeriod_msec;
  TimerHandler fHandler;
  double       fLastTime;

  MyPeriodic(int period_msec,TimerHandler handler)
  {
    assert(handler != NULL);
    fPeriod_msec = period_msec;
    fHandler  = handler;
    fLastTime = GetTimeSec();
    Start(period_msec,kTRUE);
  }

  Bool_t Notify()
  {
    double t = GetTimeSec();
    //printf("timer notify, period %f should be %f!\n",t-fLastTime,fPeriod_msec*0.001);

    if (t - fLastTime >= 0.9*fPeriod_msec*0.001)
      {
	//printf("timer: call handler %p\n",fHandler);
	if (fHandler)
	  (*fHandler)();

	fLastTime = t;
      }

    Reset();
    return kTRUE;
  }

  ~MyPeriodic()
  {
    TurnOff();
  }
};


void startRun(int transition,int run,int time)
{
  gIsRunning = true;
  gRunNumber = run;
  gIsPedestalsRun = gOdb->odbReadBool("/experiment/edit on start/Pedestals run");
  printf("Begin run: %d, pedestal run: %d\n", gRunNumber, gIsPedestalsRun);
    
  if(gOutputFile!=NULL)
  {
    gOutputFile->Write();
    gOutputFile->Close();
    gOutputFile=NULL;
  }  

  char filename[1024];
  sprintf(filename, "output%05d.root", run);
  gOutputFile = new TFile(filename, "RECREATE");
  
  //if (gOutputFile && !gOutputFile->IsOpen())
  //gOutputFile = NULL;

  assert(gOutputFile);
  assert(gOutputFile->IsOpen());

  //printf("gOutputFile: %p, isOpen %d\n", gOutputFile, gOutputFile->IsOpen());

#ifdef HAVE_LIBNETDIRECTORY
  NetDirectoryExport(gOutputFile, "outputFile");
#endif
}

void endRun(int transition,int run,int time)
{
  gIsRunning = false;
  gRunNumber = run;

#ifdef HAVE_MIDASSERVER
  if (gManaHistosFolder)
    gManaHistosFolder->Clear();
#endif

  if (gOutputFile)
    {
      gOutputFile->Write();
      gOutputFile->Close();		//close the histogram file
      gOutputFile = NULL;
    }

  printf("End of run %d\n",run);
}

#include <TH1D.h>

void HandleSample(int ichan, void* ptr, int wsize)
{
  uint16_t *samples = (uint16_t*) ptr;
  int numSamples = wsize;

  if (numSamples != 512)
    return;

  char name[256];
  sprintf(name, "channel%d", ichan);

  if (gOutputFile)
    gOutputFile->cd();

  TH1D* samplePlot = (TH1D*)gDirectory->Get(name);

  if (!samplePlot)
    {
      printf("Create [%s]\n", name);
      samplePlot = new TH1D(name, name, numSamples, 0, numSamples);
      //samplePlot->SetMinimum(0);
#ifdef HAVE_MIDASSERVER
      if (gManaHistosFolder)
        gManaHistosFolder->Add(samplePlot);
#endif
    }

  for(int ti=0; ti<numSamples; ti++)
    samplePlot->SetBinContent(ti, samples[ti]);
}


void HandleMidasEvent(TMidasEvent& event)
{
  int eventId = event.GetEventId();

  if (false&&(eventId == 1))
    {
      void *ptr;
      int size = event.LocateBank(NULL, "CHA3", &ptr);
      if (ptr)
	HandleSample(3, ptr, size);
    }
  if (false&&(eventId == 1)&&(gIsRunning==true)&&(gIsPedestalsRun==false)) // SIS data
    {
      //printf("SIS event\n");
      //event.Print();
      //void *ptr;
      //int size = event.LocateBank(event.GetData(),"MCS",&ptr);
      //HandleSIS(kMaxSisChannels,size,ptr);
    }
  else if (false&&(eventId == 2)&&(gIsRunning==true)) // ADC data
    {
      //printf("ADC event\n");
      //event.Print();
      //void *ptr;
      //int size = event.LocateBank(event.GetData(),"ADC",&ptr);
      //HandleBeamADC(kMaxADCChannels,size,ptr,event);
    }
  else if (false&&(eventId==5)&&(gIsRunning==true)&&(gIsPedestalsRun==false))// Scaler data
    {
      //event.Print();
      //void *sclrptr;
      //int sclrsize = event.LocateBank(event.GetData(),"SCLR",&sclrptr);
      //void *scrtptr;
      //int scrtsize = event.LocateBank(event.GetData(),"SCRT",&scrtptr);
      //HandleScaler(kMaxScalerChannels,sclrsize,sclrptr,scrtsize,scrtptr,event);
    }
  else
    {
      // unknown event type
      event.Print();
    }
}

void eventHandler(const void*pheader,const void*pdata,int size)
{
  TMidasEvent event;
  memcpy(event.GetEventHeader(), pheader, sizeof(TMidas_EVENT_HEADER));
  event.SetData(size, (char*)pdata);
  event.SetBankList();
  HandleMidasEvent(event);
}

int ProcessMidasFile(TApplication*app,const char*fname)
{
  TMidasFile f;
  bool tryOpen = f.Open(fname);

  if (!tryOpen)
    {
      printf("Cannot open input file \"%s\"\n",fname);
      return -1;
    }

  int i=0;
  while (1)
    {
      TMidasEvent event;
      if (!f.Read(&event))
	break;

      int eventId = event.GetEventId();
      //printf("Have an event of type %d\n",eventId);

      if ((eventId & 0xFFFF) == 0x8000)
	{
	  // begin run
	  event.Print();

	  //char buf[256];
	  //memset(buf,0,sizeof(buf));
	  //memcpy(buf,event.GetData(),255);
	  //printf("buf is [%s]\n",buf);

	  //
	  // Load ODB contents from the ODB XML file
	  //
	  if (gOdb)
	    delete gOdb;
	  gOdb = new XmlOdb(event.GetData(),event.GetDataSize());

	  startRun(0,event.GetSerialNumber(),0);
	}
      else if ((eventId & 0xFFFF) == 0x8001)
	{
	  // end run
	  event.Print();
	}
      else if ((eventId & 0xFFFF) == 0x8002) 
        { 
          // log message 
          event.Print(); 
          printf("Log message: %s\n", event.GetData()); 
        } 
      else
	{
	  event.SetBankList();
	  //event.Print();
	  HandleMidasEvent(event);
	}
	
      if((i%500)==0)
	{
	  //resetClock2time();
	  printf("Processing event %d\n",i);
	  //SISperiodic();
	  //StepThroughSISBuffer();
	}
      
      i++;
      if ((gEventCutoff!=0)&&(i>=gEventCutoff))
	{
	  printf("Reached event %d, exiting loop.\n",i);
	  break;
	}
    }
  
  f.Close();

  endRun(0,gRunNumber,0);

  // start the ROOT GUI event loop
  //  app->Run(kTRUE);

  return 0;
}

#ifdef HAVE_MIDAS

void MidasPollHandler()
{
  if (!(TMidasOnline::instance()->poll(0)))
    gSystem->ExitLoop();
}

int ProcessMidasOnline(TApplication*app, const char* hostname, const char* exptname)
{
   TMidasOnline *midas = TMidasOnline::instance();

   int err = midas->connect(hostname, exptname, "rootana");
   if (err != 0)
     {
       fprintf(stderr,"Cannot connect to MIDAS, error %d\n", err);
       return -1;
     }

   gOdb = midas;

   midas->setTransitionHandlers(startRun,endRun,NULL,NULL);
   midas->registerTransitions();

   /* reqister event requests */

   midas->setEventHandler(eventHandler);
   midas->eventRequest("SYSTEM",-1,-1,(1<<1));

   /* fill present run parameters */

   gRunNumber = gOdb->odbReadInt("/runinfo/Run number");

   if ((gOdb->odbReadInt("/runinfo/State") == 3))
     startRun(0,gRunNumber,0);

   printf("Startup: run %d, is running: %d, is pedestals run: %d\n",gRunNumber,gIsRunning,gIsPedestalsRun);
   
   MyPeriodic tm(100,MidasPollHandler);
   //MyPeriodic th(1000,SISperiodic);
   //MyPeriodic tn(1000,StepThroughSISBuffer);
   //MyPeriodic to(1000,Scalerperiodic);

   /*---- start main loop ----*/

   //loop_online();
   app->Run(kTRUE);

   /* disconnect from experiment */
   midas->disconnect();

   return 0;
}

#endif

#include <TGMenu.h>

class MainWindow: public TGMainFrame {

private:
  TGPopupMenu*		menuFile;
  //TGPopupMenu* 		menuControls;
  TGMenuBar*		menuBar;
  TGLayoutHints*	menuBarLayout;
  TGLayoutHints*	menuBarItemLayout;
  
public:
  MainWindow(const TGWindow*w,int s1,int s2);
  virtual ~MainWindow(); // Closing the control window closes the whole program
  virtual void CloseWindow();
  
  Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
};

#define M_FILE_EXIT 0

Bool_t MainWindow::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // printf("GUI Message %d %d %d\n",(int)msg,(int)parm1,(int)parm2);
    switch (GET_MSG(msg))
      {
      default:
	break;
      case kC_COMMAND:
	switch (GET_SUBMSG(msg))
	  {
	  default:
	    break;
	  case kCM_MENU:
	    switch (parm1)
	      {
	      default:
		break;
	      case M_FILE_EXIT:
	        if(gIsRunning)
    		   endRun(0,gRunNumber,0);
		gSystem->ExitLoop();
		break;
	      }
	    break;
	  }
	break;
      }

    return kTRUE;
}

MainWindow::MainWindow(const TGWindow*w,int s1,int s2) // ctor
    : TGMainFrame(w,s1,s2)
{
   //SetCleanup(kDeepCleanup);
   
   SetWindowName("ROOT Analyzer Control");

   // layout the gui
   menuFile = new TGPopupMenu(gClient->GetRoot());
   menuFile->AddEntry("Exit", M_FILE_EXIT);

   menuBarItemLayout = new TGLayoutHints(kLHintsTop|kLHintsLeft, 0, 4, 0, 0);

   menuFile->Associate(this);
   //menuControls->Associate(this);

   menuBar = new TGMenuBar(this, 1, 1, kRaisedFrame);
   menuBar->AddPopup("&File",     menuFile,     menuBarItemLayout);
   //menuBar->AddPopup("&Controls", menuControls, menuBarItemLayout);
   menuBar->Layout();

   menuBarLayout = new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandX);
   AddFrame(menuBar,menuBarLayout);
   
   MapSubwindows(); 
   Layout();
   MapWindow();
}

MainWindow::~MainWindow()
{
    delete menuFile;
    //delete menuControls;
    delete menuBar;
    delete menuBarLayout;
    delete menuBarItemLayout;
}

void MainWindow::CloseWindow()
{
    if(gIsRunning)
    	endRun(0,gRunNumber,0);
    gSystem->ExitLoop();
}

static bool gEnableShowMem = false;

int ShowMem(const char* label)
{
  if (!gEnableShowMem)
    return 0;

  FILE* fp = fopen("/proc/self/statm","r");
  if (!fp)
    return 0;

  int mem = 0;
  fscanf(fp,"%d",&mem);
  fclose(fp);

  if (label)
    printf("memory at %s is %d\n", label, mem);

  return mem;
}

void help()
{
  printf("\nUsage:\n");
  printf("\n./analyzer.exe [-h] [-Hhostname] [-Eexptname] [-eMaxEvents] [-P9091] [-p9090] [-m] [-g] [file1 file2 ...]\n");
  printf("\n");
  printf("\t-h: print this help message\n");
  printf("\t-T: test mode - start and serve a test histogram\n");
  printf("\t-Hhostname: connect to MIDAS experiment on given host\n");
  printf("\t-Eexptname: connect to this MIDAS experiment\n");
  printf("\t-P: Start the TNetDirectory server on specified tcp port (for use with roody -Plocalhost:9091)\n");
  printf("\t-p: Start the old midas histogram server on specified tcp port (for use with roody -Hlocalhost:9090)\n");
  printf("\t-e: Number of events to read from input data files\n");
  printf("\t-m: Enable memory leak debugging\n");
  printf("\t-g: Enable graphics display when processing data files\n");
  printf("\n");
  printf("Example1: analyze online data: ./analyzer.exe -P9091\n");
  printf("Example2: analyze existing data: ./analyzer.exe /data/alpha/current/run00500.mid\n");
  exit(1);
}

// Main function call

int main(int argc, char *argv[])
{
   setbuf(stdout,NULL);
   setbuf(stderr,NULL);
 
   signal(SIGILL,  SIG_DFL);
   signal(SIGBUS,  SIG_DFL);
   signal(SIGSEGV, SIG_DFL);
   signal(SIGPIPE, SIG_DFL);
 
   std::vector<std::string> args;
   for (int i=0; i<argc; i++)
     {
       if (strcmp(argv[i],"-h")==0)
	 help(); // does not return
       args.push_back(argv[i]);
     }

   TApplication *app = new TApplication("rootana", &argc, argv);

   if(gROOT->IsBatch()) {
   	printf("Cannot run in batch mode\n");
	return 1;
   }

   bool forceEnableGraphics = false;
   bool testMode = false;
   int  oldTcpPort = 0;
   int  tcpPort = 0;
   int  xmlTcpPort = 0;
   const char* hostname = NULL;
   const char* exptname = NULL;

   for (unsigned int i=1; i<args.size(); i++) // loop over the commandline options
     {
       const char* arg = args[i].c_str();
       //printf("argv[%d] is %s\n",i,arg);
	   
       if (strncmp(arg,"-e",2)==0)  // Event cutoff flag (only applicable in offline mode)
	 gEventCutoff = atoi(arg+2);
       else if (strncmp(arg,"-m",2)==0) // Enable memory debugging
	 gEnableShowMem = true;
       else if (strncmp(arg,"-p",2)==0) // Set the histogram server port
	 oldTcpPort = atoi(arg+2);
       else if (strncmp(arg,"-P",2)==0) // Set the histogram server port
	 tcpPort = atoi(arg+2);
       else if (strncmp(arg,"-X",2)==0) // Set the histogram server port
	 xmlTcpPort = atoi(arg+2);
       else if (strcmp(arg,"-T")==0)
	 testMode = true;
       else if (strcmp(arg,"-g")==0)
	 forceEnableGraphics = true;
       else if (strncmp(arg,"-H",2)==0)
	 hostname = strdup(arg+2);
       else if (strncmp(arg,"-E",2)==0)
	 exptname = strdup(arg+2);
       else if (strcmp(arg,"-h")==0)
	 help(); // does not return
       else if (arg[0] == '-')
	 help(); // does not return
    }
    
   MainWindow mainWindow(gClient->GetRoot(), 200, 300);

   gROOT->cd();
   gOnlineHistDir = new TDirectory("rootana", "rootana online plots");

#ifdef HAVE_MIDASSERVER
   if (oldTcpPort)
     StartMidasServer(oldTcpPort);
#else
   if (oldTcpPort)
     fprintf(stderr,"ERROR: No support for the old midas server!\n");
#endif
#ifdef HAVE_LIBNETDIRECTORY
   if (tcpPort)
     StartNetDirectoryServer(tcpPort, gOnlineHistDir);
#else
   if (tcpPort)
     fprintf(stderr,"ERROR: No support for the TNetDirectory server!\n");
#endif
#ifdef HAVE_XMLSERVER
   XmlServer* xmlServer = NULL;
   if (xmlTcpPort)
     {
        xmlServer = new XmlServer();
        xmlServer->SetVerbose(true);
        xmlServer->Start(xmlTcpPort);
        xmlServer->Export(gOnlineHistDir, gOnlineHistDir->GetName());
     }
#else
   if (xmlTcpPort)
     fprintf(stderr,"ERROR: No support for the XML Server!\n");
#endif
	 
   gIsOffline = false;

   for (unsigned int i=1; i<args.size(); i++)
     {
       const char* arg = args[i].c_str();

       if (arg[0] != '-')  
	 {  
	   gIsOffline = true;
	   //gEnableGraphics = false;
	   //gEnableGraphics |= forceEnableGraphics;
	   ProcessMidasFile(app,arg);
	 }
     }

   if (testMode)
     {
       gOnlineHistDir->cd();
       TH1D* hh = new TH1D("test", "test", 100, 0, 100);
       hh->Fill(1);
       hh->Fill(10);
       hh->Fill(50);

       app->Run(kTRUE);
       return 0;
     }

   // if we processed some data files,
   // do not go into online mode.
   if (gIsOffline)
     return 0;
	   
   gIsOffline = false;
   //gEnableGraphics = true;
#ifdef HAVE_MIDAS
   ProcessMidasOnline(app, hostname, exptname);
#endif
   
   return 0;
}

//end

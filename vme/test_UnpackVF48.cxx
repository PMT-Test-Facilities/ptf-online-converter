
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include <math.h>

#include "UnpackVF48.h"
#include "TMidasEvent.h"
#include "TMidasFile.h"

double utime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  //printf("gettimeofday: %d %d\n", tv.tv_sec, tv.tv_usec);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

int samples = 1000;
int countBadData  = 0;
int gCountUnpacked = 0;
int gCountOffset = 0;
int doPermitNoSamples = 1;

double loctime0 = 0;
double loctime1 = 0;
double timestamp0 = 0;
double timestamp1 = 0;

bool zdebug = false;

void HandleVF48event(const VF48event *e)
{
  //printf("VF48 event!\n");
  e->PrintSummary();

  double meanmin = 99999999999.0;
  double meanmax = 0;
  double rmsmin = 999999999999.0;
  double rmsmax = 0;

  int trigNo = -1;

  for (int i=0; i<MAX_MODULES; i++)
     if (e->modules[i])
	for (int j=0; j<MAX_CHANNELS; j++)
	   if (e->modules[i]->channels[j].channel == j)
	      {
		 trigNo = e->modules[i]->trigger;

                 if (e->modules[i]->channels[j].numSamples==0 && doPermitNoSamples)
                    {
                       // empty line
                    }
                 else if (e->modules[i]->channels[j].numSamples != samples)
		    {

		       countBadData++;
		       printf("Module %d channel %d has %d, should be %d samples\n", i, j, e->modules[i]->channels[j].numSamples, samples);
		       continue;
		    }

#if 0
		 double sum0 = 0;
		 double sum1 = 0;
		 double sum2 = 0;
                 int smin = 0xFFFF;
                 int smax = 0;
		 
		 for (int k=0; k<samples; k++)
		    {
		       int v = e->modules[i]->channels[j].samples[k];

                       if (v < smin)
                          smin = v;
                       if (v > smax)
                          smax = v;

		       sum0 += 1;
		       sum1 += v;
		       sum2 += v*v;

		       //printf("sample %d %f\n", k, v);
		    }

		 double mean = sum1/sum0;
		 double var = sum2/sum0 - mean*mean;
		 double rms = 0;
		 if (var>0)
		    rms = sqrt(var);

                 if (e->modules[i]->channels[j].numSamples>0)
                    if (rms>10 || mean==0)
                       {
                          printf("event %d, error %d, trigger %d, suspect data for module %d, channel %d: min: %d, max: %d, range: %d, mean: %f, rms %f\n", e->eventNo, e->error, trigNo, i, j, smin, smax, smax-smin, mean, rms);
                          
                          if (false && mean != 0)
                             {
                                for (int k=0; k<samples; k++)
                                   {
                                      double v = e->modules[i]->channels[j].samples[k];
                                      printf("sample %d %f\n", k, v);
                                   }
                                sleep(1);
                             }
                       }

                 if (zdebug)
                    printf("event %d, error %d, trigger %d, module %d, channel %d: supp mask 0x%02x, bit 0x%02x, min: %d, max: %d, range: %d, mean: %f, rms %f\n", e->eventNo, e->error, trigNo, i, j, e->modules[i]->suppressMask[j>>3], e->modules[i]->suppressMask[j>>3]&(1<<(j&0x7)), smin, smax, smax-smin, mean, rms);

		 //printf("mean %f, rms %f\n", mean, rms);

		 if (mean < meanmin)
		    meanmin = mean;

		 if (mean > meanmax)
		    meanmax = mean;

		 if (rms < rmsmin)
		    rmsmin = rms;

		 if (rms > rmsmax)
		    rmsmax = rms;
#endif
	      }

#if 0
  printf("event %d, error %d, trigger %d, pedestals mean: %f - %f, rms %f - %f\n", e->eventNo, e->error, trigNo, meanmin, meanmax, rmsmin, rmsmax);

  if (timestamp0 == 0)
     {
        loctime0 = utime();
        timestamp0 = e->timestamp;
     }

  loctime1 = utime();
  timestamp1 = e->timestamp;

  if (rmsmax > 10)
     {
	printf("******* Bad data? Pedestal rms is %f\n", rmsmax);
     }

  if (trigNo > 0)
     {
        gCountUnpacked ++;
  
        if (trigNo != gCountUnpacked)
           {
              countBadData++;
              printf("Bad event number: got %d, should be %d\n", trigNo, gCountUnpacked);
           }
     }
#endif

  delete e;
}

int main(int argc, char* argv[])
{
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  bool xdebug = false;
  bool ydebug = false;

  int grpEnabled = 0x3F;

  for (int i=1; i<argc; i++)
     {
        if (strcmp(argv[i], "--exit")==0)
           {
              exit(0);
           }
        else if (strcmp(argv[i], "--groups")==0)
           {
              i++;
              grpEnabled = strtoul(argv[i],NULL,0);
              printf("groups 0x%x\n", grpEnabled);
           }
        else if (strcmp(argv[i], "--samples")==0)
           {
              i++;
              samples = strtoul(argv[i],NULL,0);
              printf("samples %d\n", samples);
           }
        else if (strcmp(argv[i], "--noresync")==0)
           {
              VF48event::SetTimestampResync(false);
           }
        else if (strcmp(argv[i], "--xdebug")==0)
           {
              xdebug = true;
           }
        else if (strcmp(argv[i], "--ydebug")==0)
           {
              ydebug = true;
           }
        else if (strcmp(argv[i], "--zdebug")==0)
           {
              zdebug = true;
           }
        else
           {
              TMidasFile fin;

              if (!fin.Open(argv[i]))
                 {
                    printf("Cannot open file %s\n", argv[i]);
                    exit(1);
                 }

              VF48event::BeginRun(grpEnabled, samples, 5);

              TMidasEvent ev;

              while (1)
                 {
                    if (!fin.Read(&ev))
                       break;

                    int serno = ev.GetSerialNumber();
                    int evid  = ev.GetEventId();

                    //printf("serno %d, evid %d\n", serno, evid);

                    if (evid == 1)
                       {
                          for (int unit=0; unit<9; unit++)
                             {
                                char bankName[10];
                                sprintf(bankName, "VFA%d", unit);
                                uint32_t* pdata32;
                                int len = ev.LocateBank(NULL, bankName, (void**)&pdata32);
                                if (len > 0)
                                   UnpackVF48(unit, len, pdata32, xdebug, ydebug);
                             }
                       }
                 }
              
              //printf("time %f -> %f vs %f -> %f\n", loctime0, loctime1, timestamp0, timestamp1);
              //double locdt = loctime1 - loctime0;
              //double vf48dt = (timestamp1 - timestamp0)*gVF48freq[unit];
              //printf("VF48 timestamp frequency is %.3f MHz\n", vf48dt/locdt * 1e-6);
              
              VF48event::EndRun();
              fin.Close();
           }
     }

  return 0;
}

/* emacs
 * Local Variables:
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */


#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include <math.h>

#include "midas.h"
#include "mvmestd.h"


double utime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  //printf("gettimeofday: %d %d\n", tv.tv_sec, tv.tv_usec);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

#include "msystem.h"

extern "C"
{
#include "v513.h"
#include "vf48.h"
#include "vf48_multi.h"
}

MVME_INTERFACE *gVme;

struct vf48_multi_buffer *gVF48buf;

void all_trigger(struct vf48_multi_buffer* buf)
{
   mvme_set_am(gVme, MVME_AM_A24);
   mvme_set_dmode(gVme, MVME_DMODE_D16);

   for (int i=0; i<MAX_VF48; i++)
      if (buf->vf48_addr[i] > 0)
	 vf48_Trigger(gVme, buf->vf48_addr[i]);
}

void all_status(struct vf48_multi_buffer* buf)
{
   for (int i=0; i<MAX_VF48; i++)
      if (buf->vf48_addr[i] > 0)
	 vf48_Status(gVme, buf->vf48_addr[i]);
}

void all_reset(struct vf48_multi_buffer* buf)
{
   for (int i=0; i<MAX_VF48; i++)
      if (buf->vf48_addr[i] > 0)
	 vf48_Reset(gVme, buf->vf48_addr[i]);
}

void all_init(struct vf48_multi_buffer* buf, int grpEnabled, int chanEnabled, int numSamples)
{
   for (int i=0; i<MAX_VF48; i++)
      if (buf->vf48_addr[i] > 0)
	 {
	    uint32_t vf48base = buf->vf48_addr[i];

	    int status = vf48_Reset(gVme, vf48base);
	    if (status != VF48_SUCCESS)
	       {
		  buf->vf48_addr[i] = 0;
		  continue;
	       }

	    vf48_GrpEnable(gVme, vf48base, grpEnabled);

	    // Active Channel Mask
	    for (int i=0;i<6;i++)
	       vf48_ParameterWrite(gVme, vf48base, i, VF48_ACTIVE_CH_MASK, chanEnabled);

	    for (int j=0; j<6; j++)
	       vf48_ParameterWrite(gVme, vf48base, j, VF48_SEGMENT_SIZE, numSamples);

	    for (int j=0; j<6; j++)
	       vf48_ParameterWrite(gVme, vf48base, j, VF48_PRE_TRIGGER, 30);
      
	    for (int j=0; j<6; j++)
	       vf48_ParameterWrite(gVme, vf48base, j, VF48_LATENCY, 60);
      
	    int divisor = 1;
	    if (1)
	       vf48_DivisorWrite(gVme, vf48base, divisor);
  
	    vf48_ExtTrgSet(gVme, vf48base);

#if 0      
	    for (int j=0; j<6; j++)
	       vf48_ParameterWrite(gVme, buf->vf48_addr[i], j, VF48_SEGMENT_SIZE, 256);

	    //vf48_ExtTrgSet(gVme, buf->vf48_addr[i]);
#endif
	 }
}

void all_start(struct vf48_multi_buffer* buf)
{
   for (int i=0; i<MAX_VF48; i++)
      if (buf->vf48_addr[i] > 0)
	 vf48_AcqStart(gVme, buf->vf48_addr[i]);
}

#include "UnpackVF48.h"

static int countUnpack = 0;
static int countUnpackErrors = 0;

void HandleVF48event(const VF48event* e)
{
   e->PrintSummary();
   countUnpack++;
   if (e->error)
      countUnpackErrors++;
   delete e;
}

void all_unpack(char* pevent, bool disasm = false)
{
   DWORD bklen, bktype;
   void* p32;
   int i, status;

   if (0)
      {
	 char banklist[1000];
	 banklist[0] = 0;
	 status = bk_list((BANK_HEADER *) (((EVENT_HEADER *) pevent) + 0), banklist); 
	 printf("#banks:%i Bank list:-%s-\n", status, banklist); 
      } 

   for (i=0; i<10; i++)
      {
	 char name[5];
	 sprintf(name, "VFA%d", i);
	 p32 = 0;
	 bklen = 0;
	 bktype = 0;
	 //status = bk_locate(pevent, name, &p32);
	 status = bk_find((BANK_HEADER*)(pevent), name, &bklen, &bktype, &p32);
	 //printf("module %d, name [%s], status %d, bklen %d, bktype %d, p32 %p\n", i, name, status, bklen, bktype, p32);
	 if (status != SUCCESS)
	    continue;
	 assert(bktype==TID_DWORD);
	 UnpackVF48(i, bklen, p32, disasm);
      }
}

int main(int argc, char* argv[])
{
   setbuf(stdout, NULL);
   setbuf(stderr, NULL);

#if 0
   if (argc>1)
      vf48base = strtoul(argv[1],NULL,0);
  
   if (argc>2)
      vf48samples = strtoul(argv[2],NULL,0);
  
   if (argc>3)
      extTrig = strtoul(argv[3],NULL,0);
#endif
  
   int status = mvme_open(&gVme, 0);

   mvme_set_am(gVme, MVME_AM_A24);

   static vf48_multi_buffer xbuf;
   gVF48buf = &xbuf;

   static vf48_multi_buffer xbuf2;
   static vf48_multi_buffer* gVF48buf2 = &xbuf2;

   vf48_multi_init(gVme, gVF48buf, 0, 0xa00000, "VFA0");
   vf48_multi_init(gVme, gVF48buf, 1, 0xa10000, "VFA1");
   vf48_multi_init(gVme, gVF48buf, 2, 0xa20000, "VFA2");
   //vf48_multi_init(gVme, gVF48buf2, 0, 0xa90000, "VFA2");
   vf48_multi_init(gVme, gVF48buf, 3, 0xa30000, "VFA3");
   vf48_multi_init(gVme, gVF48buf, 4, 0xa40000, "VFA4");
   vf48_multi_init(gVme, gVF48buf, 5, 0xa50000, "VFA5");

   struct vf48_multi_thread_param tparam;
   tparam.vme = gVme;
   tparam.buf = gVF48buf;
   tparam.stop = 0;
   tparam.is_running = 0;
   tparam.debug = 1;

   ss_thread_create(vf48_multi_thread, &tparam);

   struct vf48_multi_thread_param tparam2;
   tparam2.vme = gVme;
   tparam2.buf = gVF48buf2;
   tparam2.stop = 0;
   tparam2.is_running = 0;
   tparam2.debug = 1;

   ss_thread_create(vf48_multi_thread, &tparam2);

   int v513base = 0x000100;

   bool doExtTrig = true;

   if (doExtTrig)
      {
	 v513_Reset(gVme, v513base);
	 v513_Status(gVme, v513base);

	 printf("Setting v513 channels to OUTPUT mode\n");

	 for (int i=0; i<16; i++)
	    v513_SetChannelMode(gVme, v513base, i, V513_CHANMODE_OUTPUT|V513_CHANMODE_POS|V513_CHANMODE_TRANSP);
      }

   int grpEnabled = 0x3F;
   int chanEnabled = 0xFF;
   int numSamples = 1000;

   all_reset(gVF48buf);
   all_init(gVF48buf, grpEnabled, chanEnabled, numSamples);
   all_status(gVF48buf);
   all_start(gVF48buf);

   all_reset(gVF48buf2);
   all_init(gVF48buf2, grpEnabled, chanEnabled, numSamples);
   all_status(gVF48buf2);
   all_start(gVF48buf2);

   int pevent_size = 1*1024*1024;
   char pevent[pevent_size];

   VF48event::BeginRun(grpEnabled, numSamples, 6);

   bool doMT = true;
   //bool doMT = false;

   if (doMT)
      tparam.is_running = 1;
   else
      tparam.stop = 1;

   if (doMT)
      tparam2.is_running = 1;
   else
      tparam2.stop = 1;

   int countTrigger = 0;

   while (1)
      {
	 for (int i=0; i<5; i++)
	    {
	       if (doExtTrig)
		  {
		     v513_Write(gVme, v513base, 0x8000);
		     v513_Read(gVme, v513base);
		     v513_Write(gVme, v513base, 0x0);
		  }
	       else
		  all_trigger(gVF48buf);

	       countTrigger++;

	       if (!doMT)
		  break;

	       sleep(1);
	    }

	 //sleep(1);
	 //all_status(gVF48buf2);

	 if (!doMT)
	    {
	       double t0 = utime();
	       int rd = 0;

	       while (1)
		  {
		     int nb = vf48_multi_read(gVme, gVF48buf, 0);
		     if (nb<=0)
			break;
		     rd += nb;
		  }

	       double t1 = utime();

	       double td = (t1 - t0)*1000000.0;
	       
	       printf("read %d bytes in %.0f usec, %.1f Mbytes/sec\n", rd, td, rd/td);
	    }

	 if (!doMT)
	    {
	       double t0 = utime();
	       int rd = 0;

	       while (1)
		  {
		     int nb = vf48_multi_read(gVme, gVF48buf2, 0);
		     if (nb<=0)
			break;
		     rd += nb;
		  }

	       double t1 = utime();

	       double td = (t1 - t0)*1000000.0;
	       
	       printf("buf2 read %d bytes in %.0f usec, %.1f Mbytes/sec\n", rd, td, rd/td);
	    }

	 while (1)
	    {
	       double t1 = utime();

	       bk_init32(pevent);
	       int rdb = vf48_multi_bank(gVF48buf, pevent, pevent_size, 1);
	       rdb += vf48_multi_bank(gVF48buf2, pevent, pevent_size, 1);
	       int bks = bk_size(pevent);
	       
	       double t2 = utime();
	       
	       if (rdb > 0)
		  all_unpack(pevent);
	       
	       double t3 = utime();
	       
	       double tb = (t2 - t1)*1000000.0;
	       double tu = (t3 - t2)*1000000.0;
	       
	       printf("BANK: %d/%d bytes, %.1f M/s, Unpack: %.1f M/s, triggers: %d, unpacked events: %d, errors: %d\n", rdb, bks, rdb/tb, rdb/tu, countTrigger, countUnpack, countUnpackErrors);

	       if (rdb <= 0)
		  break;
	    }

         if (0 && countUnpackErrors>0)
            exit(1);

	 sleep(1);
      }
}

/* emacs
 * Local Variables:
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */


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

#include "vt48.h"

#define HAVE_GEFVME

#ifdef HAVE_GEFVME
#include "gefvme.h"
#endif

#include "vt48_multi.h"

#include "msystem.h"

static double utime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  //printf("gettimeofday: %d %d\n", tv.tv_sec, tv.tv_usec);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

int vt48_multi_init(MVME_INTERFACE*myvme, struct vt48_multi_buffer* buf, int i, uint32_t addr, const char* bank_name)
{
   int status;

   buf->vt48_addr[i] = addr;
   buf->vt48_nbytes[i] = 0;

   strncpy(buf->bank_name[i], bank_name, 4);
   buf->bank_name[i][4] = 0;

   buf->vt48_last_word[i] = 0;

   int size = 4*1024*1024;
   int max_event_size = 100*1024;

   if (buf->rb[i] == 0)
      {
	 status = rb_create(size, max_event_size, &buf->rb[i]);
	 assert(status == SUCCESS);
      }

   int nbytes = 0;
   status = rb_get_buffer_level(buf->rb[i], &nbytes);
   assert(status == SUCCESS);

   while (nbytes > 0)
      {
	 int incr;
	 uint32_t* src32;
	 status = rb_get_rp(buf->rb[i], &src32, 0);
	 assert(status == SUCCESS);
	 assert(src32[0] == 0xabbaabba);

	 incr = 8 + src32[1];
	 rb_increment_rp(buf->rb[i], incr);

	 status = rb_get_buffer_level(buf->rb[i], &nbytes);
	 assert(status == SUCCESS);

	 printf("vt48_multi_init: unit %d, flushed %d, left over %d\n", i, incr, nbytes);
      }

   return SUCCESS;
}

static inline uint32_t be2cpu(uint32_t w)
{
   return 0 |
      (w&0xFF000000)>>24 |
      (w&0x00FF0000)>>8  |
      (w&0x0000FF00)<<8  |
      (w&0x000000FF)<<24;
}

static int gDmaByteswap = 0;

int vt48_multi_read(MVME_INTERFACE*myvme, struct vt48_multi_buffer* buf, int debug)
{
   int status;
   int i, j;
   int rd;

   void*       dstaddr[MAX_VT48];
   mvme_addr_t vmeaddr[MAX_VT48];
   int         nbytes[MAX_VT48];
   int         dmaslot[MAX_VT48];

   rd = 0;
   j = 0;
   for (i=0; i<MAX_VT48; i++)
      if (buf->vt48_addr[i] > 0)
	 {
	    int nf, nb, nr;
	    DWORD csr;
	    char *ptr;

	    buf->vt48_nbytes[i] = 0;

	    csr = vt48_RegRead(myvme, buf->vt48_addr[i], VT48_CSR_RO);
	    nf  = csr & 0xFFF;

	    if (0 || debug>1)
	       printf("VT48[%d] csr: 0x%x, nf: %d\n", i, csr, nf);

	    nb = nf*4; // convert to bytes

	    nr = nb;

	    if (nr==0)
	       continue;

	    if (debug > 0)
	       printf("read adc %d, nbytes %d\n", i, nb);

	    assert(nr > 0);

	    buf->vt48_nbytes[i] = nb;

	    status = rb_get_wp(buf->rb[i], &ptr, 0);
	    assert(status == SUCCESS);

	    *(uint32_t*)(ptr+0) = 0xabbaabba;
	    *(uint32_t*)(ptr+4) = nr;

	    dstaddr[j] = ptr + 8;
	    dmaslot[i] = j;

	    if ((((int)dstaddr[j])&0x3) != 0)
	       printf("misaligned DMA: %p\n", dstaddr[j]);
	    vmeaddr[j] = buf->vt48_addr[i] + VT48_DATA_FIFO;
	    nbytes[j]  = nr;
	    
	    rd += nb;
	    j++;
	 }

   if (j==0)
      return 0;

   //printf("rd %d\n", rd);

   //if (rd < 1000)
   //goto wait;

   mvme_set_blt(myvme, MVME_BLT_NONE);
   
#ifdef HAVE_GEFVME
   if (debug>2)
      gefvme_set_dma_debug(1);
   else
      gefvme_set_dma_debug(0);

   gDmaByteswap = 1;

   status = gefvme_read_dma_multiple(myvme, j, dstaddr, vmeaddr, nbytes);
   if (status != SUCCESS)
      {
	 int i;
	 printf("Error: gefvme_read_dma_multiple() status %d\n", status);
	 for (i=0; i<j; i++)
	    printf("DMA list entry %d: dstaddr %p, vmeaddr 0x%08x, nbytes %d\n",
		   i, dstaddr[i], vmeaddr[i], nbytes[i]);


	 for (i=0; i<MAX_VT48; i++)
	    if (buf->vt48_addr[i] > 0)
	       vt48_Status(myvme, buf->vt48_addr[i]);

	 return -1;
      }
#else
   gDmaByteswap = 0;
   for (i=0; i<j; i++)
      {
	 status = mvme_read(myvme, dstaddr[i], vmeaddr[i], nbytes[i]);
	 if (status != SUCCESS)
	    {
	       printf("Error: mvme_read() status %d\n", status);
	       return -1;
	    }
      }
#endif

   for (i=0; i<MAX_VT48; i++)
      if ((buf->vt48_addr[i]>0) && (buf->vt48_nbytes[i] > 0))
	 {
	    int ii = dmaslot[i];

	    uint32_t w = *(uint32_t*)(((char*)dstaddr[ii])+buf->vt48_nbytes[i]-4);

	    if (gDmaByteswap)
	       w = be2cpu(w);

	    buf->vt48_last_word[i] = w;

	    if (0)
	       printf("VT48_MULTI: unit %d, first word 0x%08x, last words are 0x%08x and 0x%08x, last word 0x%08x\n", i, *(uint32_t*)dstaddr[ii], *(uint32_t*)(((char*)dstaddr[ii])+buf->vt48_nbytes[i]-8), *(uint32_t*)(((char*)dstaddr[ii])+buf->vt48_nbytes[i]-4), buf->vt48_last_word[i]);

	    if (0)
	       printf("VT48_MULTI: unit %d, incr %d\n", i, 8 + buf->vt48_nbytes[i]);

	    rb_increment_wp(buf->rb[i], 8 + buf->vt48_nbytes[i]);
	 }

   return rd;
}

int vt48_multi_bank(struct vt48_multi_buffer* buf, char* pevent, int pevent_size, int one_event, int expectedEventNo)
{
   int status;
   int i;
   int rd = 0;

   //printf("VT48 multi_bank event %d\n", expectedEventNo);

   for (i=0; i<MAX_VT48; i++)
      if (buf->vt48_addr[i] > 0)
	 {
	    if (!one_event)
	       {
		  DWORD* p32;

		  bk_create(pevent, buf->bank_name[i], TID_DWORD, &p32);

		  buf->vt48_bank[i] = p32;

		  while (1)
		     {
			int nw = 0;
			uint32_t* src32;
			int incr = 0;
			int j;
			
			status = rb_get_rp(buf->rb[i], &src32, 0);
			if (status == DB_TIMEOUT)
			   break;

			assert(status == DB_SUCCESS);

			//printf("VT48_MULTI: unit %d, src32: %p -> 0x%08x\n", i, src32, src32[0]);

			assert(src32[0] == 0xabbaabba);

			nw = src32[1]/4;

			src32 += 2;

			for (j=0; j<nw; j++)
			   {
			      uint32_t w = src32[j];
			      if (gDmaByteswap)
				 w = be2cpu(w);

			      *p32++ = w;
			   }

			incr = 4*nw + 8;

			//printf("VT48_MULTI: unit %d, incr %d, nw %d, j %d\n", i, incr, nw, j);

			rb_increment_rp(buf->rb[i], incr);
			
			rd += nw*4;

		     } // loop over all data segments
		  
		  bk_close(pevent, p32);
	       }
	    else  // if !one_event 
	       {
		  DWORD* p32;
		  int bc = 0;
		  int headerEventNo = -1;
		  int trailerEventNo = -1;
		  int copy = 0;

		  bk_create(pevent, buf->bank_name[i], TID_DWORD, &p32);

		  while (1)
		     {
			int nbytes = 0;
			int nw = 0;
			uint32_t* src32;
			int incr = 0;
			int j;
			
			status = rb_get_rp(buf->rb[i], &src32, 0);
			assert(status == SUCCESS);

			//printf("VT48_MULTI: unit %d, src32: %p -> 0x%08x\n", i, src32, src32[0]);

			assert(src32[0] == 0xabbaabba);

			nbytes = src32[1];
			nw = nbytes/4;

			incr += 8;

			src32 += 2;

			for (j=0; j<nw; j++)
			   {
			      uint32_t w = src32[j];
			      if (gDmaByteswap)
				 w = be2cpu(w);

			      // start copying when we see the event header
			      if ((w & 0xF0000000) == VT48_HEADER)
				 {
				    headerEventNo = (w & 0x0000FFFF);
				    //printf("VT48 %d header %d\n", i, headerEventNo);
				    copy = 1;
				 }

			      if (copy)
				 {
				    *p32++ = w;
				    bc += 4;
				 }
				 
			      incr += 4;

			      // stop copying when we see the event trailer
			      if ((w & 0xF0000000) == VT48_TRAILER)
				 {
				    trailerEventNo = (w & 0x0000FFFF);
				    copy = 0;
				    //printf("VT48 %d trailer %d\n", i, trailerEventNo);
				    j++;

				    if (1) // check for crazy duplicate event number where after the trailer we have a header with the same event number
				       {
					  uint32_t ww = src32[j];
					  if (gDmaByteswap)
					     ww = be2cpu(ww);
					  
					  if ((ww & 0xF0000000) == VT48_HEADER)
					     {
						int newHeaderEventNo = (ww & 0x0000FFFF);
						if (newHeaderEventNo == trailerEventNo)
						   {
						      printf("VT48_MULTI unit %d at 0x%08x: duplicate event data, expected event no %d, header %d, trailer %d, unexpected extra header %d\n", i, buf->vt48_addr[i], expectedEventNo, headerEventNo, trailerEventNo, newHeaderEventNo);
						      j--;
						      continue;
						   }
					     }
				       }
					  
				    break;
				 }

			   } // loop over data segment

			//printf("VT48_MULTI: unit %d, segment size %d, nbytes %d, incr %d, trailerEventNo %d, nw %d, j %d\n", i, nbytes+8, nbytes, incr, trailerEventNo, nw, j);

			if (incr != nbytes + 8)
			   {
			      int k;

			      printf("VT48 unit %d, segment size %d, nbytes %d, incr %d, trailerEventNo %d, nw %d, j %d\n", i, nbytes+8, nbytes, incr, trailerEventNo, nw, j);

			      printf("VT48 nw %d\n", nw);
			      for (k=0; k<nw+2; k++)
				 printf("VT48 word %d: 0x%08x swapped 0x%08x\n", k, src32[k-2], be2cpu(src32[k-2]));
			      cm_msg(MERROR, "vt48)multi", "VT48 0x%x sent extra data after trailer",buf->vt48_addr[i]);
			      //exit(1);
			   }

			rb_increment_rp(buf->rb[i], incr);

			if (trailerEventNo >= 0)
			   break;

		     } // loop over all data segments

		  if (expectedEventNo > 0)
		     {
			expectedEventNo &= 0xFFFF;

			//printf("VT48 unit %d at 0x%08x: expected event no %d, header %d, trailer %d\n", i, buf->vt48_addr[i], expectedEventNo, headerEventNo, trailerEventNo);

			//if (headerEventNo != expectedEventNo || trailerEventNo != expectedEventNo)
			if (headerEventNo != trailerEventNo)
			   {
			      printf("VT48 unit %d at 0x%08x: expected event no %d, header %d, trailer %d\n", i, buf->vt48_addr[i], expectedEventNo, headerEventNo, trailerEventNo);
			   }
		     }
		  
		  bk_close(pevent, p32);

		  rd += bc;
	       } // end if (one_event)
	 } // end of loop over modules

   return rd;
}

int vt48_multi_thread(void* xparam)
{
   struct vt48_multi_thread_param* param = (struct vt48_multi_thread_param*)xparam;

   printf("vt48_multi_thread: started!\n");

   while (!param->stop)
      {
	 if (!param->is_running)
	    {
	       sleep(1);
	       continue;
	    }

	 double t0 = 0;

	 if (param->debug)
	    t0 = utime();

	 int nb = vt48_multi_read(param->vme, param->buf, param->debug);
	 if (nb < 0)
	    {
	       printf("vt48_multi_thread: Error: vt48_multi_read() returned %d\n", nb);
	       break;
	    }


	 if (param->debug)
	    {
	       double t1 = utime();

	       double td = (t1 - t0)*1000000.0;

	       printf("vt48_multi_thread: read %d bytes in %.0f usec, %.1f Mbytes/sec\n", nb, td, nb/td);
	    }

	 if (nb==0)
	    sleep(1);
      }

   printf("vt48_multi_thread: stopped!\n");
   return 0;
}

/* emacs
 * Local Variables:
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */

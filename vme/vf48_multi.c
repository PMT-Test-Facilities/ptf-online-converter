
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

#include "vf48.h"

#ifndef HAVE_VMICVME
#define HAVE_GEFVME
#endif

#ifdef HAVE_GEFVME
#include "gefvme.h"
#endif

#include "vf48_multi.h"

#include "msystem.h"

static double utime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  //printf("gettimeofday: %d %d\n", tv.tv_sec, tv.tv_usec);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

static const int rb_size = 4*1024*1024;
static const int rb_max_event_size = 100+48*2*1000;

int vf48_multi_init(MVME_INTERFACE*myvme, struct vf48_multi_buffer* buf, int i, uint32_t addr, const char* bank_name)
{
   int status;

   buf->vf48_addr[i] = addr;
   buf->vf48_nbytes[i] = 0;

   buf->vf48_count_reads[i] = 0;
   buf->vf48_count_bytes[i] = 0;

   strncpy(buf->bank_name[i], bank_name, 4);
   buf->bank_name[i][4] = 0;

   buf->vf48_first_word[i] = 0;
   buf->vf48_last_word[i] = 0;

   if (buf->rb[i] == 0)
      {
	 status = rb_create(rb_size, rb_max_event_size, &buf->rb[i]);
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

	 printf("vf48_multi_init: unit %d, flushed %d, left over %d\n", i, incr, nbytes);
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

int vf48_multi_read(MVME_INTERFACE*myvme, struct vf48_multi_buffer* buf, int debug)
{
   int status;
   int i, j;
   int rd;

   void*       dstaddr[MAX_VF48];
   mvme_addr_t vmeaddr[MAX_VF48];
   int         nbytes[MAX_VF48];
   int         dmaslot[MAX_VF48];

#ifdef HAVE_GEFVME
   gDmaByteswap = 1;
#endif

   rd = 0;
   j = 0;
   for (i=0; i<MAX_VF48; i++)
      if (buf->vf48_addr[i] > 0)
	 {
	    int nf, nb, nr;
	    DWORD csr;
	    char *ptr;
            int incomplete = 0;

	    buf->vf48_nbytes[i] = 0;

	    csr = vf48_CsrRead(myvme, buf->vf48_addr[i]);
	    nf = vf48_NFrameRead(myvme, buf->vf48_addr[i]);

	    if ((csr&VF48_CSR_START_ACQ)==0)
	       continue;

	    if (csr&VF48_CSR_FIFO_EMPTY)
	       continue;

	    if (0 || debug>1)
	       printf("VF48[%d] csr: 0x%x, nf: %d, nb: %d\n", i, csr, nf, nf*4);

            incomplete = csr & VF48_CSR_FIFO_INCOMPLETE;

	    nb = nf*4; // convert to bytes

	    //if (nb%16 != 0) { // 2EVME 128-bit DMA requires 16 bytes alignement! 64bit DMA requires 8-byte alignement
	    //   nb = nb - nb%16;
	    //   nb += 16;
	    // }

	    while (nb%16 != 0) { // 2EVME 128-bit DMA requires 16 bytes alignement! 64bit DMA requires 8-byte alignement
               if (incomplete)
                  nb -= 4;
               else
                  nb += 4;
	    }

	    nr = nb;

/* 	    if (nf < 2000) // if VF48 EB FIFO is not full */
/* 	       { */
/* 		  int xnf = vf48_NFrameRead(myvme, buf->vf48_addr[i]); */

/* 		  if (xnf == nf) // VF48 finished writing to EB FIFO */
/* 		     { */
/* 			nb = nf*4; */
/* 			nr = nf*4; */

/* 			if ((nf&1) != 0) */
/* 			   { */
/* 			      nr += 4; */
/* 			      nb += 4; */
/* 			   } */
/* 		     } */

/*                   if (0) */
/*                      printf("VF48[%d] nf %d, xnf %d, align %d, nbytes %d/%d/%d, align8 %d!\n", i, nf, xnf, nf&1, nf*4, nb, nr, nr%8); */
/* 	       } */

	    if (nr==0)
	       continue;

	    assert(nr > 0);

	    status = rb_get_wp(buf->rb[i], &ptr, 0);
	    assert(status == SUCCESS);

	    dstaddr[j] = ptr + 8;
	    dmaslot[i] = j;

	    while ((((int)dstaddr[j])&0xF) != 0) {
	       if (gDmaByteswap)
		  *((uint32_t*)dstaddr[j]) = be2cpu(0xdeaddead);
	       else
		  *((uint32_t*)dstaddr[j]) = 0xdeaddead;
	       dstaddr[j] += 4;
	       nb += 4;

	       //printf("misaligned DMA: %p\n", dstaddr[j]);
	       //assert(!"cannot continue!");
	    }

	    *(uint32_t*)(ptr+0) = 0xabbaabba;
	    *(uint32_t*)(ptr+4) = nb;

	    vmeaddr[j] = buf->vf48_addr[i] + 0x1000;
	    nbytes[j]  = nr;

	    buf->vf48_nbytes[i] = nb;

            buf->vf48_count_reads[i] += 1;
            buf->vf48_count_bytes[i] += nb;

	    if (0 || debug > 0)
	       printf("read adc %d, nbytes %d, nr %d (mod16 %d), dstaddr %p\n", i, nb, nr, nr%16, dstaddr[j]);

	    rd += nb;
	    j++;
	 }

   if (j==0)
      return 0;

   //printf("rd %d\n", rd);

   //if (rd < 1000)
   //goto wait;

#ifdef HAVE_GEFVME
   mvme_set_blt(myvme, MVME_BLT_2EVME);
   //mvme_set_blt(myvme, MVME_BLT_MBLT64);
   //mvme_set_blt(myvme, MVME_BLT_BLT32);
   
   if (debug>2)
      gefvme_set_dma_debug(1);
   else
      gefvme_set_dma_debug(0);

   assert(gDmaByteswap == 1);

   status = gefvme_read_dma_multiple(myvme, j, dstaddr, vmeaddr, nbytes);
   if (status != SUCCESS)
      {
	 int i;
	 printf("Error: gefvme_read_dma_multiple() status %d\n", status);
	 for (i=0; i<j; i++)
	    printf("DMA list entry %d: dstaddr %p, vmeaddr 0x%08x, nbytes %d\n",
		   i, dstaddr[i], vmeaddr[i], nbytes[i]);


	 for (i=0; i<MAX_VF48; i++)
	    if (buf->vf48_addr[i] > 0)
	       vf48_Status(myvme, buf->vf48_addr[i]);

	 exit(123);

	 return -1;
      }
#else
   mvme_set_blt(myvme, MVME_BLT_MBLT64);
   //mvme_set_blt(myvme, MVME_BLT_BLT32);

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

   for (i=0; i<MAX_VF48; i++)
      if ((buf->vf48_addr[i]>0) && (buf->vf48_nbytes[i] > 0))
	 {
	    int j;
	    int ii = dmaslot[i];

	    for (j=4; j<32; j+=4) {
	       uint32_t w = *(uint32_t*)(((char*)dstaddr[ii])+nbytes[ii]-j);

	       if (gDmaByteswap)
		  w = be2cpu(w);

	       //printf("nbytes %d, last word 0x%x\n", nbytes[ii], w);

	       if ((w&0xF0000000) == 0xF0000000)
                  continue;
	       
	       if (w == 0xdeaddead)
                  continue;
	       
	       if (w == 0xdeadbeef)
                  continue;
	       
	       buf->vf48_last_word[i] = w;
	       break;
	    }

	    if (0)
	       printf("VF48_MULTI: unit %d, nbytes %d: first word 0x%08x, last words are 0x%08x and 0x%08x, last word 0x%08x\n", i, nbytes[i], *(uint32_t*)dstaddr[ii], *(uint32_t*)(((char*)dstaddr[ii])+nbytes[i]-8), *(uint32_t*)(((char*)dstaddr[ii])+nbytes[i]-4), buf->vf48_last_word[i]);

	    rb_increment_wp(buf->rb[i], 8 + buf->vf48_nbytes[i]);
	 }

   return rd;
}

#if 0
int vf48_multi_bank(struct vf48_multi_buffer* buf, char* pevent, int pevent_size, int one_event, int currentEvNum)
{
   int i;
   int rd = 0;

   for (i=0; i<MAX_VF48; i++)
      if (buf->vf48_addr[i] > 0)
	 {
	    int rp = buf->buf_rptr[i];
	    int wp = buf->buf_wptr[i];
	    int nbytes = wp - rp;

	    //printf("nbytes: %d\n", nbytes);

	    // 32-bit aligned by construction
	    assert(nbytes%4 == 0);

	    if (!one_event)
	       {
		  //	       if(currentEvNum < 5)
		  // printf("Entering NOT multi_buffer i = %d, currentEvNum = %d, nbytes %d\n",i,currentEvNum,nbytes);
		  DWORD* p32;

		  bk_create(pevent, buf->bank_name[i], TID_DWORD, &p32);

		  if (nbytes > pevent_size - rd - 2000)
		     {
			// check for remaining space in event buffer

			nbytes = pevent_size - rd - 3000;

			assert(nbytes%4 == 0);
		     }

		  if (!gDmaByteswap)
		     {
			memcpy(p32, buf->buf[i]+rp, nbytes);
		     }
		  else
		     {
			int nw = nbytes/4;
			uint32_t* src32 = (uint32_t*)(buf->buf[i]+rp);
			int j=0;
			for (j=0; j<nw; j++)
			   {
			      uint32_t w = src32[j];
			      p32[j] = be2cpu(w);
			   }
		     }
		  int last = (nbytes/4)-1;
		  int nevent = 0;
		  BOOL bad = 0;
		  //if(currentEvNum < 5) 
		  // if (i == 0) printf("bank %d, 1st w 0x%x, last w 0x%x\n",i,p32[0],p32[last]);
		  //Check that 1st and last events are Header and Trailer
		  if( (p32[0]&0xF0000000)!=0x80000000){
		     printf("vf48_multi_bank: mod add 0x%x, First word of bank is NOT a header 0x%x\n",buf->vf48_addr[i],p32[0]);
                     bad = 1;
		  }
		  else 
		     nevent = (p32[0]&0x00FFFFFF);
		  if( (p32[last]&0xFF000000)!=0xe5000000) {
		     printf("vf48_multi_bank: mod add 0x%x, Header ev# %d, Last word of bank is NOT a trailer 0x%x\n",buf->vf48_addr[i],nevent, p32[last]);
		     bad = 1;
		  }
		  else 
		     if( (p32[last]&0x00FFFFFF)!=nevent) {
			printf("vf48_multi_bank:mod add 0x%x, VF48 header ev# 0x%x is NOT the same as in the trailer 0x%x\n",buf->vf48_addr[i],nevent,p32[last]);
			bad = 1;
		     }
		  // The VF48 ev# should be equal to FE ev# +1 but sometimes there is a rogue event 
		  // flushed out at the beginning of the run. Then The VF48 ev# is equal to FE ev# +2
		  if (!bad) {
		     if( (p32[last]&0x00FFFFFF)!=currentEvNum+1) 
			if( (p32[last]&0x00FFFFFF)!=currentEvNum+2) {
			   printf("vf48_multi_bank mod add 0x%x, VF48 header ev# 0x%x is NOT the same as fe ev# 0x%x\n",buf->vf48_addr[i],nevent,currentEvNum);
			}
		  }
		  bk_close(pevent, p32 + nbytes/4);
		  
		  buf->buf_rptr[i] = rp + nbytes;
		  
		  rd += nbytes;

		  continue;
	       }
	    else  // if (!one_event 
	       {
	       if(currentEvNum < 2)
		  printf("Entering HERE multi_buffer i = %d, currentEvNum = %d\n",i,currentEvNum);
		  DWORD* p32;
		  int nw = nbytes/4;
		  uint32_t* src32 = (uint32_t*)(buf->buf[i]+rp);
		  int j=0;

		  // look for event header
                  if (!gDmaByteswap)
                     for (j=0; j<nw; j++)
                        {
                           uint32_t w = (src32[j]);
                           if ((w & 0xF0000000) == 0x80000000)
                              break;
                        }
                  else
                     for (j=0; j<nw; j++)
                        {
                           uint32_t w = be2cpu(src32[j]);
                           if ((w & 0xF0000000) == 0x80000000)
                              break;
                        }

#if 0
		  printf("module %d, wptr %d, rptr %d, nbytes %d, nw %d, header at %d\n",
			 i,
			 rp, 
			 wp,
			 nbytes,
			 nw, j);
#endif

		  buf->buf_rptr[i] = rp + j*4;

		  if (j==nw) 
		     continue;


		  bk_create(pevent, buf->bank_name[i], TID_DWORD, &p32);

		  int bc = 0;
		  uint32_t w = 0;

                  if (!gDmaByteswap)
                     for (; j<nw; j++)
                        {
                           w = (src32[j]);
                           *p32++ = w;
                           bc += 4;
                           
                           // stop when we see the event trailer
                           if ((w & 0xF0000000) == 0xE0000000)
                              {
                                 j++;
                                 break;
                              }
                        }
                  else
                     for (; j<nw; j++)
                        {
                           w = be2cpu(src32[j]);
                           *p32++ = w;
                           bc += 4;
                           
                           // stop when we see the event trailer
                           if ((w & 0xF0000000) == 0xE0000000)
                              {
                                 j++;
                                 break;
                              }
                        }
		  int nEvent =  (w & 0xFFFFFFF);
		  if (nEvent < 10)
		     printf ("Module  %d, trailer 0x%x, currentEv0x%x\n",i,w,currentEvNum);

		  bk_close(pevent, p32);

#if 0
		  printf("trailer at %d: 0x%08x 0x%08x\n", j, be2cpu(src32[j]), be2cpu(src32[j+1]));
#endif

		  buf->buf_rptr[i] = rp + j*4;

		  rd += bc;
	       }
	 }

   return rd;
}
#endif

int vf48_multi_bank_stream(struct vf48_multi_buffer* buf, char* pevent, int pevent_size)
{
   int status;
   int i;
   int rd = 0;

   for (i=0; i<MAX_VF48; i++)
      if (buf->vf48_addr[i] > 0)
	 {
	    DWORD* p32;

	    bk_create(pevent, buf->bank_name[i], TID_DWORD, &p32);

            //printf("pevent %p, name %s, p32 %p\n", pevent, buf->bank_name[i], p32);
	    buf->vf48_bank[i] = p32;
	    
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
		  
		  //printf("VF48_MULTI: unit %d, src32: %p -> 0x%08x\n", i, src32, src32[0]);
		  
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
		  
		  //printf("VF48_MULTI: unit %d, incr %d, nw %d, j %d\n", i, incr, nw, j);
		  
		  rb_increment_rp(buf->rb[i], incr);
		  
		  rd += nw*4;
		  
	       } // loop over all data segments
	    
	    bk_close(pevent, p32);
	 }
   
   return rd;
}

int vf48_multi_bank_event(struct vf48_multi_buffer* buf, char* pevent, int pevent_size, int expectedEventNo)
{
   int status;
   int i;
   int rd = 0;

   for (i=0; i<MAX_VF48; i++)
      if (buf->vf48_addr[i] > 0)
	 {
	    if (1)
	       {
		  DWORD* p32;
		  int bc = 0;
		  int headerEventNo = -1;
		  int trailerEventNo = -1;
		  //int copy = 0; // skip incomplete events at the beginning of buffer - DOES NOT WORK!
		  int copy = 1; // copy everything in the buffer until an end of event

		  bk_create(pevent, buf->bank_name[i], TID_DWORD, &p32);

		  while (1)
		     {
			int nbytes = 0;
			int nw = 0;
			uint32_t* src32;
			int incr = 0;
			int j;
			
			status = rb_get_rp(buf->rb[i], &src32, 0);
			if (status == DB_TIMEOUT)
			   break;
			if (status != SUCCESS)
			   printf("rb_get_rp() status %d\n", status);
			assert(status == SUCCESS);

			//printf("VF48_MULTI: unit %d, src32: %p -> 0x%08x, 0x%08x\n", i, src32, src32[0], src32[1]);

			assert(src32[0] == 0xabbaabba);
			assert(src32[1] > 0);
			assert(src32[1] < rb_size);

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
			      if ((w & 0xF0000000) == 0x80000000)
				 {
				    headerEventNo = (w & 0x00FFFFFF);
				    copy = 1;
				 }

			      if (copy)
				 {
				    *p32++ = w;
				    bc += 4;
				 }
				 
			      incr += 4;

			      // stop copying when we see the event trailer
			      if ((w & 0xF0000000) == 0xE0000000)
				 {
				    trailerEventNo = (w & 0x00FFFFFF);
				    copy = 0;
				    j++;
				    if (1) // check for crazy duplicate event number where after the trailer we have a header with the same event number
				       {
					  uint32_t ww = src32[j];
					  if (gDmaByteswap)
					     ww = be2cpu(ww);
					  
					  if ((ww & 0xF0000000) == 0X80000000)
					     {
						int newHeaderEventNo = (ww & 0x0000FFFF);
						if (newHeaderEventNo == trailerEventNo)
						   {
						      printf("VF48_MULTI: unit %d at 0x%08x: duplicate event data, expected event no %d, header %d, trailer %d, unexpected extra header %d\n", i, buf->vf48_addr[i], expectedEventNo, headerEventNo, trailerEventNo, newHeaderEventNo);
						      j--;
						      continue;
						   }
					     }
				       }
					  
				    break;
				 }

			   } // loop over data segment

			if (src32[j]==0xdeaddead || src32[j]==0xaddeadde)
			   {
			      j++;
			      incr += 4;
			   }

			//printf("VF48_MULTI: unit %d, segment size %d, nbytes %d, incr %d, trailerEventNo %d, nw %d, j %d\n", i, nbytes+8, nbytes, incr, trailerEventNo, nw, j);

			if (incr != nbytes + 8)
			   {
			      if (0)
				 {
				    int k;
				    printf("VF48 at 0x%x,  nw %d, incr %d, nbytes %d\n", buf->vf48_addr[i], nw, incr, nbytes);
				    for (k=0; k<nw+2; k++)
				       printf("word %d: 0x%08x swapped 0x%08x\n", k, src32[k-2], be2cpu(src32[k-2]));
				    //exit(1);
				 }
				    
			      assert(incr%4 == 0);
			      
			      int wp32 = incr/4;

			      src32[wp32-2] = 0xabbaabba;
			      src32[wp32-1] = nbytes - incr;
			   }

			rb_increment_rp(buf->rb[i], incr);

			if (trailerEventNo > 0)
			   break;

		     } // loop over all data segments


		  if (expectedEventNo > 0)
		     {
			expectedEventNo &= 0x00FFFFFF;

			//printf("VF48: unit %d at 0x%08x: expected event no %d, header %d, trailer %d\n", i, buf->vf48_addr[i], expectedEventNo, headerEventNo, trailerEventNo);

			if (headerEventNo != expectedEventNo || trailerEventNo != expectedEventNo)
			   {
			      printf("VF48_MULTI: unit %d at 0x%08x: expected event no %d, header %d, trailer %d\n", i, buf->vf48_addr[i], expectedEventNo, headerEventNo, trailerEventNo);
			   }
		     }
		  
		  bk_close(pevent, p32);

		  rd += bc;
	       } // end if (one_event)
	 } // end of loop over modules

   return rd;
}

int vf48_multi_thread(void* xparam)
{
   struct vf48_multi_thread_param* param = (struct vf48_multi_thread_param*)xparam;

   printf("vf48_multi_thread: started!\n");

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

	 int nb = vf48_multi_read(param->vme, param->buf, param->debug);
	 if (nb < 0)
	    {
	       printf("vf48_multi_thread: Error: vf48_multi_read() returned %d\n", nb);
	       break;
	    }


	 if (param->debug)
	    {
	       double t1 = utime();

	       double td = (t1 - t0)*1000000.0;

	       printf("vf48_multi_thread: read %d bytes in %.0f usec, %.1f Mbytes/sec\n", nb, td, nb/td);
	    }

	 if (nb==0)
	    sleep(1);
      }

   printf("vf48_multi_thread: stopped!\n");
   return 0;
}

#include <sys/time.h>

static double gettime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + 0.000001*tv.tv_usec;
}

int vf48_read_event(MVME_INTERFACE* vme,
		    struct vf48_multi_buffer* buf,
		    char* pevent, int pevent_size,
		    int expectedEventNo)
{    
   int loop, i;
   int count = 0;
   int nerror_vf48 = 0; // leftover from PIENU
   int nerror_max = 10; // leftover from PIENU

   for (i=0; i<MAX_VF48; i++)
      if (buf->vf48_addr[i] > 0) {
	 buf->vf48_first_word[i] = 0;
	 buf->vf48_last_word[i] = 0;
      }

   for (loop=0; loop<20; loop++)
      {
	 //double t0 = gettime();
	 int rd = vf48_multi_read(vme, buf, 0); // nodebug
	 //double t1 = gettime();
	 //printf("read %d, time %.6f sec\n", rd, t1-t0);
	 if (rd > 0)
	    {
	       count += rd;
	       continue;
	    }
	 
	 int read_more = 0;

	 for (i=0; i<MAX_VF48; i++)
	    if (buf->vf48_addr[i] > 0)
	       {
		  //printf("unit %d at 0x%08x: last word 0x%08x!\n", i, buf->vf48_addr[i], buf->vf48_last_word[i]);

		  if ((buf->vf48_last_word[i] & 0xF0000000) != 0xE0000000)
		     {
			//if (nerror_vf48 < nerror_max)
			//printf("VF48 unit %d at 0x%08x: last word 0x%08x, loop %d, read more!\n", i, buf->vf48_addr[i], buf->vf48_last_word[i], loop);
			read_more = 1;
			continue;
		     }
	       }

	 if (!read_more)
	    break;
      }

    int countb = vf48_multi_bank_stream(buf, pevent, pevent_size);

    for (i=0; i<MAX_VF48; i++)
       if (buf->vf48_addr[i] > 0)
	  {
	     int j;
	     int feEventNo = expectedEventNo & 0xFFFFFF;
	     int headerEventNo  = -1; 
	     int trailerEventNo = -1;

	     for (j=0; j<10; j+=1) {
		//printf("first word %d, 0x%x\n", j, buf->vf48_bank[i][j]);

		if (((buf->vf48_bank[i][j]) & 0xF0000000) == 0xD0000000)
		   continue;
		if (((buf->vf48_bank[i][j]) & 0xF0000000) == 0xF0000000)
		   continue;

		buf->vf48_first_word[i] = buf->vf48_bank[i][j];

		//if (((buf->vf48_bank[i][j]) & 0xF0000000) == 0x80000000) {
		//   headerEventNo = (buf->vf48_bank[i][j] & 0xFFFFFF);
		//}

		break;
	     }

	     if ((buf->vf48_first_word[i] & 0xF0000000) == 0x80000000)
		headerEventNo = (buf->vf48_first_word[i] & 0xFFFFFF);

	     if ((buf->vf48_last_word[i] & 0xF0000000) == 0xE0000000)
		trailerEventNo = (buf->vf48_last_word[i] & 0xFFFFFF);

	     if (0)
		printf("VF48 unit %d at 0x%08x: first word 0x%08x 0x%08x, last word 0x%08x, eventNo: %d %d!\n", i, buf->vf48_addr[i], buf->vf48_bank[i][0], buf->vf48_bank[i][1], buf->vf48_last_word[i], headerEventNo, trailerEventNo);

	     if (headerEventNo != feEventNo || trailerEventNo != feEventNo)
		{
		   nerror_vf48 +=1;
		   if (nerror_vf48 < nerror_max)
		      {
			 printf("Error: VF48 0x%08x: Event counters mismatch: header: %d, trailer: %d, frontend event %d, first word 0x%08x, last word 0x%08x\n", buf->vf48_addr[i], headerEventNo, trailerEventNo, feEventNo, buf->vf48_first_word[i], buf->vf48_last_word[i]);
		      }

		   //if (*buf->vf48_bank[i] == 0xdeaddead) {
		   //   int j;
		   //   for (j=0; j<16; j++)
		   //	 printf("buf %d: 0x%08x\n", j, buf->vf48_bank[i][j]);
		   //}
		}
	  }

    //printf("loop %d, min_data %d\n", xloop, min_data);

    return countb;
}    

/* emacs
 * Local Variables:
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */

// end

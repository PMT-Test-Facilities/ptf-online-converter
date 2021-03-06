/*********************************************************************
  Name:         vf48.c
  Created by:   Pierre-Andre Amaudruz / Jean-Pierre Martin

  Contents:     48 ch Flash ADC / 20..64 Msps from J.-P. Martin
                
  $Id: vf48.c 4489 2009-05-21 16:31:59Z olchanski $

*********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "vf48.h"

int vf48_RegisterRead(MVME_INTERFACE *mvme, DWORD base, int reg)
{
   mvme_set_am(mvme, MVME_AM_A24);
   mvme_set_dmode(mvme, MVME_DMODE_D32);

   return mvme_read_value(mvme, base+reg);
}

int vf48_RegisterWrite(MVME_INTERFACE *mvme, DWORD base, int reg, int value)
{
   mvme_set_am(mvme, MVME_AM_A24);
   mvme_set_dmode(mvme, MVME_DMODE_D32);

   return mvme_write_value(mvme, base+reg, value);
}

/********************************************************************/
/** vf48_EventRead64
    Read one Event through the local buffer. MBLT64 is used for aquiring
    a buffer, the event is then extracted from this buffer until event
    incomplete which in this case a DMA is requested again.
    @param mvme vme structure
    @param base  VF48 base address
    @param pdest Pointer to destination
    @param nentry Number of DWORD to transfer
    @return status VF48_ERROR, SUCCESS
*/
static int   idx = 0, inbuf;
int vf48_EventRead64(MVME_INTERFACE *mvme, DWORD base, DWORD *pdest, int *nentry)
{
  int   markerfound, j;
  DWORD lData[VF48_IDXMAX];
  DWORD *phead;
  
  int cmode, timeout;
  
  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  
  if (inbuf > VF48_IDXMAX) idx = 0;
  
  if (idx == 0) {
    inbuf = vf48_NFrameRead(mvme, base);
    // max readout buffer (HW dependent)
    vf48_DataRead(mvme, base, lData, &inbuf);
    //    printf("Data Read Idx=0 %d\n", inbuf);
  }
  
  // Check if available event found in local buffer
  timeout = 16;
  markerfound = 0;
  while (timeout) {
    // Scan local buffer from current index to top for Header
    while ((idx < inbuf) && ((lData[idx] & 0xF0000000) != VF48_HEADER)) {
      idx++;
    }
    if (idx < inbuf) {
      // Header found, save head, flag header, cp header
      timeout = 0;              // Reset to force exit on next test
      phead = pdest;            // Save top of event for evt length
      markerfound = 1;          //
      //      printf("Head found  idx %d\n", idx);
      *pdest++ = lData[idx];
      lData[idx] = 0xffffffff;
      idx++;
    } else {
      // No header found, request VME data (start at idx = 0)
      timeout--;               // Safe exit
      inbuf = vf48_NFrameRead(mvme, base);
      // max readout buffer (HW dependent)
      idx = 0;
      vf48_DataRead(mvme, base, lData, &inbuf);
      //      printf("Data Read Header search %d (timeout %d)\n", inbuf, timeout);
    }
  }
  if ((timeout == 0) && !markerfound) {
    //    printf("vf48_EventRead: Header Loop: Timeout Header not found\n");
    idx = 0;         // make sure we read VME on next call
    *nentry = 0;               // Set returned event length
    return VF48_ERROR;
  }
  
  //  printf("Starting data copy\n");
  timeout = 64;
  while(timeout) {
    timeout--;
    // copy rest of event until Trailer (nentry: valid# from last vme data)
    while ((idx < inbuf) && ((lData[idx] & 0xF0000000) != VF48_TRAILER)) {
      *pdest++ = lData[idx++];
    }
    if ((lData[idx] & 0xF0000000) == VF48_TRAILER) { 
      // Event complete return
      *pdest++ = lData[idx++];  // copy Trailer
      *nentry = pdest - phead;  // Compute event length
      //      printf("Event complete idx:%d last inbuf:%d evt len:%d\n", idx, inbuf, *nentry);
      return SUCCESS;
    } else {
      // No Trailer found, request VME data (start at idx = 0)
      //      timeout--;                // Safe exit
      j = 40000;
      while (j) { j--; }
      // usleep(1);
      inbuf = vf48_NFrameRead(mvme, base);
      vf48_DataRead(mvme,  base, lData, &inbuf);
      idx = 0;
      // printf("Data Read Trailer search %d (timeout:%d)\n", inbuf, timeout);
    }
    if (timeout == 0) {
      printf("vf48_EventRead: Trailer Loop: Timeout Trailer not found\n");
      idx = 0;                // make sure we read VME on next call
      *nentry = 0;               // Set returned event length
      return VF48_ERROR;
    }
  }
  printf(" coming out from here\n");
  return VF48_ERROR;
}

/********************************************************************/
/** vf48_EventRead
   Read one Event (in 32bit mode)
   @param mvme vme structure
   @param base  VF48 base address
   @param pdest Pointer to destination
   @param nentry Number of DWORD to transfer
   @return status VF48_ERROR, SUCCESS
*/
int vf48_EventRead(MVME_INTERFACE *mvme, DWORD base, DWORD *pdest, int *nentry)
{
  int cmode, timeout, nframe;
  DWORD hdata;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);

  *nentry = 0;
  nframe = mvme_read_value(mvme, base+VF48_NFRAME_R);
  //  printf("Event Nframe : %d - 0x%x\n", nframe, nframe);

  timeout=25000;  // max number of 32bits per event
  if (nframe) {
    do {
      timeout--;
      hdata = mvme_read_value(mvme, base+VF48_DATA_FIFO_R);
    } while (!((hdata & 0xF0000000) == VF48_HEADER) && (timeout)); // skip up to the header

    if (timeout == 0) {
      *nentry = 0;
      //      printf("timeout on header  data:0x%lx\n", hdata);
      mvme_set_dmode(mvme, cmode);
      return VF48_ERR_NODATA;
    }
    //    channel = (hdata >> 24) & 0xF;
    //    printf(">>>>>>>>>>>>>>> Channel : %d \n", channel);
    pdest[*nentry] = hdata;
    *nentry += 1;
    timeout=25000;  // max number of 32bits per event
    // Copy until Trailer (max event size == timeout!)
    do {
      pdest[*nentry] = mvme_read_value(mvme, base+VF48_DATA_FIFO_R);
      //      printf("pdest[%d]=0x%x\n", *nentry, pdest[*nentry]);
      timeout--;
      *nentry += 1;
    } while (!((pdest[*nentry-1] & 0xF0000000) == VF48_TRAILER) && timeout); // copy until trailer (included)
    if (timeout == 0) {
      printf("timeout on Trailer  data:0x%x\n", pdest[*nentry-1]);
      printf("nentry:%d data:0x%x base:0x%x \n", *nentry, pdest[*nentry], base+VF48_DATA_FIFO_R);
      *nentry = 0;
      return VF48_ERROR;
    }
    nentry--;
  }
  mvme_set_dmode(mvme, cmode);
  return (VF48_SUCCESS);
}

/********************************************************************/
/** vf48_EventReadPio
   Read one Event from FE FIFOs using PIO (collector firmware 0x07110411 or newer)
   @param mvme vme structure
   @param base  VF48 base address
   @param pdest Pointer to destination
   @param nentry Number of DWORD to transfer
   @return status VF48_ERROR, SUCCESS
*/
int vf48_EventReadPio(MVME_INTERFACE *mvme, DWORD base, DWORD *pdest, int *nwords)
{
  int i;
  int max_words = *nwords;
  int nread = 0;

  *nwords = 0;

  int csr = vf48_RegisterRead(mvme, base, VF48_CSR_REG_RW);

  assert(csr & VF48_CSR_FIFO_DISABLE);

  int grpEnable = vf48_RegisterRead(mvme, base, VF48_GRP_REG_RW);
  
  for (i=0; i<6; i++)
     if ((1<<i)&grpEnable) {
        int status, nw, addr, j;
        int done = 0;
        while (!done) {
           status = vf48_RegisterRead(mvme, base, VF48_FifoStat10 + 0x10*(i/2));
           if (i&1)
              status >>= 16;
           status &= 0xFFFF;
           //printf("fifo %d status 0x%04x\n", i, status);
           if (status & 0x8000) // FIFO empty?
              break;
           nw = status & 0x0FFF;
           addr = VF48_FifoData0 + 0x10*i;
           //printf("addr 0x%08x\n", addr);
           *pdest++ = 0xFFFFFFF0 + i;
           nread++;
           for (j=0; j<nw; j++) {
              DWORD w = vf48_RegisterRead(mvme, base, addr);
              *pdest++ = w;
              nread++;
              if ((w & 0xF0000000) == 0xE0000000) {
                 done = 1;
                 break;
              }
              if (nread >= max_words) {
                 done = 1;
                 break;
              }
           }
        }
        if (nread >= max_words)
           break;
     }

  *nwords = nread;

  return (VF48_SUCCESS);
}

/** vf48_EventReadSync
   Read all available data from a VF48 using fastest method available. Requires VF48 collector rev 0x07110727 or later.
   @param mvme vme structure
   @param base  VF48 base address
   @param pdest Pointer to destination
   @param nentry Number of DWORD to transfer
   @return status VF48_ERROR, SUCCESS
*/
int vf48_EventReadSync(MVME_INTERFACE *mvme, DWORD base, DWORD *pdest, int *nwords)
{
   int i, j, rd;
   int max_words = *nwords;
   int iptr = 0;
   while (1)
      {
         int csr = vf48_CsrRead(mvme, base);
         int haveData = vf48_NFrameRead(mvme, base);
         
         //printf("VF48 csr 0x%08x, has data: %6d, iptr: %6d\n", csr, haveData, iptr);

         if (haveData == 0)
            break;
         
	 if (csr&VF48_CSR_FIFO_EMPTY)
	   continue;
         
         if ((haveData < 2000) && (csr&VF48_CSR_FIFO_INCOMPLETE))
            {
               int xframe = haveData;
               int nframe;
               for (i=0; i<100; i++)
                  {
                     for (j=0; j<10; j++)
                        csr = vf48_CsrRead(mvme, base);
                     
                     nframe = vf48_NFrameRead(mvme, base);
                     

                     if ((nframe > 2000) || ((csr&VF48_CSR_FIFO_INCOMPLETE)==0))
                        break;
                     
                     if (nframe != xframe)
                        {
                           //printf("VF48 has data: %6d, new: %6d, time %f\n", haveData, nframe, now-tt0);
                           
                           xframe = nframe;
                           continue;
                        }
                     
                     //if (now - tt0 > 0.000200)
                     //   break;
                  }
               
               haveData = nframe;
               
               //printf("VF48 has data: %6d\n", haveData);
            }
         
#if 0
         int count = haveData&~0x1;
#endif
         int count = haveData&~0xF;

         //if (haveData > 2000)
         //count = 6000;
            
         if (haveData < 2000)
            count += 16;

         csr = vf48_CsrRead(mvme, base);
         
         //printf("VF48 csr 0x%08x, haveData %d, count %d 0x%x, address 0x%p\n", csr, haveData, count, count, pdest + iptr);
         
         if (count == 0)
            continue;
         
         if (iptr + count + 10 > max_words)
            {
	      assert(!"buffer overflow");
            }
         
         //pdest[iptr++] = 0xdeadbeef;
         //pdest[iptr++] = 0xdeadbeef;

         // align VME DMA start address
         while (((int)(pdest+iptr) & 0xf) != 0)
            pdest[iptr++] = 0xdeadbeef;
         
         //printf("VF48 csr 0x%08x, haveData %d, count %d 0x%x, address 0x%p\n", csr, haveData, count, count, pdest + iptr);

         rd = vf48_DataRead(mvme, base, pdest+iptr, &count);
         if (rd != count) {
            printf("Error reading VF48 data!\n");
            exit(1);
         }

         //printf("Data read: wc: %d, count: %d, %f us\n", wc, count, dxt);
         
         iptr += rd;
      }
   
   *nwords = iptr;
   
   return VF48_SUCCESS;
}

/********************************************************************/
/** vf48_DataRead
Read N entries (32bit) from the VF48 data FIFO using the MBLT64 mode
@param mvme vme structure
@param base  VF48 base address
@param pdest Destination pointer
@return nentry
 */
int vf48_DataRead(MVME_INTERFACE *mvme, DWORD base, DWORD *pdest, int *nentry)
{
  int status;

  mvme_set_am(  mvme, MVME_AM_A24);
  mvme_set_dmode(  mvme, MVME_DMODE_D32);

  mvme_set_blt(  mvme, MVME_BLT_MBLT64);

  if (0) {
    DWORD fwrev;
    fwrev = vf48_RegisterRead(mvme, base, VF48_FIRMWARE_R);
    if ((fwrev&0xFF000000)==0x07000000) 
      if (fwrev >= 0x07110427)
        mvme_set_blt(  mvme, MVME_BLT_2EVME);
  }

  // V7865 2eVME:  Event read: 24595 words in 0.002555 sec in 12 reads, 0.001678 sec DMA, 38.504 Mbytes/sec per event, 58.637 Mbytes/sec per DMA, 28.228 Mbytes/sec sustained!
  // V7865 MBLT64: Event read: 24579 words in 0.002593 sec in 12 reads, 0.002091 sec DMA, 37.915 Mbytes/sec per event, 47.021 Mbytes/sec per DMA, 27.721 Mbytes/sec sustained!
  // V7865 BLT32:  Event read: 24339 words in 0.004728 sec in  7 reads, 0.004188 sec DMA, 20.592 Mbytes/sec per event, 23.247 Mbytes/sec per DMA, 17.342 Mbytes/sec sustained!

  //mvme_set_blt(  mvme, MVME_BLT_2ESST);
  //mvme_set_blt(  mvme, MVME_BLT_2EVME);
  //mvme_set_blt(  mvme, MVME_BLT_MBLT64);
  //mvme_set_blt(  mvme, MVME_BLT_BLT32);
  //mvme_set_blt(  mvme, MVME_BLT_NONE);
  //mvme_set_blt(  mvme, 0);

  // Transfer in MBLT64 (8bytes), nentry is in 32bits(VF48)
  // *nentry * 8 / 2
  status = mvme_read(mvme, pdest, base+VF48_DATA_FIFO_R, *nentry<<2);
  if (status != MVME_SUCCESS)
    return 0;

  return (*nentry);
}

/********************************************************************/
int vf48_ParameterWrite(MVME_INTERFACE *mvme, DWORD base, int grp, int param, int value)
{
  int retry;
  int  cmode;

  if (grp < 0 || grp >= 6) {
    printf("vf48_ParameterWrite: Invalid grp %d\n", grp);
    return VF48_ERR_PARM;
  }

  switch (param) {
  case VF48_SEGMENT_SIZE:
    if (value > 1023) {
      value = 1023;
      printf("vf48_ParameterWrite: Segment size troncated to 1023\n");
    }
    break;
  case VF48_PRE_TRIGGER:
    if (value > 127) {
      value = 100;
      printf("vf48_ParameterWrite: Pre trigger troncated to 100\n");
    }
    break;
  case VF48_LATENCY:
    if (value > 127) {
      value = 127;
      printf("vf48_ParameterWrite: Latency troncated to 127\n");
    }
    break;
  };

  value &= 0xFFFF;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);

  if (0) {
    int csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW);
    printf("Writing grp %d, param %d, ID_RDY %d, DATA_RDY %d\n", grp, param, csr & VF48_CSR_PARM_ID_RDY, csr & VF48_CSR_PARM_DATA_RDY);
  }

  for (retry=20; retry>0; retry--) {
    int wr;
    int rd;
    
    wr = param | grp<<VF48_GRP_OFFSET;
    mvme_write_value(mvme, base+VF48_PARAM_ID_W, wr);
    
    // flush posted PCI writes
    mvme_read_value(mvme, base+VF48_CSR_REG_RW);
    
    mvme_write_value(mvme, base+VF48_PARAM_DATA_RW, value);
    
    // flush posted PCI writes
    mvme_read_value(mvme, base+VF48_CSR_REG_RW);

    rd = vf48_ParameterRead(mvme, base, grp, param);

    if (rd == VF48_ERR_PARM) {
      printf("vf48_ParameterWrite: Module at 0x%x, Group %d, parameter %d: Error %d during parameter read-back\n", base, grp, param, rd);
      return VF48_ERR_PARM;
    }

    //printf("Group %d, param %d, write %d, read %d\n", grp, param, value, rd);

    if (rd == value) {
      mvme_set_dmode(mvme, cmode);
      return VF48_SUCCESS;
    }

    printf("vf48_ParameterWrite: Module at 0x%x, Group %d, parameter %d: data mismatch error: wrote 0x%08x, read 0x%08x\n", base, grp, param, value, rd);
  }

  fprintf(stderr, "vf48_ParameterWrite: Group %d, parameter %d, too many retries writing VF48 parameter!\n", grp, param);
  return VF48_ERR_PARM;
}

/********************************************************************/
/**
Read any Parameter for a given group. Each group (0..5) handles
8 consecutive input channels.
@param mvme vme structure
@param base  VMEIO base address
@param grp   group number (0..5)
@return VF48_SUCCESS, VF48_ERR_PARM
*/
int vf48_ParameterRead(MVME_INTERFACE *mvme, DWORD base, int grp, int param)
{
  int  cmode, to, retry;
  int  debug = 0;

  if (vf48_isPresent(mvme, base) != VF48_SUCCESS) {
    printf("vf48_ParameterRead: There is no VF48 at VME A24 0x%x\n", base);
    return VF48_ERR_PARM;
  }

  if (grp < 0 || grp >= 6) {
    printf("vf48_ParameterRead: Invalid grp %d\n", grp);
    return VF48_ERR_PARM;
  }

  mvme_set_am(mvme, MVME_AM_A24);
  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);

  for (retry=20; retry>0; retry--) {

    int csr = 0;
    int data = 0;
    int wr;
    
    if (debug) {
      csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW);
      printf("vf48_ParameterRead: CSR 0x%x, Reading grp %d, param %d, ID_RDY %d, DATA_RDY %d\n", csr, grp, param, csr & VF48_CSR_PARM_ID_RDY, csr & VF48_CSR_PARM_DATA_RDY);
    }

    wr = VF48_PARMA_BIT_RD | param | grp<<VF48_GRP_OFFSET;

    if (debug)
       printf("write id 0x%04x\n", wr);

    mvme_write_value(mvme, base+VF48_PARAM_ID_W, wr);

    // flush posted PCI writes
    csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW);

    if (debug) {
      printf("vf48_ParameterRead: CSR 0x%x, Reading grp %d, param %d, ID_RDY %d, DATA_RDY %d\n", csr, grp, param, csr & VF48_CSR_PARM_ID_RDY, csr & VF48_CSR_PARM_DATA_RDY);

      //if (!(csr & VF48_CSR_PARM_ID_RDY))
      //exit(123);
    }

    //mvme_write_value(mvme, base+VF48_PARAM_DATA_RW, 0xFFFFFFFF);
    mvme_write_value(mvme, base+VF48_PARAM_DATA_RW, 0xFFFFFFFF);

    // flush posted PCI writes
    csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW);

    if (debug) {
      printf("vf48_ParameterRead: CSR 0x%x, Reading grp %d, param %d, ID_RDY %d, DATA_RDY %d\n", csr, grp, param, csr & VF48_CSR_PARM_ID_RDY, csr & VF48_CSR_PARM_DATA_RDY);
    }

    // wait for VF48_CSR_PARM_DATA_RDY
    for (to=10; to>0; to--) {
      csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW);
      if (debug) {
         printf("vf48_ParameterRead: CSR 0x%x, Waiting for 0x%x, TO %d\n", csr, VF48_CSR_PARM_DATA_RDY, to);
         data = mvme_read_value(mvme, base+VF48_PARAM_DATA_RW);
         printf("data register 0x%x\n", data);
      }
      if (csr & VF48_CSR_PARM_DATA_RDY)
        break;
    }

    if (csr & VF48_CSR_CRC_ERROR) {
      if (1)
        printf("vf48_ParameterRead: CSR 0x%x, CRC error!\n", csr);
      //continue;
    }

    if (! (csr & VF48_CSR_PARM_DATA_RDY)) {
      if (debug)
        printf("vf48_ParameterRead: Group %d, parameter %d, timeout waiting for VF48_CSR_PARAM_DATA_RDY, retry %d\n", grp, param, retry);
      continue;
    }

    data = mvme_read_value(mvme, base+VF48_PARAM_DATA_RW);
    
    csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW);
    
    if (debug)
      printf("vf48_ParameterRead: CSR 0x%x, data 0x%x\n", csr, data);
    
    mvme_set_dmode(mvme, cmode);
    return data & 0xFFFF;
  }

  //fprintf(stderr, "vf48_ParameterRead: Group %d, parameter %d, too many retries reading vf48 parameter!\n", grp, param);
  return VF48_ERR_PARM;
}

/********************************************************************/
int  vf48_ReconfigureFPGA(MVME_INTERFACE *mvme, DWORD base)
{
   vf48_RegisterWrite(mvme, base, VF48_SOFTWARE_RESET_W, 0);
   vf48_RegisterRead(mvme, base, VF48_CSR_REG_RW);
   return VF48_SUCCESS;
}

/********************************************************************/
int vf48_Reset(MVME_INTERFACE *mvme, DWORD base)
{
  int i, v, csr, good, try;
  DWORD fwrev;

  mvme_set_am(mvme, MVME_AM_A24);
  mvme_set_dmode(mvme, MVME_DMODE_D32);

  fwrev = vf48_RegisterRead(mvme, base, VF48_FIRMWARE_R);

  printf("vf48_Reset: Module 0x%06x: collector firmware revision is 0x%08x\n", base, fwrev);

  if (fwrev == 0xFFFFFFFF) {
     printf("vf48_Reset: There is no VF48 at VME A24 address 0x%x\n", base);
     return VF48_ERR_HW;
  }

  for (try=0; try<100; try++) {

  if (1)
    printf("vf48_Reset: CSR 0x%x\n", mvme_read_value(mvme, base+VF48_CSR_REG_RW));
  mvme_write_value(mvme, base+VF48_GLOBAL_RESET_W, 0);
  csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW); // flush posted PCI writes

  if (1)
    printf("vf48_Reset: CSR 0x%x\n", csr);

#if 0
  if (!(csr & VF48_CSR_PARM_ID_RDY) || !(csr & VF48_CSR_PARM_DATA_RDY)) {
     /* frontend realtime signals did not reset correctly,
      * reset them by toggling the "col_run" realtime signal */

     vf48_AcqStart(mvme, base);
  csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW); // flush posted PCI writes
  csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW); // flush posted PCI writes
  csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW); // flush posted PCI writes
  csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW); // flush posted PCI writes
  csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW); // flush posted PCI writes
  csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW); // flush posted PCI writes
     vf48_AcqStop(mvme, base);

     csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW); // flush posted PCI writes

     if (1)
        printf("vf48_Reset: CSR 0x%x\n", csr);
  }
#endif

  mvme_write_value(mvme, base+VF48_TEST_REG_RW, 0x00000000);
  v = mvme_read_value(mvme, base+VF48_TEST_REG_RW);
  if (v != 0)
    printf("vf48_Reset: Test register data mismatch: read 0x%08x, expected 0\n", v);

  mvme_write_value(mvme, base+VF48_TEST_REG_RW, 0xFFFFFFFF);
  v = mvme_read_value(mvme, base+VF48_TEST_REG_RW);
  if (v != 0xFFFFFFFF)
    printf("vf48_Reset: Test register data mismatch: read 0x%08x, expected 0xFFFFFFFF\n", v);

  if (vf48_isPresent(mvme, base) != VF48_SUCCESS) {
    printf("There is no VF48 at VME A24 0x%x\n", base);
    return VF48_ERR_HW;
  }

  if (1) {
     csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW);
     
     if (csr & VF48_CSR_START_ACQ)
        vf48_AcqStop(mvme, base);
  }

  // at this point, VF48 collector is reset,
  // now try to reset the frontends

  if (fwrev == 0x4d77d1fb) {
     vf48_RegisterWrite(mvme, base, VF48_GRP_REG_RW, 0x3F);
     sleep(1);
  } else if ((fwrev&0xFF000000)==0x07000000) {
     int lvdsa, lvdsb;

     v = vf48_RegisterRead(mvme, base, VF48_GRP_REG_RW);
     vf48_RegisterWrite(mvme, base, VF48_GRP_REG_RW, 0); // clear of group-enable bits places frontend FPGA into reset mode, FW rev 2011mar14
     vf48_RegisterRead(mvme, base, VF48_CSR_REG_RW); // flush posted PCI writes

     lvdsa = vf48_RegisterRead(mvme, base, VF48_LinkMonA);
     lvdsb = vf48_RegisterRead(mvme, base, VF48_LinkMonB);

     printf("vf48_Reset: LVDS link status : 0x%08x%08x\n", lvdsb, lvdsa);

     //sleep(1);

     vf48_RegisterWrite(mvme, base, VF48_GRP_REG_RW, 0x3F);
     vf48_RegisterRead(mvme, base, VF48_CSR_REG_RW); // flush posted PCI writes

     //sleep(1);

     for (i=0; i<10000; i++) {
        int xlvdsa, xlvdsb;
        xlvdsa = vf48_RegisterRead(mvme, base, VF48_LinkMonA);
        xlvdsb = vf48_RegisterRead(mvme, base, VF48_LinkMonB);
        if (xlvdsa != lvdsa || xlvdsb != lvdsb)
           printf("vf48_Reset: LVDS link status : 0x%08x%08x, wait count %d\n", xlvdsb, xlvdsa, i);
        lvdsa = xlvdsa;
        lvdsb = xlvdsb;

        if (lvdsa == 0x04040404 && lvdsb == 0x00000404)
           break;
     }
  }

  //vf48_Status(mvme, base);
  //exit(1);

  if (1) {
    // set self-trigger thresholds high to avoid firmware bug where self-trigger is no gated by the RUN signal.
    // if bug is present, because default self-trigger threshold is very low, self-trigger
    // may continuously fire and jam and disable the FE-to-COL downlink, making it impossible (hard to)
    // to access FE parameters.
    for (i=0; i<6; i++)
      vf48_ParameterWrite(mvme, base, i, VF48_TRIG_THRESHOLD, 1000);
  }

  printf("vf48_Reset: Module 0x%06x: Firmware revision: collector: 0x%08x, frontend:", base, vf48_RegisterRead(mvme, base, VF48_FIRMWARE_R));
  
  good = 1;
  for (i=0; i<6; i++) {
     int vh, vl;
     vh = vf48_ParameterRead(mvme, base, i, VF48_FIRMWARE_ID);
     vl = vf48_ParameterRead(mvme, base, i, VF48_FIRMWARE_ID_LOW);
     printf(" 0x%04x%04x", vh, vl);
     if ((vh&0xFFFF) == 0xFFFF)
	good = 0;
     if ((vl&0xFFFF) == 0xFFFF)
	good = 0;
  }
  printf("\n");

    if (good)
      break;
  }

  if (try > 0)
    printf("vf48_Reset: Module 0x%06x, %d reset retries\n", base, try);

  return VF48_SUCCESS;
}

/********************************************************************/
int vf48_ResetCollector(MVME_INTERFACE *mvme, DWORD base)
{
  mvme_set_am(mvme, MVME_AM_A24);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  mvme_write_value(mvme, base+VF48_SOFTWARE_RESET_W, 0);
  mvme_read_value(mvme, base+VF48_CSR_REG_RW); // flush posted PCI writes
  return VF48_SUCCESS;
}

/********************************************************************/
int vf48_ResetFrontends(MVME_INTERFACE *mvme, DWORD base, int groupMask)
{
  mvme_set_am(mvme, MVME_AM_A24);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  mvme_write_value(mvme, base+VF48_LVDS_RESET_W, groupMask);
  mvme_read_value(mvme, base+VF48_CSR_REG_RW); // flush posted PCI writes
  return VF48_SUCCESS;
}

/********************************************************************/
int vf48_AcqStart(MVME_INTERFACE *mvme, DWORD base)
{
  int  cmode, csr;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  mvme_set_am(mvme, MVME_AM_A24);
  csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW);
  csr |= VF48_CSR_START_ACQ;  // start ACQ
  mvme_write_value(mvme, base+VF48_CSR_REG_RW, csr);
  mvme_read_value(mvme, base+VF48_CSR_REG_RW);
  mvme_set_dmode(mvme, cmode);
  return VF48_SUCCESS;

#if 0
  status = mvme_read_value(mvme, base+VF48_NFRAME_R);
  if (status <= 0xFF) {
    mvme_write_value(mvme, base+VF48_SELECTIVE_SET_W, VF48_CSR_ACTIVE_ACQ);
  }
  mvme_set_dmode(mvme, cmode);
  return (status > 0xFF ? VF48_ERR_HW : VF48_SUCCESS);
#endif
}

/********************************************************************/
int vf48_AcqStop(MVME_INTERFACE *mvme, DWORD base)
{
  int  cmode, csr;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_am(mvme, MVME_AM_A24);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW);
  csr &= ~(VF48_CSR_START_ACQ);  // stop ACQ
  mvme_write_value(mvme, base+VF48_CSR_REG_RW, csr);
  mvme_read_value(mvme, base+VF48_CSR_REG_RW);
  mvme_set_dmode(mvme, cmode);
  return VF48_SUCCESS;

#if 0
  status = mvme_read_value(mvme, base+VF48_NFRAME_R);
  if (status <= 0xFF) {
    mvme_write_value(mvme, base+VF48_SELECTIVE_CLR_W, VF48_CSR_ACTIVE_ACQ);
  }
  mvme_set_dmode(mvme, cmode);
  return (status > 0xFF ? VF48_ERR_HW : VF48_SUCCESS);
#endif
}

/********************************************************************/
/**
Set External Trigger enable
@param mvme vme structure
@param base  VF48 base address
@return VF48_SUCCESS
*/
int vf48_ExtTrgSet(MVME_INTERFACE *mvme, DWORD base)
{
  int cmode, csr;
  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW);
  csr |= VF48_CSR_EXT_TRIGGER;  // Set
  mvme_write_value(mvme, base+VF48_CSR_REG_RW, csr);
  mvme_set_dmode(mvme, cmode);
  return VF48_SUCCESS;
}

/********************************************************************/
/**
Clear External Trigger enable
@param mvme vme structure
@param base  VMEIO base address
@return vf48_SUCCESS
*/
int vf48_ExtTrgClr(MVME_INTERFACE *mvme, DWORD base)
{
  int  cmode, csr;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  csr = mvme_read_value(mvme, base+VF48_CSR_REG_RW);
  csr &= ~(VF48_CSR_EXT_TRIGGER);  // clr
  mvme_write_value(mvme, base+VF48_CSR_REG_RW, csr);
  mvme_set_dmode(mvme, cmode);
  return VF48_SUCCESS;
}

/********************************************************************/
int vf48_NFrameRead(MVME_INTERFACE *mvme, DWORD base)
{
   return vf48_RegisterRead(mvme, base, VF48_NFRAME_R);
}

/********************************************************************/
int vf48_CsrRead(MVME_INTERFACE *mvme, DWORD base)
{
   return vf48_RegisterRead(mvme, base, VF48_CSR_REG_RW);
}

/********************************************************************/
int vf48_CsrWrite(MVME_INTERFACE *mvme, DWORD base, int value)
{
   return vf48_RegisterWrite(mvme, base, VF48_CSR_REG_RW, value);
}

/********************************************************************/
int  vf48_GrpEnable(MVME_INTERFACE *mvme, DWORD base, int grpbit)
{
  int  cmode;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  grpbit &= 0x3F;
  mvme_write_value(mvme, base+VF48_GRP_REG_RW, grpbit);
  mvme_set_dmode(mvme, cmode);
  return VF48_SUCCESS;
}

/********************************************************************/
int vf48_GrpRead(MVME_INTERFACE *mvme, DWORD base)
{
  int  cmode,grp;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  grp = mvme_read_value(mvme, base+VF48_GRP_REG_RW);
  mvme_set_dmode(mvme, cmode);
  return grp;
}

/********************************************************************/
/**
Set the segment size for all 6 groups
 */
int vf48_SegmentSizeSet(MVME_INTERFACE *mvme, DWORD base, DWORD size)
{
  int  cmode, i;

  assert(size>0);
  assert(size<=1000);

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  for (i=0;i<6;i++)
    vf48_ParameterWrite(mvme, base, i, VF48_SEGMENT_SIZE, size);
  mvme_set_dmode(mvme, cmode);
  return 1;
}

/********************************************************************/
/**
Read the segment size from a given group. This value should be the
same for all the groups.
 */
int vf48_SegmentSizeRead(MVME_INTERFACE *mvme, DWORD base, int grp)
{
  int  cmode, val;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  val =  vf48_ParameterRead(mvme, base, grp, VF48_SEGMENT_SIZE);
  mvme_set_dmode(mvme, cmode);
  return val;
}

/********************************************************************/
/**
Set the trigger threshold for the given group. the threshold value
correspond to the difference of 2 sampling values separated by 2
points (s6-s3). It is compared to a  positive value. If the slope
of the signal is negative, the signal should be inverted.
 */
int vf48_TrgThresholdSet(MVME_INTERFACE *mvme, DWORD base, int grp, DWORD size)
{
  int  cmode;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  vf48_ParameterWrite(mvme, base, grp, VF48_TRIG_THRESHOLD, size);
  mvme_set_dmode(mvme, cmode);
  return grp;
}

/********************************************************************/
/**
Read the trigger threshold for a given group.
*/
int vf48_TrgThresholdRead(MVME_INTERFACE *mvme, DWORD base, int grp)
{
  int  cmode, val;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  val =  vf48_ParameterRead(mvme, base, grp, VF48_TRIG_THRESHOLD);
  mvme_set_dmode(mvme, cmode);
  return val;
}

/********************************************************************/
/**
Set the hit threshold for the given group. the threshold value
correspond to the difference of 2 sampling values separated by 2
points (s6-s3). It is compared to a  positive value. If the slope
of the signal is negative, the signal should be inverted.
The hit threshold is used for channel suppression in case none
of the described condition is satisfied.
 */
int vf48_HitThresholdSet(MVME_INTERFACE *mvme, DWORD base, int grp, DWORD size)
{
  int  cmode;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  vf48_ParameterWrite(mvme, base, grp, VF48_HIT_THRESHOLD, size);
  mvme_set_dmode(mvme, cmode);
  return grp;
}

/********************************************************************/
/**
Read the hit threshold for a given group.
*/
int vf48_HitThresholdRead(MVME_INTERFACE *mvme, DWORD base, int grp)
{
  int  cmode, val;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  val =  vf48_ParameterRead(mvme, base, grp, VF48_HIT_THRESHOLD);
  mvme_set_dmode(mvme, cmode);
  return val;
}

/********************************************************************/
/**
Enable the channel within the given group. By default all 8 channels
are enabled.
*/
int vf48_ActiveChMaskSet(MVME_INTERFACE *mvme, DWORD base, int grp, DWORD size)
{
  int  cmode;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  vf48_ParameterWrite(mvme, base, grp, VF48_ACTIVE_CH_MASK, size);
  mvme_set_dmode(mvme, cmode);
  return grp;
}

/********************************************************************/
/**
Read the channel enable mask for a given group.
 */
int vf48_ActiveChMaskRead(MVME_INTERFACE *mvme, DWORD base, int grp)
{
  int  cmode, val;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  val =  vf48_ParameterRead(mvme, base, grp, VF48_ACTIVE_CH_MASK);
  mvme_set_dmode(mvme, cmode);
  return val;
}

/********************************************************************/
/**
Suppress the transmission of the raw data (wave form) of all the channels
of a given group. Feature Q,T are always present expect if channel masked.
 */
int vf48_RawDataSuppSet(MVME_INTERFACE *mvme, DWORD base, int grp, DWORD size)
{
  int  cmode;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  vf48_ParameterWrite(mvme, base, grp, VF48_MBIT1, size);
  mvme_set_dmode(mvme, cmode);
  return grp;
}

/********************************************************************/
/**
Read the flag of the raw data suppression of a given channel.
 */
int vf48_RawDataSuppRead(MVME_INTERFACE *mvme, DWORD base, int grp)
{
  int  cmode, val;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  val =  vf48_ParameterRead(mvme, base, grp, VF48_MBIT1);
  mvme_set_dmode(mvme, cmode);
  return val;
}

/********************************************************************/
/**
Enable the suppression of channel based on the hit threshold for a
given group.
Currently the MBIT2 contains the divisor parameter too.
This function will overwrite this parameter. Apply this
function prior the divisor.
 */
int vf48_ChSuppSet(MVME_INTERFACE *mvme, DWORD base, int grp, DWORD value)
{
  int  cmode;
  int  mbit2;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  mbit2 =  vf48_ParameterRead(mvme, base, grp, VF48_MBIT2);
  mbit2 &= ~1;
  mbit2 |= value&0x1;
  vf48_ParameterWrite(mvme, base, grp, VF48_MBIT2, mbit2);
  mvme_set_dmode(mvme, cmode);
  return grp;
}

/********************************************************************/
/**
Read the channel suppression flag for a given group.
 */
int vf48_ChSuppRead(MVME_INTERFACE *mvme, DWORD base, int grp)
{
  int  cmode, val;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  val =  0x1 & vf48_ParameterRead(mvme, base, grp, VF48_MBIT2);
  mvme_set_dmode(mvme, cmode);
  return val;
}

/********************************************************************/
/**
write the sub-sampling divisor factor to all 6 groups.
Value of 0   : base sampling
Value of x>0 : base sampling / x
 */
int vf48_DivisorWrite(MVME_INTERFACE *mvme, DWORD base, DWORD value)
{
   int i, mbit2, grp;

  assert(value > 0);
  assert(value < 256);

  mvme_set_am(mvme, MVME_AM_A24);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  grp = vf48_GrpRead(mvme, base);
  for (i=0;i<6;i++)
    if (grp&(1<<i)) {
      mbit2 =  vf48_ParameterRead(mvme, base, i, VF48_MBIT2);
      mbit2 &= ~0xFF00;
      mbit2 |= (value << 8);
      vf48_ParameterWrite(mvme, base, i, VF48_MBIT2, mbit2);
    }
  for (i=0;i<6;i++)
    if (grp&(1<<i)) {
      int rd = vf48_DivisorRead(mvme, base, i);
      if (rd != value) {
        printf("vf48_DivisorWrite: Divisor read back mismatch: module at 0x%x, group %d, wrote %d, read back %d\n", base, i, value, rd);
        abort();
      }
    }
  return value;
}

/********************************************************************/
/**
Read the divisor parameter of the given group. All the groups
should read the same value.
 */
int vf48_DivisorRead(MVME_INTERFACE *mvme, DWORD base, int grp)
{
  int  cmode, val;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  val = vf48_ParameterRead(mvme, base, grp, VF48_MBIT2);
  val = (val >> 8) & 0xff;
  mvme_set_dmode(mvme, cmode);
  return val;
}

/********************************************************************/
int  vf48_Trigger(MVME_INTERFACE *mvme, DWORD base)
{
  vf48_RegisterWrite(mvme, base, VF48_SOFT_TRIG_W, 0);
  vf48_CsrRead(mvme, base); // flush posted PCI writes
  return VF48_SUCCESS;
}

/********************************************************************/
int vf48_Setup(MVME_INTERFACE *mvme, DWORD base, int mode)
{
  int  cmode, i;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);

  switch (mode) {
  case 0x1:
    printf("Default setting after power up (mode:%d)\n", mode);
    printf("\n");
    break;
  case 0x2:
    printf("External Trigger (mode:%d)\n", mode);
    vf48_ExtTrgSet(mvme, base);
    break;
  case 0x3:
    printf("External Trigger, Seg Size=16 (mode:%d)\n", mode);
    vf48_ExtTrgSet(mvme, base);
    for (i=0;i<6;i++)
       vf48_ParameterWrite(mvme, base, i, VF48_SEGMENT_SIZE, 16);
    for (i=0;i<6;i++)
      printf("SegSize grp %i:%d\n", i, vf48_ParameterRead(mvme, base, i, VF48_SEGMENT_SIZE));
    break;
  default:
    printf("Unknown setup mode\n");
    mvme_set_dmode(mvme, cmode);
    return -1;
  }
  mvme_set_dmode(mvme, cmode);
  return 0;

}

/********************************************************************/
/** vf48_Status
   @param mvme vme structure
   @param base  VF48 base address
   @return status VF48_ERROR, SUCCESS
*/
int vf48_Status(MVME_INTERFACE *mvme, DWORD base)
{
  char *parname[] = {
    "",
    "PED",
    "Hit Det Threshold",
    "Clip Delay",
    "PreTrigger",
    "Segment Size",
    "K",
    "L",
    "M",
    "Channel enable",
    "Mbits1",
    "Mbits2",
    "Latency",
    "Firmware ID",
    "Attenuator",
    "Trigger Threshold",
    "Firmware ID low",
    "Integ.delay",
    "PLL lock cnt",
    "Link word cnt",
    NULL
  };

  int cmode, i, j;
  DWORD fwrev;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);

  fwrev = vf48_RegisterRead(mvme, base, VF48_FIRMWARE_R);

  if (fwrev == 0xFFFFFFFF) {
     printf("VF48 at VME A24 0x%06x is not present\n", base);
     return VF48_ERR_HW;
  }

  printf("VF48 at VME A24 0x%06x, status:\n", base);
  printf("  CSR:          0x%08x\n", mvme_read_value(mvme, base + 0x0));
  printf("  Test reg:     0x%08x\n", mvme_read_value(mvme, base + 0x20));
  printf("  Firmware:     0x%08x\n", mvme_read_value(mvme, base + 0x30));
  printf("  Group enable: 0x%08x\n", mvme_read_value(mvme, base + 0x90));
  printf("  NbrFrames:    0x%08x\n", mvme_read_value(mvme, base + 0xA0));
  printf("  TrigConfA:    0x%08x\n", mvme_read_value(mvme, base + 0xC0));
  printf("  TrigConfB:    0x%08x\n", mvme_read_value(mvme, base + 0xD0));
  if ((fwrev&0xFF000000)==0x07000000) {
     printf("  LVDS links:   0x%08x%08x\n", mvme_read_value(mvme, base + 0x110), mvme_read_value(mvme, base + 0x100));
     printf("  FIFO 1,0:     0x%08x\n", mvme_read_value(mvme, base + 0x120));
     printf("  FIFO 3,2:     0x%08x\n", mvme_read_value(mvme, base + 0x130));
     printf("  FIFO 5,4:     0x%08x\n", mvme_read_value(mvme, base + 0x140));
     printf("  LVDS WC 1,0:  0x%08x\n", mvme_read_value(mvme, base + 0x1B0));
     printf("  FE_RT_IN 1,0: 0x%08x\n", mvme_read_value(mvme, base + 0x1C0));
     printf("  FE_RT_IN 3,2: 0x%08x\n", mvme_read_value(mvme, base + 0x1D0));
     printf("  FE_RT_IN 5,4: 0x%08x\n", mvme_read_value(mvme, base + 0x1E0));
     printf("  Trig Thresh:  0x%08x\n", mvme_read_value(mvme, base + 0x1F0));
     printf("  MST_RT_OUT:   0x%08x\n", mvme_read_value(mvme, base + 0x200));
  }

  if (mvme_read_value(mvme, base + 0x0) & 1) {
    printf("Module is in running mode: cannot read parameters\n");
  } else {
    printf("Parameters:\n");
    for (i=1; parname[i]!=NULL; i++) {
      printf("  par%02d (%-20s): ", i, parname[i]);
      for (j=0; j<6; j++) {
	int v = vf48_ParameterRead(mvme, base, j, i);
	printf(" %6d (0x%04x) ", v, v);
      }
      printf("\n");
    }
  }

  mvme_set_dmode(mvme, cmode);
  return VF48_SUCCESS;
}

/********************************************************************/
/** vf48_isPresent
   @param mvme vme structure
   @param base  VF48 base address
   @return status VF48_ERROR if module is not present, SUCCESS
*/
int vf48_isPresent(MVME_INTERFACE *mvme, DWORD base)
{
  int cmode;
  int missing = 0;

  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);

  missing |= (0xFFFFFFFF==mvme_read_value(mvme, base + 0x0));
  missing |= (0xFFFFFFFF==mvme_read_value(mvme, base + 0x30));

  mvme_set_dmode(mvme, cmode);

  if (missing)
    return VF48_ERROR;

  return VF48_SUCCESS;
}

/**** TIGC functions ****/

int  vf48_SetTigcSumEnable(MVME_INTERFACE *mvme, DWORD base, int value)
{
  int  cmode;
  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  mvme_write_value(mvme, base+TIGC_TRIGREG_SUMENABLE, value);
  mvme_set_dmode(mvme, cmode);
  return VF48_SUCCESS;
}
int  vf48_SetTigcTrigThreshold(MVME_INTERFACE *mvme, DWORD base, int value)
{
  int  cmode;
  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  mvme_write_value(mvme, base+TIGC_TRIGREG_THRESHOLD, value);
  mvme_set_dmode(mvme, cmode);
  return VF48_SUCCESS;
}
int  vf48_SetTigcDisableMask(MVME_INTERFACE *mvme, DWORD base, int value)
{
  int  cmode;
  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  mvme_write_value(mvme, base+TIGC_TRIGREG_DISABLEM, value);
  mvme_set_dmode(mvme, cmode);
  return VF48_SUCCESS;
}
int  vf48_SetTigcSumGains(MVME_INTERFACE *mvme, DWORD base, int value)
{
  int  cmode;
  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  mvme_write_value(mvme, base+TIGC_TRIGREG_SUMGAINS, value);
  mvme_set_dmode(mvme, cmode);
  return VF48_SUCCESS;
}
int vf48_TigcGenReset(MVME_INTERFACE *mvme, DWORD base)
{
  int  cmode;
  mvme_get_dmode(mvme, &cmode);
  mvme_set_dmode(mvme, MVME_DMODE_D32);
  mvme_write_value(mvme, base+TIGC_GENRESET, (int)0 );
  mvme_set_dmode(mvme, cmode);
  return VF48_SUCCESS;
}

/********************************************************************/
#ifdef MAIN_ENABLE
int main (int argc, char* argv[]) {

  MVME_INTERFACE *myvme;

  DWORD VF48_BASE = 0xAF0000;
  DWORD buf[200000], buf1[200000], buf2[200000];
  DWORD evt=0;
  int status, i, nframe, nframe1, nframe2, nentry, pnentry;
  int segsize = 10, latency=5, grpmsk = 0x3F, pretrig=0x20;
  int tthreshold = 10,hthreshold = 10, evtflag=0;

  // Test under vmic
  status = mvme_open(&myvme, 0);

  // Set am to A24 non-privileged Data
  mvme_set_am(myvme, MVME_AM_A24);

  // Set dmode to D32
  mvme_set_dmode(myvme, MVME_DMODE_D32);

#if 1

  if (argc > 1) {

    for (i=1;i<argc; i+=2) {
      if (argv[i][1] == 's') {
	segsize = atoi(argv[i+1]);
      }
      else if (argv[i][1] == 'l') {
	latency = atoi(argv[i+1]);
      }
      else if (argv[i][1] == 'p') {
	pretrig = atoi(argv[i+1]);
      }
      else if (argv[i][1] == 't') {
	tthreshold = atoi(argv[i+1]);
      }
      else if (argv[i][1] == 'u') {
	hthreshold = atoi(argv[i+1]);
      }
      else if (argv[i][1] == 'r') {
	i--;
	vf48_Reset(myvme, VF48_BASE);
      }
      else if (argv[i][1] == 'e') {
	evtflag = atoi(argv[i+1]);
      }
      else if (argv[i][1] == 'g') {
	grpmsk = strtol(argv[i+1], (char **)NULL, 16);
      }
      else if (argv[i][1] == 'h') {
	printf("\nvf48         : Run read only (no setup)\n");
	printf("vf48 -h      : This help\n");
	printf("vf48 -r      : Reset now\n");
	printf("vf48 -e val  : Event flag for event display, 0, x, 64\n");
	printf("vf48 -s val  : Setup Segment size value & Run [def:10]\n");
	printf("vf48 -l val  : Setup Latency value & Run [def:5]\n");
	printf("vf48 -g 0xval: Setup Group Enable pattern & Run [def:0x3F]\n");
	printf("vf48 -p val  : Setup PreTrigger & Run [def:0x20]\n");
	printf("vf48 -t val  : Setup Trig Threshold & Run [def:0x0A]\n");
	printf("vf48 -u val  : Setup Hit Threshold & Run [def:0x0A]\n");
	printf("Comments     : Setup requires Run stopped\n\n");
	return 0;
      }
    }

  // Stop DAQ for seting up the parameters
  vf48_AcqStop(myvme, VF48_BASE);

  // Segment Size
  printf("Set Segment size to all grp to %d\n", segsize);
  for (i=0;i<6;i++) {
    vf48_ParameterWrite(myvme, VF48_BASE, i, VF48_SEGMENT_SIZE, segsize);
    printf("SegSize grp %i:%d\n", i, vf48_ParameterRead(myvme, VF48_BASE, i, VF48_SEGMENT_SIZE));
  }

  // Latency
  printf("Set Latency to all grp to %d\n", latency);
  for (i=0;i<6;i++) {
    vf48_ParameterWrite(myvme, VF48_BASE, i, VF48_LATENCY, latency);
    printf("Latency grp %i:%d\n", i, vf48_ParameterRead(myvme, VF48_BASE, i, VF48_LATENCY));
  }

  // PreTrigger
  printf("Set PreTrigger to all grp to %d\n", pretrig);
  for (i=0;i<6;i++) {
    vf48_ParameterWrite(myvme, VF48_BASE, i, VF48_PRE_TRIGGER, pretrig);
    printf("Pre Trigger grp %i:%d\n", i, vf48_ParameterRead(myvme, VF48_BASE, i, VF48_PRE_TRIGGER));
  }

  // Trigger Threshold
  printf("Set Trigger Threshold to all grp to %d\n", tthreshold);
  for (i=0;i<6;i++) {
    vf48_ParameterWrite(myvme, VF48_BASE, i, VF48_TRIG_THRESHOLD, tthreshold);
    printf("Trigger Threshold grp %i:%d\n", i, vf48_ParameterRead(myvme, VF48_BASE, i, VF48_TRIG_THRESHOLD));
  }

  // Hit Threshold
  printf("Set Hit Threshold to all grp to %d\n", hthreshold);
  for (i=0;i<6;i++) {
    vf48_ParameterWrite(myvme, VF48_BASE, i, VF48_HIT_THRESHOLD, hthreshold);
    printf("Hit Threshold grp %i:%d\n", i, vf48_ParameterRead(myvme, VF48_BASE, i, VF48_HIT_THRESHOLD));
  }

  // Active Channel Mask
  for (i=0;i<6;i++)
    vf48_ParameterWrite(myvme, VF48_BASE, i, VF48_ACTIVE_CH_MASK, VF48_ALL_CHANNELS_ACTIVE);

  // Raw Data Suppress (Mbit 1)
  for (i=0;i<6;i++)
    vf48_ParameterWrite(myvme, VF48_BASE, i, VF48_MBIT1, VF48_RAW_DISABLE);

  // Inverse Signal (Mbit 1)
  for (i=0;i<6;i++)
    vf48_ParameterWrite(myvme, VF48_BASE, i, VF48_MBIT1, VF48_INVERSE_SIGNAL);

  // Channel Suppress (Mbit 2)
  for (i=0;i<6;i++)
    vf48_ParameterWrite(myvme, VF48_BASE, i, VF48_MBIT2, VF48_CH_SUPPRESS_ENABLE);

  // External trigger
  printf("External Trigger\n");
  vf48_ExtTrgSet(myvme, VF48_BASE);

  // Group Enable
  vf48_GrpEnable(myvme, VF48_BASE, grpmsk);
  printf("Group Enable :0x%x\n", vf48_GrpRead(myvme, VF48_BASE));

  // Start DAQ
  vf48_AcqStart(myvme, VF48_BASE);

  printf("Run Started : CSR Buffer: 0x%x\n", vf48_CsrRead(myvme, VF48_BASE));

  printf("Access mode:%d\n", evtflag);

  // Wait for a few data in buffer first
  do {
    // Read Frame counter
    nentry = vf48_NFrameRead(myvme, VF48_BASE);
  } while (nentry <= 0);
  printf("1 NFrame: 0x%x\n", nentry);
  }

#endif  // if 0/1

  if (evtflag == 0) {
    printf(" DataRead transfer\n");
    while (1) // (vf48_NFrameRead(myvme, VF48_BASE))
      {
	usleep(100000);
	nframe = vf48_NFrameRead(myvme, VF48_BASE);
	if (nframe) {
	  //  printf("Doing DMA read now\n");
	nframe = vf48_NFrameRead(myvme, VF48_BASE);
	vf48_DataRead(myvme, VF48_BASE, (DWORD *)&buf, &nframe);

	usleep(1000);

	nframe1 = vf48_NFrameRead(myvme, VF48_BASE);
	vf48_DataRead(myvme, VF48_BASE, (DWORD *)&buf1, &nframe1);
	usleep(1000);
	
	nframe2 = vf48_NFrameRead(myvme, VF48_BASE);
	vf48_DataRead(myvme, VF48_BASE, (DWORD *)&buf2, &nframe2);
	
	/*
	printf("CSR Buffer: 0x%x\n", vf48_CsrRead(myvme, VF48_BASE));
	printf("NFrame: buf %d  buf1 %d  buf2 %d\n", nframe, nframe1, nframe2);
	for (i=0;i<nframe+2;i++) {
	  printf("Buf   data[%d]: 0x%x\n", i, buf[i]);
	}
	for (i=0;i<nframe1+2;i++) {
	  printf("Buf1  data[%d]: 0x%x\n", i, buf1[i]);
	}
	for (i=0;i<nframe2+2;i++) {
	  printf("Buf2  data[%d]: 0x%x\n", i, buf2[i]);
	}
	*/
	printf("End of Data for %d %d %d\n", nframe, nframe1, nframe2);
      }
    }
  }

  if (evtflag == 64) {
    printf(" 64 bit transfer\n");
    while (1) {
      usleep(100000);
      nentry = 0;
      vf48_EventRead64(myvme, VF48_BASE, (DWORD *)&buf, &nentry);
      if (nentry != 0) {
	if (evt+1 != (buf[0] & 0xFFFFFF))
	  printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>--------------- Event# mismatch %x %x\n", evt, (buf[0] & 0xFFFFFF));

	evt = (buf[0] & 0xFFFFFF);

	for (i=0;i<nentry;i++) {
	  //    printf("buf[%d]:0x%x\n", i, buf[i]);
	  if ((buf[i] & 0xe0000000) == 0xe0000000 ||
	      (buf[i] & 0x10000BAD) == 0x10000BAD)
	  printf("\t\t\t\t\t\t\t\t %d buf[%d]:0x%x\n", nentry, i, buf[i]);
	}
	
	if (pnentry != nentry)
	  printf("End of Event previous(%d) (%d)\n", pnentry, nentry);
      else
	printf("End of Event              (%d) T(0x%x)\n", nentry, buf[nentry]);
	pnentry = nentry;
      }
    }
  }
  else {   // 32bit
    printf(" 32 bit transfer\n");
    while (1) {
      usleep(100000);
      nentry = 0;
      vf48_EventRead(myvme, VF48_BASE, (DWORD *)&buf, &nentry);
      if (nentry != 0) {
	if (evt+1 != (buf[0] & 0xFFFFFF))
	  printf("--------------- Event# mismatch %x %x\n", evt, (buf[0] & 0xFFFFFF));
	
	evt = (buf[0] & 0xFFFFFF);
	
	for (i=0;i<nentry;i++) {
	  if ((buf[i] & 0xe0000000) == 0xe0000000)
	  printf("\t\t\t\t\t\t\t\t  %d buf[%d]:0x%x\n", nentry, i, buf[i]);
	}
	
	if (pnentry != nentry)
	  printf("End of Event previous(%d) (%d)\n", pnentry, nentry);
	else
	  printf("End of Event              (%d) T(0x%x)\n", nentry, buf[nentry]);
	pnentry = nentry;
      }
    }
  }

  // Close VME
  status = mvme_close(myvme);
  return 1;
}
#endif

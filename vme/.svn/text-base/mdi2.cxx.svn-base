
int mdi2_RegRead16(MVME_INTERFACE* mvme, int base, int reg)
{
   mvme_set_am(mvme, MVME_AM_A24);
   mvme_set_dmode(mvme, MVME_DMODE_D16);
   return mvme_read_value(mvme, base + reg);
}

void mdi2_RegWrite16(MVME_INTERFACE* mvme, int base, int reg, int value)
{
   mvme_set_am(mvme, MVME_AM_A24);
   mvme_set_dmode(mvme, MVME_DMODE_D16);
   mvme_write_value(mvme, base + reg, value);
}

int mdi2_Status(MVME_INTERFACE* mvme, int base)
{
   printf("MDI2 at VME A24 0x%06x:\n", base);
   printf("  reg 0x6000: 0x%04x - address_source\n", mdi2_RegRead16(mvme, base, 0x6000));
   printf("  reg 0x6002: 0x%04x - address_reg\n",    mdi2_RegRead16(mvme, base, 0x6002));
   printf("  reg 0x6004: 0x%04x - module_id\n", mdi2_RegRead16(mvme, base, 0x6004));
   printf("  reg 0x6008: 0x%04x - soft_reset\n", mdi2_RegRead16(mvme, base, 0x6008));
   //printf("  reg 0x600E: 0x%04x - firmware_revision\n", mdi2_RegRead16(mvme, base, 0x600E));
   //printf("  reg 0x6010: 0x%04x - irq_level (0=OFF)\n", mdi2_RegRead16(mvme, base, 0x6010));
   //printf("  reg 0x601A: 0x%04x - max_transfer_data\n", mdi2_RegRead16(mvme, base, 0x601A));
   //printf("  reg 0x601C: 0x%04x - withdraw irq\n", mdi2_RegRead16(mvme, base, 0x601C));

   printf("  reg 0x6030: 0x%04x - buffer_data_length\n", mdi2_RegRead16(mvme, base, 0x6030));
   printf("  reg 0x6032: 0x%04x - data_len_format\n", mdi2_RegRead16(mvme, base, 0x6032));
   printf("  reg 0x6034: 0x%04x - readout_reset\n", mdi2_RegRead16(mvme, base, 0x6034));
   //printf("  reg 0x6036: 0x%04x - multievent\n", mdi2_RegRead16(mvme, base, 0x6036));
   printf("  reg 0x6038: 0x%04x - marking_type\n", mdi2_RegRead16(mvme, base, 0x6038));
   printf("  reg 0x603A: 0x%04x - start_acq\n", mdi2_RegRead16(mvme, base, 0x603A));
   printf("  reg 0x603C: 0x%04x - fifo_reset\n", mdi2_RegRead16(mvme, base, 0x603C));
   printf("  reg 0x603E: 0x%04x - data_ready\n", mdi2_RegRead16(mvme, base, 0x603E));

   printf("  reg 0x6040: 0x%04x - seq_enable\n", mdi2_RegRead16(mvme, base, 0x6040));
   printf("  reg 0x6042: 0x%04x - trig_source0\n", mdi2_RegRead16(mvme, base, 0x6042));
   printf("  reg 0x6044: 0x%04x - trig_source1\n", mdi2_RegRead16(mvme, base, 0x6044));
   printf("  reg 0x6046: 0x%04x - com_trig_source\n", mdi2_RegRead16(mvme, base, 0x6046));
   printf("  reg 0x6048: 0x%04x - gen_trigger\n", mdi2_RegRead16(mvme, base, 0x6048));
   printf("  reg 0x604A: 0x%04x - gen_event\n", mdi2_RegRead16(mvme, base, 0x604A));
   printf("  reg 0x604C: 0x%04x - veto_gate\n", mdi2_RegRead16(mvme, base, 0x604C));

   printf("  reg 0x6050: 0x%04x - hold_delay0\n", mdi2_RegRead16(mvme, base, 0x6050));
   printf("  reg 0x6052: 0x%04x - hold_delay1\n", mdi2_RegRead16(mvme, base, 0x6052));
   printf("  reg 0x6054: 0x%04x - hold_width0\n", mdi2_RegRead16(mvme, base, 0x6054));
   printf("  reg 0x6056: 0x%04x - hold_width1\n", mdi2_RegRead16(mvme, base, 0x6056));

   printf("  reg 0x6060: 0x%04x - bus_watchdog\n", mdi2_RegRead16(mvme, base, 0x6060));
   printf("  reg 0x6062: 0x%04x - frontend_reset\n", mdi2_RegRead16(mvme, base, 0x6062));
   printf("  reg 0x6064: 0x%04x - seq_clk_freq0\n", mdi2_RegRead16(mvme, base, 0x6064));
   printf("  reg 0x6066: 0x%04x - seq_clk_freq1\n", mdi2_RegRead16(mvme, base, 0x6066));
   printf("  reg 0x6068: 0x%04x - seq_busy\n", mdi2_RegRead16(mvme, base, 0x6068));
   printf("  reg 0x606A: 0x%04x - sample_delay_reg0\n", mdi2_RegRead16(mvme, base, 0x606A));
   printf("  reg 0x606C: 0x%04x - sample_delay_reg1\n", mdi2_RegRead16(mvme, base, 0x606C));
   printf("  reg 0x606E: 0x%04x - enable_busy\n", mdi2_RegRead16(mvme, base, 0x606E));

   //printf("  reg 0x6070: 0x%04x - seq0_mct\n", mdi2_RegRead16(mvme, base, 0x6070));
   //printf("  reg 0x6072: 0x%04x - seq1_mct\n", mdi2_RegRead16(mvme, base, 0x6072));
   printf("  reg 0x6074: 0x%04x - seq0_cct\n", mdi2_RegRead16(mvme, base, 0x6074));
   printf("  reg 0x6076: 0x%04x - seq1_cct\n", mdi2_RegRead16(mvme, base, 0x6076));
   printf("  reg 0x6078: 0x%04x - allow_sync_word\n", mdi2_RegRead16(mvme, base, 0x6078));

   printf("  reg 0x6090: 0x%04x - evctr_res\n", mdi2_RegRead16(mvme, base, 0x6090));
   printf("  reg 0x6092: 0x%04x - evctr_lo\n", mdi2_RegRead16(mvme, base, 0x6092));
   printf("  reg 0x6094: 0x%04x - evctr_hi\n", mdi2_RegRead16(mvme, base, 0x6094));
   //printf("  reg 0x6096: 0x%04x - ts_sources (0=VME clock)\n", mdi2_RegRead16(mvme, base, 0x6096));
   //printf("  reg 0x6098: 0x%04x - ts_divisor (1=no divisor)\n", mdi2_RegRead16(mvme, base, 0x6098));
   //printf("  reg 0x609C: 0x%04x - ts_counter_lo\n", mdi2_RegRead16(mvme, base, 0x609C));
   //printf("  reg 0x609E: 0x%04x - ts_counter_hi\n", mdi2_RegRead16(mvme, base, 0x609E));

   //printf("  reg 0x60A0: 0x%04x - adc_busy_time_lo\n", mdi2_RegRead16(mvme, base, 0x60A0));
   //printf("  reg 0x60A2: 0x%04x - adc_busy_time_hi\n", mdi2_RegRead16(mvme, base, 0x60A2));
   //printf("  reg 0x60A4: 0x%04x - gate1_time_lo\n", mdi2_RegRead16(mvme, base, 0x60A4));
   //printf("  reg 0x60A6: 0x%04x - gate1_time_hi\n", mdi2_RegRead16(mvme, base, 0x60A6));
   //printf("  reg 0x60A8: 0x%04x - time_0\n", mdi2_RegRead16(mvme, base, 0x60A8));
   //printf("  reg 0x60AA: 0x%04x - time_1\n", mdi2_RegRead16(mvme, base, 0x60AA));
   //printf("  reg 0x60AC: 0x%04x - time_2\n", mdi2_RegRead16(mvme, base, 0x60AC));
   //printf("  reg 0x60AE: 0x%04x - stop_ctr (0=run)\n", mdi2_RegRead16(mvme, base, 0x60AE));
   return 0;
}

void mdi2_Reset(MVME_INTERFACE* mvme, int base)
{
   mdi2_RegWrite16(mvme, base, 0x603A, 0); // stop acq
   mdi2_RegWrite16(mvme, base, 0x6008, 0); // soft reset
   mdi2_RegWrite16(mvme, base, 0x603A, 0); // stop acq
   mdi2_RegWrite16(mvme, base, 0x603C, 0); // fifo reset
   mdi2_RegWrite16(mvme, base, 0x6090, 3); // counters reset
}

void mdi2_Init(MVME_INTERFACE* mvme, int base)
{
   mdi2_RegWrite16(mvme, base, 0x6032, 2); // data len format in 32-bit words
   mdi2_RegWrite16(mvme, base, 0x6038, 0); // event footer contains 0=eventno, 1=timestamp

   mdi2_RegWrite16(mvme, base, 0x6040, 1); // enable sequencer: 2 bits
   mdi2_RegWrite16(mvme, base, 0x6042, 4); // seq0 trig source: common
   mdi2_RegWrite16(mvme, base, 0x6044, 0); // seq1 trig source: off
   mdi2_RegWrite16(mvme, base, 0x6046, 1); // com trig source

   mdi2_RegWrite16(mvme, base, 0x604C, 1); // "veto" input is the gate

   mdi2_RegWrite16(mvme, base, 0x6050, 1000); // hold delay
   mdi2_RegWrite16(mvme, base, 0x6052, 1000); // hold delay
   mdi2_RegWrite16(mvme, base, 0x6054, 34); // hold width
   mdi2_RegWrite16(mvme, base, 0x6056, 34); // hold width

   //mdi2_RegWrite16(mvme, base, 0x6060, 1); // bus watchdog
   mdi2_RegWrite16(mvme, base, 0x6060, 0); // bus watchdog

   mdi2_RegWrite16(mvme, base, 0x6064, 2); // clock freq
   mdi2_RegWrite16(mvme, base, 0x6066, 2); // clock freq
   //mdi2_RegWrite16(mvme, base, 0x6064, 3); // clock freq
   //mdi2_RegWrite16(mvme, base, 0x6066, 3); // clock freq

   mdi2_RegWrite16(mvme, base, 0x606A, 3); // sample delay
   mdi2_RegWrite16(mvme, base, 0x606C, 3); // sample delay

   mdi2_RegWrite16(mvme, base, 0x606E, 1); // enable busy output

   mdi2_RegWrite16(mvme, base, 0x6074, 136); // 17 counts per MTM16
   mdi2_RegWrite16(mvme, base, 0x6076, 136); // 17 counts per MTM16

   // zero out zero suppression thresholds
   for (int i=0; i<2*1024; i++)
      mdi2_RegWrite16(mvme, base, 0x4000 + i*2, 0); // 17 counts per MTM16
}

int mdi2_DataReady(MVME_INTERFACE* mvme, int base)
{
   return mdi2_RegRead16(mvme, base, 0x603E); // data_ready
}

void mdi2_SCX(MVME_INTERFACE* mvme, int base)
{
   int bus = 0;
   for (int addr=0; addr<15; addr++) {
      mdi2_RegWrite16(mvme, base, 0x6080, bus); // select bus 0 or 1
      mdi2_RegWrite16(mvme, base, 0x6082, addr); // MTM-16 module address
      mdi2_RegWrite16(mvme, base, 0x6084, 6);
      mdi2_RegWrite16(mvme, base, 0x6086, 0);
      mdi2_RegWrite16(mvme, base, 0x6088, 0);
    
      for (int c=0; c<100; c++) {
         int status = mdi2_RegRead16(mvme, base, 0x608A);
         int data = mdi2_RegRead16(mvme, base, 0x6088);
         printf("addr %d, status 0x%x, data 0x%x, count %d\n", addr, status, data, c);
         if ((status & 1) == 0)
            break;
      }
   }

   mdi2_RegWrite16(mvme, base, 0x6080, 0);
}

int mdi2_mtm16_read(MVME_INTERFACE* mvme, int base, int busno, int modnum, int opcode, int adr)
{
   mdi2_RegWrite16(mvme, base, 0x6080, busno); // select bus 0 or 1
   mdi2_RegWrite16(mvme, base, 0x6082, modnum); // MTM-16 module address
   mdi2_RegWrite16(mvme, base, 0x6084, opcode);
   mdi2_RegWrite16(mvme, base, 0x6086, adr);
   mdi2_RegWrite16(mvme, base, 0x6088, 0);

   double t = gettime();
    
   for (int c=0; c<1000; c++) {
      int status = mdi2_RegRead16(mvme, base, 0x608A);
      int data = mdi2_RegRead16(mvme, base, 0x6088);
      //printf("addr %d, status 0x%x, data 0x%x, count %d\n", adr, status, data, c);
      if ((status & 1) == 0) {
         printf("status 0x%04x, data 0x%04x, time %f sec\n", status, data, gettime() - t);
         return data;
      }
   }

   printf("timeout after %f sec\n", gettime() - t);

   return -1;
}

int mdi2_mtm16_write(MVME_INTERFACE* mvme, int base, int busno, int modnum, int opcode, int adr, int dat)
{
 retry:

   mdi2_RegWrite16(mvme, base, 0x6080, busno); // select bus 0 or 1
   mdi2_RegWrite16(mvme, base, 0x6082, modnum); // MTM-16 module address
   mdi2_RegWrite16(mvme, base, 0x6084, opcode);
   mdi2_RegWrite16(mvme, base, 0x6086, adr);
   mdi2_RegWrite16(mvme, base, 0x6088, dat);
    
   for (int c=0; c<1000; c++) {
      int status = mdi2_RegRead16(mvme, base, 0x608A);
      int data = mdi2_RegRead16(mvme, base, 0x6088);
      //printf("addr %d, status 0x%x, data 0x%x, count %d\n", adr, status, data, c);
      if ((status & 1) == 0) {
         printf("bus %d, mod %2d, opcode %2d, adr %d, status 0x%04x, write 0x%04x readback 0x%04x\n", busno, modnum, opcode, adr, status, dat, data);
         if (data != dat)
            goto retry;
         return data;
      }
   }

   return -1;
}

int mdi2_AcqStart(MVME_INTERFACE *mvme, int base)
{
   mdi2_RegWrite16(mvme, base, 0x603A, 1); // start acq
   return 0;
}


int mdi2_AcqStop(MVME_INTERFACE *mvme, int base)
{
   mdi2_RegWrite16(mvme, base, 0x603A, 0); // stop acq
   return 0;
}

/*-- Frontend Init -------------------------------------------------*/

#if 0
INT frontend_init()
{
   mdi2_Reset(gVme, gMdi2base);
   mdi2_Init(gVme, gMdi2base);
   mdi2_Status(gVme, gMdi2base);
   //mdi2_SCX(gVme, gMdi2base);
}
#endif

/*-- Event readout -------------------------------------------------*/

int read_mdi2(MVME_INTERFACE *mvme, int base, uint32_t* pdata32)
{
   if (!mdi2_DataReady(mvme, base))
      return 0;
   
   //mdi2_Status(gVme, base);
   
   //disable_trigger();
   //mdi2_Status(gVme, base);
   
   int nwords = mdi2_RegRead16(mvme, base, 0x6030);
   
   if (0)
      printf("nwords: %d\n", nwords);
   
   mvme_set_dmode(mvme, MVME_DMODE_D32);

   int wc = 0;
   
   for (int i=0; i<nwords; i++) {
      uint32_t w = mvme_read_value(mvme, base);
      *pdata32++ = w;
      wc++;
      
      if (0)
         printf("w %d: 0x%08x\n", i, w);
      
      if (w == 0xFFFFFFFF)
         break;
   }
   
   mdi2_RegWrite16(mvme, base, 0x6034, 0);
   
   //mdi2_Status(mvme, base);
   //exit(123);
   
   return wc;
}

//end

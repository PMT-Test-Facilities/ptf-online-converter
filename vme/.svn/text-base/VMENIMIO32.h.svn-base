//
// Name: VMENIMIO32.h
// Date:  Summer 2010
// Driver for the VME-NIMIO32 multifunction VME module
//
// $Id$
//
#ifndef IO32_H
#define IO32_H

#include <stdio.h>
#include <stdint.h>

// IO32 registers

#define IO32_REVISION (4*0)
#define IO32_CSR      (4*1)
#define IO32_COMMAND  (4*1) // command register
#define IO32_NIMOUT   (4*2) // NIM output
#define IO32_NIMIN    (4*3) // NIM input
#define IO32_USER     (4*4)
#define IO32_14       (4*5)
#define IO32_TS       (4*6) // 20 MHz timestamp
#define IO32_LVDSIN   (4*7) // LVDS/ECL input

#define IO32_TSC4_ROUTE (4*10) // TSC4 input routing - which of the 32 inputs are routed into the 4 inputs of TSC4, 5 bits per input
#define IO32_TSC4_FIFO  (4*11) // TSC4 data fifo
#define IO32_TSC4_CTRL  (4*12) // TSC4 control

#define IO32_SCALER_0 0x40 // there are 16 scalers, read IO32_TS before reading scalers

#define IO32_PULSER     (4*49) // programmable pulser, NIM output 2 function 2

#define IO32_TRIG_COUNT (4*53) // NIM_IN[1] trigger counter
#define IO32_TRIG_TS    (4*54) // NIM_IN[1] trigger timestamp

#define IO32_SCALERS_TS (4*55) // scalers latch timestamp

#define IO32_SCALER_STATUS       (4*60) // scaler FIFO status
#define IO32_SCALER_FIFO         (4*61) // scaler FIFO data
#define IO32_SCALER_DISABLE_MASK (4*62) // scaler channel disable mask
#define IO32_SCALER_LATCH_MASK   (4*63) // scaler self-latch channel enable mask

// IO32_COMMAND commands

#define IO32_CMD_RESET     1 // general reset
#define IO32_CMD_FPGA_RECONFIGURE 2 // FPGA reboot/reconfigure
#define IO32_CMD_RESET_TS  3 // reset timestamp counter
#define IO32_CMD_RESET_SCALERS 4 // reset scalers
#define IO32_CMD_LATCH_SCALERS 5 // latch scalers

//
// Interface for the VME NIMIO32 board
//

class VMENIMIO32
{
public: // public methods
  VMENIMIO32(void* vmeInterface, uint32_t vmeA32addr); // ctor

  int init(); ///< initialize the TTC board

  /// functions to read and write TTC registers
  
  uint32_t read32(int ireg); ///< read 32-bit register
  void     write32(int ireg, uint32_t data); ///< write 32-bit register
  uint32_t flip32(int ireg, uint32_t setBits, uint32_t clearBits, uint32_t invertBits); ///< set and clear bits in a 32-bit register, return old value

  /// misc functions

  uint32_t revision();
  int status();

  /// I/O functions

  int SetNimOutputFunction(int ioutput, int ifunc);
  void SetTsc4Input(int itsc4chan, int input);
  int NimOutput(int setBits, int clearBits);

  /// scaler functions

  int ReadScalers33(uint32_t* data); /// read 16 NIM scalers, 16 LVDS scalers and 1 scaler latch timestamp
  int ReadScaler(uint32_t* data, int maxwords); /// read the scalers, return number of words read

public: // private data
  void* fVme;
  uint32_t fVmeBaseAddr;
  int fDebug;

  uint32_t fRevision;
};

#endif

/* emacs
 * Local Variables:
 * mode:c++
 * tab-width: 8
 * c-basic-offset: 3
 * indent-tabs-mode: nil
 * End:
 */

// end file

// vmescan.cxx

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <mvmestd.h>

int mvme_read16(MVME_INTERFACE*vme,int addr)
{
  mvme_set_dmode(vme, MVME_DMODE_D16);
  return mvme_read_value(vme, addr);
}

int mvme_read32(MVME_INTERFACE*vme,int addr)
{
  mvme_set_dmode(vme, MVME_DMODE_D32);
  return mvme_read_value(vme, addr);
}

int probe(MVME_INTERFACE*vme,int addr,const char* name)
  {
    int data16 = mvme_read16(vme,addr);
    int data32 = mvme_read32(vme,addr);
    if (1 || data16 != 0xFFFF || data32 != 0xFFFFFFFF)
      {
	printf("at 0x%08x: data16: 0x%04x, data32: 0x%08x%s\n",addr,data16,data32,name);
	return 1;
      }
    return 0;
  }

int main(int argc,char*argv[])
{
  int status;
  MVME_INTERFACE *vme = 0;

  status = mvme_open(&vme,0);
  status = mvme_set_am(vme, MVME_AM_A32_ND);

  for (int ii=1; ii<argc; ii++)
    {
      int i = strtoul(argv[ii], NULL, 0);
      int ret = probe(vme,i,"");
      //if (!ret) ret = probe(vme,i+0xFC," (CAEN V513)");
      //if (!ret) ret = probe(vme,i+0x1C," (TRIUMF VMEIO)"); // probe TRIUMF VMEIO
      //if (!ret) ret = probe(vme,i+0x02," (CAEN V288)"); // probe CAEN V288
      //if (!ret) ret = probe(vme,i+0x100," (ALPHA TTC)"); // probe ALPHA TTC
      //if (!ret) ret = probe(vme,i+0x8200," (CAEN V1729)");
      sleep(1);
    }

  mvme_close(vme);

  return 0;
}

// end

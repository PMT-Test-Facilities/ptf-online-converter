//
// vt48_multi.h
//

#ifndef INCLUDE_vt48_multi
#define INCLUDE_vt48_multi

#include <stdio.h>
#include <stdint.h>

#define MAX_VT48 20

struct vt48_multi_buffer
{
   uint32_t vt48_addr[MAX_VT48];
   int      vt48_nbytes[MAX_VT48];
   char     bank_name[MAX_VT48][5];
   int      rb[MAX_VT48];
   int      vt48_last_word[MAX_VT48];
   uint32_t* vt48_bank[MAX_VT48];
};

int vt48_multi_init(MVME_INTERFACE*myvme, struct vt48_multi_buffer* buf, 
		    int i, uint32_t addr, const char* bank_name);

int vt48_multi_read(MVME_INTERFACE*myvme, struct vt48_multi_buffer* buf, 
		    int debug);

int vt48_multi_bank(struct vt48_multi_buffer* buf,
		    char* pevent, int pevent_size,
		    int one_event, int currentEvNum);

struct vt48_multi_thread_param
{
   MVME_INTERFACE *vme;
   struct vt48_multi_buffer *buf;
   int debug;
   int is_running;
   int stop;
};

int vt48_multi_thread(void* xparam);

#endif

/* emacs
 * Local Variables:
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */

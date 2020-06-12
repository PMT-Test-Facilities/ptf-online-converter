//
// vf48_multi.h
//

#ifndef INCLUDE_vf48_multi
#define INCLUDE_vf48_multi

#include <stdio.h>
#include <stdint.h>

#define MAX_VF48 20

struct vf48_multi_buffer
{
   uint32_t vf48_addr[MAX_VF48];
   int      vf48_nbytes[MAX_VF48];
   char     bank_name[MAX_VF48][5];
   int      rb[MAX_VF48];
   uint32_t  vf48_first_word[MAX_VF48];
   uint32_t  vf48_last_word[MAX_VF48];
   uint32_t* vf48_bank[MAX_VF48];
   int       vf48_count_reads[MAX_VF48];
   int       vf48_count_bytes[MAX_VF48];
};

int vf48_multi_init(MVME_INTERFACE*myvme, struct vf48_multi_buffer* buf, 
		    int i, uint32_t addr, const char* bank_name);

int vf48_multi_read(MVME_INTERFACE*myvme, struct vf48_multi_buffer* buf, 
		    int debug);

int vf48_multi_bank_stream(struct vf48_multi_buffer* buf,
			   char* pevent, int pevent_size);

int vf48_multi_bank_event(struct vf48_multi_buffer* buf,
			  char* pevent, int pevent_size,
			  int currentEvNum);

int vf48_read_event(MVME_INTERFACE* vme,
		    struct vf48_multi_buffer* buf,
		    char* pevent, int pevent_size,
		    int currentEvNum);

struct vf48_multi_thread_param
{
   MVME_INTERFACE *vme;
   struct vf48_multi_buffer *buf;
   int debug;
   int is_running;
   int stop;
};

int vf48_multi_thread(void* xparam);

#endif

/* emacs
 * Local Variables:
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */

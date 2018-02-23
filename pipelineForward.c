#include "pipeline.h"

/* Pipeline control unit. This implementation uses data forwarding to
   handle hazards, in addition to stalls when required. */
int pipelineControl(const int_reg_fd_t *f, const int_reg_de_t *d, const int_reg_em_t *e,
		    const int_reg_mw_t *m, const int_reg_ret_t *w,
		    int_reg_fetch_t *F, int_reg_fd_t *D, int_reg_de_t *E, int_reg_em_t *M,
		    int_reg_mw_t *W) {
  
  /* TO BE COMPLETED BY THE STUDENT */

  return w->status != ST_EXCEPTION && w->iCode != I_HALT;
}

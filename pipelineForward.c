#include "pipeline.h"

/* Pipeline control unit. This implementation uses data forwarding to
   handle hazards, in addition to stalls when required. */
int pipelineControl(const int_reg_fd_t *f, const int_reg_de_t *d, const int_reg_em_t *e,
		    const int_reg_mw_t *m, const int_reg_ret_t *w,
		    int_reg_fetch_t *F, int_reg_fd_t *D, int_reg_de_t *E, int_reg_em_t *M,
		    int_reg_mw_t *W) {

  F->PC = f->nextPC;
  F->status = ST_REGULAR;
  *D = *f;
  *E = *d;
  *M = *e;
  *W = *m;



  // do forwarding for valA
  if( d->srcA != R_NONE)
    {
      if( d->srcA == e->dstE )      
	E->valA = e->valE;
      else if( d->srcA == m->dstM )
	E->valA = m->valM;
      else if( d->srcA == m->dstE )
	E->valA = m->valE;
    }


  if( d->srcB != R_NONE )
    {
      // do forwarding for valB
      if( d->srcB == e->dstE )  
	E->valB = e->valE;
      else if( d->srcB == m->dstM )
	E->valB = m->valM;
      else if( d->srcB == m->dstE )
	E->valB = m->valE; 
    }

  static int bubble_position = -1;

  // if mrmov in E and E_dstM is d_srcA or d_srcB then one bubble
  if( d->iCode == I_MRMOVQ && (d->dstM == f->rA || d->dstM == f->rB )
   &&  bubble_position < 0 )
    {
      F->status = ST_STALLED;
      D->status = ST_STALLED;
      
      bubble_position = 2;
    }
   else if(bubble_position <= 3 && bubble_position >= 0 )
    {
      if(bubble_position == 0)
	{
	  F->status = ST_REGULAR;
	  D->status = ST_REGULAR;
	}
      else
	{
	  F->status = ST_STALLED;
	  D->status = ST_STALLED;
	}
      
      E->status = ST_BUBBLE;
      E->iCode = I_NOP;

      bubble_position--;     
    }
  else
    {
      F->status = ST_REGULAR;
      D->status = ST_REGULAR;
      E->status = ST_REGULAR;

      bubble_position--;
    }


  return w->status != ST_EXCEPTION && w->iCode != I_HALT;
}

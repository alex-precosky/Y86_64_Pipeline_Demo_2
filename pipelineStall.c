#include "pipeline.h"

/* Pipeline control unit. This implementation uses only stalls when
   required to avoid data and control hazards. */
int pipelineControl(const int_reg_fd_t *f, const int_reg_de_t *d, const int_reg_em_t *e,
		    const int_reg_mw_t *m, const int_reg_ret_t *w,
		    int_reg_fetch_t *F, int_reg_fd_t *D, int_reg_de_t *E, int_reg_em_t *M,
		    int_reg_mw_t *W) {
  
  static int bubble_position = -1; // 3=decode, 2=execute, 1=memory, 0=write back, -1=no bubble
  static int first_call = 1;

  
  if(((f->rA == d->dstE && f->rA != R_NONE)
    ||(f->rB == d->dstE && f->rB != R_NONE && f->iCode != I_IRMOVQ && f->iCode != I_RRMVXX)
    ||(f->rB == d->dstM && f->rB != R_NONE)
    ||(f->rA == d->srcB && d->iCode == I_OPQ)
    ||(f->rB == d->srcB && d->iCode == I_OPQ)
    ||(f->iCode == I_PUSHQ && d->iCode == I_PUSHQ)
    ||(f->iCode == I_POPQ && d->iCode == I_PUSHQ)
    ||(f->iCode == I_POPQ && d->iCode == I_POPQ)
    ||(f->iCode == I_PUSHQ && d->iCode == I_POPQ)
    ||(f->rA == d->dstM && f->rA != R_NONE)
    ||(f->iCode == I_CALL)
    )
     && bubble_position < 0 && first_call != 1)
    {

      F->status = ST_STALLED;
      F->PC = f->nextPC;

      *D = *f;
      D->status = ST_STALLED;

      *E = *d;

      *M = *e;
      *W = *m;

      bubble_position = 3;
    }
  else if((f->iCode == I_RET
	   ||(f->rB == e->dstE && f->rB != R_NONE && f->iCode != I_IRMOVQ && f->iCode != I_RRMVXX)
	   ||(f->rA == e->dstE && f->rA != R_NONE)
	   ||(f->rA == e->dstM && f->rA != R_NONE)
	   ||(f->rB == e->dstM && f->rB != R_NONE))
	  && bubble_position < 0)
    {

      F->status = ST_STALLED;
      F->PC = f->nextPC;

      *D = *f;
      D->status = ST_STALLED;

      *E = *d;

      *M = *e;
      *W = *m;

      bubble_position = 2;      
    }
  else if(((f->rB==m->dstE && f->rB != R_NONE && f->iCode != I_IRMOVQ && f->iCode != I_RRMVXX)
	   ||(f->rA == m->dstE && f->rA != R_NONE)
	   ||(f->rA == m->dstM && f->rA != R_NONE)
	   ||(f->rB == m->dstM && f->rB != R_NONE))
	  && bubble_position < 0)
    {

      F->status = ST_STALLED;
      F->PC = f->nextPC;

      *D = *f;
      D->status = ST_STALLED;

      *E = *d;

      *M = *e;
      *W = *m;

      bubble_position = 1;
    }
  else if(bubble_position <= 3 && bubble_position >= 0 )
    {

      if(bubble_position == 0)
	{
	  F->PC = f->nextPC;
	  *D = *f;
	  F->status = ST_REGULAR;
	  D->status = ST_REGULAR;
	}
      else
	{
	  F->status = ST_STALLED;
	  D->status = ST_STALLED;
	}



      
      *E = *d;
      E->status = ST_BUBBLE;
      E->iCode = I_NOP;


      *M = *e;
      *W = *m;

      bubble_position--;     
     
    }
  
  else
    {
      F->PC = f->nextPC;
      F->status = ST_REGULAR;
      *D = *f;
      D->status = ST_REGULAR;
      *E = *d;
      E->status = ST_REGULAR;
      *M = *e;
      *W = *m;

      bubble_position--;

    }

  first_call = 0;
  return w->status != ST_EXCEPTION && w->iCode != I_HALT;
}

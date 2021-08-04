#ifndef _GD32_INCLUDES_H_
#define _GD32_INCLUDES_H_



/****************************************************************************/
/* GD32F10x Familiy                                                         */
/****************************************************************************/
#if defined(GD32F10x)
  #include "gd32f10x.h"

  #if defined(GD32103C_EVAL)
    #include "gd32f103c_eval.h"
  #elif defined(GD32103E_EVAL)
    #include "gd32f103e_eval.h"
  #elif defined(GD32107C_EVAL)
    #include "gd32f107c_eval.h"
  #elif defined(GD32103B_EVAL)
    #include "gd32f103b_eval.h"
  #endif

/****************************************************************************/
/* GD32F1x0 Familiy                                                         */
/****************************************************************************/
#elif defined(GD32F1x0)
  #include "gd32f1x0.h"

  #if defined(GD32130C_START)
    #include "gd32f130c_start.h"
  #elif defined(GD32130F_START)
    #include "gd32f130f_start.h"
  #elif defined(GD32130G_START)
    #include "gd32f130g_start.h"
  #elif defined(GD32130K_START)
    #include "gd32f130k_start.h"
  #elif defined(GD32150C_START)
    #include "gd32f150c_start.h"
  #elif defined(GD32150G_START)
    #include "gd32f150g_start.h"
  #elif defined(GD32150R_EVAL)
    #include "gd32f150r_eval.h"
  #elif defined(GD32170C_START)
    #include "gd32f170c_start.h"
  #elif defined(GD32190R_EVAL)
    #include "gd32f190r_eval.h"
  #endif

/****************************************************************************/
/* GD32F20x Familiy                                                         */
/****************************************************************************/
#elif defined(GD32F20x)
  #include "gd32f20x.h"

  #if defined(GD32205R_START)
    #include "gd32f205r_start.h"
  #elif defined(GD32207C_EVAL)
    #include "gd32f207c_eval.h"
  #elif defined(GD32207I_EVAL)
    #include "gd32f207i_eval.h"
  #endif

/****************************************************************************/
/* GD32F30x Familiy                                                         */
/****************************************************************************/
#elif defined(GD32F30x)
  #include "gd32f30x.h"

  #if defined(GD32303C_EVAL)
    #include "gd32f303c_eval.h"
  #elif defined(GD32307E_EVAL)
    #include "gd32f307e_eval.h"
  #elif defined(GD32307C_EVAL)
    #include "gd32f307c_eval.h"
  #elif defined(GD32303C_START)
    #include "gd32f303c_start.h"
  #elif defined(GD32303E_EVAL)
    #include "gd32f303e_eval.h"
  #elif defined(GD32305R_START)
    #include "gd32f305r_start.h"
  #endif

/****************************************************************************/
/* GD32F3x0 Familiy                                                         */
/****************************************************************************/
#elif defined(GD32F3x0)
  #include "gd32f3x0.h"

  #if defined(GD32330C_START)
    #include "gd32f330c_start.h"
  #elif defined(GD32330F_START)
    #include "gd32f330f_start.h"
  #elif defined(GD32330G_START)
    #include "gd32f330g_start.h"
  #elif defined(GD32350C_START)
    #include "gd32f350c_start.h"
  #elif defined(GD32350G_START)
    #include "gd32f350g_start.h"
  #elif defined(GD32350R_EVAL)
    #include "gd32f350r_eval.h"
  #endif

/****************************************************************************/
/* GD32F4x0 Familiy                                                         */
/****************************************************************************/
#elif defined(GD32F40x)
  #include "gd32f4xx.h"

  #if defined(GD32403Z_EVAL)
    #include "gd32f403z_eval.h"
  #elif defined(GD32407R_START)
    #include "gd32f407r_start.h"
  #elif defined(GD32407V_START)
    #include "gd32f407v_start.h"
  #elif defined(GD32450I_EVAL)
    #include "gd32f450i_eval.h"
  #elif defined(GD32450Z_EVAL)
    #include "gd32f450z_eval.h"
  #endif

/****************************************************************************/
/* GD32E10x Familiy                                                         */
/****************************************************************************/
#elif defined(GD32E10X)
  #include "gd32e10x.h"

  #if defined(GD32E103C_START)
    #include "gd32e103c_start.h"
  #elif defined(GD32E103R_START)
    #include "gd32e103r_start.h"
  #elif defined(GD32E103T_START)
    #include "gd32e103t_start.h"
  #elif defined(GD32E103V_EVAL)
    #include "gd32e103v_eval.h"
  #endif

/****************************************************************************/
/* GD32E20x Familiy                                                         */
/****************************************************************************/
#elif defined(GD32E20X)
  #include "gd32e20x.h"

  #if defined(GD32E230C_EVAL)
    #include "gd32e230c_eval.h"
  #elif defined(GD32E230C_START)
    #include "gd32e230c_start.h"
  #elif defined(GD32E230F_START)
    #include "gd32e230f_start.h"
  #elif defined(GD32E230G_START)
    #include "gd32e230g_start.h"
  #elif defined(GD32E230K_START)
    #include "gd32e230k_start.h"
  #elif defined(GD32E230V_START)
    #include "gd32e230v_start.h"
  #elif defined(GD32E231C_START)
    #include "gd32e231c_start.h"
  #endif

/****************************************************************************/
/* GD32E50x Familiy                                                         */
/****************************************************************************/
#elif defined(GD32E50X)
  #include "gd32e50x.h"

  #if defined(GD32E503C_START)
    #include "gd32e503c_start.h"
  #elif defined(GD32E503R_START)
    #include "gd32e503r_start.h"
  #elif defined(GD32E503V_EVAL)
    #include "gd32e503v_eval.h"
  #elif defined(GD32E503Z_EVAL)
    #include "gd32e503z_eval.h"
  #elif defined(GD32E507R_START)
    #include "gd32e507r_start.h"
  #elif defined(GD32E507V_START)
    #include "gd32e507v_start.h"
  #elif defined(GD32E507Z_EVAL)
    #include "gd32e507z_eval.h"
  #elif defined(GD32EPRTV_START)
    #include "gd32eprtv_start.h"
  #endif

#endif

#endif /* #ifndef _GD32_INCLUDES_H_ */

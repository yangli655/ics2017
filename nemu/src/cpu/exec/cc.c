#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:
      break;
    case CC_B:
      
      break;
    case CC_E:
      rtl_get_ZF(&t0);
      rtl_neq0(&t1,&t0);
      rtl_mv(dest,&t1);
      break;
    case CC_BE:
      rtl_get_CF(&t0);
      rtl_get_ZF(&t1);
      rtl_or(&t0,&t0,&t1);
      rtl_neq0(&t1,&t0);
      rtl_mv(dest,&t1);
      break;
    case CC_S:
      break;
    case CC_L:
      rtl_get_SF(&t3);
      rtl_get_OF(&t1);
      rtl_xor(&t3,&t1,&t3);
      rtl_eq0(&t3,&t3);
      rtl_mv(dest,&t3);
      break;
    case CC_LE:
      rtl_get_ZF(&t0);
      rtl_neq0(&t0,&t0);
      rtl_get_SF(&t1);
      rtl_get_OF(&t3);
      rtl_xor(&t1,&t1,&t3);
      rtl_eq0(&t1,&t0);
      rtl_mv(dest,&t1);
      break;
    case CC_P: panic("n86 does not have PF");
      break;
    case CC_NP:
      rtl_get_SF(&t1);
      rtl_get_OF(&t3);
      rtl_xor(&t1,&t1,&t3);
      rtl_and(&t0,&t0,&t1);
      rtl_mv(dest,&t0);
      break;
    default: panic("should not reach here");
      break;
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}

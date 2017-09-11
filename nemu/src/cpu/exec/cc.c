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
      break;
    case CC_S:
      break;
    case CC_L:
      break;
    case CC_LE:
    rtl_get_ZF(&t0);
    printf("%d\t",t0);
    rtl_neq0(&t0,&t0);
    printf("%d\n",t0);
    rtl_get_SF(&t1);
    rtl_get_OF(&t2);
    rtl_xor(&t1,&t1,&t2);
    rtl_and(&t0,&t0,&t1);
    rtl_mv(dest,&t0);
      break;

      //TODO();
    default: panic("should not reach here");
      break;
    case CC_P: panic("n86 does not have PF");
      break;
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}

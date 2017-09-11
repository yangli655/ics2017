#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

make_EHelper(call);
make_EHelper(ret);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(add);
make_EHelper(adc);
make_EHelper(sub);
make_EHelper(sbb);
make_EHelper(dec);
make_EHelper(and);
make_EHelper(xor);
make_EHelper(or);
make_EHelper(test);
make_EHelper(cmp);
make_EHelper(lea);
make_EHelper(xchg);
make_EHelper(movzx);
make_EHelper(setcc);
make_EHelper(jcc);
make_EHelper(sar);
make_EHelper(shl);

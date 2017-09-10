#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
  if (id_dest->type == OP_TYPE_MEM) {
    rtl_li(&t1,id_dest->val);
    rtl_sext(&t1,&t1,id_dest->width);
    rtl_push(&t1);
  }
  else {
    rtl_push(&reg_l(id_dest->reg));
  }

  print_asm_template1(push);
}

make_EHelper(pop) {
  //TODO();
  rtl_pop(&reg_l(id_dest->reg));
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  TODO();

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}

make_EHelper(xchg) {
  //TODO()
  if (id_dest->width == 2) {
    rtl_lr_w(&t0,id_dest->reg);
    rtl_lr_w(&t1,R_AX);
    rtl_sr_w(R_AX,&t0);
    rtl_sr_w(id_dest->reg,&t1);
    print_asm(str(xchg) "%c %%ax,%s", suffix_char(id_dest->width), id_dest->str);
  }
  else {
    rtl_lr_l(&t0,id_dest->reg);
    rtl_lr_l(&t1,R_EAX);
    rtl_sr_l(R_EAX,&t0);
    rtl_sr_l(id_dest->reg,&t1);
    print_asm(str(xchg) "%c %%eax,%s", suffix_char(id_dest->width), id_dest->str);
  }
}

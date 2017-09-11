#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
  if (id_dest->type == OP_TYPE_MEM) {
    rtl_li(&t3,id_dest->val);
    rtl_sext(&t3,&t3,id_dest->width);
    rtl_push(&t3);
  }
  else if (id_dest->type == OP_TYPE_IMM){
    rtl_li(&t3,id_dest->val);
    rtl_push(&t3);
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
  //TODO();
  rtl_lr_l(&t3,R_ESP);
  rtl_lr_l(&t2,R_EBP);
  rtl_sr_l(R_ESP,&t2);
  rtl_sr_l(R_EBP,&t3);
  rtl_pop(&reg_l(R_EBP));

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    //TODO();
    if (reg_w(R_AX)<0) {
      rtl_li(&t3,0xffff);
      rtl_sr(R_DX,2,&t3);
    }
    else {
      rtl_li(&t3,0x0000);
      rtl_sr(R_DX,2,&t3);
    }
  }
  else {
    //TODO();
    if (reg_l(R_EAX)<0) {
      rtl_li(&t3,0xffffffff);
      rtl_sr(R_EDX,4,&t3);
    }
    else {
      rtl_li(&t3,0x00000000);
      rtl_sr(R_EDX,4,&t3);
    }
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
  rtl_zext(&t3,&id_src->val,2);
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(movzb) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_zext(&t3,&id_src->val,1);
  operand_write(id_dest, &t3);
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
    rtl_lr_w(&t3,id_dest->reg);
    rtl_lr_w(&t2,R_AX);
    rtl_sr_w(R_AX,&t3);
    rtl_sr_w(id_dest->reg,&t2);
    print_asm(str(xchg) "%c %%ax,%s", suffix_char(id_dest->width), id_dest->str);
  }
  else {
    rtl_lr_l(&t3,id_dest->reg);
    rtl_lr_l(&t2,R_EAX);
    rtl_sr_l(R_EAX,&t3);
    rtl_sr_l(id_dest->reg,&t2);
    print_asm(str(xchg) "%c %%eax,%s", suffix_char(id_dest->width), id_dest->str);
  }
}

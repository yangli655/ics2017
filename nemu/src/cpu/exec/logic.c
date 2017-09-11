#include "cpu/exec.h"

make_EHelper(test) {
  //TODO();
  if (id_dest->type == OP_TYPE_MEM) {
    rtl_lr(&t2,id_src->reg,id_src->width);
    rtl_andi(&t2,&t2,id_dest->val);
  }
  else {
    rtl_lr(&t2,id_dest->reg,id_dest->width);
    rtl_lr(&t1,id_src->reg,id_src->width);
    rtl_and(&t2,&t1,&t2);
  }
  rtl_li(&t3,0);
  rtl_set_OF(&t3);
  rtl_set_CF(&t3);
  rtl_update_ZFSF(&t2,id_dest->width);

  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  rtl_li(&t1,id_src->val);
  rtl_sext(&t1,&t1,id_src->width);
  rtl_li(&t3,0);
  rtl_set_CF(&t3);
  rtl_set_OF(&t3);
  rtl_update_ZFSF(&reg_l(id_dest->reg),id_dest->width);
  rtl_and(&reg_l(id_dest->reg),&reg_l(id_dest->reg),&t1);

  print_asm_template2(and);
}

make_EHelper(xor) {
  //TODO();
  rtl_xor(&reg_l(id_dest->reg),&reg_l(id_dest->reg),&reg_l(id_src->reg));
  rtl_li(&t3,0);
  rtl_set_OF(&t3);
  rtl_set_CF(&t3);
  rtl_update_ZFSF(&reg_l(id_dest->reg),id_dest->width);

  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  rtl_or(&reg_l(id_dest->reg),&reg_l(id_dest->reg),&reg_l(id_src->reg));
  rtl_li(&t3,0);
  rtl_set_OF(&t3);
  rtl_set_CF(&t3);
  rtl_update_ZFSF(&reg_l(id_dest->reg),id_dest->width);

  print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_lr(&t3,id_dest->reg,id_dest->width);
  rtl_sari(&t3,&t3,id_src->val);
  rtl_sr(id_dest->reg,id_dest->width,&t3);
  rtl_update_ZFSF(&t3,id_dest->width);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  if (id_src->val == 0){
    rtl_lr(&t3,id_dest->reg,id_dest->width);
    rtl_update_ZFSF(&t3,id_dest->width);
    rtl_li(&t2,1);
    rtl_set_ZF(&t2);
  }
  else {
    rtl_lr(&t3,id_dest->reg,id_dest->width);
    rtl_shli(&t3,&t3,id_src->val);
    rtl_sr(id_dest->reg,id_dest->width,&t3);
    rtl_update_ZFSF(&t3,id_dest->width);
  }

  print_asm_template2(shl);
}

make_EHelper(shr) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t3, subcode);
  operand_write(id_dest, &t3);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  TODO();
 
  print_asm_template1(not);
}

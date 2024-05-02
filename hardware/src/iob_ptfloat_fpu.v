`timescale 1ns / 1ps

`include "iob_ptfloat_defs.vh"

module iob_ptfloat_fpu
  #(
    parameter DATA_W = 32,
    parameter EW_W   = 4
    )
   (
    input                 clk_i,
    input                 arst_i,
    input                 cke_i,

    input [`OPCODE_W-1:0] op_i,
    input                 start_i,
    output                done_o,

    input [DATA_W-1:0]    op1_i,
    input [DATA_W-1:0]    op2_i,

    output [DATA_W-1:0]   data_o,
    output                overflow_o,
    output                underflow_o,
    output                div_by_zero_o
    );

   wire [`OPCODE_W-1:0]   op_reg;
   iob_reg
     #(
       .DATA_W  (`OPCODE_W),
       .RST_VAL ({`OPCODE_W{1'b0}})
       )
   reg_op
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (op_i),
      .data_o (op_reg)
      );

   wire                   done_unpack_op1;
   wire [`EXP_MAX_W-1:0]  exp_op1;
   wire [`MAN_MAX_W-1:0]  man_op1;
   iob_ptfloat_unpack
     #(
       .DATA_W (DATA_W),
       .EW_W   (EW_W)
       )
   unpack_op1
     (
      .clk_i   (clk_i),
      .arst_i  (arst_i),
      .cke_i   (cke_i),

      .start_i (start_i),
      .done_o  (done_unpack_op1),

      .data_i  (op1_i),

      .exp_o   (exp_op1),
      .man_o   (man_op1)
      );

   wire                   done_unpack_op2;
   wire [`EXP_MAX_W-1:0]  exp_op2;
   wire [`MAN_MAX_W-1:0]  man_op2;
   iob_ptfloat_unpack
     #(
       .DATA_W (DATA_W),
       .EW_W   (EW_W)
       )
   unpack_op2
     (
      .clk_i   (clk_i),
      .arst_i  (arst_i),
      .cke_i   (cke_i),

      .start_i (start_i),
      .done_o  (done_unpack_op2),

      .data_i  (op2_i),

      .exp_o   (exp_op2),
      .man_o   (man_op2)
      );

   wire                   done_addsub;
   wire                   start_addsub;
   assign start_addsub = done_unpack_op1 & done_unpack_op2 & ~op_reg[1];

   wire [`EXP_MAX_W+1:0]  exp_res_addsub;
   wire [`RES_MAX_W-1:0]  man_res_addsub;
   iob_ptfloat_addsub
     #(
       .DATA_W (DATA_W),
       .EW_W   (EW_W)
       )
   addsub
     (
      .clk_i   (clk_i),
      .arst_i  (arst_i),
      .cke_i   (cke_i),

      .op_i    (op_reg[0]),
      .start_i (start_addsub),
      .done_o  (done_addsub),

      .exp_a_i (exp_op1),
      .man_a_i (man_op1),

      .exp_b_i (exp_op2),
      .man_b_i (man_op2),

      .exp_o   (exp_res_addsub),
      .man_o   (man_res_addsub)
      );

   wire                   done_mul;
   wire                   start_mul;
   assign start_mul = done_unpack_op1 & done_unpack_op2 & op_reg[1] & ~op_reg[0];

   wire [`EXP_MAX_W+1:0]  exp_res_mul;
   wire [`RES_MAX_W-1:0]  man_res_mul;
   iob_ptfloat_mul
     #(
       .DATA_W (DATA_W),
       .EW_W   (EW_W)
       )
   mul
     (
      .clk_i   (clk_i),
      .arst_i  (arst_i),
      .cke_i   (cke_i),

      .start_i (start_mul),
      .done_o  (done_mul),

      .exp_a_i (exp_op1),
      .man_a_i (man_op1),

      .exp_b_i (exp_op2),
      .man_b_i (man_op2),

      .exp_o   (exp_res_mul),
      .man_o   (man_res_mul)
      );

   wire                   done_div;
   wire                   start_div;
   assign start_div = done_unpack_op1 & done_unpack_op2 & op_reg[1] & op_reg[0];

   wire [`EXP_MAX_W+1:0]  exp_res_div;
   wire [`RES_MAX_W-1:0]  man_res_div;
   iob_ptfloat_div
     #(
       .DATA_W (DATA_W),
       .EW_W   (EW_W)
       )
   div
     (
      .clk_i         (clk_i),
      .arst_i        (arst_i),
      .cke_i         (cke_i),

      .start_i       (start_div),
      .done_o        (done_div),

      .exp_a_i       (exp_op1),
      .man_a_i       (man_op1),

      .exp_b_i       (exp_op2),
      .man_b_i       (man_op2),

      .exp_o         (exp_res_div),
      .man_o         (man_res_div),

      .div_by_zero_o (div_by_zero_o)
      );

   wire [`EXP_MAX_W+1:0]  exp_res;
   wire [`RES_MAX_W-1:0]  man_res;
   wire                   done_op;
   assign exp_res = done_addsub? exp_res_addsub:
                       done_mul? exp_res_mul:
                                 exp_res_div;
   assign man_res = done_addsub? man_res_addsub:
                       done_mul? man_res_mul:
                                 man_res_div;
   assign done_op = done_addsub | done_mul | done_div;

   wire                   overflow;
   wire                   underflow;
   iob_ptfloat_pack
     #(
       .DATA_W (DATA_W),
       .EW_W   (EW_W)
       )
   pack
     (
      .clk_i       (clk_i),
      .arst_i      (arst_i),
      .cke_i       (cke_i),

      .start_i     (done_op),
      .done_o      (done_o),

      .exp_i       (exp_res),
      .man_i       (man_res),

      .data_o      (data_o),
      .overflow_o  (overflow),
      .underflow_o (underflow)
      );

   assign overflow_o = overflow & done_o;
   assign underflow_o = underflow & done_o;

endmodule

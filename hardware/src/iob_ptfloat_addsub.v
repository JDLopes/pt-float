`timescale 1ns / 1ps

`include "iob_ptfloat_defs.vh"

module iob_ptfloat_addsub
  #(
    parameter DATA_W = 32,
    parameter EW_W   = 4
    )
   (
    input                         clk_i,
    input                         arst_i,
    input                         cke_i,

    input                         op_i,
    input                         start_i,
    output                        done_o,

    input signed [`EXP_MAX_W-1:0] exp_a_i,
    input signed [`MAN_MAX_W-1:0] man_a_i,

    input signed [`EXP_MAX_W-1:0] exp_b_i,
    input signed [`MAN_MAX_W-1:0] man_b_i,

    output [`EXP_MAX_W+1:0]       exp_o,
    output [`RES_MAX_W-1:0]       man_o
    );

   wire                           done;
   assign done = start_i;

   wire                           cmp_exp;
   assign cmp_exp = (exp_a_i >= exp_b_i)? 1'b1: 1'b0;

   wire [`EXP_MAX_W+1:0]          exp_big, exp_small, exp_diff;
   assign exp_big   = cmp_exp? {{2{exp_a_i[`EXP_MAX_W-1]}}, exp_a_i}: {{2{exp_b_i[`EXP_MAX_W-1]}}, exp_b_i};
   assign exp_small = cmp_exp? {{2{exp_b_i[`EXP_MAX_W-1]}}, exp_b_i}: {{2{exp_a_i[`EXP_MAX_W-1]}}, exp_a_i};
   assign exp_diff  = exp_big - exp_small;

   wire signed [`RES_MAX_W-1:0]   man_a_int, man_b_int;
`ifndef ROUNDING
   assign man_a_int = cmp_exp? man_a_i: man_a_i >>> exp_diff;
   assign man_b_int = cmp_exp? man_b_i >>> exp_diff: man_b_i;
`else
   wire signed [`RES_MAX_W-1:0]   man_a_i_, man_b_i_;
   assign man_a_i_ = {man_a_i, {(`RES_MAX_W-`MAN_MAX_W){1'b0}}};
   assign man_b_i_ = {man_b_i, {(`RES_MAX_W-`MAN_MAX_W){1'b0}}};

   assign man_a_int = cmp_exp? man_a_i_: man_a_i_ >>> exp_diff;
   assign man_b_int = cmp_exp? man_b_i_ >>> exp_diff: man_b_i_;

   wire                           sticky;
   wire [`RES_MAX_W-1:0]          man_op_shift;

   assign man_op_shift = cmp_exp? man_b_i_: man_a_i_;

   iob_sticky
     #(
       .DATA_W(`RES_MAX_W),
       .SHIFT_W(`EXP_MAX_W+2)
       )
   sticky_norm
     (
      .man_i    (man_op_shift),
      .shift_i  (exp_diff),
      .sticky_o (sticky)
      );
`endif

   // pipeline stage 1
   wire [`EXP_MAX_W+1:0]          exp_big_reg;
   iob_reg
     #(
       .DATA_W  (`EXP_MAX_W+2),
       .RST_VAL ({(`EXP_MAX_W+2){1'b0}})
       )
   reg_exp_big0
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (exp_big),
      .data_o (exp_big_reg)
      );

   wire [`RES_MAX_W-1:0]          man_a_reg;
   iob_reg
     #(
       .DATA_W  (`RES_MAX_W),
       .RST_VAL ({`RES_MAX_W{1'b0}})
       )
   reg_man_a
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (man_a_int),
      .data_o (man_a_reg)
      );

   wire [`RES_MAX_W-1:0]          man_b_reg;
   iob_reg
     #(
       .DATA_W  (`RES_MAX_W),
       .RST_VAL ({`RES_MAX_W{1'b0}})
       )
   reg_man_b
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (man_b_int),
      .data_o (man_b_reg)
      );

   wire                           op_reg;
   iob_reg
     #(
       .DATA_W  (1),
       .RST_VAL (1'b0)
       )
   reg_op
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (op_i),
      .data_o (op_reg)
      );

`ifdef ROUNDING
   wire                           sticky_reg;
   iob_reg
     #(
       .DATA_W  (1),
       .RST_VAL (1'b0)
       )
   reg_sticky
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (sticky),
      .data_o (sticky_reg)
      );
`endif

   wire                           done_reg;
   iob_reg
     #(
       .DATA_W  (1),
       .RST_VAL (1'b0)
       )
   reg_done0
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (done),
      .data_o (done_reg)
      );

   wire [`RES_MAX_W-1:0]          man_int;
   wire                           carry;
   wire                           shift;
   iob_addsub
     #(`RES_MAX_W)
   addsub
     (
      .op_a     (man_a_reg),
      .op_b     (man_b_reg),
      .op       (op_reg),
      .res      (man_int),
      .carry    (carry),
      .overflow (shift)
      );

   wire [`RES_MAX_W-1:0]          man;
`ifndef ROUNDING
   assign man = man_int;
`else
   assign man = {man_int[`RES_MAX_W-1:1], man_int[0] | sticky_reg};
`endif

   // pipeline stage 2
   wire [`EXP_MAX_W+1:0]          exp_big_reg2;
   iob_reg
     #(
       .DATA_W  (`EXP_MAX_W+2),
       .RST_VAL ({(`EXP_MAX_W+2){1'b0}})
       )
   reg_exp_big1
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (exp_big_reg),
      .data_o (exp_big_reg2)
      );

   wire [`RES_MAX_W-1:0]          man_reg;
   iob_reg
     #(
       .DATA_W  (`RES_MAX_W),
       .RST_VAL ({`RES_MAX_W{1'b0}})
       )
   reg_man
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (man),
      .data_o (man_reg)
      );

   wire                           carry_reg;
   iob_reg
     #(
       .DATA_W  (1),
       .RST_VAL (1'b0)
       )
   reg_carry
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (carry),
      .data_o (carry_reg)
      );

   wire                           shift_reg;
   iob_reg
     #(
       .DATA_W  (1),
       .RST_VAL (1'b0)
       )
   reg_shift
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (shift),
      .data_o (shift_reg)
      );

   wire                           done_reg2;
   iob_reg
     #(
       .DATA_W  (1),
       .RST_VAL (1'b0)
       )
   reg_done1
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (done_reg),
      .data_o (done_reg2)
      );

   wire [`EXP_MAX_W+1:0]          exp_shift;
   assign exp_shift = exp_big_reg2 + 1'b1;

   wire [`RES_MAX_W-1:0]          man_shift;
`ifndef ROUNDING
   assign man_shift = {carry_reg, man_reg[`RES_MAX_W-1:1]};
`else
   assign man_shift = {carry_reg, man_reg[`RES_MAX_W-1:2], |man_reg[1:0]};
`endif

   wire [`EXP_MAX_W+1:0]          exp_norm;
   wire [`RES_MAX_W-1:0]          man_norm;
   iob_norm
     #(
       .EXP_W  (`EXP_MAX_W+2),
       .DATA_W (`RES_MAX_W)
       )
   norm
     (
      .exp_i (exp_big_reg2),
      .man_i (man_reg),
      .exp_o (exp_norm),
      .man_o (man_norm)
      );

   wire [`EXP_MAX_W+1:0]          exp_res;
   wire [`RES_MAX_W-1:0]          man_res;
   assign exp_res = shift_reg? exp_shift: exp_norm;
   assign man_res = shift_reg? man_shift: man_norm;

   // pipeline stage 3
   wire [`EXP_MAX_W+1:0]          exp_res_reg;
   iob_reg
     #(
       .DATA_W  (`EXP_MAX_W+2),
       .RST_VAL ({(`EXP_MAX_W+2){1'b0}})
       )
   reg_exp_res
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (exp_res),
      .data_o (exp_res_reg)
      );

   wire [`RES_MAX_W-1:0]          man_res_reg;
   iob_reg
     #(
       .DATA_W  (`RES_MAX_W),
       .RST_VAL ({`RES_MAX_W{1'b0}})
       )
   reg_man_res
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (man_res),
      .data_o (man_res_reg)
      );

   iob_reg
     #(
       .DATA_W  (1),
       .RST_VAL (1'b0)
       )
   reg_done2
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (done_reg2),
      .data_o (done_o)
      );

   wire                           zero;
   assign zero = ~|man_res_reg;

   assign exp_o = zero? {2'b11, `EXP_MIN}: exp_res_reg;
   assign man_o = man_res_reg;

endmodule

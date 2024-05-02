`timescale 1ns / 1ps

`include "iob_ptfloat_defs.vh"

module iob_ptfloat_div
  #(
    parameter DATA_W = 32,
    parameter EW_W   = 4
    )
   (
    input                   clk_i,
    input                   arst_i,
    input                   cke_i,

    input                   start_i,
    output                  done_o,

    input [`EXP_MAX_W-1:0]  exp_a_i,
    input [`MAN_MAX_W-1:0]  man_a_i,

    input [`EXP_MAX_W-1:0]  exp_b_i,
    input [`MAN_MAX_W-1:0]  man_b_i,

    output [`EXP_MAX_W+1:0] exp_o,
    output [`RES_MAX_W-1:0] man_o,

    output                  div_by_zero_o
    );

   wire [`EXP_MAX_W:0]      exp_a_norm, exp_a;
   wire [`MAN_MAX_W-1:0]    man_a_norm;
   assign exp_a = {exp_a_i[`EXP_MAX_W-1], exp_a_i};
   iob_norm
     #(
       .EXP_W  (`EXP_MAX_W+1),
       .DATA_W (`MAN_MAX_W)
       )
   norm_op_a
     (
      .exp_i (exp_a),
      .man_i (man_a_i),
      .exp_o (exp_a_norm),
      .man_o (man_a_norm)
      );

   assign div_by_zero_o = ~|man_b_i;

   wire [`EXP_MAX_W:0]      exp_b_norm, exp_b;
   wire [`MAN_MAX_W-1:0]    man_b_norm;
   assign exp_b = {exp_b_i[`EXP_MAX_W-1], exp_b_i};
   iob_norm
     #(
       .EXP_W  (`EXP_MAX_W+1),
       .DATA_W (`MAN_MAX_W)
       )
   norm_op_b
     (
      .exp_i (exp_b),
      .man_i (man_b_i),
      .exp_o (exp_b_norm),
      .man_o (man_b_norm)
      );

   wire [`EXP_MAX_W+1:0]    exp_int;
   assign exp_int = {exp_a_norm[`EXP_MAX_W-1], exp_a_norm} - {exp_b_norm[`EXP_MAX_W-1], exp_b_norm};

   wire [`RES_MAX_W+`MAN_MAX_W-1:0] man_a_int, man_b_int;
   assign man_a_int = {man_a_norm[`MAN_MAX_W-1], man_a_norm, {(`RES_MAX_W-1){1'b0}}};
   assign man_b_int = {{(`RES_MAX_W-1){1'b0}}, man_b_norm[`MAN_MAX_W-1], man_b_norm};

   wire                             div_en;
   assign div_en = ~start_i;

   wire [`RES_MAX_W+`MAN_MAX_W-1:0] man;
   wire                             div_done;
   iob_div_subshift
     #(
       .DATA_W(`RES_MAX_W+`MAN_MAX_W)
       )
   div
     (
      .clk       (clk_i),

      .en        (div_en),
      .sign      (1'b1),
      .done      (div_done),

      .dividend  (man_a_int),
      .divisor   (man_b_int),
      .quotient  (man),
      .remainder ()
      );

   wire                             div_done_reg;
   iob_reg
     #(
       .DATA_W  (1),
       .RST_VAL (1'b1)
       )
   reg_div_done
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (div_done),
      .data_o (div_done_reg)
      );

   wire                             done;
   assign done = div_done & ~div_done_reg;

   wire                             shift;
   assign shift = man[`RES_MAX_W] ^ man[`RES_MAX_W-1];

   wire [`EXP_MAX_W+1:0]            exp_shift;
   assign exp_shift = exp_int + 1'b1;

   wire [`RES_MAX_W-1:0]            man_shift;
`ifndef ROUNDING
   assign man_shift = man[`RES_MAX_W:1];
`else
   assign man_shift = {man[`RES_MAX_W:2], |man[1:0]};
`endif

   wire [`EXP_MAX_W+1:0]            exp_norm;
   wire [`RES_MAX_W-1:0]            man_norm;
   iob_norm
     #(
       .EXP_W  (`EXP_MAX_W+2),
       .DATA_W (`RES_MAX_W)
       )
   norm_res
     (
      .exp_i (exp_int),
      .man_i (man[`RES_MAX_W-1:0]),
      .exp_o (exp_norm),
      .man_o (man_norm)
      );

   wire [`EXP_MAX_W+1:0]            exp_res;
   wire [`RES_MAX_W-1:0]            man_res;
   assign exp_res = shift? exp_shift: exp_norm;
   assign man_res = shift? man_shift: man_norm;

   // pipeline stage 1
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
   reg_done
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (done),
      .data_o (done_o)
      );

   wire                           zero;
   assign zero = ~|man_res_reg;

   assign exp_o = zero? {2'b11, `EXP_MIN}: exp_res_reg;
   assign man_o = man_res_reg;

endmodule

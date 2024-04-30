`timescale 1ns / 1ps

`include "iob_ptfloat_defs.vh"

module iob_ptfloat_mul
  #(
    parameter DATA_W = 32,
    parameter EW_W   = 4
    )
   (
    input                         clk_i,
    input                         arst_i,
    input                         cke_i,

    input                         start_i,
    output                        done_o,

    input signed [`EXP_MAX_W-1:0] exp_a_i,
    input [`MAN_MAX_W-1:0]        man_a_i,

    input signed [`EXP_MAX_W-1:0] exp_b_i,
    input [`MAN_MAX_W-1:0]        man_b_i,

    output [`EXP_MAX_W+1:0]       exp_o,
    output [`RES_MAX_W-1:0]       man_o
    );

   wire                           done;
   assign done = start_i;

   wire [`EXP_MAX_W+1:0]          exp_int;
   assign exp_int = {{2{exp_a_i[`EXP_MAX_W-1]}}, exp_a_i} + {{2{exp_b_i[`EXP_MAX_W-1]}}, exp_b_i};

   wire [2*`MAN_MAX_W-1:0]        man_int;
   iob_mul
     #(
       .DATA_W(`MAN_MAX_W)
       )
   mul
     (
      .op_a (man_a_i),
      .op_b (man_b_i),
      .res  (man_int)
      );

   // pipeline stage 1
   wire [`EXP_MAX_W+1:0]          exp_reg;
   iob_reg
     #(
       .DATA_W  (`EXP_MAX_W+2),
       .RST_VAL ({(`EXP_MAX_W+2){1'b0}})
       )
   reg_exp0
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (exp_int),
      .data_o (exp_reg)
      );

   wire [2*`MAN_MAX_W-1:0]        man_reg;
   iob_reg
     #(
       .DATA_W  (2*`MAN_MAX_W),
       .RST_VAL ({(2*`MAN_MAX_W){1'b0}})
       )
   reg_man
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (man_int),
      .data_o (man_reg)
      );

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

   wire                           shift;
   assign shift = man_reg[2*`MAN_MAX_W-1] ^ man_reg[2*`MAN_MAX_W-2];

   wire [`EXP_MAX_W+1:0]          exp_shift;
   assign exp_shift = exp_reg + 1'b1;

   wire [2*`MAN_MAX_W-1:0]        man_shift;
   assign man_shift = man_reg;

   wire [2*`MAN_MAX_W-1:0]        man_reg_;
   assign man_reg_ = man_reg << 1'b1;

   wire [`EXP_MAX_W+1:0]          exp_norm;
   wire [2*`MAN_MAX_W-1:0]        man_norm;
   iob_norm
     #(
       .EXP_W  (`EXP_MAX_W+2),
       .DATA_W (2*`MAN_MAX_W)
       )
   norm
     (
      .exp_i  (exp_reg),
      .man_i  (man_reg_),
      .exp_o  (exp_norm),
      .man_o  (man_norm)
      );

   wire [`EXP_MAX_W+1:0]          exp_res;
   wire [2*`MAN_MAX_W-1:0]        man_res_;
   assign exp_res  = shift? exp_shift: exp_norm;
   assign man_res_ = shift? man_shift: man_norm;

   wire [`RES_MAX_W-1:0]          man_res;
`ifndef ROUNDING
   assign man_res = man_res_[2*`MAN_MAX_W-1-:`RES_MAX_W];
`else
   assign man_res = {man_res_[2*`MAN_MAX_W-1-:`RES_MAX_W-1], |man_res_[2*`MAN_MAX_W-`RES_MAX_W:0]};
`endif

   // pipeline stage 2
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
   reg_done1
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (done_reg),
      .data_o (done_o)
      );

   wire                           zero;
   assign zero = ~|man_res_reg;

   assign exp_o = zero? {2'b11, `EXP_MIN}: exp_res_reg;
   assign man_o = man_res_reg;

endmodule

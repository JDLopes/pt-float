`timescale 1ns / 1ps

`include "iob_ptfloat_defs.vh"

module iob_ptfloat_unpack
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

    input [DATA_W-1:0]      data_i,

    output [`EXP_MAX_W-1:0] exp_o,
    output [`MAN_MAX_W-1:0] man_o
    );

   wire [EW_W-1:0]          ew;
   assign ew = data_i[EW_W-1:0];

   wire [`EXP_MAX_W-1:0]    e;
   assign e = $signed({~data_i[DATA_W-1], data_i[DATA_W-1 -: `EW_MAX]}) >>> (`EW_MAX - ew);

   wire [`F_MAX_W-1:0]      f;
   assign f = data_i[DATA_W-1:EW_W] << ew;

   wire                     sub;
   assign sub = (ew == `EW_MAX && ~|e[`EW_MAX-1:0])? 1'b1: 1'b0;

   wire [`EXP_MAX_W-1:0]    exp_int;
   assign exp_int = e[`EXP_MAX_W-1]? e + 1'b1: e;

   wire [`EXP_MAX_W-1:0]    exp_o_int;
   assign exp_o_int = ~|ew? {`EXP_MAX_W{1'b0}}:
                       sub? exp_int + 1'b1:
                            exp_int;

   wire [`MAN_MAX_W-1:0]    man_o_int;
   assign man_o_int = sub? {f[`F_MAX_W-1], f}: {~f[`F_MAX_W-1], f};

   //  pipeline stage 1
   iob_reg
     #(
       .DATA_W  (`EXP_MAX_W),
       .RST_VAL ({`EXP_MAX_W{1'b0}})
       )
   reg_exp
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (exp_o_int),
      .data_o (exp_o)
      );

   iob_reg
     #(
       .DATA_W  (`MAN_MAX_W),
       .RST_VAL ({`MAN_MAX_W{1'b0}})
       )
   reg_man
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (man_o_int),
      .data_o (man_o)
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
      .data_i (start_i),
      .data_o (done_o)
      );

endmodule

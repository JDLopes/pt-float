`timescale 1ns / 1ps

`include "iob_ptfloat_defs.vh"

module iob_ptfloat_ew
  #(
    parameter DATA_W = 32,
    parameter EW_W   = 4
    )
   (
    input [`EXP_MAX_W-1:0] exp_i,
    output [EW_W-1:0]      ew_o
    );

   wire [`EXP_MAX_W-1:0]   exp_int;
   assign exp_int = exp_i[`EXP_MAX_W-1]? exp_i - 1'b1: exp_i;

   wire [$clog2(`EXP_MAX_W+1)-1:0] lds;
   iob_cls
     #(`EXP_MAX_W)
   cls
     (
      .data_i  (exp_int),
      .count_o (lds)
      );

   wire [$clog2(`EXP_MAX_W+1)-1:0] ew;
   assign ew = `EXP_MAX_W - lds - 1'b1;

   assign ew_o = |exp_i? ew[EW_W-1:0]: {EW_W{1'b0}};

endmodule

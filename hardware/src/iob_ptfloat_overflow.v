`timescale 1ns / 1ps

`include "iob_ptfloat_defs.vh"

module iob_ptfloat_overflow
  #(
    parameter DATA_W = 32,
    parameter EW_W   = 4
    )
  (
   input [`EXP_MAX_W+1:0]  exp_i,
   input                   man_sign_i,

   output [`MAN_MAX_W-1:0] man_o,
   output                  overflow_o
   );

   assign overflow_o = ~((exp_i[`EXP_MAX_W+1] & exp_i[`EXP_MAX_W] & exp_i[`EXP_MAX_W-1]) |
                         (~exp_i[`EXP_MAX_W+1] & ~exp_i[`EXP_MAX_W] & ~exp_i[`EXP_MAX_W-1]));

   assign man_o = man_sign_i? {`MAN_MIN, {(`EW_MAX){1'b0}}}:
                              {`MAN_MAX, {(`EW_MAX){1'b0}}};

endmodule

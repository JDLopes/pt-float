`timescale 1ns / 1ps

`include "iob_ptfloat_defs.vh"
`include "iob_ieee754_defs.vh"

module iob_ptfloat2double
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

    input signed [`EXP_MAX_W-1:0] exp_i,
    input [`MAN_MAX_W-1:0]        man_i,

    output [`FP_DP_DATA_W-1:0]    fp_o
    );

   generate
      if (`EXP_MAX_W > `FP_DP_EXP_W) begin
         wire signed [`EXP_MAX_W:0] exp_int;
         assign exp_int = {exp_i[`EXP_MAX_W-1], exp_i} + {{(`EXP_MAX_W - `FP_DP_EXP_W + 1){1'b0}}, `FP_DP_BIAS};
      end else begin
         wire signed [`FP_DP_EXP_W:0] exp_int;
         assign exp_int = {{(`FP_DP_EXP_W - `EXP_MAX_W + 1){exp_i[`EXP_MAX_W-1]}}, exp_i} + {1'b0, `FP_DP_BIAS};
      end
   endgenerate

   wire [`MAN_MAX_W:0]                man_int;
   assign man_int = {(`MAN_MAX_W+1){man_i[`MAN_MAX_W-1]}} ^ {man_i[`MAN_MAX_W-1], man_i} + {{`MAN_MAX_W{1'b0}}, man_i[`MAN_MAX_W-1]};

endmodule

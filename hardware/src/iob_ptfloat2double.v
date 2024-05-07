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

   wire [`MAN_MAX_W:0]            man_int_;
   assign man_int_ = {(`MAN_MAX_W+1){man_i[`MAN_MAX_W-1]}} ^ {man_i[`MAN_MAX_W-1], man_i} + {{`MAN_MAX_W{1'b0}}, man_i[`MAN_MAX_W-1]};

   wire [`FP_DP_MAN_W-1:0]        man_int;
   assign man_int = {man_int_, {(`FP_DP_MAN_W - `MAN_MAX_W - 1){1'b0}}};

   wire                           exp_int_sign;
   wire [`FP_DP_EXP_W-1:0]        exp_norm;
   wire [`FP_DP_MAN_W-1:0]        man_shift, man_norm;
   generate
      if (`EXP_MAX_W > `FP_DP_EXP_W) begin
         wire signed [`EXP_MAX_W:0] exp_int;
         assign exp_int = {exp_i[`EXP_MAX_W-1], exp_i} + {{(`EXP_MAX_W - `FP_DP_EXP_W + 1){1'b0}}, `FP_DP_BIAS};
         assign exp_int_sign = exp_int[`EXP_MAX_W];

         wire [`EXP_MAX_W:0]        shift;
         assign shift = {(`EXP_MAX_W+1){exp_int[`EXP_MAX_W]}} ^ exp_int + {{`EXP_MAX_W{1'b0}}, exp_int[`EXP_MAX_W]};

         wire signed [`FP_DP_MAN_W+2:0] man_int_, man_shift_;
         assign man_int_   = {man_int, 3'd0};
         assign man_shift_ = man_int_ >>> shift;

         wire                           man_lsb, guard_bit, round_bit;
         assign man_lsb   = man_shift_[3];
         assign guard_bit = man_shift_[2];
         assign round_bit = man_shift_[1];

         wire                           sticky_bit;
         iob_sticky
           #(
             .DATA_W  (`FP_DP_MAN_W),
             .SHIFT_W (`EXP_MAX_W+1)
             )
         csticky
           (
            .man_i    (man_int_),
            .shift_i  (shift),
            .sticky_o (sticky_bit)
            );

         wire                           round;
         assign round = guard_bit & (man_lsb | round_bit | sticky_bit);

         assign man_shift = man_shift_[`FP_DP_MAN_W+2:3] + round;

         wire [`EXP_MAX_W:0]            exp_norm_;
         wire [`FP_DP_MAN_W-1:0]        man_norm_;
         iob_norm
           #(
             .EXP_W  (`EXP_MAX_W+1),
             .DATA_W (`FP_DP_MAN_W)
             )
         norm
           (
            .exp_i (exp_int),
            .man_i (man_int),
            .exp_o (exp_norm_),
            .man_o (man_norm_)
            );

         wire                           infinity;
         assign infinity = (exp_norm_ > `FP_DP_EXP_MAX)? 1'b1: 1'b0;

         assign exp_norm = infinity? `FP_DP_EXP_INF: exp_norm_[`FP_DP_EXP_W-1:0];
         assign man_norm = infinity? 1'b0: man_norm_;
      end else begin
         wire signed [`FP_DP_EXP_W:0] exp_int;
         assign exp_int = {{(`FP_DP_EXP_W - `EXP_MAX_W + 1){exp_i[`EXP_MAX_W-1]}}, exp_i} + {1'b0, `FP_DP_BIAS};
         assign exp_int_sign = exp_int[`FP_DP_EXP_W];

         wire [`FP_DP_EXP_W:0]        shift;
         assign shift = {(`FP_DP_EXP_W+1){exp_int[`FP_DP_EXP_W]}} ^ exp_int + {{`FP_DP_EXP_W{1'b0}}, exp_int[`FP_DP_EXP_W]};

         wire signed [`FP_DP_MAN_W+2:0] man_int_, man_shift_;
         assign man_int_   = {man_int, 3'd0};
         assign man_shift_ = man_int_ >>> shift;

         wire                           man_lsb, guard_bit, round_bit;
         assign man_lsb   = man_shift_[3];
         assign guard_bit = man_shift_[2];
         assign round_bit = man_shift_[1];

         wire                           sticky_bit;
         iob_sticky
           #(
             .DATA_W  (`FP_DP_MAN_W),
             .SHIFT_W (`FP_DP_EXP_W+1)
             )
         csticky
           (
            .man_i    (man_int_),
            .shift_i  (shift),
            .sticky_o (sticky_bit)
            );

         wire                           round;
         assign round = guard_bit & (man_lsb | round_bit | sticky_bit);

         assign man_shift = man_shift_[`FP_DP_MAN_W+2:3] + round;

         wire [`FP_DP_EXP_W:0]          exp_norm_;
         wire [`FP_DP_MAN_W-1:0]        man_norm_;
         iob_norm
           #(
             .EXP_W  (`FP_DP_EXP_W+1),
             .DATA_W (`FP_DP_MAN_W)
             )
         norm
           (
            .exp_i (exp_int),
            .man_i (man_int),
            .exp_o (exp_norm_),
            .man_o (man_norm_)
            );

         wire                           infinity;
         assign infinity = (exp_norm_ > `FP_DP_EXP_MAX)? 1'b1: 1'b0;

         assign exp_norm = infinity? `FP_DP_EXP_INF: exp_norm_[`FP_DP_EXP_W-1:0];
         assign man_norm = infinity? 1'b0: man_norm_;
      end
   endgenerate

   wire                                 zero;
   assign zero = (exp_i == `EXP_MIN)? ~|man_i: 1'b0;

   wire [`FP_DP_EXP_W-1:0]              exp_f;
   assign exp_f =         zero? 1'b0:
                  exp_int_sign? 1'b0:
                                exp_norm;

   wire [`FP_DP_F_W-1:0]                frac_f;
   assign frac_f =         zero? 1'b0:
                   exp_int_sign? man_shift[`FP_DP_F_W-1:0]:
                                 man_norm[`FP_DP_F_W-1:0];

   assign fp_o = {man_i[`MAN_MAX_W-1], exp_f, frac_f};

endmodule

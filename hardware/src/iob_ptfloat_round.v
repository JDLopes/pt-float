`timescale 1ns / 1ps

`include "iob_ptfloat_defs.vh"

module iob_ptfloat_round
  #(
    parameter DATA_W = 32,
    parameter EW_W   = 4
    )
   (
    input [`EXP_MAX_W+1:0]  exp_i,
    input [`RES_MAX_W-1:0]  man_i,

    output [`EXP_MAX_W+1:0] exp_o,
    output [`MAN_MAX_W-1:0] man_o,
    output [EW_W-1:0]       ew_o
    );

   wire [EW_W-1:0]          ew;
   iob_ptfloat_ew
     #(
       .DATA_W (DATA_W),
       .EW_W   (EW_W)
       )
   cew
     (
      .exp_i (exp_i[`EXP_MAX_W-1:0]),
      .ew_o  (ew)
      );

   wire [`EXP_MAX_W-1:0]    mask, ew_int;
   assign ew_int = {{(`EXP_MAX_W - EW_W){1'b0}}, ew};
   iob_mask
     #(
       .DATA_W (`EXP_MAX_W)
       )
   mask0
     (
      .maskl_i (ew_int),
      .mask_o  (mask)
      );

   wire [`MAN_MAX_W-1:0]    man_i_;
   assign man_i_ = man_i[`RES_MAX_W-1:3] ^ {{(`MAN_MAX_W-`EXP_MAX_W){1'b0}}, mask};

   wire [`EXP_MAX_W-1:0]    mask_1;
   assign mask_1 = mask >> 1'b1;

   wire [`RES_MAX_W-1:0]    man_shift;
   assign man_shift = man_i >> ew;

   wire                     man_lsb, guard_bit, round_bit;
   assign man_lsb   = man_shift[3];
   assign guard_bit = man_shift[2];
   assign round_bit = man_shift[1];

   wire                     sticky_bit;
   iob_sticky
     #(
       .DATA_W  (`RES_MAX_W),
       .SHIFT_W (EW_W)
       )
   csticky
     (
      .man_i    (man_i),
      .shift_i  (ew),
      .sticky_o (sticky_bit)
      );

   wire                     round;
   assign round = guard_bit & (man_lsb | round_bit | sticky_bit);

   wire [`MAN_MAX_W-1:0]    incr;
   assign incr = {{(`MAN_MAX_W-1){1'b0}}, 1'b1} << ew;

   wire [`MAN_MAX_W-1:0]    man_rnd_int;
   assign man_rnd_int = man_i_ + incr;

   wire                     norm;
   assign norm = man_i[`RES_MAX_W-1]? man_rnd_int[`MAN_MAX_W-2]: man_rnd_int[`MAN_MAX_W-1];

   wire [`EXP_MAX_W+1:0]    exp_norm;
   assign exp_norm = man_i[`RES_MAX_W-1]? exp_i - 1'b1: exp_i + 1'b1;

   wire [`MAN_MAX_W-1:0]    man_norm;
   assign man_norm = man_i[`RES_MAX_W-1]? {man_rnd_int[`MAN_MAX_W-2:0], 1'b0}: {1'b0, man_rnd_int[`MAN_MAX_W-1:1]};

   wire [EW_W-1:0]          ew_norm;
   assign ew_norm =                ~|ew? 1'b1:
                    man_i[`RES_MAX_W-1]? {exp_i[`EXP_MAX_W-1]? {~|(exp_norm & mask)? ew + 1'b1: ew}:
                                                               {(ew == 1'b1)? 1'b0:
                                                               {|(exp_norm & mask_1)? ew - 1'b1: ew}}}:
                                         {exp_i[`EXP_MAX_W-1]? {(ew == 1'b1)? 1'b0:
                                                               {|(exp_norm & mask_1)? ew - 1'b1: ew}}:
                                                               {~|(exp_norm & mask)? ew + 1'b1: ew}};

   wire [`EXP_MAX_W+1:0]    exp_rnd;
   assign exp_rnd = norm? exp_norm: exp_i;

   wire [`MAN_MAX_W-1:0]    man_rnd;
   assign man_rnd = norm? man_norm: man_rnd_int;

   wire [EW_W-1:0]          ew_rnd;
   assign ew_rnd = norm? ew_norm: ew;

   assign exp_o = round? exp_rnd: exp_i;
   assign man_o = round? man_rnd: man_i[`RES_MAX_W-1:3];
   assign ew_o  = round? ew_rnd: ew;

endmodule

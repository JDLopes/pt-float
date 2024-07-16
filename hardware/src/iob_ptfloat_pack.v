`timescale 1ns / 1ps

`include "iob_ptfloat_defs.vh"

module iob_ptfloat_pack
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

    input [`EXP_MAX_W+1:0]        exp_i,
    input signed [`RES_MAX_W-1:0] man_i,

    output [DATA_W-1:0]           data_o,
    output                        overflow_o,
    output                        underflow_o
    );

   wire                           done;
   assign done = start_i;

   wire [`EXP_MAX_W+1:0]          exp_diff;
   assign exp_diff = {2'b11, `EXP_MIN} - exp_i;

   wire [`EXP_MAX_W+1:0]          exp_sub;
   wire [`RES_MAX_W-1:0]          man_sub;
   wire                           underflow;
`ifdef SUBNORMALS
   wire [`RES_MAX_W-1:0]          man_;
   assign man_ = man_i >>> exp_diff;

   wire [`RES_MAX_W-1:0]          man_sub_int;
 `ifndef ROUNDING
   assign man_sub_int = man_;
 `else
   wire                           sticky;
   iob_sticky
     #(
       .DATA_W(`RES_MAX_W),
       .SHIFT_W(`EXP_MAX_W+2)
       )
   csticky
     (
      .man_i    (man_i),
      .shift_i  (exp_diff),
      .sticky_o (sticky)
      );
   assign man_sub_int = {man_[`RES_MAX_W-1:1], man_[0] | sticky};
 `endif

   wire                           underflow_int;
   assign underflow_int = (exp_diff > `MAN_MIN_W)? 1'b1: 1'b0;

   assign exp_sub = exp_diff[`EXP_MAX_W+1]? exp_i: {2'b11, `EXP_MIN};
   assign man_sub = exp_diff[`EXP_MAX_W+1]? man_i:
                             underflow_int? 1'b0:
                                            man_sub_int;
   assign underflow = underflow_int & ~exp_diff[`EXP_MAX_W+1];
`else
   assign exp_sub = exp_diff[`EXP_MAX_W+1]? exp_i: {2'b11, `EXP_MIN};
   assign man_sub = exp_diff[`EXP_MAX_W+1]? man_i: 1'b0;
   assign underflow = ~exp_diff[`EXP_MAX_W+1];
`endif

   // pipeline stage 1
   wire [`EXP_MAX_W+1:0]          exp_sub_reg;
   iob_reg
     #(
       .DATA_W  (`EXP_MAX_W+2),
       .RST_VAL ({(`EXP_MAX_W+2){1'b0}})
       )
   reg_exp_sub
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (exp_sub),
      .data_o (exp_sub_reg)
      );

   wire [`RES_MAX_W-1:0]          man_sub_reg;
   iob_reg
     #(
       .DATA_W  (`RES_MAX_W),
       .RST_VAL ({`RES_MAX_W{1'b0}})
       )
   reg_man_sub
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (man_sub),
      .data_o (man_sub_reg)
      );

   wire                           underflow_reg;
   iob_reg
     #(
       .DATA_W  (1),
       .RST_VAL (1'b0)
       )
   reg_underflow0
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (underflow),
      .data_o (underflow_reg)
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

   wire [`EXP_MAX_W+1:0]          exp_int;
   wire [`MAN_MAX_W-1:0]          man_int;
   wire [EW_W-1:0]                ew_int;
`ifdef ROUNDING
   iob_ptfloat_round
     #(
       .DATA_W (DATA_W),
       .EW_W   (EW_W)
       )
   round
     (
      .exp_i (exp_sub_reg),
      .man_i (man_sub_reg),
      .exp_o (exp_int),
      .man_o (man_int),
      .ew_o  (ew_int)
      );
`else
   assign exp_int = exp_sub_reg;
   assign man_int = man_sub_reg;

   iob_ptfloat_ew
     #(
       .DATA_W (DATA_W),
       .EW_W   (EW_W)
       )
   cew
     (
      .exp_i (exp_sub_reg[`EXP_MAX_W-1:0]),
      .ew_o  (ew_int)
      );
`endif

   wire [`MAN_MAX_W-1:0]          man_over;
   iob_ptfloat_overflow
     #(
       .DATA_W (DATA_W),
       .EW_W   (EW_W)
       )
   ptfloat_overflow
     (
      .exp_i      (exp_int),
      .man_sign_i (man_int[`MAN_MAX_W-1]),
      .man_o      (man_over),
      .overflow_o (overflow_o)
      );

   wire [`EXP_MAX_W-1:0]          exp_f;
   wire [`MAN_MAX_W-1:0]          man_f;
   wire [EW_W-1:0]                ew_f;

   assign exp_f = overflow_o? `EXP_MAX: exp_int[`EXP_MAX_W-1:0];
   assign man_f = overflow_o? man_over: man_int;
   assign ew_f  = overflow_o? {EW_W{1'b1}}: ew_int;

   // pipeline stage 2
   wire [`EXP_MAX_W-1:0]          exp_f_reg;
   iob_reg
     #(
       .DATA_W  (`EXP_MAX_W),
       .RST_VAL ({`EXP_MAX_W{1'b0}})
       )
   reg_exp_f
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (exp_f),
      .data_o (exp_f_reg)
      );

   wire [`MAN_MAX_W-1:0]          man_f_reg;
   iob_reg
     #(
       .DATA_W  (`MAN_MAX_W),
       .RST_VAL ({`MAN_MAX_W{1'b0}})
       )
   reg_man_f
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (man_f),
      .data_o (man_f_reg)
      );

   wire [EW_W-1:0]                ew_f_reg;
   iob_reg
     #(
       .DATA_W  (EW_W),
       .RST_VAL ({EW_W{1'b0}})
       )
   reg_ew_f
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (ew_f),
      .data_o (ew_f_reg)
      );

   iob_reg
     #(
       .DATA_W  (1),
       .RST_VAL (1'b0)
       )
   reg_underflow1
     (
      .clk_i  (clk_i),
      .arst_i (arst_i),
      .cke_i  (cke_i),
      .data_i (underflow_reg),
      .data_o (underflow_o)
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

   wire [`EXP_MAX_W-1:0]          exp_c;
   assign exp_c = (~(man_f_reg[`MAN_MAX_W-1] ^ man_f_reg[`MAN_MAX_W-2]) && exp_f_reg == `EXP_MIN)? 1'b0:
                                                                          exp_f_reg[`EXP_MAX_W-1]? exp_f_reg - 1'b1:
                                                                                                   exp_f_reg;

   wire [`F_MAX_W-1:0]            exp_c_;
   assign exp_c_ = {{(`F_MAX_W-(`EXP_MAX_W-1)){1'b0}}, exp_c[`EXP_MAX_W-2:0]} << (`F_MAX_W - ew_f_reg);

   wire [`F_MAX_W-1:0]            frac;
   assign frac = man_f_reg[`F_MAX_W-1:0] >> ew_f_reg;

   assign data_o = {exp_c_ | frac, ew_f_reg};

endmodule

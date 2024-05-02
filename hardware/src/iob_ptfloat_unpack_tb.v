`timescale 1ns / 1ps

`include "iob_ptfloat_defs.vh"

module iob_ptfloat_unpack_tb;

   parameter DATA_W = 32;
   parameter EW_W = 4;

   parameter clk_per = 20;

   reg clk_i = 0;
   reg rst_i = 0;
   reg cke_i = 1;

   reg start_i;
   wire done_o;

   reg [DATA_W-1:0] data_i;

   wire [`EXP_MAX_W-1:0] exp_o;
   wire [`MAN_MAX_W-1:0] man_o;

   iob_ptfloat_unpack
     #(
       .DATA_W (DATA_W),
       .EW_W   (EW_W)
       )
   uut
     (
      .clk_i   (clk_i),
      .arst_i  (rst_i),
      .cke_i   (cke_i),
      .start_i (start_i),
      .done_o  (done_o),
      .data_i  (data_i),
      .exp_o   (exp_o),
      .man_o   (man_o)
      );

   initial begin

      $dumpfile("iob_ptfloat_unpack.vcd");
      $dumpvars();

      data_i = 0;

      #(clk_per+1);

      rst_i = 1;
      #(100*clk_per);
      rst_i = 0;

      #clk_per;
      data_i = 'h0000000f;

      #clk_per;
      data_i = 'h0002000f;

      #clk_per;
      data_i = 'h80000000;

      #clk_per;
      data_i = 'hc0000001;

      #clk_per;
      data_i = 'ha0000002;

      #clk_per;
      data_i = 'h60000002;

      #clk_per;
      data_i = 'h00000000;

      #clk_per $finish;
   end

   always #(clk_per/2) clk_i = ~clk_i;

endmodule

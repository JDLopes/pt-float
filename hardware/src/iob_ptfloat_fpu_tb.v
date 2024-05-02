`timescale 1ns / 1ps

`include "iob_ptfloat_defs.vh"

module iob_ptfloat_fpu_tb;

   parameter DATA_W = 32;
   parameter EW_W = 4;

   parameter clk_per = 20;

   reg clk = 0;
   reg rst = 0;
   reg cke = 1;

   reg [`OPCODE_W-1:0] op;
   reg                 start;
   wire                done;

   reg [DATA_W-1:0]    op1;
   reg [DATA_W-1:0]    op2;

   wire [DATA_W-1:0]   data_o;
   wire                overflow;
   wire                underflow;
   wire                div_by_zero;

   iob_ptfloat_fpu
     #(
       .DATA_W (DATA_W),
       .EW_W   (EW_W)
       )
   uut
     (
      .clk_i         (clk),
      .arst_i        (rst),
      .cke_i         (cke),

      .op_i          (op),
      .start_i       (start),
      .done_o        (done),

      .op1_i         (op1),
      .op2_i         (op2),

      .data_o        (data_o),
      .overflow_o    (overflow),
      .underflow_o   (underflow),
      .div_by_zero_o (div_by_zero)
      );

   initial begin

      $dumpfile("iob_ptfloat_fpu.vcd");
      $dumpvars();

      op = 0;

      op1 = 0;
      op2 = 0;

      start = 0;

      #(clk_per+1);

      rst = 1;
      #(100*clk_per);
      rst = 0;

      // Addition/Subtraction tests

      #clk_per;
      op = 'b00;
      op1 = 'hc0000001;
      op2 = 'ha0000002;
      start = 1;

      #clk_per;
      op = 'b01;

      #clk_per;
      op = 'b00;
      op1 = 'h0000000f;

      #clk_per;
      op = 'b01;

      #clk_per;
      op = 'b00;
      op1 = 'h0002000f;

      #clk_per;
      op = 'b01;

      #clk_per;
      op = 'b00;
      op1 = 'h80000000;

      #clk_per;
      op = 'b01;

      #clk_per;
      op = 'b00;
      op1 = 'hc0000001;

      #clk_per;
      op = 'b01;

      #clk_per;
      op = 'b00;
      op1 = 'ha0000002;

      #clk_per;
      op = 'b01;

      #clk_per;
      op = 'b00;
      op1 = 'h60000002;

      #clk_per;
      op = 'b01;

      #clk_per;
      op = 'b00;
      op1 = 'h00000000;

      #clk_per;
      op = 'b01;

      #clk_per;
      start = 0;
      while (done) #clk_per;

      // Multiplication tests

      #clk_per;
      op = 'b10;
      op1 = 'ha0000002;
      op2 = 'hb0000002;
      start = 1;

      #clk_per;
      op1 = 'h0000000f;

      #clk_per;
      op1 = 'h0002000f;

      #clk_per;
      op1 = 'h80000000;

      #clk_per;
      op1 = 'hc0000001;

      #clk_per;
      op1 = 'ha0000002;

      #clk_per;
      op1 = 'h60000002;

      #clk_per;
      op1 = 'h00000000;

      #clk_per;
      start = 0;
      while (done) #clk_per;

      // Division tests

      #clk_per;
      op = 'b11;
      op1 = 'ha0000002;
      op2 = 'hb0000002;
      start = 1;

      #clk_per;
      start = 0;

      while (~done) #clk_per;
      op1 = 'h0000000f;
      start = 1;

      #clk_per;
      start = 0;

      while (~done) #clk_per;

      #clk_per;
      op1 = 'h0002000f;
      start = 1;

      #clk_per;
      start = 0;

      while (~done) #clk_per;

      #clk_per;
      op1 = 'h80000000;
      start = 1;

      #clk_per;
      start = 0;

      while (~done) #clk_per;

      #clk_per;
      op1 = 'hc0000001;
      start = 1;

      #clk_per;
      start = 0;

      while (~done) #clk_per;

      #clk_per;
      op1 = 'ha0000002;
      start = 1;

      #clk_per;
      start = 0;

      while (~done) #clk_per;

      #clk_per;
      op1 = 'h60000002;
      start = 1;

      #clk_per;
      start = 0;

      while (~done) #clk_per;

      #clk_per;
      op1 = 'h00000000;
      start = 1;

      #clk_per;
      start = 0;

      while (~done) #clk_per;

      #clk_per;
      op2 = 'h0000000f;
      start = 1;

      #clk_per;
      start = 0;

      while (~done) #clk_per;

      #clk_per $finish;
   end

   always #(clk_per/2) clk = ~clk;

endmodule

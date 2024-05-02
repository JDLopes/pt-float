
`define OPCODE_W 2

//
//   PT-Float<DATA_W, EW_W>
//
// |<--------DATA_W-------->|
// |_______ _______ ________|
// |___E___|___F___|___EW___|
// |       |       |        |
// |<-EW-->|<-F_W->|<-EW_W->|
//

`define EW_MAX  ((1 << EW_W) - 1'b1)
`define F_W(ew) (DATA_W - (ew) - EW_W)
`define F_MAX_W `F_W(0)
`define F_MIN_W `F_W(`EW_MAX)

`define EXP_MAX_W (`EW_MAX + 1'b1)
`define EXP_W(ew) ((ew)? ((ew) + 1'b1): `EXP_MAX_W'd0)
`define MAN_MAX_W (`F_MAX_W + 1'b1)
`define MAN_MIN_W (`F_MIN_W + 1'b1)
`define MAN_W(ew) (`F_W(ew) + 1'b1)

`define EXP_MAX {1'b0, {`EW_MAX{1'b1}}}
`define EXP_MIN {1'b1, {(`EW_MAX-2){1'b0}}, 2'b10}
`define MAN_MAX {1'b0, {`F_MIN_W{1'b1}}}
`define MAN_MIN {1'b1, {`F_MIN_W{1'b0}}}

`ifdef ROUNDING
 `define RES_MAX_W (`MAN_MAX_W+2'd3)
`else
 `define RES_MAX_W `MAN_MAX_W
`endif

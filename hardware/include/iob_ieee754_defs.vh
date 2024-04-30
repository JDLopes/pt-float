
`define FP_DP_DATA_W 64
`define FP_DP_EXP_W  11
`define FP_DP_BIAS   `FP_DP_EXP_W'd1023
`define FP_DP_F_W    52
`define FP_DP_MAN_W  (`FP_DP_F_W + 1'b1)

`define FP_SP_DATA_W 32
`define FP_SP_EXP_W  8
`define FP_SP_BIAS   `FP_SP_EXP_W'd127
`define FP_SP_F_W    23
`define FP_SP_MAN_W  (`FP_SP_F_W + 1'b1)

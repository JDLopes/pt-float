
`define FP_DP_DATA_W 64
`define FP_DP_EXP_W  11
`define FP_DP_BIAS   {1'b0, {(`FP_DP_EXP_W-1){1'b1}}}
`define FP_DP_F_W    52
`define FP_DP_MAN_W  (`FP_DP_F_W + 1'b1)

`define FP_SP_DATA_W 32
`define FP_SP_EXP_W  8
`define FP_SP_BIAS   {1'b0, {(`FP_SP_EXP_W-1){1'b1}}}
`define FP_SP_F_W    23
`define FP_SP_MAN_W  (`FP_SP_F_W + 1'b1)

`define FP_DP_EXP_MAX {{(`FP_DP_EXP_W-1){1'b1}}, 1'b0}
`define FP_DP_EXP_MIN `FP_DP_EXP_W'd1
`define FP_DP_EXP_INF {`FP_DP_EXP_W{1'b1}}
`define FP_DP_EXP_NAN {`FP_DP_EXP_W{1'b1}}
`define FP_DP_EXP_SUB `FP_DP_EXP_W'd0

`define FP_SP_EXP_MAX {{(`FP_SP_EXP_W-1){1'b1}}, 1'b0}
`define FP_SP_EXP_MIN `FP_SP_EXP_W'd1
`define FP_SP_EXP_INF {`FP_SP_EXP_W{1'b1}}
`define FP_SP_EXP_NAN {`FP_SP_EXP_W{1'b1}}
`define FP_SP_EXP_SUB `FP_SP_EXP_W'd0

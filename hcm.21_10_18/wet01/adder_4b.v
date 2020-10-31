module HA (A, B, S, Co );
 input A, B;
 output S, Co;
 and2 a ( .A(A), .B(B), .Y(S) );
 xor2 c ( .A(A), .B(B), .Y(Co) );
endmodule // HA
module FA (A, B, Ci, S, Co );
 input A, B, Ci;
 output S, Co;
 wire Sab, Cab, Csc;
 HA h0 ( .A(A), .B(B), .S(Sab), .Co(Cab) );
 HA h1 ( .A(Sab), .B(Ci), .S(S), .Co(Csc) );
 or2 o ( .A(Cab), .B(Csc), .Y(Co) );
endmodule // FA
module ADDER4B (A, B, S, Co);
 input [3:0] A;
 input [3:0] B;
 output [3:0] S;
 output Co;
 wire C0, C1, C2;
 FA i0 ( .A(A[0]), .B(B[0]), .Ci(1'b0), .S(S[0]), .Co(C0) );
 FA i1 ( .A(A[1]), .B(B[1]), .Ci(C0), .S(S[1]), .Co(C1) );
 FA i2 ( .A(A[2]), .B(B[2]), .Ci(C1), .S(S[2]), .Co(C2) );
 FA i3 ( .A(A[3]), .B(B[3]), .Ci(C2), .S(S[3]), .Co(Co) );
endmodule // ADDER4B

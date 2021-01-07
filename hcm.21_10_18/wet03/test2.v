module AND2( O, P, Z);

   input	O, P;
   output Z;
   
   and2 Ao2_0( .A(P), .B(O), .Y(Z) );

endmodule // AND2

module OR2( O, P, Z);

   input	O, P;
   output Z;
   
   or2 Ao( .A(P), .B(O), .Y(Z) );

endmodule // OR2

module XOR2( O, P, Z);

   input	O, P;
   output Z;
   
   xor2 A_2( .A(P), .B(O), .Y(Z) );

endmodule // XOR2

module TEST1( X, Y, Z);

   input	X, Y;
   output Z;

   or2 G1( .A(X), .B(Y), .Y(Z) );

endmodule // TEST1

module TEST2(W, X, Y, Z);

	input W, X, Y;
	output Z;
	
	or2 G1 ( .A(W), .B(X), .Y(line0) );
	or2 G2 ( .A(Y), .B(line0), .Y(Z) );
	
endmodule //TEST2

module TEST3(X, Z);

	input X;
	output Z;
	
	buffer G1( .A(X), .Y(line0) );
	buffer G2( .A(line0), .Y(line1) );
	buffer G3( .A(line1), .Y(line2) );
	inv G4( .A(line2), .Y(Z) );
	
endmodule //TEST3

module TEST4(W, X, Y, Z);

	input W, X, Y;
	output Z;
	
	TEST1 G1( .X(X), .Y(Y), .Z(line0) );
	TEST2 G2( .W(W), .X(X), .Y(Y), .Z(line1) );
	xor2 G3( .A(line0), .B(line1), .Y(line2) );
	TEST3 G4( .X(line2), .Z(Z) );
	
endmodule //TEST4

module TEST5(A, C, D, Z);

	input A, C, D;
	output Z;
	
	nand2 G1( .A(D), .B(D), .Y(line0) );
	and2 G2( .A(C), .B(C), .Y(line1) );
	or2 G3( .A(line0), .B(line1), .Y(line2) );
	nand2 G4( .A(A), .B(line2), .Y(line3) );
	and2 G5( .A(C), .B(D), .Y(line4) );
	or2 G6( .A(line3), .B(line4), .Y(Z) );
	
endmodule //TEST5

module TEST6(A, B, D, Z);

	input A, B, D;
	output Z;
	
	not G1( .A(D), .Y(notD) );
	and2 G2( .A(D), .B(notD), .Y(line0) );
	and2 G3( .A(B), .B(B), .Y(line1) );
	or2 G4( .A(line0), .B(line1), .Y(line2) );
	and2 G5( .A(D), .B(line2), .Y(line3) );
	nand2 G6( .A(1'b0), .B(A), .Y(line4) );
	or2 G7( .A(line3), .B(line4), .Y(Z) );
	
endmodule //TEST6

module TEST7(A, B, C, D, E, Z); //E is not used

	input A, B, C, D, E;
	output Z;
	
	inv G1( .A(D), .Y(notD) );
	and2 G2( .A(A), .B(B), .Y(line0) );
	and2 G3( .A(notD), .B(B), .Y(line1) );
	and2 G4( .A(B), .B(C), .Y(line2) );
	or2 G5( .A(line1), .B(line2), .Y(line3) );
	and2 G6( .A(line3), .B(D), .Y(line4) );
	or2 G7( .A(line0), .B(line4), .Y(Z) );
	buffer G8( .A(E), .Y(lineNotUsed) );
	
endmodule //TEST7

module TEST8(X, Z, K); // will not pass

	input X;
	output Z, K;
	
	buffer G1( .A(X), .Y(line0) );
	buffer G2( .A(line0), .Y(line1) );
	buffer G3( .A(line1), .Y(line2) );
	inv G4( .A(line2), .Y(Z) );
	
endmodule //TEST8

module TEST9(A, B, C, clk, Z);

	input A, B, C, clk;
	output Z;

	TEST1 G1( .X(A), .Y(B), .Z(line0) );
	TEST2 G2( .W(A), .X(B), .Y(C), .Z(line1) );
	xor2 G3( .A(line0), .B(line1), .Y(FFin) );
	dff ff1( .D(FFin), .CLK(clk), .Q(Z) );

endmodule //TEST9

module TEST10(A, B, C, D, clk, Z);

	input A, B, C, D, clk;
	output Z;

	TEST3 G1( .X(A), .Z(line0) );
	TEST5 G2( .A(B), .C(C), .D(D), .Z(line1) );
	xor2 G3( .A(line0), .B(line1), .Y(line2) );
	and2 G4( .A(line1), .B(line2), .Y(FFin) );
	dff ff2( .D(FFin), .CLK(clk), .Q(Z) );

endmodule //TEST10

module TEST11(A, B, C, D, E, F, G, H, clk, Z);

	input A, B, C, D, E, F, G, H, clk;
	output Z;

	TEST4 G1( .W(A), .X(B), .Y(C), .Z(line0) );
	TEST7 G2( .A(E), .B(F), .C(G), .D(H), .E(A), .Z(line1) );
	TEST9 G3( .A(A), .B(B), .C(D), .clk(clk), .Z(line2) );
	and2 G4( .A(line0), .B(line1), .Y(FFin) );
	dff ff4( .D(FFin), .CLK(clk), .Q(FFout) );
	or2 G5( .A(FFout), .B(line2), .Y(Z) );

endmodule //TEST11

module TEST12(A, B, C, D, E, F, G, H, clk, Y, Z);

	input A, B, C, D, E, F, G, H, clk;
	output Y, Z;

	TEST6 G1( .A(A), .B(B), .D(C), .Z(line0) );
	TEST7 G2( .A(D), .B(E), .C(F), .D(G), .E(A), .Z(line1) );
	xor2 G3( .A(line1), .B(H), .Y(Y) );
	nand2 G4( .A(line0), .B(line1), .Y(FFin) );
	dff ff4( .D(FFin), .CLK(clk), .Q(Z) );

endmodule //TEST12

module TEST13(IN, clk, OUT); //TOP

	input [21:0] IN;
	input clk;
	output [2:0] OUT;

	TEST9 TopG1( IN[0], IN[1], IN[2], clk, line0 );
	TEST10 TopG2( IN[3], IN[4], IN[5], IN[6], clk, line1 );
	TEST11 TopG3( line0, line1, IN[7], IN[8], IN[9], IN[10], IN[11], IN[12], clk, OUT[0] );
	nor2 TopG4( .A(IN[13]), .B(IN[14]), .Y(line2) );
	TEST12 TopG5( line2, IN[15], IN[16], IN[17], IN[18], IN[19], IN[20], IN[21], clk, line3, FFin );
	dff ff5( .D(FFin), .CLK(clk), .Q(OUT[1]) );
	or2 TopG6( .A(FFin), .B(line3), .Y(OUT[2]) );
	
endmodule //TEST13 - TOP


















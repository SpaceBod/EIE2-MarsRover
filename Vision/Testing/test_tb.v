module test_tb();
    logic[7:0] r,g,b;
    logic[9:0] h,v;
    logic[17:0] s;

    initial begin

        $dumpfile("test_tb.vcd");

        $dumpvars(0, test_tb);

        r = 149;

        g = 179;

        b = 89;

        #10

        $display("r: %d, g: %d, b:  %d", r,g,b);

        $display("h: %d, s: %d, v: %d", h,s,v);

        r = 22;

        g = 48;

        b = 65;

        #10

        $display("r: %d, g: %d, b:  %d", r,g,b);

        $display("h: %d, s: %d, v: %d", h,s,v);

    end

    test dut(
        .r(r),
        .g(g),
        .b(b),
        .h(h),
        .s(s),
        .v(v)
    );

endmodule
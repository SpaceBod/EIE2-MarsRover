module test_tb();
    logic[7:0] red,green,blue;
    logic[9:0] hue,val;
    logic[17:0] sat;

    initial begin

        $dumpfile("test_tb.vcd");

        $dumpvars(0, test_tb);

        red = 0;

        green = 0;

        blue = 255;

        #10

        $display("r: %d, g: %d, b:  %d", red,green,blue);

        $display("h: %d, s: %d, v: %d", hue,sat,val);

        red = 22;

        green = 48;

        blue = 65;

        #10

        $display("r: %d, g: %d, b:  %d", red,green,blue);

        $display("h: %d, s: %d, v: %d", hue,sat,val);

    end

    test dut(
        .red(red),
        .green(green),
        .blue(blue),
        .hue(hue),
        .sat(sat),
        .val(val)
    );

endmodule
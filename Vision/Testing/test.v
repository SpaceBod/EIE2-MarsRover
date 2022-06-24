module test(
    input logic [7:0] r,g,b,
    output logic [9:0] h,v,
    output logic [17:0] s
);

    wire [7:0] cmax, cmin;
    assign cmax = ((r >= g) && (r >= b)) ? r : ((g >= b) && (g >= r)) ? g : b;
    assign cmin = ((r <= g) && (r <= b)) ? r : ((g <= b) && (g <= r)) ? g : b;

    assign h = ((cmax - cmin) == 0) ? 10'd0 : (cmax == r) ? (((g > b) ? ((170*(g-b)/(cmax - cmin)) + 1020) : (1020 - (170*(b-g))/(cmax - cmin))) % 1020) : (cmax == g) ? (((b > r) ? ((170*(b-r)/(cmax - cmin)) + 340) : (340 - (170*(r-b))/(cmax - cmin))) % 1020) : (((r > g) ? ((170*(r-g)/(cmax - cmin)) + 680) : (680 - (170*(g-r))/(cmax - cmin))) % 1020);
    assign s = (cmax == 0) ? 10'd0 : (((cmax - cmin)<<10)/cmax);
    assign v = cmax << 2;

endmodule

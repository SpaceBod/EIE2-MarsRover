module test(
    input logic [7:0] red,green,blue,
    output logic [9:0] hue,val,
    output logic [17:0] sat
);

    wire [7:0] cmax, cmin;
    assign cmax = ((red >= green) && (red >= blue)) ? red : ((green >= blue) && (green >= red)) ? green : blue;
    assign cmin = ((red <= green) && (red <= blue)) ? red : ((green <= blue) && (green <= red)) ? green : blue;

    assign hue = ((cmax - cmin) == 0) ? 10'd0 : (cmax == red) ? (((green > blue) ? ((170*(green-blue)/(cmax - cmin)) + 1020) : (1020 - (170*(blue-green))/(cmax - cmin))) % 1020) : (cmax == green) ? (((blue > red) ? ((170*(blue-red)/(cmax - cmin)) + 340) : (340 - (170*(red-blue))/(cmax - cmin))) % 1020) : (((red > green) ? ((170*(red-green)/(cmax - cmin)) + 680) : (680 - (170*(green-red))/(cmax - cmin))) % 1020);
    assign sat = (cmax == 0) ? 18'd0 : (((cmax - cmin)<<10)/cmax);
    assign val = cmax << 2;

endmodule

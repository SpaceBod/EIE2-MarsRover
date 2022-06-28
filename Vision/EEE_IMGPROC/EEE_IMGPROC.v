module EEE_IMGPROC(
	// global clock & reset
	clk,
	reset_n,
	
	// mm slave
	s_chipselect,
	s_read,
	s_write,
	s_readdata,
	s_writedata,
	s_address,

	// stream sink
	sink_data,
	sink_valid,
	sink_ready,
	sink_sop,
	sink_eop,
	
	// streaming source
	source_data,
	source_valid,
	source_ready,
	source_sop,
	source_eop,
	
	// conduit
	mode
	
);


// global clock & reset
input	clk;
input	reset_n;

// mm slave
input							s_chipselect;
input							s_read;
input							s_write;
output	reg	[31:0]	s_readdata;
input	[31:0]				s_writedata;
input	[2:0]					s_address;


// streaming sink
input	[23:0]            	sink_data;
input								sink_valid;
output							sink_ready;
input								sink_sop;
input								sink_eop;

// streaming source
output	[23:0]			  	   source_data;
output								source_valid;
input									source_ready;
output								source_sop;
output								source_eop;

// conduit export
input                         mode;

////////////////////////////////////////////////////////////////////////
//
parameter IMAGE_W = 11'd640;
parameter IMAGE_H = 11'd480;
parameter MESSAGE_BUF_MAX = 256;
parameter MSG_INTERVAL = 6;
parameter BB_COL_DEFAULT = 24'h00ff00;


wire [7:0]   red, green, blue, grey;
wire [9:0]   hue, val;
wire [17:0]  sat;
wire [7:0]   red_out, green_out, blue_out;

wire         sop, eop, in_valid, out_ready;
////////////////////////////////////////////////////////////////////////

// RGB to HSV conversion
wire [7:0] cmax, cmin;
assign cmax = ((red >= green) && (red >= blue)) ? red : ((green >= blue) && (green >= red)) ? green : blue;
assign cmin = ((red <= green) && (red <= blue)) ? red : ((green <= blue) && (green <= red)) ? green : blue;

assign hue = ((cmax - cmin) == 0) ? 10'd0 : (cmax == red) ? (((green > blue) ? ((170*(green-blue)/(cmax - cmin)) + 1020) : (1020 - (170*(blue-green))/(cmax - cmin))) % 1020) : (cmax == green) ? (((blue > red) ? ((170*(blue-red)/(cmax - cmin)) + 340) : (340 - (170*(red-blue))/(cmax - cmin))) % 1020) : (((red > green) ? ((170*(red-green)/(cmax - cmin)) + 680) : (680 - (170*(green-red))/(cmax - cmin))) % 1020);
assign sat = (cmax == 0) ? 10'd0 : (((cmax - cmin)<<10)/cmax);
assign val = cmax << 2;

// Detect colours
wire fuchsia_detect, teal_detect, yellow_detect, blue_detect, red_detect, green_detect, black_detect, white_detect, wall_detect;

assign green_detect = (hue >= 245) && (hue <= 343) && (sat >= 371) && (sat <= 807) && (val >= 457) && (val <= 1023) && (censor == 1);
assign blue_detect = (hue >= 325) && (hue <= 717) && (sat >= 326) && (sat <= 858) && (val >= 150) && (val <= 644) && (censor == 1);
assign teal_detect = (hue >= 301) && (hue <= 457) && (sat >= 468) && (sat <= 734) && (val >= 210) && (val <= 1023) && (censor == 1);
assign yellow_detect = (hue >= 151) && (hue <= 192) && (sat >= 545) && (sat <= 845) && (val >= 697) && (val <= 1023) && (censor == 1);
assign red_detect = (hue >= 5) && (hue <= 77) && (sat >= 706) && (sat <= 1023) && (val >= 438) && (val <= 1023) && (censor == 1);
assign fuchsia_detect = (hue >= 24) && (hue <= 94) && (sat >= 457) && (sat <= 854) && (val >= 717) && (val <= 1023) && (censor == 1);
assign black_detect =  (hue >= 0) && (hue <= 1023) && (sat >= 0) && (sat <= 1023) && (val >= 0) && (val <= 163) && (censor == 1);
assign white_detect = (hue >= 59) && (hue <= 787) && (sat >= 191) && (sat <= 549) && (val >= 554) && (val <= 1023) && (censor == 1);


wire red_on, green_on, blue_on, teal_on, yellow_on, fuchsia_on, wall_on;

assign red_on = red_detect & red_detect_1 & red_detect_2 & red_detect_3 & red_detect_4;
assign green_on = green_detect & green_detect_1 & green_detect_2 & green_detect_3 & green_detect_4;
assign blue_on = blue_detect & blue_detect_1 & blue_detect_2 & blue_detect_3 & blue_detect_4;
assign teal_on = teal_detect & teal_detect_1 & teal_detect_2 & teal_detect_3 & teal_detect_4;
assign yellow_on = yellow_detect & yellow_detect_1 & yellow_detect_2 & yellow_detect_3 & yellow_detect_4;
assign fuchsia_on = fuchsia_detect & fuchsia_detect_1 & fuchsia_detect_2 & fuchsia_detect_3 & fuchsia_detect_4;
assign wall_on = (black_detect & black_detect_1 & black_detect_2 & white_detect_3 & white_detect_4 & white_detect_5) | (white_detect & white_detect_1 & white_detect_2 & black_detect_3 & black_detect_4 & black_detect_5);

// Find boundary of cursor box

// Highlight detected areas
wire [23:0] fuchsia_high, yellow_high, red_high, teal_high, blue_high, green_high;


assign grey = green[7:1] + red[7:2] + blue[7:2]; //Grey = green/2 + red/4 + blue/4
assign red_high  =  red_on ? {8'hff, 8'h0, 8'h0} : {grey, grey, grey};
assign green_high  =  green_on ? {8'h0, 8'hff, 8'h0} : red_high;
assign blue_high  =  blue_on ? {8'h0, 8'h0, 8'hff} : green_high;
assign yellow_high  =  yellow_on ? {8'hff, 8'ha5, 8'h0} : blue_high;
assign teal_high  =  teal_on ? {8'h0, 8'h80, 8'h80} : yellow_high;
assign fuchsia_high  =  fuchsia_on ? {8'hff, 8'h0, 8'hff} : teal_high;


/*
assign grey = green[7:1] + red[7:2] + blue[7:2]; //Grey = green/2 + red/4 + blue/4
assign red_high  =  red_detect ? {8'hff, 8'h0, 8'h0} : {grey, grey, grey};
assign green_high  =  green_detect ? {8'h0, 8'hff, 8'h0} : red_high;
assign blue_high  =  blue_detect ? {8'h0, 8'h0, 8'hff} : green_high;
assign yellow_high  =  yellow_detect ? {8'hff, 8'ha5, 8'h0} : blue_high;
assign teal_high  =  teal_detect ? {8'h0, 8'h80, 8'h80} : yellow_high;
assign fuchsia_high  =  fuchsia_detect ? {8'hff, 8'h0, 8'hff} : teal_high;
*/

// Show bounding box
wire [23:0] new_image;
wire bb_active_red, bb_active_green, bb_active_blue, bb_active_yellow, bb_active_teal, bb_active_fuchsia, bb_active_wall;
assign bb_active_red = (x == left_red) | (x == right_red) | (y == top_red) | (y == bottom_red);
assign bb_active_green = (x == left_green) | (x == right_green) | (y == top_green) | (y == bottom_green);
assign bb_active_blue = (x == left_blue) | (x == right_blue) | (y == top_blue) | (y == bottom_blue);
assign bb_active_yellow = (x == left_yellow) | (x == right_yellow) | (y == top_yellow) | (y == bottom_yellow);
assign bb_active_teal = (x == left_teal) | (x == right_teal) | (y == top_teal) | (y == bottom_teal);
assign bb_active_fuchsia = (x == left_fuchsia) | (x == right_fuchsia) | (y == top_fuchsia) | (y == bottom_fuchsia);
assign bb_active_wall = (x == left_wall) | (x == right_wall);
assign new_image = bb_active_wall ? {8'h0, 8'hff, 8'hff} : bb_active_red ? {8'hff, 8'h0, 8'h0} : bb_active_green ? {8'h0, 8'hff, 8'h0} : bb_active_blue ? {8'h0, 8'h0, 8'hff} : bb_active_yellow ? {8'hff, 8'ha5, 8'h0} : bb_active_teal ? {8'h0, 8'h80, 8'h80} : bb_active_fuchsia ? {8'hff, 8'h0, 8'hff} : fuchsia_high;

// Switch output pixels depending on mode switch
// Don't modify the start-of-packet word - it's a packet descriptor
// Don't modify data in non-video packets
assign {red_out, green_out, blue_out} = (mode & ~sop & packet_video) ? new_image : {red,green,blue};

//Count valid pixels to tget the image coordinates. Reset and detect packet type on Start of Packet.
reg [10:0] x, y;
reg packet_video, censor;
always@(posedge clk) begin
	if (sop) begin
		x <= 11'h0;
		y <= 11'h0;
		packet_video <= (blue[3:0] == 3'h0);
		censor <= 0;
	end
	else if (in_valid) begin
		if (x == IMAGE_W-1) begin
			if (y == 159) begin
				censor <= 1;
			end
			x <= 11'h0;
			y <= y + 11'h1;
		end
		else begin
			x <= x + 11'h1;
		end
	end
end

//Find first and last red pixels
reg [10:0] wall_x_min, wall_x_max;
reg wall_flag;
always@(posedge clk) begin
	if (wall_on & in_valid) begin
		if (wall_flag == 0) begin
			wall_x_min <= x;
			wall_flag <= 1;
		end
		if (x > wall_x_max) wall_x_max <= x;
	end
	if (sop & in_valid) begin	
		wall_x_min <= IMAGE_W-11'h1;
		wall_x_max <= 0;
		wall_flag <= 0;
	end
end

//Find first and last red pixels
reg [10:0] red_x_min, red_y_min, red_x_max, red_y_max;
always@(posedge clk) begin
	if (red_on & in_valid) begin	//Update bounds when the pixel is red
		if (x < red_x_min) red_x_min <= x;
		if (x > red_x_max) red_x_max <= x;
		if (y < red_y_min) red_y_min <= y;
		if (y > red_y_max) red_y_max <= y;
	end
	if (sop & in_valid) begin	
		red_x_min <= IMAGE_W-11'h1;
		red_x_max <= 0;
		red_y_min <= IMAGE_H-11'h1;
		red_y_max <= 0;
	end
end

reg [10:0] green_x_min, green_y_min, green_x_max, green_y_max;
always@(posedge clk) begin
	if (green_on & in_valid) begin	//Update bounds when the pixel is green
		if (x < green_x_min) green_x_min <= x;
		if (x > green_x_max) green_x_max <= x;
		if (y < green_y_min) green_y_min <= y;
		if (y > green_y_max) green_y_max <= y;
	end
	if (sop & in_valid) begin	
		green_x_min <= IMAGE_W-11'h1;
		green_x_max <= 0;
		green_y_min <= IMAGE_H-11'h1;
		green_y_max <= 0;
	end
end

reg [10:0] blue_x_min, blue_y_min, blue_x_max, blue_y_max;
always@(posedge clk) begin
	if (blue_on & in_valid) begin	//Update bounds when the pixel is blue
		if (x < blue_x_min) blue_x_min <= x;
		if (x > blue_x_max) blue_x_max <= x;
		if (y < blue_y_min) blue_y_min <= y;
		if (y > blue_y_max) blue_y_max <= y;
	end
	if (sop & in_valid) begin	
		blue_x_min <= IMAGE_W-11'h1;
		blue_x_max <= 0;
		blue_y_min <= IMAGE_H-11'h1;
		blue_y_max <= 0;
	end
end

reg [10:0] fuchsia_x_min, fuchsia_y_min, fuchsia_x_max, fuchsia_y_max;
always@(posedge clk) begin
	if (fuchsia_on & in_valid) begin	//Update bounds when the pixel is fuchsia
		if (x < fuchsia_x_min) fuchsia_x_min <= x;
		if (x > fuchsia_x_max) fuchsia_x_max <= x;
		if (y < fuchsia_y_min) fuchsia_y_min <= y;
		if (y > fuchsia_y_max) fuchsia_y_max <= y;
	end
	if (sop & in_valid) begin	
		fuchsia_x_min <= IMAGE_W-11'h1;
		fuchsia_x_max <= 0;
		fuchsia_y_min <= IMAGE_H-11'h1;
		fuchsia_y_max <= 0;
	end
end

reg [10:0] yellow_x_min, yellow_y_min, yellow_x_max, yellow_y_max;
always@(posedge clk) begin
	if (yellow_on & in_valid) begin	//Update bounds when the pixel is yellow
		if (x < yellow_x_min) yellow_x_min <= x;
		if (x > yellow_x_max) yellow_x_max <= x;
		if (y < yellow_y_min) yellow_y_min <= y;
		if (y > yellow_y_max) yellow_y_max <= y;
	end
	if (sop & in_valid) begin	
		yellow_x_min <= IMAGE_W-11'h1;
		yellow_x_max <= 0;
		yellow_y_min <= IMAGE_H-11'h1;
		yellow_y_max <= 0;
	end
end

reg [10:0] teal_x_min, teal_y_min, teal_x_max, teal_y_max;
always@(posedge clk) begin
	if (teal_on & in_valid) begin	//Update bounds when the pixel is teal
		if (x < teal_x_min) teal_x_min <= x;
		if (x > teal_x_max) teal_x_max <= x;
		if (y < teal_y_min) teal_y_min <= y;
		if (y > teal_y_max) teal_y_max <= y;
	end
	if (sop & in_valid) begin	
		teal_x_min <= IMAGE_W-11'h1;
		teal_x_max <= 0;
		teal_y_min <= IMAGE_H-11'h1;
		teal_y_max <= 0;
	end
end

//Process bounding box at the end of the frame.
reg [7:0] red_1, red_2, green_1, green_2, blue_1, blue_2;
reg wall_detect_1, wall_detect_2, wall_detect_3, wall_detect_4, green_detect_1, green_detect_2, green_detect_3, green_detect_4, red_detect_1, red_detect_2, red_detect_3, red_detect_4, blue_detect_1, blue_detect_2, blue_detect_3, blue_detect_4, teal_detect_1, teal_detect_2, teal_detect_3, teal_detect_4, yellow_detect_1, yellow_detect_2, yellow_detect_3, yellow_detect_4, fuchsia_detect_1, fuchsia_detect_2, fuchsia_detect_3, fuchsia_detect_4;
reg black_detect_1, black_detect_2, black_detect_3, black_detect_4, black_detect_5, white_detect_1, white_detect_2, white_detect_3, white_detect_4, white_detect_5;
reg [2:0] msg_state;
reg [10:0] left_red, right_red, top_red, bottom_red;
reg [10:0] left_green, right_green, top_green, bottom_green;
reg [10:0] left_blue, right_blue, top_blue, bottom_blue;
reg [10:0] left_teal, right_teal, top_teal, bottom_teal;
reg [10:0] left_yellow, right_yellow, top_yellow, bottom_yellow;
reg [10:0] left_fuchsia, right_fuchsia, top_fuchsia, bottom_fuchsia;
reg [10:0] left_wall, right_wall;
reg [7:0] frame_count;
always@(posedge clk) begin
	if (eop & in_valid & packet_video) begin  //Ignore non-video packets

		//Latch edges for display overlay on next frame
		left_wall <= wall_x_min;
		right_wall <= wall_x_max;

		left_red <= red_x_min;
		right_red <= red_x_max;
		top_red <= red_y_min;
		bottom_red <= red_y_max;
		
		left_green <= green_x_min;
		right_green <= green_x_max;
		top_green <= green_y_min;
		bottom_green <= green_y_max;
		
		left_blue <= blue_x_min;
		right_blue <= blue_x_max;
		top_blue <= blue_y_min;
		bottom_blue <= blue_y_max;
		
		left_teal <= teal_x_min;
		right_teal <= teal_x_max;
		top_teal <= teal_y_min;
		bottom_teal <= teal_y_max;
		
		left_yellow <= yellow_x_min;
		right_yellow <= yellow_x_max;
		top_yellow <= yellow_y_min;
		bottom_yellow <= yellow_y_max;
		
		left_fuchsia <= fuchsia_x_min;
		right_fuchsia <= fuchsia_x_max;
		top_fuchsia <= fuchsia_y_min;
		bottom_fuchsia <= fuchsia_y_max;
		
		//Start message writer FSM once every MSG_INTERVAL frames, if there is room in the FIFO
		frame_count <= frame_count - 1;
		
		if (frame_count == 0 && msg_buf_size < MESSAGE_BUF_MAX - 3) begin
			msg_state <= 3'b001;
			frame_count <= MSG_INTERVAL-1;
		end
	end
	
	//Cycle through message writer states once started
	if (msg_state != 3'b000) msg_state <= msg_state + 3'b001;

	
	if (in_valid) begin

		red_detect_1 <= red_detect;
		red_detect_2 <= red_detect_1;
		red_detect_3 <= red_detect_2;
		red_detect_4 <= red_detect_3;
		green_detect_1 <= green_detect;
		green_detect_2 <= green_detect_1;
		green_detect_3 <= green_detect_2;
		green_detect_4 <= green_detect_3;
		blue_detect_1 <= blue_detect;
		blue_detect_2 <= blue_detect_1;
		blue_detect_3 <= blue_detect_2;
		blue_detect_4 <= blue_detect_3;
		yellow_detect_1 <= yellow_detect;
		yellow_detect_2 <= yellow_detect_1;
		yellow_detect_3 <= yellow_detect_2;
		yellow_detect_4 <= yellow_detect_3;
		teal_detect_1 <= teal_detect;
		teal_detect_2 <= teal_detect_1;
		teal_detect_3 <= teal_detect_2;
		teal_detect_4 <= teal_detect_3;
		fuchsia_detect_1 <= fuchsia_detect;
		fuchsia_detect_2 <= fuchsia_detect_1;
		fuchsia_detect_3 <= fuchsia_detect_2;
		fuchsia_detect_4 <= fuchsia_detect_3;
		black_detect_1 <= black_detect;
		black_detect_2 <= black_detect_1;
		black_detect_3 <= black_detect_2;
		black_detect_4 <= black_detect_3;
		black_detect_5 <= black_detect_4;
		white_detect_1 <= white_detect;
		white_detect_2 <= white_detect_1;
		white_detect_3 <= white_detect_2;
		white_detect_4 <= white_detect_3;
		white_detect_5 <= white_detect_4;
	end
	

end

//Generate output messages for CPU
reg [31:0] msg_buf_in; 
wire [31:0] msg_buf_out;
reg msg_buf_wr;
wire msg_buf_rd, msg_buf_flush;
wire [7:0] msg_buf_size;
wire msg_buf_empty;

always@(*) begin	//Write words to FIFO as state machine advances
	case(msg_state)
		3'b000: begin
			msg_buf_in = 32'b0;
			msg_buf_wr = 1'b0;
		end
		3'b001: begin
			msg_buf_in = {5'b00001, red_x_min, 5'b0, red_x_max};
			msg_buf_wr = 1'b1;
		end
		3'b010: begin
			msg_buf_in = {5'b00010, green_x_min, 5'b0, green_x_max};
			msg_buf_wr = 1'b1;
		end
		3'b011: begin
			msg_buf_in = {5'b00011, blue_x_min, 5'b0, blue_x_max};
			msg_buf_wr = 1'b1;
		end
		3'b100: begin
			msg_buf_in = {5'b00100, yellow_x_min, 5'b0, yellow_x_max};
			msg_buf_wr = 1'b1;
		end
		3'b101: begin
			msg_buf_in = {5'b00101, teal_x_min, 5'b0, teal_x_max};
			msg_buf_wr = 1'b1;
		end
		3'b110: begin
			msg_buf_in = {5'b00110, fuchsia_x_min, 5'b0, fuchsia_x_max};
			msg_buf_wr = 1'b1;
		end
		3'b111: begin
			msg_buf_in = {5'b00111, wall_x_min, 5'b0, wall_x_max};
			msg_buf_wr = 1'b1;
		end
	endcase
end


//Output message FIFO
MSG_FIFO	MSG_FIFO_inst (
	.clock (clk),
	.data (msg_buf_in),
	.rdreq (msg_buf_rd),
	.sclr (~reset_n | msg_buf_flush),
	.wrreq (msg_buf_wr),
	.q (msg_buf_out),
	.usedw (msg_buf_size),
	.empty (msg_buf_empty)
	);


//Streaming registers to buffer video signal
STREAM_REG #(.DATA_WIDTH(26)) in_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(sink_ready),
	.valid_out(in_valid),
	.data_out({red,green,blue,sop,eop}),
	.ready_in(out_ready),
	.valid_in(sink_valid),
	.data_in({sink_data,sink_sop,sink_eop})
);

STREAM_REG #(.DATA_WIDTH(26)) out_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(out_ready),
	.valid_out(source_valid),
	.data_out({source_data,source_sop,source_eop}),
	.ready_in(source_ready),
	.valid_in(in_valid),
	.data_in({red_out, green_out, blue_out, sop, eop})
);


/////////////////////////////////
/// Memory-mapped port		 /////
/////////////////////////////////

// Addresses
`define REG_STATUS    			0
`define READ_MSG    				1
`define READ_ID    				2
`define REG_BBCOL					3

//Status register bits
// 31:16 - unimplemented
// 15:8 - number of words in message buffer (read only)
// 7:5 - unused
// 4 - flush message buffer (write only - read as 0)
// 3:0 - unused


// Process write

reg  [7:0]   reg_status;
reg	[23:0]	bb_col;

always @ (posedge clk)
begin
	if (~reset_n)
	begin
		reg_status <= 8'b0;
		bb_col <= BB_COL_DEFAULT;
	end
	else begin
		if(s_chipselect & s_write) begin
		   if      (s_address == `REG_STATUS)	reg_status <= s_writedata[7:0];
		   if      (s_address == `REG_BBCOL)	bb_col <= s_writedata[23:0];
		end
	end
end


//Flush the message buffer if 1 is written to status register bit 4
assign msg_buf_flush = (s_chipselect & s_write & (s_address == `REG_STATUS) & s_writedata[4]);


// Process reads
reg read_d; //Store the read signal for correct updating of the message buffer

// Copy the requested word to the output port when there is a read.
always @ (posedge clk)
begin
   if (~reset_n) begin
	   s_readdata <= {32'b0};
		read_d <= 1'b0;
	end
	
	else if (s_chipselect & s_read) begin
		if   (s_address == `REG_STATUS) s_readdata <= {16'b0,msg_buf_size,reg_status};
		if   (s_address == `READ_MSG) s_readdata <= {msg_buf_out};
		if   (s_address == `READ_ID) s_readdata <= 32'h1234EEE2;
		if   (s_address == `REG_BBCOL) s_readdata <= {8'h0, bb_col};
	end
	
	read_d <= s_read;
end

//Fetch next word from message buffer after read from READ_MSG
assign msg_buf_rd = s_chipselect & s_read & ~read_d & ~msg_buf_empty & (s_address == `READ_MSG);
						


endmodule


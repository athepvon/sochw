library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;
use ieee.numeric_bit.all;


ENTITY bayer2rgb is
	generic(bitsPerPixel : natural :=10;
			nbPixelPerLine: natural :=752;
			nbLines: natural :=480;
			bufferLineSize: natural:=1024);
	port(
	
	clk               : in std_logic;
	reset             : in std_logic;
	
	newframe 			: in std_logic;
	--newline 			: in std_logic;
	pixel_in 			: in std_logic_vector(bitsPerPixel-1 downto 0);
	pixelAvailable		: in std_logic;

	newframe_out 		: out std_logic;
	newline_out 		: out std_logic;
	pixel_out 			: out std_logic_vector(31 downto 0);
	pixelAvailable_out  : out std_logic;
	lastPixel_out		: out std_logic;
	lastPixel_out_ack : in std_logic
	
	
	);
end bayer2rgb; 

architecture arch of bayer2rgb is

	type bufferLine is array(bufferLineSize-1 downto 0) of std_logic_vector(bitsPerPixel-1 downto 0);
	
	
	signal pixelsBuffer_0 		: bufferLine;
	signal pixelsBuffer_1 		: bufferLine;
	signal pixelsBuffer_2 		: bufferLine;
	
	type pixelArray is array(2 downto 0) of std_logic_vector(bitsPerPixel-1 downto 0);--three pixels of n bits

	
	signal pixelsLine_0 		: pixelArray;
	signal pixelsLine_1		: pixelArray;
	signal pixelsLine_2 		: pixelArray; 
	
	signal col_reg		      : std_logic_vector(9 downto 0);
	signal line_reg		   : std_logic_vector(8 downto 0);
	
	type stateType is (waitBegin,twoLines_B,twoLines_G,
							 BGB_first_B,BGB_B,BGB_G,BGB_last_G,
							 GRG_first_G,GRG_G,GRG_R,GRG_last_R,
							 twoLines_end_R,twoLines_end_G,
							 twoLines_end_last_R,endFrame);
	signal state : stateType;
	
	signal nbPixels_reg: std_logic_vector(18 downto 0);
	
	signal lastLineShift: std_logic;
	
BEGIN


	process(clk,reset)
		variable FIFOhead : 	integer range 0 to bufferLineSize-1;
		variable FIFOtail :		integer range 0 to bufferLineSize-1;
	begin
	
		if(reset='1')then
			nbPixels_reg<=(others=>'0');			
			FIFOhead:=0;
			FIFOtail:=0;
		elsif(rising_edge(clk))then
	
			if(newFrame='1')then
				nbPixels_reg<=(others=>'0');
			end if;
			
			if(pixelAvailable='1' or lastLineShift='1')then
			
				FIFOtail:=FIFOtail+1;
				if(FIFOtail=nbPixelPerLine-2)then
					FIFOtail:=0;
				end if;

				pixelsBuffer_2(FIFOhead)<=pixel_in;
				pixelsBuffer_1(FIFOhead)<=pixelsLine_2(0);
				pixelsBuffer_0(FIFOhead)<=pixelsLine_1(0);

				pixelsLine_2(1 downto 0)<=pixelsLine_2(2 downto 1);
				pixelsLine_1(1 downto 0)<=pixelsLine_1(2 downto 1);
				pixelsLine_0(1 downto 0)<=pixelsLine_0(2 downto 1);

				FIFOhead:=FIFOhead+1;
				if(FIFOhead=nbPixelPerLine-2)then
					FIFOhead:=0;
				end if;
			
			end if;

			if(pixelAvailable='1')then
				nbPixels_reg<=nbPixels_reg+1;
			end if;	
			
				
			pixelsLine_2(2)<=pixelsBuffer_2(FIFOtail);
			pixelsLine_1(2)<=pixelsBuffer_1(FIFOtail);
			pixelsLine_0(2)<=pixelsBuffer_0(FIFOtail);
		end if;
		
		
		
	
	end process;


	process(clk,reset)
		variable red 	: std_logic_vector(2+bitsPerPixel-1 downto 0);
		variable green 	: std_logic_vector(2+bitsPerPixel-1 downto 0);
		variable blue 	: std_logic_vector(2+bitsPerPixel-1 downto 0);
		
	begin
	
		if(reset='1')then
			state<=waitBegin;
			lastPixel_out<='0';
		elsif(rising_edge(clk))then
	
			pixelAvailable_out<='0';
			newline_out<='0';
			newframe_out<='0';
			lastLineShift<='0';
			if lastpixel_out_ack = '1' then
				lastPixel_out<='0';
			end if;
			
			--new:
			if newframe = '1' then
				state<=waitBegin;
			end if;
			
			case state is

				when waitBegin =>
				
					line_reg<=(others=>'0');
					col_reg<=(others=>'0');

					if(nbPixels_reg=(nbPixelPerLine*2-2) and pixelAvailable='1')then
						state<=twoLines_B;
						
						newframe_out<='1';
						newline_out<='1';
					end if;
					
				when twoLines_B =>
					
					
					--	2	1	0
					--
					--	G	B	x	--2
					--	R	G	x	--1	
					--	x	x	x	--0
					blue:= "00" & pixelsLine_2(1);
							
					green:= "00" & pixelsLine_1(1);
							
					red:="00" & pixelsLine_1(2);

					if(pixelAvailable='1')then
						col_reg<=col_reg+1;
						state<=twoLines_G;				
						pixelAvailable_out<='1';
					end if;
					
				when twoLines_G =>
					
					
					if(col_reg/=nbPixelPerLine-2)then
				
						--
						--	2	1	0
						--
						--  G	B	G	--2
						--	R	G	R	--1
						--	x	x	x	--0

						blue:=	"00" & pixelsLine_2(1);
						green:=	"00" 	& pixelsLine_1(1);		
						red:=	"00" & pixelsLine_2(1);			
					else					
						--
						--	2	1	0
						--
						--  x	B	G	--2
						--	x	G	R	--1
						--	x	x	x	--0

						blue:=	"00" & pixelsLine_2(1);
						green:=	"00" 	& pixelsLine_1(1);		
						red:=	"00" & pixelsLine_1(0);				
					end if;

					
					if(pixelAvailable='1')then
						if(col_reg=(nbPixelPerLine-1))then
							state<=GRG_first_G;
							line_reg<=line_reg+1;
							col_reg<=(others=>'0');
						else
							col_reg<=col_reg+1;
							state<=twoLines_B;
						end if;
						pixelAvailable_out<='1';
					end if;
					
				when GRG_first_G =>
						
					--
					--	2	1	0
					--
					--  R	G	x	--2
					--	G	B	x	--1
					--	R	G	x	--0

					blue:= "00" & pixelsLine_1(1);
					green:= ( ("00" 	& pixelsLine_2(1))
							+ ("00" 	& pixelsLine_1(2)));
					green(bitsPerPixel-1 downto 0):=green(bitsPerPixel downto 1);
					
					red:= ( ("00" 	& pixelsLine_2(2))
							+ ("00" 	& pixelsLine_0(2)));
					red(bitsPerPixel-1 downto 0):=red(bitsPerPixel downto 1);
						
					
					if(pixelAvailable='1')then
						col_reg<=col_reg+1;
						state<=GRG_R;
						pixelAvailable_out<='1';
						newline_out<='1';
					end if;
												
				when GRG_R =>
				--
				--	G		R		G		//line 2
				--  B		G		B		//line 1
				--	G		R		G		//line 0			
	
					blue:= ( ("00" 	& pixelsLine_1(0))
							+ ("00" 	& pixelsLine_1(2)));
					blue(bitsPerPixel-1 downto 0):=blue(bitsPerPixel downto 1);
					
					red:= ( ("00" 	& pixelsLine_2(1))
							+ ("00" 	& pixelsLine_0(1)));
					red(bitsPerPixel-1 downto 0):=red(bitsPerPixel downto 1);
							
					green:= "00" & pixelsLine_1(1);
					
									
					if(pixelAvailable='1')then
						col_reg<=col_reg+1;
						state<=GRG_G;
						pixelAvailable_out<='1';
					else
						state<=GRG_R;
					end if;
					
				when GRG_G =>
				--
				--	R		G		R
				--  G		B		G
				--	R		G		R
					
					green:=	( ("00" 	& pixelsLine_2	(1))
								+ ("00" 	& pixelsLine_0	(1))
								+ ("00" 	& pixelsLine_1	(2))
								+ ("00" 	& pixelsLine_1	(0)));
								
					green(bitsPerPixel-1 downto 0):=green(bitsPerPixel+1 downto 2);
								
					red:=	(	("00" 	&	pixelsLine_2	(2))
							+ 	("00" 	&	pixelsLine_2	(0))
							+ 	("00" 	&	pixelsLine_0	(2))
							+ 	("00" 	&	pixelsLine_0	(0)));
								
					red(bitsPerPixel-1 downto 0):=red(bitsPerPixel+1 downto 2);
							
					blue:=	"00" 	&	pixelsLine_1(1);	


					if(pixelAvailable='1')then
						
						if(col_reg=(nbPixelPerLine-2))then
							state<=GRG_last_R;	
						else
							state<=GRG_R;
						end if;
						col_reg<=col_reg+1;
						pixelAvailable_out<='1';
					else
						state<=GRG_G;
					end if;
			
				when GRG_last_R =>
				--
				--	x		R		G		//line 2
				--  x		G		B		//line 1
				--	x		R		G		//line 0			
			
					green:="00" & pixelsLine_1(1);
					red:=	( ("00" 	& pixelsLine_2	(1))
							+ ("00" 	& pixelsLine_0	(1)));			
					red(bitsPerPixel-1 downto 0):=red(bitsPerPixel downto 1);
					blue:="00" & pixelsLine_1(0);

					
					if(pixelAvailable='1')then
						
						state<=BGB_first_B;
						pixelAvailable_out<='1';
						line_reg<=line_reg+1;
						col_reg<=(others=>'0');
						
						if(line_reg=nbLines-3)then
							lastLineShift<='1';
						end if;
					else
						state<=GRG_last_R;
					end if;				
				
				
				when BGB_first_B =>
				--
				--  G		B		x	--2
				--	R		G		x	--1
				--  G		B		x	--0
					
				
					green:="00" & pixelsLine_1(1);
					blue:=	( ("00" 	& pixelsLine_2	(1))
							+ ("00" 	& pixelsLine_0	(1)));
							
					blue(bitsPerPixel-1 downto 0):=blue(bitsPerPixel downto 1);
							
					red:=	"00" & pixelsLine_1(2);

					
					if(line_reg=nbLines-2)then
						state<=BGB_G;
						newLine_out<='1';
						pixelAvailable_out<='1';
						lastLineShift<='1';
						col_reg<=col_reg+1;
						blue(9 downto 0):=col_reg(9 downto 0);
					elsif(pixelAvailable='1')then
						state<=BGB_G;		
						newLine_out<='1';
						pixelAvailable_out<='1';
						col_reg<=col_reg+1;
					else
						state<=BGB_first_B;
					end if;
										
				when BGB_G =>
				--
				--  B		G		B
				--	G		R		G
				--  B		G		B

					green:=	( ("00" 	& pixelsLine_2	(1))
							+ ("00" 	& pixelsLine_0	(1))
							+ ("00" 	& pixelsLine_1	(2))
							+ ("00" 	& pixelsLine_1	(0)));
								
					green(bitsPerPixel-1 downto 0):=green(bitsPerPixel+1 downto 2);
								
					blue:=	(	("00" 	&	pixelsLine_2	(2))
							+ 	("00" 	&	pixelsLine_2	(0))
							+ 	("00" 	&	pixelsLine_0	(2))
							+ 	("00" 	&	pixelsLine_0	(0)));
								
					blue(bitsPerPixel-1 downto 0):=blue(bitsPerPixel+1 downto 2);
						
							
					red:=	"00" 	&	pixelsLine_1(1);

							
					if(line_reg=nbLines-2)then
						state<=BGB_B;
						pixelAvailable_out<='1';
						lastLineShift<='1';
						col_reg<=col_reg+1;
					elsif(pixelAvailable='1')then
						state<=BGB_B;
						pixelAvailable_out<='1';
						col_reg<=col_reg+1;
					else
						state<=BGB_G;
					end if;
				
				when BGB_B =>
				--
				--  G		B		G
				--	R		G		R
				--  G		B		G
					
					if(col_reg<nbPixelPerLine-1)then
						red:= ( ("00" 	& pixelsLine_1(0))
							+ ("00" 	& pixelsLine_1(2)));
					   red(bitsPerPixel-1 downto 0):=red(bitsPerPixel downto 1);
					
					   blue:= ( ("00" 	& pixelsLine_2(1))
						  	+ ("00" 	& pixelsLine_0(1)));
					   blue(bitsPerPixel-1 downto 0):=blue(bitsPerPixel downto 1);
							
					   green:= "00" & pixelsLine_1(1);
						
								
					end if;
					
				
					if(line_reg=nbLines-2)then
						if(col_reg=(nbPixelPerLine-2))then
							state<=BGB_last_G;
						else
							state<=BGB_G;
						end if;
						col_reg<=col_reg+1;
						pixelAvailable_out<='1';
						lastLineShift<='1';
					elsif(pixelAvailable='1')then
						if(col_reg=(nbPixelPerLine-2))then
							state<=BGB_last_G;
						else
							state<=BGB_G;
						end if;
						col_reg<=col_reg+1;
						pixelAvailable_out<='1';
					else
						state<=BGB_B;
					end if;
					
				when BGB_last_G =>
				--
				--	x		x		x	--2
				--	x		R		G	--1
				--  x		G		B	--0


					green:= ( ("00" 	& pixelsLine_1(0))
						  	+ ("00" 	& pixelsLine_0(1)));
					green(bitsPerPixel-1 downto 0):=green(bitsPerPixel downto 1);
					blue:=	("00" 	& 	pixelsLine_0	(0));	
					red:=	("00" 	&	pixelsLine_1	(1));
					
					if(line_reg=nbLines-2)then
						state<=twoLines_end_G;
						pixelAvailable_out<='1';
						lastLineShift<='1';
						line_reg<=line_reg+1;
						col_reg<=(others=>'0');
					elsif(pixelAvailable='1')then			
						state<=GRG_first_G;
						pixelAvailable_out<='1';
						line_reg<=line_reg+1;
						col_reg<=(others=>'0');
					else
						state<=BGB_last_G;
					end if;
					

				when twoLines_end_G =>
				--
				--	x		x		x	--2
				--	G		B		x	--1
				--  R		G		x	--0
				

					if(col_reg=0)then
						newLine_out<='1';
					end if;
					
					lastLineShift<='1';
					
					green:= ( ("00" 	& pixelsLine_1(2))
						  	+ ("00" 	& pixelsLine_0(1)));
					green(bitsPerPixel-1 downto 0):=green(bitsPerPixel downto 1);
					blue:=	("00" 	& 	pixelsLine_1	(1));	
					red:=	("00" 	&	pixelsLine_0	(2));

			
					if(col_reg=(nbPixelPerLine-2))then
						state<=twoLines_end_last_R;
						col_reg<=col_reg+1;
					else
						state<=twoLines_end_R;
						col_reg<=col_reg+1;
					end if;
					
					pixelAvailable_out<='1';
		
					
				when twoLines_end_R =>
				--
				--	x		x		x	--2
				--	B		G		B	--1
				--  G		R		x	--0
				
					
					lastLineShift<='1';
					
					green:=	("00" 	& 	pixelsLine_1	(1));	
					
					blue:= ( ("00" 	& pixelsLine_1	(2))
							+ ("00" 	& pixelsLine_1	(0)));
					blue(bitsPerPixel-1 downto 0):=blue(bitsPerPixel downto 1);
					red:=	("00" 	&	pixelsLine_0	(1));
		
					
					state<=twoLines_end_G;
					pixelAvailable_out<='1';
					col_reg<=col_reg+1;
			
				
				when twoLines_end_last_R =>
				--
				--	x		x		x	--2
				--	x		G		B	--1
				--  x		R		G	--0
				
				
					lastLineShift<='1';

					
					green:=	("00" 	& 	pixelsLine_1	(1));	
					blue:=	("00" 	& 	pixelsLine_1	(0));	
					red:=	("00" 	&	pixelsLine_0	(1));
					
					
					state<=endFrame;--waitBegin;
					pixelAvailable_out<='1';
					col_reg<=(others=>'0');
--					lastPixel_out<='1';

			when endFrame =>
				lastPixel_out <= '1';
				state<=waitBegin;
				
			end case;


		--new:
--		pixel_out(bitsPerPixel-1 downto 0)					<=	red(bitsPerPixel-1 downto 0);
--		pixel_out(2*bitsPerPixel-1 downto bitsPerPixel)		<=	green(bitsPerPixel-1 downto 0);
--		pixel_out(3*bitsPerPixel-1 downto 2*bitsPerPixel)	<= 	blue(bitsPerPixel-1 downto 0);
		pixel_out <= x"00" & blue(bitsPerPixel-1 downto 2)
						       & green(bitsPerPixel-1 downto 2)
								 & red(bitsPerPixel-1 downto 2);
	
		end if;
	
	end process;

END arch; 

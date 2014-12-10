library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;
use ieee.numeric_bit.all;


ENTITY sdi_bayer2rgb  is
	generic(bitsPerPixel : natural :=10;
			nbPixelPerLine: natural :=640;
			nbLines: natural :=480;
			bufferLineSize: natural:=2048);
	port(
	clk               : in std_logic;
	rst             : in std_logic;
	pu_reset          : in std_logic;
	sof  : in std_logic;
        sof_out  : out std_logic;
	eof_out  : out std_logic;
	eof_ack : in std_logic;
	stream_in_data		: in std_logic_vector(bitsPerPixel-1 downto 0);
	stream_in_valid	: in std_logic;
	stream_in_stop    : out std_logic;
	stream_out_data	: out std_logic_vector(63 downto 0);
	stream_out_valid  : out std_logic;
	stream_out_stop   : in std_logic	
	);
end sdi_bayer2rgb ; 

--architecture arch of sdi_bayer2rgb is
--
--	component bayer2rgb
--	generic(bitsPerPixel : natural :=10;
--			nbPixelPerLine: natural :=640;
--			nbLines: natural :=480;
--			bufferLineSize: natural:=1024);
--	port(
--	
--	clk               : in std_logic;
--	reset             : in std_logic;
--	
--	newframe 			: in std_logic;
----	newline 			: in std_logic;
--	pixel_in 			: in std_logic_vector(bitsPerPixel-1 downto 0);
--	pixelAvailable		: in std_logic;
--
--	newframe_out 		: out std_logic;
--	newline_out 		: out std_logic;
--	pixel_out 			: out std_logic_vector(31 downto 0);
--	pixelAvailable_out  : out std_logic;
--	lastPixel_out		: out std_logic;
--	lastPixel_out_ack	: in std_logic
--	
--	
--	);
--end component; 
--	
--	signal data_out_s : std_logic_vector(63 downto 0);
--	
--	signal reset : std_logic;
--	signal out_count : std_logic;
--	signal pixelAvailable_out_s : std_logic;
--	signal pixel_out_s : std_logic_vector(31 downto 0);
--	
--BEGIN
--
--	reset <= rst or pu_reset;
--	CORE :  bayer2rgb
--	generic map(bitsPerPixel => bitsPerPixel,
--			nbPixelPerLine => nbPixelPerLine,
--			nbLines => nbLines,
--			bufferLineSize => bufferLineSize)
--	port map(
--	clk => clk,
--	reset => reset,
--	newframe => sof,
--	--newline => open,
--	pixel_in =>	stream_in_data,
--	pixelAvailable	=> stream_in_valid,
--	newframe_out => open,
--	newline_out => open,
--	pixel_out => pixel_out_s,
--	pixelAvailable_out => pixelAvailable_out_s,
--	lastPixel_out => eof,
--	lastPixel_out_ack => eof_ack
--	);
--
--
--	--out process
--	process(clk,rst,pu_reset)
--	begin
--		if rst='1' or pu_reset='1' then
--			out_count <= '0';
--			data_out_s <= (others => '0');
--			stream_out_valid <= '0';
--		elsif rising_edge(clk) then
--			stream_out_valid <= '0';
--			if pixelAvailable_out_s = '1' then
--				if out_count = '0' then
--					data_out_s(31 downto 0) <= pixel_out_s;
--					out_count <= '1';
--				else
--					data_out_s(63 downto 32) <= pixel_out_s;
--					out_count <= '0';
--					stream_out_valid <= '1';
--				end if;
--			end if;
--		end if;
--	end process;
--	
--	stream_in_stop <= stream_out_stop;
--	stream_out_data <= data_out_s;
--
--END arch;

--architecture arch of sdi_bayer2rgb is
--
--	type PixelArray is array(2 downto 0) of std_logic_vector(9 downto 0);
--	signal line0, line1, line2 : PixelArray;
--	
--	type STATES is (IDLE,GR,BG,INC_ADDR_GR,INC_ADDR_BG);
--	signal state : STATES;
--	
--	signal addrGR, addrBG : std_logic_vector(10 downto 0);
--	signal weaGR, weaBG : std_logic_vector(0 downto 0);
--	signal doutGR, doutBG : std_logic_vector(9 downto 0);
--	signal x : integer range 0 to 2048;
--	signal y : integer range 0 to 1536;
--	
--	component Bit10RAM port (
--	clka: IN std_logic;
--	dina: IN std_logic_VECTOR(9 downto 0);
--	addra: IN std_logic_VECTOR(10 downto 0);
--	wea: IN std_logic_VECTOR(0 downto 0);
--	douta: OUT std_logic_VECTOR(9 downto 0));
--	end component;
--	
--begin
--
--	GR_RAM : Bit10RAM
--		port map (
--			clka => clk,
--			dina => stream_in_data,
--			addra => addrGR,
--			wea => weaGR,
--			douta => doutGR);
--			
--	BG_RAM : Bit10RAM
--		port map (
--			clka => clk,
--			dina => stream_in_data,
--			addra => addrBG,
--			wea => weaBG,
--			douta => doutBG);
--			
--	stream_in_stop <= stream_out_stop;
--			
--	STD_PROC : process(clk,rst,pu_reset)
--		variable blue,red,green : std_logic_vector(11 downto 0);
--	begin
--		
--		if rst='1' or pu_reset='1' then
--			state <= IDLE;
--			eof <= '0';
--		elsif rising_edge(clk) then
--			weaGR(0) <= '0';
--			weaBG(0) <= '0';
--			stream_out_valid <= '0';
--			if eof_ack = '1' then
--				eof <= '0';
--			end if;
--			
--			case state is
--			when IDLE =>
--				if sof='1' then
--					state <= GR;
--					x <= 0;
--					y <= 0;
--					addrGR <= (others => '0');
--					addrBG <= (others => '0');
--				end if;
--				
--			when GR =>
--				if stream_in_valid = '1' then
--					--new pixel
--					line2 <= line2(1 downto 0) & doutGR;
--					line1 <= line1(1 downto 0) & doutBG;
--					line0 <= line0(1 downto 0) & stream_in_data;
--					weaGR(0)<='1';
--					if addrGR(0) = '0' then --on G pixel
--						if x<2 or y<2 then
--							stream_out_data(31 downto 0) <= x"00000000";
--						else
--							blue := ("00"&line1(2)) + ("00"&line1(0));
--							green := "00"&line1(1);
--							red := ("00"&line2(1)) + ("00"&line0(1));
--							stream_out_data(31 downto 0) <= x"00" & blue(10 downto 3)
--																		& green(9 downto 2)
--																		& red(10 downto 3);
--						end if;
--					else --on R pixel
--						if x<2 or y<2 then
--							stream_out_data(63 downto 32) <= x"00000000";
--						else
--							blue  := "00"&line1(1);
--							green := ("00"&line2(1)) + ("00"&line1(2)) + ("00"&line1(0)) + ("00"&line0(1));
--							red   := ("00"&line2(2)) + ("00"&line2(0)) + ("00"&line0(2)) + ("00"&line0(0));
--							stream_out_data(63 downto 32) <= x"00" & blue(9 downto 2)
--																		& green(11 downto 4)
--																		& red(11 downto 4);
--						end if;
--						stream_out_valid <= '1';
--					end if;
--					
--					state <= INC_ADDR_GR;
--				end if;
--				
--			when INC_ADDR_GR =>
--				if x < nbPixelPerLine-1 then
--					x <= x+1;
--					addrGR <= addrGR + 1;
--					addrBG <= addrBG + 1;
--					state <= GR;
--				elsif y<nbLines-1 then
--					x <= 0;
--					y <= y+1;
--					addrGR <= (others => '0');
--					addrBG <= (others => '0');
--					state <= BG;
--				else
--					state <= IDLE;
--					eof <= '1';
--				end if;
--					
--			when BG =>
--				if stream_in_valid = '1' then
--					--new pixel
--					line2 <= line2(1 downto 0) & doutBG;
--					line1 <= line1(1 downto 0) & doutGR;
--					line0 <= line0(1 downto 0) & stream_in_data;
--					weaBG(0)<='1';
--					if addrBG(0) = '0' then --on B pixel
--						if x<2 or y<2 then
--							stream_out_data(31 downto 0) <= x"00000000";
--						else
--							blue  := ("00"&line2(2)) + ("00"&line2(0)) + ("00"&line0(2)) + ("00"&line0(0));
--							green := ("00"&line2(1)) + ("00"&line1(2)) + ("00"&line1(0)) + ("00"&line0(1));
--							red   := "00"&line1(1);
--							stream_out_data(31 downto 0) <= x"00" & blue(11 downto 4)
--																		& green(11 downto 4)
--																		& red(9 downto 2);
--						end if;
--					else --on G pixel
--						if x<2 or y<2 then
--							stream_out_data(63 downto 32) <= x"00000000";
--						else
--							blue  := ("00"&line2(1)) + ("00"&line0(1));
--							red   := ("00"&line1(2)) + ("00"&line1(0));
--							green := "00"&line1(1);
--							stream_out_data(63 downto 32) <= x"00" & blue(10 downto 3)
--																		& green(10 downto 3)
--																		& red(9 downto 2);
--						end if;
--						stream_out_valid <= '1';
--					end if;
--					
--					state <= INC_ADDR_BG;
--				end if;
--			
--			when INC_ADDR_BG =>
--				if x < nbPixelPerLine-1 then
--					x <= x+1;
--					addrGR <= addrGR + 1;
--					addrBG <= addrBG + 1;
--					state <= BG;
--				elsif y<nbLines-1 then
--					x <= 0;
--					y <= y+1;
--					addrGR <= (others => '0');
--					addrBG <= (others => '0');
--					state <= GR;
--				else
--					state <= IDLE;
--					eof <= '1';
--				end if;
--			
--			end case;
--						
--		end if;
--		
--	end process;
--
--END arch; 

architecture arch of sdi_bayer2rgb  is

	type PixelArray is array(1 downto 0) of std_logic_vector(9 downto 0);
	signal line0, line1 : PixelArray;
	
	type STATES is (GR,BG);
	signal state : STATES;
	
	signal addr : std_logic_vector(9 downto 0);
	signal wea : std_logic_vector(0 downto 0);
	signal dout : std_logic_vector(9 downto 0);
	signal x : integer range 0 to 752;
	signal y : integer range 0 to 480;
	
component Bit10RAM
  PORT (
    clka : IN STD_LOGIC;
    wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addra : IN STD_LOGIC_VECTOR(9 DOWNTO 0);
    dina : IN STD_LOGIC_VECTOR(9 DOWNTO 0);
    douta : OUT STD_LOGIC_VECTOR(9 DOWNTO 0)
  );
	end component;
	function times77( a : std_logic_vector(7 downto 0) )
	return std_logic_vector is
	variable ret : std_logic_vector(15 downto 0);
begin
	--77 = 64 + 8 + 4 + 1
	ret :=   ("00"&a&"000000") --64
	       + ("00000"&a&"000") -- 8
			 + ("000000"&a&"00") -- 4
			 + ("00000000"&a);   -- 1
	return ret;
end times77;

function times151( a : std_logic_vector(7 downto 0) )
	return std_logic_vector is
	variable ret : std_logic_vector(15 downto 0);
begin
	--151 = 128 + 16 + 4 + 2 + 1
	ret :=   ("0"&a&"0000000") --128
	       + ("0000"&a&"0000") -- 16
			 + ("000000"&a&"00") -- 4
		 	 + ("0000000"&a&"0") -- 2
		 	 + ("00000000"&a);   -- 1
	return ret;
end times151;

function times28( a : std_logic_vector(7 downto 0) )
	return std_logic_vector is
	variable ret : std_logic_vector(15 downto 0);
begin
	--28 = 16 + 8 + 4
	ret :=   ("0000"&a&"0000")  -- 16
			 + ("00000"&a&"000")  -- 8
			 + ("000000"&a&"00"); -- 4
	return ret;
end times28;
begin
        sof_out<=sof;
	PIXEL_RAM : Bit10RAM
		port map (
			clka => clk,
			dina => stream_in_data,
			addra => addr,
			wea => (others => stream_in_valid),--wea,
			douta => dout);
			
	stream_in_stop <= stream_out_stop;
			
	STD_PROC : process(clk,rst,pu_reset)
		variable blue,red,green : std_logic_vector(11 downto 0);
		variable gray0 : std_logic_vector(15 downto 0);
	begin
		
		if rst='1' or pu_reset='1' then
			--state <= IDLE;
			state <= GR;
					x <= 0;
					y <= 0;
					addr <= (others => '0');
			eof_out <= '0';
		elsif rising_edge(clk) then
			wea(0) <= '0';
			stream_out_valid <= '0';
			if eof_ack = '1' then
				eof_out <= '0';
			end if;
			
			case state is
--			when IDLE =>
--				if sof='1' then
--					state <= GR;
--					x <= 0;
--					y <= 0;
--					addr <= (others => '0');
--				end if;
				
			when GR =>
				if stream_in_valid = '1' then
					--new pixel
					line1 <= line1(0) & dout;
					line0 <= line0(0) & stream_in_data;
					wea(0)<='1';
					if addr(0) = '0' then --on G pixel
						if x<1 or y<1 then
							stream_out_data(31 downto 0) <= x"00000000";
						else
							blue  := "00"&dout;
							green := "00"&line1(0) + ("00"&stream_in_data);
							red   := "00"&line0(0);
							gray0 := times77(red(9 downto 2)) + times151(green(10 downto 3)) + times28(blue(9 downto 2));
							--stream_out_data(31 downto 0) <= x"00" & blue(9 downto 2)& green(10 downto 3)& red(9 downto 2);
							stream_out_data(31 downto 0) <= x"00" & blue(9 downto 2)& green(10 downto 3)& gray0(15 downto 8);
						end if;
					else --on R pixel
						if x<1 or y<1 then
							stream_out_data(63 downto 32) <= x"00000000";
						else
							blue  := "00"&line1(0);
							green := "00"&line0(0) + ("00"&dout);
							red   := "00"&stream_in_data;
							gray0 := times77(red(9 downto 2)) + times151(green(10 downto 3)) + times28(blue(9 downto 2));
							--	stream_out_data(63 downto 32) <= x"00" & blue(9 downto 2)& green(10 downto 3)& red(9 downto 2);
							stream_out_data(63 downto 32) <= x"00" & blue(9 downto 2)& green(10 downto 3)& gray0(15 downto 8);


						end if;
						stream_out_valid <= '1';
					end if;
					
					--state <= INC_ADDR_GR;
					if x < nbPixelPerLine-1 then
					x <= x+1;
					addr <= addr + 1;
					state <= GR;
				elsif y<nbLines-1 then
					x <= 0;
					y <= y+1;
					addr <= (others => '0');
					state <= BG;
				else
					--state <= IDLE;
					state <= GR;
					x <= 0;
					y <= 0;
					addr <= (others => '0');
					eof_out <= '1';
				end if;
				end if;
				
--			when INC_ADDR_GR =>
--				if x < nbPixelPerLine-1 then
--					x <= x+1;
--					addr <= addr + 1;
--					state <= GR;
--				elsif y<nbLines-1 then
--					x <= 0;
--					y <= y+1;
--					addr <= (others => '0');
--					state <= BG;
--				else
--					state <= IDLE;
--					eof_out <= '1';
--				end if;
					
			when BG =>
				if stream_in_valid = '1' then
					--new pixel
					line1 <= line1(0) & dout;
					line0 <= line0(0) & stream_in_data;
					wea(0)<='1';
					if addr(0) = '0' then --on B pixel
						if x<1 or y<1 then
							stream_out_data(31 downto 0) <= x"00000000";--
						else
							blue  := "00"&stream_in_data;
							green := "00"&line0(0) + ("00"&dout);
							red   := "00"&line1(0);
							gray0 := times77(red(9 downto 2)) + times151(green(10 downto 3)) + times28(blue(9 downto 2));
							--stream_out_data(31 downto 0) <= x"00" & blue(9 downto 2)& green(10 downto 3)& red(9 downto 2);
							stream_out_data(31 downto 0) <= x"00" & blue(9 downto 2)& green(10 downto 3)& gray0(15 downto 8);

						end if;
					else --on G pixel
						if x<1 or y<1 then
							stream_out_data(63 downto 32) <= x"00000000";
						else
							blue  := "00"&line0(0);
							green := "00"&line1(0) + ("00"&stream_in_data);
							red   := "00"&dout;
							gray0 := times77(red(9 downto 2)) + times151(green(10 downto 3)) + times28(blue(9 downto 2));
							--	stream_out_data(63 downto 32) <= x"00" & blue(9 downto 2)& green(10 downto 3)& red(9 downto 2);
							stream_out_data(63 downto 32) <= x"00" & blue(9 downto 2)& green(10 downto 3)& gray0(15 downto 8);

						end if;
						stream_out_valid <= '1';
					end if;
					
					--state <= INC_ADDR_BG;
					if x < nbPixelPerLine-1 then
					x <= x+1;
					addr <= addr + 1;
					state <= BG;
				elsif y<nbLines-1 then
					x <= 0;
					y <= y+1;
					addr <= (others => '0');
					state <= GR;
				else
					--state <= IDLE;
					state <= GR;
					x <= 0;
					y <= 0;
					addr <= (others => '0');
					eof_out <= '1';
				end if;
				end if;
--				
--			when INC_ADDR_BG =>
--				if x < nbPixelPerLine-1 then
--					x <= x+1;
--					addr <= addr + 1;
--					state <= BG;
--				elsif y<nbLines-1 then
--					x <= 0;
--					y <= y+1;
--					addr <= (others => '0');
--					state <= GR;
--				else
--					state <= IDLE;
--					eof_out <= '1';
--				end if;
			
			end case;
						
		end if;
		
	end process;

END arch; 

--architecture arch of sdi_bayer2rgb is
--	signal out_cnt : std_logic;
--	signal color : std_logic_vector(31 downto 0);
--begin
--
--	stream_in_stop <= stream_out_stop;
--	color <= x"00" & stream_in_data(9 downto 2)
--						& stream_in_data(9 downto 2)
--						& stream_in_data(9 downto 2);
--	STD_PROC : process(clk,rst,pu_reset)
--	begin
--		if rst='1' or pu_reset='1' then
--			out_cnt <= '0';
--		elsif rising_edge(clk) then
--			stream_out_valid <= '0';
--			if stream_in_valid = '1' then
--				if out_cnt = '0' then
--					stream_out_data(31 downto 0) <= color;
--					out_cnt <= '1';
--				else
--					stream_out_data(63 downto 32) <= color;
--					out_cnt <= '1';
--					stream_out_valid <= '1';
--				end if;
--			end if;
--		end if;
--	end process;
--end arch;

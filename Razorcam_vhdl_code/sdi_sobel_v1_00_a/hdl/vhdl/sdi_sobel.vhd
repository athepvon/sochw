library ieee;
  use ieee.std_logic_1164.all;
  use ieee.std_logic_unsigned.all;
  use ieee.std_logic_arith.all;
  
entity sdi_sobel is
generic(
	C_IMAGE_WIDTH : integer := 640;
	mode :integer;
	C_IMAGE_HEIGHT : integer := 480
	);
port(
	-- Streaming I/O
    rst               : in std_logic;
    clk               : in std_logic;
	 pu_reset : in std_logic;
	 stream_in_stop3   : out std_logic;
    stream_in_valid3  : in std_logic;
    stream_in_data3   : in std_logic_vector(63 downto 0);
	 stream_out_stop3   : in std_logic;
    stream_out_valid3  : out std_logic;
    stream_out_data3   : out std_logic_vector(63 downto 0);
	 sof: in std_logic
);
end sdi_sobel;

architecture behavior of sdi_sobel is
signal stream_in_stop   : std_logic;
signal stream_in_valid  : std_logic;
signal stream_in_data   : std_logic_vector(7 downto 0);
signal stream_out_dx_stop   : std_logic;
signal stream_out_dx_valid  :  std_logic;
signal stream_out_dx_data   :  std_logic_vector(7 downto 0);
signal stream_out_dy_stop   :  std_logic;
signal stream_out_dy_valid  :  std_logic;
signal stream_out_dy_data   :  std_logic_vector(7 downto 0);
signal stream_out_image_stop   :  std_logic;
signal stream_out_image_valid  :  std_logic;
signal stream_out_image_data   :  std_logic_vector(7 downto 0);
 component fifo_generator_v4_4_8_16
  PORT (
    rst : IN STD_LOGIC;
    wr_clk : IN STD_LOGIC;
    rd_clk : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC;
    valid : OUT STD_LOGIC;
    rd_data_count : OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
    wr_data_count : OUT STD_LOGIC_VECTOR(8 DOWNTO 0)
  );
END component;
component fifo_generator_v4_4_16_8
  PORT (
    rst : IN STD_LOGIC;
    wr_clk : IN STD_LOGIC;
    rd_clk : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(15 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC;
    valid : OUT STD_LOGIC;
    rd_data_count : OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
    wr_data_count : OUT STD_LOGIC_VECTOR(8 DOWNTO 0)
  );
END component;
component BIT8_RAM
	port (
	clka: IN std_logic;
	dina: IN std_logic_VECTOR(7 downto 0);
	addra: IN std_logic_VECTOR(9 downto 0);
	wea: IN std_logic_VECTOR(0 downto 0);
	douta: OUT std_logic_VECTOR(7 downto 0));
end component;

--signal x : integer range 0 to C_IMAGE_WIDTH;
signal y : integer range 0 to C_IMAGE_HEIGHT;

signal addr : std_logic_vector(9 downto 0);
signal wea0,wea1,wea2 : std_logic_vector(0 downto 0);
signal dout0, dout1, dout2 : std_logic_vector(7 downto 0);
signal data16 : STD_LOGIC_VECTOR(15 DOWNTO 0);
type pixelArray is array(2 downto 0) of std_logic_vector(7 downto 0);--three pixels of n bits
signal lastPixels0 : pixelArray;
signal lastPixels1 : pixelArray;
signal lastPixels2 : pixelArray; 

type STATES is (IDLE,PIXEL);--GET_SOBEL);
signal state : STATES;

signal activeLine : integer range 0 to 2;
--init is 1 for the first two lines
--signal init : std_logic;
signal     empty1 : STD_LOGIC;
signal     rst1 : STD_LOGIC;
signal     full : STD_LOGIC;
begin

--stream_in_stop3<= full or stream_out_stop3;
rst1<=rst or stream_in_stop;
DATA_CAPTURE_FIFO_WRITER1 : fifo_generator_v4_4_16_8
		port map (
			din => stream_in_data3(39 downto 32) & stream_in_data3(7 downto 0),--conv_std_logic_vector((conv_integer(stream_in_data3(39 downto 32))) + (conv_integer(stream_in_data3(47 downto 40))) + (conv_integer(stream_in_data3(55 downto 48))),8) & conv_std_logic_vector((conv_integer(stream_in_data3(7 downto 0))) + (conv_integer(stream_in_data3(15 downto 8))) + (conv_integer(stream_in_data3(23 downto 16))),8),--
			rd_clk => clk,
			rd_en =>not stream_in_stop,
			rst => rst1,
			wr_clk =>clk,
			wr_en => stream_in_valid3,
			dout => stream_in_data,
			empty => open,
			full => stream_in_stop3,--full,
			valid=>stream_in_valid,
         rd_data_count => open,
         wr_data_count => open ); 
			
image:if (mode = 0) generate
DATA_CAPTURE_FIFO_WRITER : fifo_generator_v4_4_8_16
		port map (
			din => stream_out_image_data(7 downto 0),
			rd_clk => clk,
			rd_en => (not empty1),--'1',
			rst => rst1,
			wr_clk =>clk,
			wr_en => stream_out_image_valid,
			dout => data16,
			empty => empty1,
			valid=>stream_out_valid3,
			full => open,
         rd_data_count => open,
         wr_data_count => open );
			stream_out_data3<=X"000000" & data16(15 downto 8) & X"000000" & data16(7 downto 0) ; 
end generate;
sobelx: if (mode =1) generate
DATA_CAPTURE_FIFO_WRITER : fifo_generator_v4_4_8_16
		port map (
			din => stream_out_dx_data(7 downto 0),
			rd_clk => clk,
			rd_en =>(not empty1),--'1',
			rst => rst1,
			wr_clk =>clk,
			wr_en => stream_out_dx_valid,
			dout => data16,
			empty => empty1,
			valid=>stream_out_valid3,
			full => open,
         rd_data_count => open,
         wr_data_count => open );
			stream_out_data3<=X"000000" & data16(15 downto 8) & X"000000" & data16(7 downto 0) ; 
end generate;
sobely:if (mode = 2) generate
DATA_CAPTURE_FIFO_WRITER : fifo_generator_v4_4_8_16
		port map (
			din => stream_out_dy_data(7 downto 0),
			rd_clk => clk,
			rd_en =>(not empty1),--'1',
			rst => rst1,
			wr_clk =>clk,
			wr_en => stream_out_dy_valid,
			dout => data16,
			empty => empty1,
			valid=>stream_out_valid3,
			full => open,
         rd_data_count => open,
         wr_data_count => open );
			stream_out_data3<=X"000000" & data16(15 downto 8) & X"000000" & data16(7 downto 0) ; 
end generate;
RAM0 : BIT8_RAM
		port map (
			clka => clk,
			dina => stream_in_data,
			addra => addr,
			wea => wea0,
			douta => dout0);
			
RAM1 : BIT8_RAM
		port map (
			clka => clk,
			dina => stream_in_data,
			addra => addr,
			wea => wea1,
			douta => dout1);
			
RAM2 : BIT8_RAM
		port map (
			clka => clk,
			dina => stream_in_data,
			addra => addr,
			wea => wea2,
			douta => dout2);

wea0(0) <= '1' when activeLine = 0 and stream_in_valid = '1' else '0';
wea1(0) <= '1' when activeLine = 1 and stream_in_valid = '1' else '0';
wea2(0) <= '1' when activeLine = 2 and stream_in_valid = '1' else '0';

stream_in_stop <= stream_out_stop3;
stream_out_dx_stop<=stream_out_stop3  ;
stream_out_dy_stop<=stream_out_stop3 ;
stream_out_image_stop<=stream_out_stop3 ;

STD_PROC : process (clk,rst,pu_reset)
	variable lineDx0,lineDx2,diffDx : std_logic_vector(9 downto 0);
	variable lineDy0,lineDy2,diffDy : std_logic_vector(9 downto 0);
begin

	if rst1='1' or pu_reset='1' then
		state <= IDLE;
--		wea0(0) <= '0';
--		wea1(0) <= '0';
--		wea2(0) <= '0';
--		init <= '1';
	elsif rising_edge(clk) then
--		wea0(0) <= '0';
--		wea1(0) <= '0';
--		wea2(0) <= '0';
		stream_out_dx_valid <= '0';
		stream_out_dy_valid <= '0';
		stream_out_image_valid <= '0';
		
		case state is
		when IDLE =>
			if sof = '1' then
				addr <= ( others => '0' );
--				x <= 0;
				y <= 0;
				activeLine <= 0;
--				init <= '1';
				state <= PIXEL;
			end if;
			
		when PIXEL =>
			if stream_in_valid = '1' then
				--and store current pixel and shift pixel window
				lineDx0 := ("00"&lastPixels0(1)) + ("0"&lastPixels1(1)&"0") + ("00"&lastPixels2(1));
				lineDy0 := ("00"&lastPixels2(1)) + ("0"&lastPixels2(0)&"0") + ("00"&stream_in_data);
				case activeLine is
				when 0 =>
--					wea0(0) <= '1';
					lastPixels0 <= lastPixels0(1 downto 0) & dout1;
					lastPixels1 <= lastPixels1(1 downto 0) & dout2;
					lastPixels2 <= lastPixels2(1 downto 0) & stream_in_data;
					lineDx2 := ("00"&dout1) + ("0"&dout2&"0") + ("00"&stream_in_data);
					lineDy2 := ("00"&lastPixels0(1)) + ("0"&lastPixels0(0)&"0") + ("00"&dout1);
				when 1 =>
--					wea1(0) <= '1';
					lastPixels0 <= lastPixels0(1 downto 0) & dout2;
					lastPixels1 <= lastPixels1(1 downto 0) & dout0;
					lastPixels2 <= lastPixels2(1 downto 0) & stream_in_data;
					lineDx2 := ("00"&dout2) + ("0"&dout0&"0") + ("00"&stream_in_data);
					lineDy2 := ("00"&lastPixels0(1)) + ("0"&lastPixels0(0)&"0") + ("00"&dout2);
				when 2 =>
--					wea2(0) <= '1';
					lastPixels0 <= lastPixels0(1 downto 0) & dout0;
					lastPixels1 <= lastPixels1(1 downto 0) & dout1;
					lastPixels2 <= lastPixels2(1 downto 0) & stream_in_data;
					lineDx2 := ("00"&dout0) + ("0"&dout1&"0") + ("00"&stream_in_data);
					lineDy2 := ("00"&lastPixels0(1)) + ("0"&lastPixels0(0)&"0") + ("00"&dout0);
				end case;
			
				--get new sobel
				stream_out_dx_valid <= '1';
				stream_out_dy_valid <= '1';
				stream_out_image_valid <= '1';
				
				stream_out_image_data <= stream_in_data;
				if y<2 or addr<2 then
--				if init = '1' or addr < 2 then
					stream_out_dx_data <= "00000000";
					stream_out_dy_data <= "00000000";
				else
					
					--|-1  0  1| lastPixels0
					--|-2  0  2| lastPixels1
					--|-1  0  1| lastPixels2
--					lineDx0 := ("00"&lastPixels0(2)) + ("0"&lastPixels1(2)&"0") + ("00"&lastPixels2(2));
--					lineDx2 := ("00"&lastPixels0(0)) + ("0"&lastPixels1(0)&"0") + ("00"&lastPixels2(0));
					if lineDx0 > lineDx2 then
						diffDx := lineDx0 - lineDx2;
					else
						diffDx := lineDx2 - lineDx0;
					end if;
					if diffDx(9) = '1' or diffDx(8) = '1' then --overflow
						stream_out_dx_data <= "11111111";
					else
						stream_out_dx_data <= diffDx(7 downto 0);
					end if;
					
					--| 1   2   1| lastPixels0
					--| 0   0   0| lastPixels1
					--|-1  -2  -1| lastPixels2
--					lineDy0 := ("00"&lastPixels0(2)) + ("0"&lastPixels0(1)&"0") + ("00"&lastPixels0(0));
--					lineDy2 := ("00"&lastPixels2(2)) + ("0"&lastPixels2(1)&"0") + ("00"&lastPixels2(0));
					if lineDy0 > lineDy2 then
						diffDy := lineDy0 - lineDy2;
					else
						diffDy := lineDy2 - lineDy0;
					end if;
					if diffDy(9) = '1' or diffDy(8) = '1' then --overflow
						stream_out_dy_data <= "11111111";
					else
						stream_out_dy_data <= diffDy(7 downto 0);
					end if;
				end if;
				
				--and inc address
				if addr < C_IMAGE_WIDTH - 1 then
					addr <= addr + 1;
--					x <= x + 1;
--					state <= PIXEL;
				elsif y < C_IMAGE_HEIGHT - 1 then
					if activeLine = 0 then
						activeLine <= 1;
					elsif activeLine = 1 then
						activeLine <= 2;
--						init <= '0';
					else
						activeLine <= 0;
					end if;
					addr <= (others => '0');
--					x <= 0;
					y <= y+1;
--					state <= PIXEL;
				else
					state <= IDLE;
				end if;
			
			end if;
			
		end case;
	
	end if; --rising edge clk

end process;

end behavior;



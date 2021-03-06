-----------------------------------------------------------------------------------------------------------------------
-- FILE         : imagesensor.vhd
-- TITLE        : 
-- PROJECT      : PICSY
-- AUTHOR       : Felix & akzare (University of Potsdam, Computer Sceince Department)
-- DESCRIPTION	: image sensor interface with the SDI Controller 
--                for producing video stream data and writing to the Writer FIFO.
-----------------------------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------------------------
-- Libraries
-----------------------------------------------------------------------------------------------------------------------
library ieee;
  use ieee.std_logic_1164.all;
  use ieee.std_logic_unsigned.all;
  use ieee.std_logic_arith.all;

-----------------------------------------------------------------------------------------------------------------------
-- Ports
-----------------------------------------------------------------------------------------------------------------------
entity imagesensor is
  generic
  (
    C_IMAGE_WIDTH           : integer := 752;
    C_IMAGE_HIGHT           : integer := 480;
    C_DWIDTH                : integer := 80;
    C_NUM_CONFIG_REGS       : integer := 2
  );
  port
  (
	-- Streaming I/O
    rst               : in std_logic;
    clk               : in std_logic;
   
	-- configuration
    config_regs       : in std_logic_vector(C_NUM_CONFIG_REGS*32 - 1 downto 0);
	-- image sensor signals
    ISI_D             : in std_logic_vector(9 downto 0);
    ISI_LNE_VLD       : in std_logic;
    ISI_FRM_VLD       : in std_logic;
    ISI_RESET_N       : out std_logic;
    ISI_XMCLK         : out std_logic;
    ISI_PXCLK         : in std_logic;		
	-- stream out 8 pixels
    stream_out_stop   : in std_logic;
    stream_out_valid  : out std_logic;
    stream_out_valid_wide : out std_logic;
    stream_out_data   : out std_logic_vector(C_DWIDTH-1 downto 0);
	sof : out std_logic;
	eof : out std_logic
  );
end imagesensor;

-----------------------------------------------------------------------------------------------------------------------
-- Architecture
-----------------------------------------------------------------------------------------------------------------------
architecture behavior of imagesensor is

	constant image_width    : natural := C_IMAGE_WIDTH;
	constant image_height   : natural := C_IMAGE_HIGHT;

	signal stream_out_data_s        : std_logic_vector(C_DWIDTH-1 downto 0);
	signal stream_out_valid_s       : std_logic;
	
	signal clk_divider              : std_logic_vector(2 downto 0);
	signal cam_clk                  : std_logic;
	
	signal capture                  : std_logic;

	signal stream_out_valid_delay   : std_logic_vector(1 downto 0);
	
	signal last_frame_valid			  : std_logic;
    component fifo_80_to_80
	  PORT (
	    rst : IN STD_LOGIC;
	    wr_clk : IN STD_LOGIC;
	    rd_clk : IN STD_LOGIC;
	    din : IN STD_LOGIC_VECTOR(79 DOWNTO 0);
	    wr_en : IN STD_LOGIC;
	    rd_en : IN STD_LOGIC;
	    dout : OUT STD_LOGIC_VECTOR(79 DOWNTO 0);
	    full : OUT STD_LOGIC;
	    empty : OUT STD_LOGIC;
	    valid : OUT STD_LOGIC
	  );
end component;
begin

	CLOCK_DIVIDER_PROC : process(clk,rst)
	begin
	   if ( rst='1' ) then 
		   clk_divider <= (Others => '0');
		elsif rising_edge(clk) then
			 clk_divider <= clk_divider + 1;
		end if;
	end process CLOCK_DIVIDER_PROC;

	cam_clk <= clk_divider(2);

	ISI_RESET_N <= not rst;
	ISI_XMCLK <= not ISI_PXCLK;--cam_clk;


	DELAY_STREAM_OUT_PROC : process (rst,stream_out_valid_s)
	begin
		if ( rst='1' ) then
			stream_out_valid_delay <= (Others => '0');			
		elsif rising_edge(stream_out_valid_s) then
			stream_out_valid_delay <= stream_out_valid_delay+1;
 		end if;
	end process DELAY_STREAM_OUT_PROC;
  
	stream_out_valid_wide <= stream_out_valid_delay(0);

	IMAGE_CAPTURE_PROC : process(rst,clk)
		variable index           : integer range 0 to 7 := 0;
		variable xpos            : natural range 0 to image_width;
		variable ypos            : natural range 0 to image_height;
		variable xmin, xmax      : natural;
		variable ymin, ymax      : natural;
		variable last_cam_clk    : std_logic;
		variable last_line_valid : std_logic;
--		variable data            : std_logic_vector((C_DWIDTH/8)-1 downto 0) := (others => '1');
	begin
		ymin := 0;--conv_integer(config_regs(15 downto 0));
		ymax := 479;--conv_integer(config_regs(31 downto 16));
		xmin := 0;--conv_integer(config_regs(47 downto 32));
		xmax := 639;--conv_integer(config_regs(63 downto 48));
		if ( rst='1' ) then
			index := 7;
--			data := (others => '1');
			xpos := 0;
			ypos := 0;
			last_frame_valid <= '0';
			sof <= '0';
			eof <= '0';
		elsif rising_edge(clk) then
			if (last_frame_valid='0' and ISI_FRM_VLD='1') then
				sof <= '1';
			else
				sof <= '0';
			end if;
			if (last_frame_valid='1' and ISI_FRM_VLD='0') then
				eof <= '1';
			else
				eof <= '0';
			end if;
			last_frame_valid <= ISI_FRM_VLD;
			
      
			stream_out_valid_s <= '0';
			if (last_cam_clk='1' and ISI_PXCLK='0') then  -- trigger on falling edge
				if ISI_FRM_VLD = '0' then
					xpos := 0;
					ypos := 0;
					index := 7;
					capture <= not stream_out_stop;
				else
					if ISI_LNE_VLD = '1' then
						if ((capture = '1') and (xpos >= xmin) and (xpos <= xmax) and (ypos >= ymin) and (ypos <= ymax)) then
							--handle 8 bytes stream
							if C_DWIDTH = 80 then
--								data := ISI_D;
								stream_out_data_s(index*10+9 downto index*10) <= ISI_D;
							elsif C_DWIDTH = 64 then
--								data := ISI_D(9 downto 2);
								stream_out_data_s(index*8+7 downto index*8) <= ISI_D(9 downto 2);
							else
--								data := ISI_D;
								stream_out_data_s(index*(C_DWIDTH/8)+(C_DWIDTH/8-1) downto index*(C_DWIDTH/8)) <= ISI_D;
							end if;
--							stream_out_data_s(index*(C_DWIDTH/8)+(C_DWIDTH/8-1) downto index*(C_DWIDTH/8)) <= data;
							if index = 0 then
								index := 7;
								stream_out_valid_s <= '1';
							else
								index := index - 1;
							end if;
							
						end if;
						xpos := xpos + 1;
					elsif last_line_valid = '1' then
						xpos := 0;
						ypos := ypos + 1;
					end if;
				end if;
				last_line_valid := ISI_LNE_VLD;
		end if;
			last_cam_clk := ISI_PXCLK;
		end if;
	end process IMAGE_CAPTURE_PROC;

--DATA_CAPTURE_out: fifo_80_to_80
--		port map (
--			din =>stream_out_data_s,
--			rd_clk => clk,
--			rd_en =>capture,
--			rst => rst,
--			wr_clk =>ISI_PXCLK,
--			wr_en => stream_out_valid_s,
--			dout => stream_out_data,
--			empty => open,
--         valid=>stream_out_valid,
--			full =>open
--         ); 
	stream_out_data <= stream_out_data_s;
	stream_out_valid <= stream_out_valid_s;

end behavior;

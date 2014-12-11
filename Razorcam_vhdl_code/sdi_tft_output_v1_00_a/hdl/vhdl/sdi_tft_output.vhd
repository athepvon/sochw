-----------------------------------------------------------------------------------------------------------------------
-- FILE         : sdi_vga_output.vhd
-- TITLE        : 
-- PROJECT      : PICSY
-- AUTHOR       : Felix & Ali (University of Potsdam, Computer Sceince Department)
-- DESCRIPTION	: vga output 
--                for consuming data from Redear FIFO of SDI Controller and producing video RGB color data on VGA output.
-----------------------------------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------------------------------
-- Libraries
-----------------------------------------------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;
library UNISIM;
use UNISIM.vcomponents.all;
entity sdi_tft_output is
	generic
	(
      C_REG_WIDTH : integer :=  32;
		C_DWIDTH : integer	:=	32
	);
	port
	(
		-- rst and Clocks --------------------------------------------------
		rst : in std_logic;
		clk	: in	std_logic;
                debug:out std_logic_vector(69 downto 0);
                trig0:out std_logic_vector(0 downto 0);
		-- Stream In ---------------------------------------------------------
		pu_reset : in std_logic;
		request_frame : out std_logic;
		request_frame_ack : in std_logic;
		stream_in_stop : out	std_logic;
		stream_in_valid : in	std_logic;
		stream_in_data : in	std_logic_vector(C_DWIDTH-1 downto 0);
		-- Config -----------------------------------------------------------
		config_reg : in std_logic_vector(C_REG_WIDTH-1 downto 0);
		sof : in std_logic;
		sof_ack : out std_logic;
		tft_ready1 : out std_logic;
		-- VGA ---------------------------------------------------------------
		x: out std_logic_vector(8 downto 0);
      y: out  std_logic_vector(8 downto 0);
		JA : out  STD_LOGIC_VECTOR (3 downto 0); 
		IM : out std_logic_vector(2 downto 0);
                JA1 : in  STD_LOGIC 
	);

end sdi_tft_output;
------------------------------------------------------------------------------
-- Architecture section
------------------------------------------------------------------------------

architecture Behavioral of sdi_tft_output is
        signal request_frame1 : std_logic;
	-- -------------------------------
	-- Constants Declarations
	-- -------------------------------
		component synth_Bram is
		 generic ( d_width : natural := 8;
					  a_width : natural := 12           
					);
		 Port ( clka : in  STD_LOGIC;
				  dina : in  STD_LOGIC_VECTOR (d_width-1 downto 0);
				  addra : in  STD_LOGIC_VECTOR (a_width-1 downto 0);
				  wea : in  STD_LOGIC;
				  clkb : in  STD_LOGIC;
				  addrb : in  STD_LOGIC_VECTOR (a_width-1 downto 0);
				  doutb : out  STD_LOGIC_VECTOR (d_width-1 downto 0));
	end component;
	signal JAx :  STD_LOGIC_VECTOR (3 downto 0); 
	signal	stream_in_stop1 : std_logic;
	signal	stream_in_valid1 :std_logic;
	signal	stream_in_data1 : std_logic_vector(7 downto 0);
	constant COLOR_24BIT	: std_logic_vector(1 downto 0) := "00";
	constant GRAY_8BIT	: std_logic_vector(1 downto 0) := "01";
	component tft 
	Port (  rst:in  STD_LOGIC;
		JA : out  STD_LOGIC_VECTOR (3 downto 0); 
		IM : out std_logic_vector(2 downto 0);
                JA1 : in  STD_LOGIC; 
		tft_ready : out STD_LOGIC;
		x: out std_logic_vector(8 downto 0);
		y: out  std_logic_vector(8 downto 0);
                framedone: out std_logic;
	        framestart:in  std_logic;
		pixeldata: in std_logic_vector(15 downto 0);
		pixeldone: out std_logic;
		newpixel:  in std_logic:='1';
		clk : in  STD_LOGIC);								-- Onboard 100 Mhz clock
	end component;
component fifo_32_to_8 
  PORT (
     rst : IN STD_LOGIC;
    wr_clk : IN STD_LOGIC;
    rd_clk : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC;
    valid : OUT STD_LOGIC
  );
end component;
component fifo_64_to_8
  PORT (
    rst : IN STD_LOGIC;
    wr_clk : IN STD_LOGIC;
    rd_clk : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(63 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC;
    valid : OUT STD_LOGIC
  );
end component;
  	signal x1: std_logic_vector(8 downto 0);
   signal y1:  std_logic_vector(8 downto 0);
   signal pixeldata:  std_logic_vector(15 downto 0);
	signal pixeldone	: std_logic;
	signal newpixel	: std_logic;
	signal startpixel	: std_logic;
	signal tft_ready :  STD_LOGIC;
	signal xcurrent_pos:natural range 0 to 752;
	signal ycurrent_pos:natural range 0 to 480;
   signal config_regx :  std_logic_vector(31 downto 0);
--signal debug:std_logic_vector(29 downto 0);
--signal trig0:std_logic_vector(0 downto 0);
signal CONTROL0:  std_logic_vector(35 downto 0);
signal dat:  std_logic_vector(31 downto 0);
signal  framedone: std_logic;
signal framestart: std_logic;
signal doutb_debug:std_logic_vector(7 downto 0);
signal doutb_debug1:std_logic_vector(15 downto 0);
signal x_pos_w, x_pos_w1, x_pos_r : natural range 0 to 752 := 0;
signal y_pos_w, y_pos_w1, y_pos_r : natural range 0 to 480 := 0;
signal addra_d,addrb_d : std_logic_vector(16 downto 0);
--signal addra_d,addrb_d : std_logic_vector(9 downto 0);
signal y_vect_d_w, y_vect_d_r  : std_logic_vector(8 downto 0);
signal douta_d, dinb_d, web_d, wea_d,wea_d1  : std_logic_vector(0 downto 0);
signal framestarted :  STD_LOGIC;
signal i :  STD_LOGIC;
signal  rd_en: std_logic;
signal empty: std_logic;
signal wd: std_logic;
signal rd: std_logic;
signal stream_out_stop1: STD_LOGIC; 
signal stream_out_stop2: STD_LOGIC; 
signal stream_out_stop3: STD_LOGIC; 
signal stream_out_stop: STD_LOGIC;  
signal stream_out_valid1: STD_LOGIC; 
signal stream_out_valid2: STD_LOGIC; 
signal stream_out_valid3: STD_LOGIC; 
signal stream_out_valid: STD_LOGIC;  
signal stream_out_data : STD_LOGIC_VECTOR (63 downto 0); 
signal stream_out_data2 : STD_LOGIC_VECTOR (7 downto 0);
signal tft_index: integer range 0 to 3;
signal tft_we_sig: STD_LOGIC; 
signal tft_we: STD_LOGIC; 
signal		xx: std_logic_vector(8 downto 0);
signal		yy: std_logic_vector(8 downto 0);
signal fiforst:STD_LOGIC; signal rst1:STD_LOGIC; 
begin
tft_ready1<= not tft_ready;
trig0(0)<=request_frame1;
rst1<=not rst;
--debug<= X"0000000000" & "0001" & framestart & framedone & is_visible & tft_ready  & stream_in_valid  & startpixel & newpixel & pixeldone  & x1  & y1;
request_frame<='1';--framedone;--request_frame1 ;
x<=xx; y<=yy;
sof_ack<=not framedone;
fiforst<=rst1 or sof;
--rd<= '1' when ((bramfull1='0') or(bramfull1='1' and (conv_integer(addrb_d)>conv_integer(addra_d)))) else '0';				
dat<= stream_in_data(55 downto 48) & stream_in_data(39 downto 32)& stream_in_data(23 downto 16) & stream_in_data(7 downto 0);
DATA_CAPTURE: fifo_64_to_8
		port map (
			din => stream_in_data,
			rd_clk => clk,
			rd_en =>'1',--rd,--
			rst => fiforst,
			wr_clk =>clk,
			wr_en => stream_in_valid,
			dout => stream_out_data2,
			empty => empty,
         valid=>stream_out_valid2,
			full => stream_in_stop
             );
				 
pixeldata<= doutb_debug(7 downto 3) & doutb_debug(7 downto 2) & doutb_debug(7 downto 3);
		my_video :	synth_bram
			generic map(
						  d_width => 8,
						  a_width => 17)--10)
			port map (
				clka  => clk,
				dina  => stream_out_data2,--cam_sys_pixel_data,--thresholded_din, --color,--
				addra => addra_d,
				wea   => wea_d(0),
				clkb  => clk,
				addrb => addrb_d,
				doutb => doutb_debug);
				--thresholded_din(0) <= '1' when conv_integer(cam_sys_pixel_data) > conv_integer(otsu_threshold) else '0';
				wea_d(0) <= stream_out_valid2 when(x_pos_w<320) and (y_pos_w<240)else '0';
				--wea_d1(0) <= we1;
				web_d <= "0";
				y_vect_d_w <= std_logic_vector(to_unsigned(y_pos_w,9));
            y_vect_d_r <= std_logic_vector(to_unsigned(y_pos_r,9));
				
				addra_d(16 downto 9) <= y_vect_d_w(7 downto 0);
				addra_d(8 downto  0) <= std_logic_vector(to_unsigned(x_pos_w,9));
				--addra_d<= std_logic_vector(to_unsigned(((320*y_pos_w) rem 1024)+x_pos_w,10));
				
				addrb_d(16 downto 9) <= y_vect_d_r(7 downto 0);
				addrb_d(8 downto  0) <= std_logic_vector(to_unsigned(x_pos_r,9));
            --addrb_d<= std_logic_vector(to_unsigned(((320*y_pos_r)rem 1024)+x_pos_r,10));

				WRITE_VGA : process(clk, rst1) is
				begin
					if rst1 = '1' then
						x_pos_w <= 0;
						y_pos_w <= 0;
					elsif ( clk'event and clk= '1' ) then
						if (sof = '1' ) then
							x_pos_w <= 0;
						   y_pos_w <= 0;
						elsif (stream_out_valid2='1') then
							if x_pos_w=639 then
								x_pos_w <= 0;
									if y_pos_w = 479 then
										y_pos_w <= 0;
									else
										y_pos_w <= y_pos_w+1;
									end if;
							else
								x_pos_w <= x_pos_w+1;
							end if;
						end if;
					end if;
				end process WRITE_VGA;

x_pos_r<=conv_integer(xx);
y_pos_r<=conv_integer(yy);	
WRITE_TFT : process(clk, rst1) is
begin
	if rst1 = '1' then		
		tft_index  <= 0;
		tft_we_sig <= '0';
	elsif rising_edge(clk) then
      if (tft_index = 2) then
		   tft_index  <= 0;
			tft_we_sig <= '1';
		else
			tft_index <= tft_index + 1;
			tft_we_sig <= '0';
	   end if;
	end if;	
end process WRITE_TFT;
tft_we <= tft_we_sig;
tft_m : tft
Port map  (rst=>rst1,
		JA =>JAx,
		IM=>IM,
      JA1 =>JA1,
		tft_ready=>tft_ready,
		x=>xx,
		y=>yy,
      framestart=>sof,--framestart,
      framedone=>framedone,
		pixeldata=>pixeldata,
		pixeldone=> pixeldone,
		newpixel=>tft_we,
		clk=> clk
       );
JA(3 downto 0)<=JAx(3 downto 0);

end Behavioral;


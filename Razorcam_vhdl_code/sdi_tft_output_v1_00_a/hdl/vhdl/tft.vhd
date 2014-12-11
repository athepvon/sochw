
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


-- *******************************************************************************
--  								Define block inputs and outputs
-- *******************************************************************************
entity tft is
    Port (  rst:in  STD_LOGIC;
            	JA : out  STD_LOGIC_VECTOR (3 downto 0); 
					IM : out  STD_LOGIC_VECTOR (2 downto 0); 
        JA1 : in  STD_LOGIC; 
               framedone: out std_logic;
			  framestart:in  std_logic;
				tft_ready : out STD_LOGIC;
				x: out std_logic_vector(8 downto 0);
            y: out  std_logic_vector(8 downto 0);
				pixeldata: in std_logic_vector(15 downto 0);
				pixeldone: out std_logic;
            newpixel:  in std_logic:='1';
           --Leds : out  STD_LOGIC_VECTOR (3 downto 0);
            clk : in  STD_LOGIC);								-- Onboard 100 Mhz clock
end tft;

architecture Behavioral of tft is
--signal pixeldata:  std_logic_vector(15 downto 0);
--signal newpixel:  std_logic;


signal  x1:  std_logic_vector(8 downto 0):='0' & X"00";
signal y1:  std_logic_vector(8 downto 0):='0' & X"00";
					--signal x: std_logic_vector(8 downto 0):='0' & X"A0";
					--signal y:  std_logic_vector(8 downto 0):='0' & X"78";
					--signal pixeldata: std_logic_vector(23 downto 0):=X"FF0000";
					--signal pixeldone:  std_logic;
              -- signal newpixel:  std_logic:='1';
		-- ++++++++++++++++++++++++++++++++++++++++++++++
		-- 					Master Interface
		-- ++++++++++++++++++++++++++++++++++++++++++++++
		component master_interface
			port ( begin_transmission : out std_logic;
					 end_transmission : in std_logic;
					 clk : in std_logic;
					 rst : in std_logic;
					 x: out std_logic_vector(8 downto 0);
					 y: out std_logic_vector(8 downto 0);
					 pixeldata: in std_logic_vector(15 downto 0);
					 pixeldone: out std_logic;
					 newpixel:in  std_logic;
					 tft_ready : out STD_LOGIC;
					 slave_reset : out std_logic;
					 slave_select : out std_logic;
                                           framedone: out std_logic;
			                   framestart:in  std_logic;
					 sel : out integer range 0 to 69;
					 sel1 : out integer range 0 to 69;
			       temp_data1 : in  STD_LOGIC_VECTOR (7 downto 0);
					 temp_data : in std_logic_vector(7 downto 0);
					 send_data : out std_logic_vector(7 downto 0));
		end component;

		-- ++++++++++++++++++++++++++++++++++++++++++++++
		-- 				Serial Port Interface
		-- ++++++++++++++++++++++++++++++++++++++++++++++
		component spi_interface
			port ( begin_transmission : in std_logic;
					 slave_select : in std_logic;
					 tft_ready : in std_logic;
					 send_data : in std_logic_vector(7 downto 0);
					 miso : in std_logic;
					 clk : in std_logic;
					 rst : in std_logic;
					 --recieved_data : out std_logic_vector(7 downto 0);
					 end_transmission : out std_logic;
					 mosi : out std_logic;
					 sclk : out std_logic);
		end component;


		-- ++++++++++++++++++++++++++++++++++++++++++++++
		-- 					Command Lookup
		-- ++++++++++++++++++++++++++++++++++++++++++++++
		component command_lookup
			port( sel : in integer range 0 to 69;
					data_out : out std_logic_vector(7 downto 0));
		end component;
		component pixel_command_lookup
			port( sel : in integer range 0 to 69;
			       x: in std_logic_vector(8 downto 0);
					 y: in std_logic_vector(8 downto 0);
					data_out : out std_logic_vector(7 downto 0));
		end component;
		
-- *******************************************************************************
--  									Signals and Constants
-- *******************************************************************************
 signal tft_ready1 : STD_LOGIC;
	-- Active low signal for writing data to PmodCLS
	signal slave_select : std_logic;
        signal slave_reset : std_logic;
	-- Initializes data transfer with PmodCLS
	signal begin_transmission : std_logic;
	-- Handshake signal to signify data transfer done
	signal end_transmission : std_logic;
	-- Selects which ASCII value to send to PmodCLS
	signal sel : integer range 0 to 69;
	signal sel1 : integer range 0 to 69;
	-- Output data from C2 to C0
	signal temp_data : std_logic_vector(7 downto 0);
	signal temp_data1 : std_logic_vector(7 downto 0);
	-- Output data from C0 to C1
	signal send_data : std_logic_vector(7 downto 0);
-- *******************************************************************************
--  										Implementation
-- *******************************************************************************
begin
   tft_ready<=tft_ready1;
   --newpixel<='1'; 
	--pixeldata<=y1(7 downto 0) & y1(7 downto 0);
   --Leds<= rst & rst & rst & rst;--conv_std_logic_vector(sel,4);
	-- Produces signals for controlling SPI interface, and selecting output data.
	x<=x1; y<=y1;
	C0: master_interface port map( 
			begin_transmission => begin_transmission,
			end_transmission => end_transmission,
			clk => clk,
			rst => rst,
			x=> x1,
			y=> y1,
			pixeldata=> pixeldata,
			pixeldone=> pixeldone,
			newpixel=>newpixel,
			slave_reset =>slave_reset,
			tft_ready =>tft_ready1,
			slave_select => slave_select,
			sel1 => sel1,
         framestart=>framestart,
         framedone=>framedone,
			temp_data1 => temp_data1,
			temp_data => temp_data,
			send_data => send_data,
			sel => sel
	);

	-- Interface between the PmodCLS and FPGA, proeduces SCLK signal, etc.
	C1 : spi_interface port map(
			begin_transmission => begin_transmission,
			tft_ready => tft_ready1,
			slave_select => slave_select,
			send_data => send_data,
			--recieved_data => recieved_data,
			miso => JA1,
			clk => clk,
			rst => rst,
			end_transmission => end_transmission,
			mosi => JA(1),
			sclk => JA(3)
	);

	-- Contains the ASCII characters for commands
	C2 : command_lookup port map (
			sel => sel,
			data_out => temp_data
	);
	
	C3 : pixel_command_lookup port map (
			sel => sel1,
			x=> x1,
			y=> y1,
			data_out => temp_data1
	);
	--  Active low slave select signal
	JA(0) <= slave_select;
   JA(2) <= slave_reset;
   IM<="100";
end Behavioral;


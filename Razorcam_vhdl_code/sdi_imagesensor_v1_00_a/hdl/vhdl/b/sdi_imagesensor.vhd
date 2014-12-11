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
entity sdi_imagesensor is
	generic
	(
		C_IMAGE_WIDTH     : integer := 752;
		C_IMAGE_HIGHT     : integer := 480;
		C_DWIDTH          : integer := 80;
		C_REGWIDTH 		   : integer := 32;
		C_NUM_CONFIG_REGS : integer := 4;
		C_NUM_STATUS_REGS : integer := 1
  );
  port
  (
    rst               : in std_logic;
    clk               : in std_logic;
	 config_reg       : in std_logic_vector(C_NUM_CONFIG_REGS*C_REGWIDTH-1 downto 0);
	 status_reg       : out std_logic_vector(C_NUM_STATUS_REGS*C_REGWIDTH-1 downto 0);
--ports for 2 image sensors
	-- image sensor 1 signals
    ISI1_D             : in std_logic_vector(9 downto 0);
    ISI1_LNE_VLD       : in std_logic;
    ISI1_FRM_VLD       : in std_logic;
    ISI1_RESET_N       : out std_logic;
    ISI1_XMCLK         : out std_logic;
    ISI1_PXCLK         : in std_logic;		
	-- stream out 8 pixels
	 debug:out std_logic_vector(69 downto 0);
    trig0:out std_logic_vector(0 downto 0);
    stream_out_stop   : in std_logic;
    stream_out_valid  : out std_logic;
    stream_out_valid_wide : out std_logic;
    stream_out_data   : out std_logic_vector(C_DWIDTH-1 downto 0);
	 sof : out std_logic;
	 eof : out std_logic
  );
end sdi_imagesensor;

-----------------------------------------------------------------------------------------------------------------------
-- Architecture
-----------------------------------------------------------------------------------------------------------------------
architecture behavior of sdi_imagesensor is

component imagesensor generic
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
	--debug

	-- ISI_MASK : in std_logic_vector(7 downto 0);
	sof : out std_logic;
	eof : out std_logic
  );
end component;

--signal reg0,reg1 : std_logic_vector(0 to C_DWIDTH-1);
--register 0 (in MHS first from right, in C struct the first) is 31..0
--register 1 (in MHS second from right, in C struct the second) is 63..32
--register 2,3 is for image sensor

type START_STATES is (IDLE_0, WAIT_FOR_ZERO_1);
signal start_state : START_STATES;
signal start : std_logic;

signal stream1_valid_s : std_logic;
signal stream1_valid_wide_s : std_logic;
signal stream1_data : std_logic_vector(C_DWIDTH-1 downto 0);
signal stream3_valid_s : std_logic;
signal stream3_valid_wide_s : std_logic;
signal stream3_data : std_logic_vector(C_DWIDTH-1 downto 0);
signal sof1_s : std_logic;
signal sof3_s : std_logic;
signal eof1_s : std_logic;
signal eof3_s : std_logic;
signal config_regx       : std_logic_vector(127 downto 0);
  
begin

	--reg0 <= config_reg(C_DWIDTH to 2*C_DWIDTH-1);
	--reg1 <= config_reg(0 to C_DWIDTH-1);
--config_regx<=X"02EF000001DF000002EF000001DF0000";
--config_regx<=X"013F000000EF0000013F000000EF0000";
trig0(0)<=stream1_valid_s;
--debug<=  ISI1_LNE_VLD & ISI1_FRM_VLD & ISI1_PXCLK & stream_out_stop & stream1_valid_s  & stream1_data(64 downto 0);

ISI1 : imagesensor generic map(
  C_IMAGE_WIDTH=>C_IMAGE_WIDTH,
    C_IMAGE_HIGHT=>C_IMAGE_HIGHT,
    C_DWIDTH=>C_DWIDTH,
    C_NUM_CONFIG_REGS=>2)--C_NUM_CONFIG_REGS)
	 port map(
    rst => rst, --sensor_reset,
    clk => clk,
    config_regs => config_reg(4*32-1 downto 2*32),
	 ISI_D => ISI1_D,
    ISI_LNE_VLD => ISI1_LNE_VLD,
    ISI_FRM_VLD => ISI1_FRM_VLD,
    ISI_RESET_N => ISI1_RESET_N,
    ISI_XMCLK => ISI1_XMCLK,
    ISI_PXCLK => ISI1_PXCLK,	
	-- stream out
    stream_out_stop => stream_out_stop,
    stream_out_valid => stream1_valid_s,
    stream_out_valid_wide => stream1_valid_wide_s,
    stream_out_data => stream1_data,
	--debug
	-- ISI_MASK : in std_logic_vector(7 downto 0);
	sof => sof1_s,
	eof => eof1_s
  );
  
	 --sensor_reset <= Bus2IP_Reset or slv_reg_write(3); 
	 stream_out_valid      <= stream1_valid_s      ;
	 stream_out_valid_wide <= stream1_valid_wide_s ;
	 stream_out_data       <= stream1_data         ;
	 sof                   <= sof1_s               ;
	 eof                   <= eof1_s              ;
  
end behavior;

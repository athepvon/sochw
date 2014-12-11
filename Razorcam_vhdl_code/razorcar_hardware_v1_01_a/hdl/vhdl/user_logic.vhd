------------------------------------------------------------------------------
-- user_logic.vhd - entity/architecture pair
------------------------------------------------------------------------------
--
-- ***************************************************************************
-- ** Copyright (c) 1995-2012 Xilinx, Inc.  All rights reserved.            **
-- **                                                                       **
-- ** Xilinx, Inc.                                                          **
-- ** XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"         **
-- ** AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND       **
-- ** SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,        **
-- ** OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,        **
-- ** APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION           **
-- ** THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,     **
-- ** AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE      **
-- ** FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY              **
-- ** WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE               **
-- ** IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR        **
-- ** REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF       **
-- ** INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS       **
-- ** FOR A PARTICULAR PURPOSE.                                             **
-- **                                                                       **
-- ***************************************************************************
--
------------------------------------------------------------------------------
-- Filename:          user_logic.vhd
-- Version:           1.01.a
-- Description:       User logic.
-- Date:              Wed Nov 20 22:10:32 2013 (by Create and Import Peripheral Wizard)
-- VHDL Standard:     VHDL'93
------------------------------------------------------------------------------
-- Naming Conventions:
--   active low signals:                    "*_n"
--   clock signals:                         "clk", "clk_div#", "clk_#x"
--   reset signals:                         "rst", "rst_n"
--   generics:                              "C_*"
--   user defined types:                    "*_TYPE"
--   state machine next state:              "*_ns"
--   state machine current state:           "*_cs"
--   combinatorial signals:                 "*_com"
--   pipelined or register delay signals:   "*_d#"
--   counter signals:                       "*cnt*"
--   clock enable signals:                  "*_ce"
--   internal version of output port:       "*_i"
--   device pins:                           "*_pin"
--   ports:                                 "- Names begin with Uppercase"
--   processes:                             "*_PROCESS"
--   component instantiations:              "<ENTITY_>I_<#|FUNC>"
------------------------------------------------------------------------------

-- DO NOT EDIT BELOW THIS LINE --------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library proc_common_v3_00_a;
use proc_common_v3_00_a.proc_common_pkg.all;

-- DO NOT EDIT ABOVE THIS LINE --------------------

--USER libraries added here

------------------------------------------------------------------------------
-- Entity section
------------------------------------------------------------------------------
-- Definition of Generics:
--   C_NUM_REG                    -- Number of software accessible registers
--   C_SLV_DWIDTH                 -- Slave interface data bus width
--
-- Definition of Ports:
--   Bus2IP_Clk                   -- Bus to IP clock
--   Bus2IP_Resetn                -- Bus to IP reset
--   Bus2IP_Data                  -- Bus to IP data bus
--   Bus2IP_BE                    -- Bus to IP byte enables
--   Bus2IP_RdCE                  -- Bus to IP read chip enable
--   Bus2IP_WrCE                  -- Bus to IP write chip enable
--   IP2Bus_Data                  -- IP to Bus data bus
--   IP2Bus_RdAck                 -- IP to Bus read transfer acknowledgement
--   IP2Bus_WrAck                 -- IP to Bus write transfer acknowledgement
--   IP2Bus_Error                 -- IP to Bus error response
------------------------------------------------------------------------------

entity user_logic is
  generic
  (
    -- ADD USER GENERICS BELOW THIS LINE ---------------
    SYSTEM_CLOCK	             	  : positive 				 := 50000000; -- 50MHz
    C_NUM_LEDS                     : integer					 := 4;
	 C_NUM_BUTTONS						  : integer					 := 2;
	 C_NUM_SWITCHES					  : integer					 := 4;
    -- ADD USER GENERICS ABOVE THIS LINE ---------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol parameters, do not add to or delete
    C_NUM_REG                      : integer              := 6;
    C_SLV_DWIDTH                   : integer              := 32
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );
  port
  (
    -- ADD USER PORTS BELOW THIS LINE ------------------   	 
	 steeringIn	    : in  STD_LOGIC;
	 speedIn		    : in  STD_LOGIC;
	 speedPwmOut 	 : out STD_LOGIC;
	 steeringPwmOut : out STD_LOGIC;
	 mux				 : out STD_LOGIC;
	 razor_Button	 : in STD_LOGIC_VECTOR(C_NUM_BUTTONS-1 downto 0);
	 razor_Switch	 : in STD_LOGIC_VECTOR(C_NUM_SWITCHES - 1 downto 0);-- Only SWITCH1 is used because SWITCH2 shared locations with the TFT pins.
	 --LED section
	 led            : out STD_LOGIC_VECTOR(C_NUM_LEDS-1 downto 0);	 
	 btnIntr			 : out STD_LOGIC;
	 swIntr			 : out STD_LOGIC;
	 --Buzzer
	 buzzer			 : out STD_LOGIC;
	 -- ADC 
	 spi_sdi : in  std_logic;
	 spi_sdo : out std_logic;
	 spi_clk : out std_logic;
	 spi_csn : out std_logic;
    -- ADD USER PORTS ABOVE THIS LINE ------------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol ports, do not add to or delete
    Bus2IP_Clk                     : in  std_logic;
    Bus2IP_Resetn                  : in  std_logic;
    Bus2IP_Data                    : in  std_logic_vector(C_SLV_DWIDTH-1 downto 0);
    Bus2IP_BE                      : in  std_logic_vector(C_SLV_DWIDTH/8-1 downto 0);
    Bus2IP_RdCE                    : in  std_logic_vector(C_NUM_REG-1 downto 0);
    Bus2IP_WrCE                    : in  std_logic_vector(C_NUM_REG-1 downto 0);
    IP2Bus_Data                    : out std_logic_vector(C_SLV_DWIDTH-1 downto 0);
    IP2Bus_RdAck                   : out std_logic;
    IP2Bus_WrAck                   : out std_logic;
    IP2Bus_Error                   : out std_logic
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );

  attribute MAX_FANOUT : string;
  attribute SIGIS : string;

  attribute SIGIS of Bus2IP_Clk    : signal is "CLK";
  attribute SIGIS of Bus2IP_Resetn : signal is "RST";

end entity user_logic;

------------------------------------------------------------------------------
-- Architecture section
------------------------------------------------------------------------------

architecture IMP of user_logic is

  --USER signal declarations added here, as needed for user logic
  component LedControl is
	  generic(	NUM_LED     : positive := 4);
	  port(	   clk 			: in  std_logic; 										-- blinking clock frequency
				   enable   	: in  std_logic; 										-- enable or disable all Led's 0=off; 1=on
				   led_setup	: in  std_logic_vector((NUM_LED*2-1) downto 0); -- Led setup input 8x2 bit 
				   led      	: out std_logic_vector((NUM_LED-1) downto 0)
			);  	
  end component LedControl;
  
  component BuzzerControl is
    Port ( clk : in  STD_LOGIC;
			  reset : in  STD_LOGIC;	
           enable : in  STD_LOGIC;
           freq : in  std_logic_vector(2 downto 0);
           buzzer_out : out  STD_LOGIC);
  end component;
  
  component Steering_Speed is
	Port(
		   clock : in std_logic;
			steering_bit_in : in std_logic_vector(7 downto 0);		
			speed_bit_in : in std_logic_vector(7 downto 0);
			mode:	in std_logic_vector(2 downto 0);
			steering_in : in std_logic;		
			speed_in : in std_logic;			
			steering_out : out STD_LOGIC;
			speed_out : out  STD_LOGIC			
	);
  end component;
  
  component BtnSwitch is
    Port ( clk       : in  STD_LOGIC;
           rst       : in  STD_LOGIC;
           button    : in  STD_LOGIC_VECTOR(C_NUM_BUTTONS-1  downto 0);
           switch    : in  STD_LOGIC_VECTOR(C_NUM_SWITCHES-1 downto 0);
           enBtnIntr : in  STD_LOGIC;
           enSwIntr  : in  STD_LOGIC;
           btnIntr   : out STD_LOGIC;
           swIntr    : out STD_LOGIC);
  end component;
	
  component adc78h89 is
	generic (
				iCLK : Boolean  :=  false;
				iCNV : Boolean  :=  false;
				iDIN : Boolean  :=  false;
				iDOU : Boolean  :=  false;
				Nbit : Positive := 16;
				Ndiv : Positive := 100); -- divide the clock to get the spi clock
	port (
			 clk     : in  std_logic;
			 srst    : in  std_logic;

			 start   : in  std_logic;
			 nextch  : in  std_logic_vector( 2 downto 0);

			 rdy     : out std_logic;
			 busy    : out std_logic;
			 dout    : out std_logic_vector(11 downto 0);
			 
			 spi_sdi : in  std_logic;
			 spi_sdo : out std_logic;
			 spi_clk : out std_logic;
			 spi_csn : out std_logic);
	end component;
  -- User Signal declaration added here
  constant TICKS_FOR_1HZ : positive := SYSTEM_CLOCK/2;
  signal internal_led_clk: std_logic;     
  signal clk100khz,temp: std_logic;  
  signal rst : STD_LOGIC;
  signal pre_reset: STD_LOGIC_VECTOR(15 downto 0):="1111111111111110";
  signal temp_inf,clk_inf : STD_LOGIC;
  signal tmp_btnIntr, tmp_swIntr: STD_LOGIC;
  ------------------------------------------
  -- Signals for user logic slave model s/w accessible register example
  ------------------------------------------
  signal slv_reg0                       : std_logic_vector(C_SLV_DWIDTH-1 downto 0);
  signal slv_reg1                       : std_logic_vector(C_SLV_DWIDTH-1 downto 0);
  signal slv_reg2                       : std_logic_vector(C_SLV_DWIDTH-1 downto 0);
  signal slv_reg3                       : std_logic_vector(C_SLV_DWIDTH-1 downto 0);
  signal slv_reg4                       : std_logic_vector(C_SLV_DWIDTH-1 downto 0);
  signal slv_reg5                       : std_logic_vector(C_SLV_DWIDTH-1 downto 0);
  signal slv_reg_write_sel              : std_logic_vector(5 downto 0);
  signal slv_reg_read_sel               : std_logic_vector(5 downto 0);
  signal slv_ip2bus_data                : std_logic_vector(C_SLV_DWIDTH-1 downto 0);
  signal slv_read_ack                   : std_logic;
  signal slv_write_ack                  : std_logic;

begin

  --USER logic implementation added here

  ------------------------------------------
  -- Example code to read/write user logic slave model s/w accessible registers
  -- 
  -- Note:
  -- The example code presented here is to show you one way of reading/writing
  -- software accessible registers implemented in the user logic slave model.
  -- Each bit of the Bus2IP_WrCE/Bus2IP_RdCE signals is configured to correspond
  -- to one software accessible register by the top level template. For example,
  -- if you have four 32 bit software accessible registers in the user logic,
  -- you are basically operating on the following memory mapped registers:
  -- 
  --    Bus2IP_WrCE/Bus2IP_RdCE   Memory Mapped Register
  --                     "1000"   C_BASEADDR + 0x0
  --                     "0100"   C_BASEADDR + 0x4
  --                     "0010"   C_BASEADDR + 0x8
  --                     "0001"   C_BASEADDR + 0xC
  -- 
  ------------------------------------------
  slv_reg_write_sel <= Bus2IP_WrCE(5 downto 0);
  slv_reg_read_sel  <= Bus2IP_RdCE(5 downto 0);
  slv_write_ack     <= Bus2IP_WrCE(0) or Bus2IP_WrCE(1) or Bus2IP_WrCE(2) or Bus2IP_WrCE(3) or Bus2IP_WrCE(4) or Bus2IP_WrCE(5);
  slv_read_ack      <= Bus2IP_RdCE(0) or Bus2IP_RdCE(1) or Bus2IP_RdCE(2) or Bus2IP_RdCE(3) or Bus2IP_RdCE(4) or Bus2IP_RdCE(5);

  ----------------------------- Infrared Module -----------------------------------
  CLOCKDEVIDER: process(Bus2IP_Clk)
		variable count : integer range 0 to 127:=0;
	begin
		if rising_edge(Bus2IP_Clk) then
            if (count = 100) then
                temp_inf <= NOT(temp_inf);
                count := 0;
            else
                count := count + 1;
            end if;
        end if;
	end process CLOCKDEVIDER;
	
   clk_inf <= temp_inf;
	
   RESET: process(Bus2IP_Clk)
		variable count : integer range 0 to 127:=0;
	begin
		if rising_edge(Bus2IP_Clk) then
            if (count = 100) then
                pre_reset<= pre_reset(14 downto 0) & pre_reset(0);
                count := 0;
            else
                count := count + 1;
            end if;
        end if;
	end process RESET;
	
   rst <= pre_reset(15);
	
	adc:  adc78h89
	port map(
				 clk     => clk_inf,
				 srst    => rst,

				 start   => slv_reg1(0),
				 nextch  => slv_reg1(10 downto 8),

				 rdy     => slv_reg0(24),
				 busy    => slv_reg0(16),
				 dout    => slv_reg0(11 downto 0),

				 spi_sdi => spi_sdi,
				 spi_sdo => spi_sdo,
				 spi_clk => spi_clk,
				 spi_csn => spi_csn
			  );
  ------------- Clock Divider to Generate a 1Hz clock from a 50MHz input clock
  CLK_DIV: process(Bus2IP_Clk, Bus2IP_Resetn)
					variable count: natural range 0 to TICKS_FOR_1HZ := TICKS_FOR_1HZ - 1;
				begin
					if rising_edge(Bus2IP_Clk) then						
						if count = 0 then
							count := TICKS_FOR_1HZ - 1;
							internal_led_clk <= not internal_led_clk;			
						else
							count := count - 1;  --counter for blinker leds 1Hz
						end if;				
					end if;
			   end process;
				
  LED_C: LedControl generic map(4) 
						  port map(
						           clk 		=> internal_led_clk, --is only 1Hz
									  enable    => slv_reg2(8),
									  led_setup => slv_reg2(7 downto 0),
									  led       => led
									 );								
  ------------------ Clock Divider to generate 100KHz from a 50MHz input clock ---------------
  CLOCKDIVIDER: process(Bus2IP_Clk)
     variable count : integer range 0 to 700 := 0;
  begin
		if rising_edge(Bus2IP_Clk) then
            if (count = 250) then
                temp <= NOT(temp);
                count := 0;
            else
                count := count + 1;
            end if;
        end if;
	end process CLOCKDIVIDER;  
	clk100khz <= temp;
  ----------------------------- Buzzer -------------------------------------------------
  BUZZ: BuzzerControl Port map( 
								 clk        => clk100khz,
								 reset 		=> not Bus2IP_Resetn,
								 enable     => slv_reg2(24),
								 freq       => slv_reg2(18 downto 16),
								 buzzer_out => buzzer
							 );									
  ----------------------------- Steering and Speed Logic -----------------------------------	
	ccu: Steering_Speed 
	Port map(
		   clock           => clk100khz,
		   steering_bit_in => slv_reg3(7 downto 0),		
		   speed_bit_in    => slv_reg3(15 downto 8),						
			mode				 => slv_reg3(18 downto 16),
			steering_in     => steeringIn,		
			speed_in 		 => speedIn,
		   steering_out 	 => steeringPwmOut,
		   speed_out 		 => speedPwmOut
	);	
   ------------------ Buttons & Switches Module -----------------------------------
	mux 			 	  <= '0';	 -- This will disable SWITCHE2 to allow TFT display to work
	
   BTNSW: BtnSwitch port map(	clk        => Bus2IP_Clk,
										rst        => not Bus2IP_Resetn,
										button     => razor_Button,
										switch     => razor_Switch,										
										enBtnIntr  => slv_reg3(24),
										enSwIntr   => slv_reg3(25),																				
										btnIntr    => tmp_btnIntr,
										swIntr     => tmp_swIntr									   									   
									 );
  btnIntr <= tmp_btnIntr;	
  swIntr  <= tmp_swIntr;
  -- implement slave model software accessible register(s)
  SLAVE_REG_WRITE_PROC : process( Bus2IP_Clk ) is
  begin

    if Bus2IP_Clk'event and Bus2IP_Clk = '1' then
      if Bus2IP_Resetn = '0' then
--        slv_reg0 <= (others => '0');
        slv_reg1 <= (others => '0');
        slv_reg2 <= (others => '0');
        slv_reg3 <= (others => '0');
        slv_reg4 <= (others => '0');
        slv_reg5 <= (others => '0');
      else		
		  
		  slv_reg4(1 downto 0)   <= razor_Button;
   	  slv_reg4(11 downto 8)  <= razor_Switch;
		  slv_reg4(17 downto 16) <= tmp_swIntr & tmp_btnIntr;  
		  
        case slv_reg_write_sel is
          when "100000" =>
--            for byte_index in 0 to (C_SLV_DWIDTH/8)-1 loop
--              if ( Bus2IP_BE(byte_index) = '1' ) then
--                slv_reg0(byte_index*8+7 downto byte_index*8) <= Bus2IP_Data(byte_index*8+7 downto byte_index*8);
--              end if;
--            end loop;
          when "010000" =>
            for byte_index in 0 to (C_SLV_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
                slv_reg1(byte_index*8+7 downto byte_index*8) <= Bus2IP_Data(byte_index*8+7 downto byte_index*8);
              end if;
            end loop;
          when "001000" =>
            for byte_index in 0 to (C_SLV_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
                slv_reg2(byte_index*8+7 downto byte_index*8) <= Bus2IP_Data(byte_index*8+7 downto byte_index*8);
              end if;
            end loop;
          when "000100" =>
            for byte_index in 0 to (C_SLV_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
                slv_reg3(byte_index*8+7 downto byte_index*8) <= Bus2IP_Data(byte_index*8+7 downto byte_index*8);
              end if;
            end loop;
          when "000010" =>
--            for byte_index in 0 to (C_SLV_DWIDTH/8)-1 loop
--              if ( Bus2IP_BE(byte_index) = '1' ) then
--                slv_reg4(byte_index*8+7 downto byte_index*8) <= Bus2IP_Data(byte_index*8+7 downto byte_index*8);
--              end if;
--            end loop;
          when "000001" =>
            for byte_index in 0 to (C_SLV_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
                slv_reg5(byte_index*8+7 downto byte_index*8) <= Bus2IP_Data(byte_index*8+7 downto byte_index*8);
              end if;
            end loop;
          when others => null;
        end case;
      end if;
    end if;

  end process SLAVE_REG_WRITE_PROC;

  -- implement slave model software accessible register(s) read mux
  SLAVE_REG_READ_PROC : process( slv_reg_read_sel, slv_reg0, slv_reg1, slv_reg2, slv_reg3, slv_reg4, slv_reg5 ) is
  begin

    case slv_reg_read_sel is
      when "100000" => slv_ip2bus_data <= slv_reg0;
      when "010000" => slv_ip2bus_data <= slv_reg1;
      when "001000" => slv_ip2bus_data <= slv_reg2;
      when "000100" => slv_ip2bus_data <= slv_reg3;
      when "000010" => slv_ip2bus_data <= slv_reg4;
      when "000001" => slv_ip2bus_data <= slv_reg5;
      when others => slv_ip2bus_data <= (others => '0');
    end case; 

  end process SLAVE_REG_READ_PROC;

  ------------------------------------------
  -- Example code to drive IP to Bus signals
  ------------------------------------------
  IP2Bus_Data  <= slv_ip2bus_data when slv_read_ack = '1' else
                  (others => '0');

  IP2Bus_WrAck <= slv_write_ack;
  IP2Bus_RdAck <= slv_read_ack;
  IP2Bus_Error <= '0';

end IMP;

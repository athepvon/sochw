----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    16:33:12 07/22/2013 
-- Design Name: 
-- Module Name:    ClockDivider100MHzto100kHZ - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity ClockDivider is
	 generic (SCALE : integer := 250);
    Port ( clk_in  : in  STD_LOGIC;
			  reset   : in  STD_LOGIC;	
           clk_out : out STD_LOGIC
			 );
end ClockDivider;

architecture Behavioral of ClockDivider is

signal internal_led_clk: std_logic;

begin

	process( clk_in, reset )
	  variable led_counter: integer range 0 to SCALE - 1;
	begin
		if clk_in'event and clk_in = '1' then
			if reset = '1' then
				led_counter 			:= 0;
				internal_led_clk		<= '0';
			else
				--counter for blinker leds 1hz
				led_counter := led_counter + 1;
				if led_counter = SCALE - 1 then
					led_counter := 0;
					internal_led_clk <= not internal_led_clk;
				end if;				
			end if;
		end if;
	end process;
   
   clk_out <= internal_led_clk;
 	
end Behavioral;


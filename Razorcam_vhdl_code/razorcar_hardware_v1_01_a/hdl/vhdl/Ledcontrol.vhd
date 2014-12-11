----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    22:20:13 09/30/2013 
-- Design Name: 
-- Module Name:    Ledcontrol - Behavioral 
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

entity Ledcontrol is
   generic(NUM_LED : positive := 4);
	port(	clk 		: in  std_logic; 										-- blinking clock frequency
			enable   : in  std_logic; 										-- enable or disable all Led's 0=off; 1=on
			led_setup: in  std_logic_vector((NUM_LED*2-1) downto 0); 	-- Led setup input 8x2 bit 
         led      : out std_logic_vector((NUM_LED-1) downto 0)
		  );  	-- Led output
end Ledcontrol;

architecture Behavioral of Ledcontrol is

begin
	process(clk, led_setup, enable) 
	Begin
		for i in (NUM_LED-1) downto 0 loop
			led(i) <=  ((enable and led_setup(2*i) and not clk) or (enable and led_setup(2*i+1) and clk));
		end loop;
	end process;
end Behavioral;


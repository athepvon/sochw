----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    22:15:30 10/03/2013 
-- Design Name: 
-- Module Name:    Buzzer - Behavioral 
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

entity Buzzercontrol is
    Port ( clk : in  STD_LOGIC;
			  reset : in  STD_LOGIC;	
           enable : in  STD_LOGIC;
           freq : in  std_logic_vector(2 downto 0);
           buzzer_out : out  STD_LOGIC);
end Buzzercontrol;

architecture Behavioral of Buzzercontrol is

signal scale : natural range 0 to 50;
signal  temp: std_logic;

begin

	process (freq)
	begin
		case (freq) is 
			when "000" 	=> scale <= 50; -- 1KHz
			when "001" 	=> scale <= 25; -- 2KHz
			when "010" 	=> scale <= 16; -- 3KHz 
			when "011" 	=> scale <= 12; -- 4KHz 
			when "100" 	=> scale <= 10; -- 5KHz
			when "101" 	=> scale <= 8;  -- 6KHz
			when "110" 	=> scale <= 7;  -- 7KHz 	
			when others => scale <= 6;  -- 8KHz				
		end case;
	end process;

	process(clk)
	  variable counter : natural range 0 to 50 := 0;
	begin
		  if reset = '1' then
			   counter := 0;
				temp <= '0';
		  elsif rising_edge(clk) then
			 if(counter >= scale) then
				temp    <= not temp;
				counter := 0;
			 else				
				counter := counter + 1;
			 end if;
		  end if;
	end process;
	
	buzzer_out <= enable and temp;
	
end Behavioral;


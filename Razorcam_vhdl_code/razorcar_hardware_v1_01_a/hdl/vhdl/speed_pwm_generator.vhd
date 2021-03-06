----------------------------------------------------------------------------------
-- Company: 
-- Engineer:        Franck Yonga
-- 
-- Create Date:    10:32:13 04/24/2013 
-- Design Name: 
-- Module Name:    speed_pwm - Behavioral 
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
use IEEE.NUMERIC_STD.all;

entity speed_pwm_generator is
    generic (				 
	          LIMIT_MAX : integer range -64 to 63 := 60;			
			    LIMIT_MIN : integer range -64 to 63 := -40			
	    );
    Port ( clock : in  STD_LOGIC;	
           speed_bit_in : in  STD_LOGIC_VECTOR (7 downto 0);
           speed_pwm_out : out  STD_LOGIC);
end speed_pwm_generator;

architecture Behavioral of speed_pwm_generator is

signal on_limit: integer range 0 to 255 := 147; -- Default Value for the calibration
signal cycle : integer range 0 to 1023 := 0;

begin
PWM: process(clock)	
	variable var : integer range -64 to 63;
begin	
   if(rising_edge(clock)) then			
		if(cycle < 1000) then
			cycle <= cycle + 1;
		else
			cycle <= 0;								
			var := to_integer(signed(speed_bit_in));
			if var > LIMIT_MAX then
				var := LIMIT_MAX;	
			elsif var < LIMIT_MIN then
				var := LIMIT_MIN;				
			end if;

			on_limit <= 146 + var;
		end if;				
		
		if (cycle < on_limit) then			
			 speed_pwm_out <= '1';				
		else		
			 speed_pwm_out <= '0';				
		end if;
   end if;
end process PWM;
	
end Behavioral;



----------------------------------------------------------------------------------
-- Company: 
-- Engineer:       Franck Yonga
-- 
-- Create Date:    19:04:18 07/22/2013 
-- Design Name:    	
-- Module Name:    steering_pwm - Behavioral 
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

entity steering_pwm_generator is
    generic (
	      LIMIT_MAX : natural range 0 to 127 := 90
	   );
    Port ( clock : in  STD_LOGIC;
           steering_bit_in : in  STD_LOGIC_VECTOR (6 downto 0);
           steering_pwm_out : out  STD_LOGIC);
end steering_pwm_generator;

architecture Behavioral of steering_pwm_generator is

signal on_limit: integer range 0 to 255 := 152; -- Default Value for the calibration
signal cycle : integer range 0 to 1023 := 0;

begin
PWM: process(clock)	
	variable var : natural range 0 to 127;
begin	
	if(rising_edge(clock)) then			
	  if(cycle < 1000) then
	     cycle <= cycle + 1;
	  else
	    cycle <= 0;
	    var := to_integer(unsigned(steering_bit_in));
	    if var > LIMIT_MAX then
	       var := LIMIT_MAX;				
	    end if;

	    on_limit <= 110 + var;
	  end if;				
		
	  if (cycle < on_limit) then				
	      steering_pwm_out <= '1';				
	  else				
	      steering_pwm_out <= '0';				
	  end if;
   end if;
end process PWM;

end Behavioral;


----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    20:04:22 08/15/2013 
-- Design Name: 
-- Module Name:    transmission_detection - Behavioral 
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

entity transmission_detection is
	 generic (DETECT_RANGE : integer := 5);
    Port ( clock : in  STD_LOGIC;
           steering_in : in  STD_LOGIC;
           speed_in : in  STD_LOGIC;
			  enable_transm : in  STD_LOGIC;
           detected_trans : out  STD_LOGIC);
end transmission_detection;

architecture Behavioral of transmission_detection is

signal temp_steer, temp_speed : STD_LOGIC;

begin
------------------------------ Monitoring incoming signal from transmitter ----------------------------------------------
	--- Steering signal ---
	MONITORING_STEERING: process(clock)
		variable cycle_count : integer range 0 to 250;
		variable cycle_count_old : integer range 0 to 250;
		variable flag : boolean := false;	
	begin		
		if rising_edge(clock) then		
			if steering_in = '1' then
				cycle_count := cycle_count + 1;
				flag := true;				
			else
				if flag = true then
					if cycle_count < (cycle_count_old + DETECT_RANGE) and cycle_count > (cycle_count_old - DETECT_RANGE) then
						temp_steer <= '0';
					else
						temp_steer <= '1';
					end if;
				
					cycle_count_old := cycle_count;
					cycle_count := 0;
					flag := false;	
				end if;					
		  	end if;
	  end if;		
	end process MONITORING_STEERING;
	
	MONITORING_SPEED: process(clock)
		variable cycle_count : integer range 0 to 250;
		variable cycle_count_old : integer range 0 to 250;
		variable flag : boolean := false;	
	begin		
	   if rising_edge(clock) then	    
			if speed_in = '1' then
				cycle_count := cycle_count + 1;
				flag := true;				
			else
				if flag = true then
					if cycle_count < (cycle_count_old + DETECT_RANGE) and cycle_count > (cycle_count_old - DETECT_RANGE) then
						temp_speed <= '0';
					else
						temp_speed <= '1';
					end if;
				
					cycle_count_old := cycle_count;
					cycle_count := 0;
					flag := false;	
				end if;					
			end if;
	  end if;		
	end process MONITORING_SPEED;
	
	detected_trans <= enable_transm and (temp_steer or temp_speed); 

end Behavioral;


----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    19:57:11 08/19/2013 
-- Design Name: 
-- Module Name:    state_machine - Behavioral 
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

entity state_machine is
    Port ( clk : in  STD_LOGIC;
           detected_trans : in  STD_LOGIC;
           enable : in  STD_LOGIC;
           sel : out  STD_LOGIC);
end state_machine;

architecture Behavioral of state_machine is
-- states for the state machine
type state_type is (WAIT_DETECTED_TRANS, WAIT_ENABLE);
signal state   : state_type := WAIT_DETECTED_TRANS;

begin

	process (clk)
	begin
       if (rising_edge(clk)) then
			case state is				
				when WAIT_DETECTED_TRANS => -- Automatic mode
					if detected_trans = '1' then 
						state <= WAIT_ENABLE; -- Switch to manual mode
					else
						state <= WAIT_DETECTED_TRANS; -- -- Stay on automatic mode
					end if;
				when WAIT_ENABLE => -- Manual mode
					if enable = '1' then
						state <= WAIT_DETECTED_TRANS; -- Request to switch to Automatic mode received
					else
						state <= WAIT_ENABLE;
					end if;
			end case;
		end if;
	end process;
	
	-- Output depends solely on the current state
	process (state)
	begin	
		if(state = WAIT_ENABLE) then
			sel <= '1'; -- Manual mode
		else
			sel <= '0'; -- Automatic Mode				
		end if;
	end process;
	
end Behavioral;


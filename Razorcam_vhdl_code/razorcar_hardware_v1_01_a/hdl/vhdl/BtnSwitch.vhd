----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    22:43:46 01/05/2010 -- updated on 10/21/2013
-- Design Name: 
-- Module Name:    BtnSwitch - Behavioral 
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
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

---- Uncomment the following library declaration if instantiating
---- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity BtnSwitch is
    Port ( clk       : in  STD_LOGIC;
           rst       : in  STD_LOGIC;
           button    : in  STD_LOGIC_VECTOR(1 downto 0);
           switch    : in  STD_LOGIC_VECTOR(3 downto 0);
           enBtnIntr : in  STD_LOGIC;
           enSwIntr  : in  STD_LOGIC;
           btnIntr   : out STD_LOGIC;
           swIntr    : out STD_LOGIC);
end BtnSwitch;

architecture Behavioral of BtnSwitch is
signal switchReg: STD_LOGIC_VECTOR(3 downto 0);
signal buttonReg: STD_LOGIC_VECTOR(1 downto 0);

begin

	Process(clk, rst, buttonReg, switchReg, enBtnIntr, enSwIntr, switch, button) 
	begin
		if (rst = '1') then
			switchReg <= switch;
			buttonReg <= button;
		elsif rising_edge(clk) then
			swIntr    <= '0';
			btnIntr	 <= '0';
			
			if (switchReg = switch) then
				switchReg <= switch;
			else
				switchReg <= switch;
				if (enSwIntr = '1') then 
					 swIntr <= '1';
				end if;
			end if;
			
			if (buttonReg = button) then
				buttonReg <= button;
			else
				buttonReg <= button;
				if (enBtnIntr = '1') then
					btnIntr	 <= '1';
				end if;
			end if;
			
		end if;
	end process;

end Behavioral;

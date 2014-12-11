library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
entity pixel_command_lookup is
    Port ( sel : in integer range 0 to 69;
			       x: in std_logic_vector(8 downto 0):=(others=>'0');
					 y: in std_logic_vector(8 downto 0):=(others=>'0');
           data_out : out  STD_LOGIC_VECTOR (7 downto 0));
end pixel_command_lookup;

architecture Behavioral of pixel_command_lookup is

	-- Define data type to hold bytes of data to be written to PmodCLS
	type LOOKUP is array ( 0 to 17 ) of std_logic_vector (7 downto 0);
   signal  command : LOOKUP;
	-- Hexadecimal values below represent ASCII characters
	
begin
 command <=( 
					X"03",X"00",--X"00",--x(7 downto 0),--
					X"02",X"00",--X"00",--"0000000" & x(8),					
					X"05",X"3F",--X"3F",--x(7 downto 0),					
					X"04",X"01",--X"01",--"0000000" & x(8),					
					X"07",X"00",--X"00",--y(7 downto 0),					
					X"06",X"00",--X"00",--"0000000" & y(8),
					X"09",X"EF",--X"EF",--y(7 downto 0),
					X"08",X"00",--X"00",--"0000000" & y(8),--
					X"22",X"FF" 
					);
	-- Assign byte to output
	data_out <= command( sel );

end Behavioral;


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
entity command_lookup is
    Port ( sel : in integer range 0 to 69;
           data_out : out  STD_LOGIC_VECTOR (7 downto 0));
end command_lookup;

architecture Behavioral of command_lookup is

	-- Define data type to hold bytes of data to be written to PmodCLS
	type LOOKUP is array ( 0 to 69 ) of std_logic_vector (7 downto 0);

	-- Hexadecimal values below represent ASCII characters
	constant command : LOOKUP  := ( X"EA",X"00",
					X"EB",X"20",												
					X"EC",X"0C",												
					X"ED",X"C4",
					X"E8",X"40",
					X"E9",X"38",
					X"F1",X"01",
					X"F2",X"10",
					X"27",X"A3",
					X"1B",X"1B",
					X"1A",X"01",
					X"24",X"2F",
					X"25",X"57",
					X"23",X"8D",
					X"18",X"36",
					X"19",X"01",
					X"01",X"00",
					X"1F",X"88",
					X"1F",X"80",
					X"1F",X"90",
					X"1F",X"D0",
					X"17",X"05",
					X"36",X"00",
					X"28",X"38",
					X"28",X"3C",
					X"16",X"A8",
					X"03",X"00",
					X"02",X"00",					
					X"05",X"3F",					
					X"04",X"01",					
					X"07",X"00",					
					X"06",X"00",
					X"09",X"EF",
					X"08",X"00",
					X"22",X"00" -- CLEAR COLOR
					);
begin

	-- Assign byte to output
	data_out <= command( sel );

end Behavioral;


----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    11:17:24 11/21/2009 
-- Design Name: 
-- Module Name:    synth_Bram - Behavioral 
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
use IEEE.NUMERIC_STD.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

---- Uncomment the following library declaration if instantiating
---- any Xilinx primitives in this code.
library UNISIM;
use UNISIM.VComponents.all;

Library UNIMACRO;
use UNIMACRO.vcomponents.all;

entity synth_Bram is
    generic ( d_width : natural := 22; --ausgehend vom größten Bild das die Kamera liefert
	                                    --können höchstens 2048*1536 Pixel der selben Farbe auftreten
              a_width : natural := 8  --Bitbreite einer Komponente            
				);
	 Port ( clka : in  STD_LOGIC;
           dina : in  STD_LOGIC_VECTOR (d_width-1 downto 0);
           addra : in  STD_LOGIC_VECTOR (a_width-1 downto 0);
           wea : in  STD_LOGIC;
           clkb : in  STD_LOGIC;
           addrb : in  STD_LOGIC_VECTOR (a_width-1 downto 0);
           doutb : out  STD_LOGIC_VECTOR (d_width-1 downto 0));
end synth_Bram;

architecture Behavioral of synth_Bram is

  type ram_type is array ((2**a_width)-1 downto 0)  
        of std_logic_vector (d_width-1 downto 0);  
  signal RAM : ram_type := (others => (others => '0'));    
  signal read_dpra : std_logic_vector(a_width-1 downto 0);   
  
begin  
  process (clka)  
  begin  
    if (clka'event and clka = '1') then  
      if (wea = '1') then  
        RAM(to_integer(unsigned(addra))) <= dina;  
      end if; 
    end if;  
  end process;
  
  process (clkb)
  begin
    if (clkb'event and clkb = '1') then
	    read_dpra <= addrb;  
    end if;
  end process;  
  doutb <= RAM(to_integer(unsigned(read_dpra)));  

end Behavioral;


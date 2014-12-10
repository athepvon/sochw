library ieee;
  use ieee.std_logic_1164.all;
  use ieee.std_logic_unsigned.all;
  use ieee.std_logic_arith.all;
  
  
entity sdi_deserialize is
generic(
C_DWIDTH : integer := 80;
C_PIXEL_NO : integer := 8
);
port(
	
	clk               : in std_logic;
	rst             : in std_logic;
	pu_reset          : in std_logic;
	
	stream_in_data		: in std_logic_vector(C_DWIDTH-1 downto 0);
	stream_in_valid	: in std_logic;
	stream_in_stop    : out std_logic;

	
	stream_out_data	: out std_logic_vector(((C_DWIDTH/C_PIXEL_NO)-1) downto 0);
	stream_out_valid  : out std_logic;
	stream_out_stop   : in std_logic
);
end sdi_deserialize;

architecture arch of sdi_deserialize is
	signal count : integer range 0 to C_PIXEL_NO-1 := C_PIXEL_NO-1;
	
	type STATES is (IDLE,HANDLE_PIXEL);
	signal state : STATES;
begin
	
	STD_PROC : process(clk,rst,pu_reset)
	begin
		if rst='1' or pu_reset='1' then
			state <= IDLE;
			stream_in_stop <= '0';
		elsif rising_edge(clk) then
			--stream_in_stop <= '0';
			--stream_out_valid <= '0';
			
			case state is
			when IDLE =>
			   stream_out_valid<= '0';
				if stream_in_valid = '1' then
					state <= HANDLE_PIXEL;
					stream_in_stop <= '1';
					if stream_out_stop = '0' then
						stream_out_valid <= '1';
						stream_out_data <= stream_in_data(C_DWIDTH-1 downto C_DWIDTH-(C_DWIDTH/C_PIXEL_NO));
						count <= C_PIXEL_NO-2;
					else
						count <= C_PIXEL_NO-1;
					end if;
					
				end if;
				
			when HANDLE_PIXEL =>
				stream_in_stop <= '1';
				if stream_out_stop = '0' then
					stream_out_valid <= '1';
					stream_out_data <=  stream_in_data(count*(C_DWIDTH/C_PIXEL_NO) + (C_DWIDTH/C_PIXEL_NO)-1 downto count*(C_DWIDTH/C_PIXEL_NO));
					if count > 0 then
						count <= count - 1;
					else
						state <= IDLE;
						stream_in_stop <= '0';
					end if;
				end if;
				
			end case;
					
		end if;
	end process;
end arch;
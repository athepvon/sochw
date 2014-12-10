LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.all;

-- $Id$:

-- MSB first
-- SCLK from 50 kHz to 8 MHz

entity adc78h89 is
generic (
         iCLK : Boolean  :=  false;
         iCNV : Boolean  :=  false;
         iDIN : Boolean  :=  false;
         iDOU : Boolean  :=  false;
         Nbit : Positive := 16;
         Ndiv : Positive := 2); -- divide the clock to get the spi clock
port (
    clk     : in  std_logic;
    srst    : in  std_logic;

    start   : in  std_logic;
    nextch  : in  std_logic_vector( 2 downto 0);

    rdy     : out std_logic;
    busy    : out std_logic;
    dout    : out std_logic_vector(11 downto 0);


    spi_sdi : in  std_logic;
    spi_sdo : out std_logic;
    spi_clk : out std_logic;
    spi_csn : out std_logic);
end adc78h89;

architecture a of adc78h89 is

signal bits  : Integer range 0 to Nbit-1;
signal bcnt  : Integer range 0 to Ndiv-1;

-- output : 16 bits, of them the first 4 sent are zero, the rest the ADC result, MSB first.
--          available at the rising edge of the SCLK
-- input  : "--ccc--..." - two don't care bits, 3 bits with the ADC channel (MSB first), the rest don't care
--          latched with the rising edge of the SCLK

type sm_type is (idle, clk0, clk1, finish);

-- The syn_encoding attribute has 4 values:
-- sequential, onehot, gray and safe.
attribute syn_enum_encoding : string;
attribute syn_enum_encoding of sm_type : type is "gray";

signal sm : sm_type := idle;

signal addr   : std_logic_vector(nextch'length+2-1 downto 0);
signal adc    : std_logic_vector(dout'range);

signal spi_cnv_i : std_logic;
signal spi_clk_i : std_logic;
signal spi_sdi_i : std_logic;
signal busy_i    : std_logic;
signal start_i   : std_logic;
signal shift_in  : std_logic;
signal shift_out : std_logic;

begin
    -- write to the address register, only if not busy!
    -- else shift
    process(clk)
    begin
        if rising_edge(clk) then
            start_i <= '0';
            -- write the new values to the dac registers
            if start = '1' and busy_i = '0' then
                addr    <= "00" & nextch;
                start_i <= '1';
                busy_i  <= '1';
            else
                if shift_out = '1' then -- send channel address to the IR 
                   addr <= addr(addr'high-1 downto 0) & '0'; -- left shift
                end if;
            end if;

            if shift_in  = '1' then -- receive data from the IR sensors
					adc <= adc(adc'high-1 downto 0) & spi_sdi_i; -- left shift & concatenate to the right
				end if;

            if start_i = '1' then 
               busy_i <= '1';
            elsif sm = idle then 
               busy_i <= '0'; 
            end if;

        end if;
    end process;

    spi_sdo <= not addr(addr'high) when iDOU else addr(addr'high);
    spi_sdi_i <= not spi_sdi when iDIN else spi_sdi;
    busy    <= busy_i;
    dout    <= adc;

    process(clk)
    begin
        if rising_edge(clk) then
            shift_out <= '0';
            shift_in  <= '0';
            spi_clk_i <= '1';
            spi_cnv_i <= '1';
            rdy       <= '0';
            if srst = '1' then
                sm <= idle;
                bcnt <= Ndiv - 1;
                bits <= Nbit-1;
            else
					case sm is
					when idle => bcnt <= Ndiv - 1;
									 bits <= Nbit - 1;
									 if start = '1' then
										 sm <= clk0;
										 spi_cnv_i <= '0'; -- CS goes low before SCLK!
									end if;
					when clk0 => bcnt <= bcnt - 1;
									 spi_clk_i <= '0';
									 spi_cnv_i <= '0';

									 if bcnt = Ndiv/2 then
										 sm <= clk1;
										 shift_in <= '1';
									 end if;
					when clk1 =>
									 spi_cnv_i <= '0';
									 if bcnt = 0 then
										 bcnt <= Ndiv - 1;
										 if bits = 0 then
											  sm <= finish;
											  rdy <= '1';
										 else
											  sm <= clk0;
											  shift_out <= '1';
											  bits <= bits - 1;
										 end if;
									 else
										 bcnt <= bcnt - 1;
									 end if;

					when finish =>
										 if bcnt = 0 then
											  sm <= idle;
										 else
											  bcnt <= bcnt - 1;
										 end if;
					end case;
            end if;
        end if;
    end process;

    spi_clk   <= not spi_clk_i when iCLK else spi_clk_i;
    spi_csn   <= not spi_cnv_i when iCNV else spi_cnv_i;
end;

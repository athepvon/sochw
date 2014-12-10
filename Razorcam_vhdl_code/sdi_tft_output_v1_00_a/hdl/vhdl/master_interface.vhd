
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

-- *******************************************************************************
--  								Define block inputs and outputs
-- *******************************************************************************
entity master_interface is
    Port ( begin_transmission : out  STD_LOGIC;
           end_transmission : in  STD_LOGIC;
           clk : in  STD_LOGIC;
           rst : in  STD_LOGIC;
			  x: out std_logic_vector(8 downto 0);
			  y: out std_logic_vector(8 downto 0);
			  pixeldata: in std_logic_vector(15 downto 0);
			  pixeldone: out std_logic;
			  newpixel:in  std_logic;
                          framedone: out std_logic;
			  framestart:in  std_logic;
			  tft_ready : out STD_LOGIC;
	        slave_reset : out STD_LOGIC;
           slave_select : out  STD_LOGIC;
	        sel : out integer range 0 to 69;
			  sel1 : out integer range 0 to 69;
			  temp_data1 : in  STD_LOGIC_VECTOR (7 downto 0);
           temp_data : in  STD_LOGIC_VECTOR (7 downto 0);
           send_data : out  STD_LOGIC_VECTOR (7 downto 0));
end master_interface;

architecture Behavioral of master_interface is
	
-- *******************************************************************************
--  								Signals, Constants, and Types
-- *******************************************************************************

	-- FSM states
	type stateType is ( idle ,resets,vga,wait_vga,cs_disablepvga,cs_enable,send_reg,wait_reg,display,wait_run,cs_disable1,cs_enable1,send_data1,wait_data,display1,wait_run1,send_clear,wait_clear,cs_disable,done,cs_enablep,send_regp ,wait_regp,displayp,wait_runp,cs_disable1p ,cs_enable1p,send_data1p,wait_datap,display1p ,wait_run1p,send_color,wait_color,send_color1,wait_color1,cs_disablep,fin);
	signal STATE : stateType;

signal tft_ready1 :  STD_LOGIC;
	-- Used to select data sent to SPI Interface component and used for FSM control
	signal count_sel : integer range 0 to 69;
	signal count_sel1 : integer range 0 to 69;
	-- Counts up to COUNT_SS_MAX, once reached the SS is asserted (i.e. shut off)
	signal count_ss : std_logic_vector( 11 downto 0 );
	-- Determines the duration that SS is deasserted (i.e. turned on)
	constant COUNT_SS_MAX : std_logic_vector( 11 downto 0 ) := X"FFF";
	-- Signal to execute a reset on the PmodCLS
	signal exeRst : std_logic := '0';
	signal count : integer range 0 to 153600;
	signal color : std_logic_vector( 15 downto 0 );
--	signal x1:integer range 0 to 320;
--	signal y1:integer range 0 to 240;
   shared variable x1:integer range 0 to 320;
	shared variable y1:integer range 0 to 240;
	signal resetdone: std_logic := '0';
-- *******************************************************************************
--  										Implementation
-- *******************************************************************************
begin
   tft_ready<=tft_ready1;
   color<=pixeldata;--pixeldata &pixeldata; pixeldata(23 downto 19) & pixeldata(15 downto 10) & pixeldata(7 downto 3);
	-- Master Interface FSM
	process(clk)
	variable counter:integer:=0;
	begin
	 if(rst='1') then
	  counter:=0;
	  resetdone<='0';
	 else
	   counter:=counter+1;
		if(counter=5000) then
		  resetdone<='1';
		  counter:=0;
		end if;
	 end if;
	end process;
	
	master_interface : process( clk ) 
        variable countr: integer;
	begin
	if rising_edge( clk ) then 
		-- Reset
		if rst = '1' then
			STATE <= idle;
			count_sel <= 0;
			count_sel1 <= 0;
			tft_ready1<='0';
			count<= 0;
			send_data <= ( others => '0' );
			slave_select <= '1';
	                slave_reset<= '0';
			count_ss <= ( others => '0' );
			begin_transmission <= '0';
			pixeldone<='1';
			x1:=0;
			y1:=0;
         framedone<='1';
--		elsif (tft_ready1='1')  and (framestart='1' ) then 
--               begin_transmission <= '0';
--					slave_select <= '1';	
--					count_sel1 <= 0;
--					x1:=0;
--					y1:=0;
--					pixeldone<='1';
--					STATE <= cs_enablep;
--               framedone<='1';		
		else
			-- FSM
			case STATE is			
				when idle =>
					count_sel <= 0;
					count<= 0;
					count_sel1 <= 0;
					slave_select <= '1';
                slave_reset<= '0';
					STATE <=  resets;
					tft_ready1<='0';
					pixeldone<='1';
					x1:=0;
			                y1:=0;
                                     framedone<='1';
                                    countr:=0;
				when resets=>
                                    countr:=countr+1;
				if countr = 50000 then
						STATE <=  cs_enable;
					else 
						null;
					end if;				  
			   when cs_enable =>
				   slave_reset<= '1';
					slave_select <= '0';
					STATE <= send_reg;
				when send_reg =>
               send_data <= X"70";
					begin_transmission <= '1';
					STATE <= wait_reg;
				when wait_reg=>
					begin_transmission <= '0';
					if end_transmission = '1' then
						STATE <= display;
					else 
						null;
					end if;
				when display =>
					send_data <= temp_data;
					begin_transmission <= '1';
					STATE <= wait_run;
				when wait_run=>
					begin_transmission <= '0';
					if end_transmission = '1' then
						STATE <= cs_disable1;
						count_sel <= count_sel + 1;
					else 
						null;
					end if;
				 when cs_disable1 =>
					 slave_select <= '1';
					 STATE <= cs_enable1;
            when cs_enable1 =>
					slave_select <= '0';
					STATE <= send_data1;					 
				 when send_data1 =>
               send_data <= X"72";
					begin_transmission <= '1';
					STATE <= wait_data;
				when wait_data=>
					begin_transmission <= '0';
					if end_transmission = '1' then
						STATE <= display1;
					else 
						null;
					end if;	
				 when display1 =>
					send_data <= temp_data;
					begin_transmission <= '1';
					STATE <= wait_run1;
				when wait_run1=>
					begin_transmission <= '0';
					if end_transmission = '1' then
						if count_sel = 69 then
						  --count_sel <= count_sel - 1;
						  STATE <= send_clear;
						  --count<=count+1;
						ELSE
						  count_sel <= count_sel + 1;
						  STATE <= cs_disable;
						end if;
					else 
						null;
					end if;
				 when send_clear =>
               send_data <= temp_data;
					begin_transmission <= '1';
					STATE <= wait_clear;
				when wait_clear=>
					begin_transmission <= '0';
					if end_transmission = '1' then						
						if count = 153600 then --153600 
						  --count_sel <= count_sel + 1;
						  STATE <= cs_disable;
						ELSE
						  count<=count+1;
						  STATE <= send_clear;
						end if;
					else 
						null;
					end if;		
				 when cs_disable =>
					slave_select <= '1';
					if count_sel = 69 then
						STATE <= done;
					else 
					   STATE <= cs_enable;	
					end if;
				when done =>
					begin_transmission <= '0';
					slave_select <= '1';	
					count_sel1 <= 0;
					x1:=0;
					y1:=0;
					pixeldone<='1';
					tft_ready1<='1';
               framedone<='1';
					--if (newpixel='1') then 
					if (newpixel='1')  and (framestart='1' )then 
					   STATE <= cs_enablep;
					else 
						null;
					end if;
            
				-- When signals indicate an invalid state
				
				when cs_enablep =>
					slave_select <= '0';
                framedone<='0';
					STATE <= send_regp;
					pixeldone<='0';
				when send_regp =>
               send_data <= X"70";
					begin_transmission <= '1';
					STATE <= wait_regp;
				when wait_regp=>
					begin_transmission <= '0';
					if end_transmission = '1' then
						STATE <= displayp;
					else 
						null;
					end if;
				when displayp =>
					send_data <= temp_data1;
					begin_transmission <= '1';
					STATE <= wait_runp;
				when wait_runp=>
					begin_transmission <= '0';
					if end_transmission = '1' then
						STATE <= cs_disable1p;
						count_sel1 <= count_sel1 + 1;
					else 
						null;
					end if;
				 when cs_disable1p =>
					 slave_select <= '1';
					 STATE <= cs_enable1p;
            when cs_enable1p =>
					slave_select <= '0';
					STATE <= send_data1p;					 
				 when send_data1p =>
               send_data <= X"72";
					begin_transmission <= '1';
					STATE <= wait_datap;
				when wait_datap=>
					begin_transmission <= '0';
					if end_transmission = '1' then
						if count_sel1 = 17 then
						  STATE <= send_color;
						ELSE
						  STATE <= display1p;
						end if;						  
					else 
						null;


					end if;
				 when display1p =>
					send_data <= temp_data1;
					begin_transmission <= '1';
					STATE <= wait_run1p;
				when wait_run1p=>
					begin_transmission <= '0';
					if end_transmission = '1' then	
                    count_sel1 <= count_sel1 + 1;				   
						  STATE <= cs_disablep;					  
					else 
						null;
					end if;
				when send_color =>
				   pixeldone<='0';
					send_data <= color(15 downto 8);
					begin_transmission <= '1';
					STATE <= wait_color;
				when wait_color=>
					begin_transmission <= '0';
					if end_transmission = '1' then
						STATE <= send_color1;
					else 
						null;
					end if;
	         when send_color1 =>
					send_data <= color(7 downto 0);
					begin_transmission <= '1';
					STATE <= wait_color1;
			when wait_color1=>
					begin_transmission <= '0';
					if end_transmission = '1' then
						--STATE <= cs_disablep;
						pixeldone<='1';
						STATE <= vga;
					else 
						null;
					end if;	
            when vga=>
                  x1:=x1+1;
						if(x1=320) then
						  y1:=y1+1;
						  x1:=0;
						end if;
						if(y1=240) then
						  y1:=0;
						end if;
						 if (x1=0) and (y1=0)  then 
						   STATE <= cs_disablepvga;
						 elsif (newpixel='1') then 
					    STATE <= send_color;
						 else
						  STATE <= wait_vga;
					    end if;	
            when wait_vga=>	
                  if (newpixel='1') then 
					    STATE <= send_color;
						 else
						  null;
					    end if;	
            when cs_disablepvga =>
					slave_select <= '1';
					STATE <= done;	
		         framedone<='0';
				when cs_disablep =>
					slave_select <= '1';
					if count_sel1 = 17 then
						STATE <= done;
						x1:=x1+1;
						if(x1=320) then
						 y1:=y1+1;
						  x1:=0;
						end if;
						if(y1=240) then
						  y1:=0;
						end if;
						--STATE <= fin;
					else 
					   STATE <= cs_enablep;	
					end if;
            when fin => 
					null;					
				when others => 
					null;
					
			end case;
		end if;
	end if;
	end process;
	-- End Master Interface FSM

	--  Signal to select the data to be sent to the PmodCLS
	SEL <= count_sel;
   SEL1 <= count_sel1;
	x<=conv_std_logic_vector(x1,9);
	y<=conv_std_logic_vector(y1,9);
end Behavioral;

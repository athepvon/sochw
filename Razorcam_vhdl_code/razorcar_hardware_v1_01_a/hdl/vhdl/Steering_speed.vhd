----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    16:28:23 07/22/2013 
-- Design Name: 
-- Module Name:    start - Behavioral 
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
library UNISIM;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.all;
use UNISIM.vcomponents.all;
--use work.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity Steering_Speed is
	Port(
			clock: 				in std_logic;
			steering_bit_in : in std_logic_vector(7 downto 0);		
			speed_bit_in : 	in std_logic_vector(7 downto 0);
			mode : 				in STD_LOGIC_VECTOR(2 downto 0);
			steering_in : 		in std_logic;		
			speed_in : 			in std_logic;			
			steering_out : 	out STD_LOGIC;
			speed_out : 		out  STD_LOGIC			
	);
end Steering_Speed;

architecture Behavioral of Steering_Speed is

component speed_pwm_generator is
	 generic (				 
				   LIMIT_MAX : integer range -64 to 63 := 60;			
			      LIMIT_MIN : integer range -64 to 63 := -40			
				);
    Port ( clock : 			in STD_LOGIC; 
           speed_bit_in : 	in STD_LOGIC_VECTOR (7 downto 0);
           speed_pwm_out : out STD_LOGIC);
end component;

component steering_pwm_generator is
generic (
			 LIMIT_MAX : integer range -64 to 63 := 63;
			 LIMIT_MIN : integer range -64 to 63 := -40
			);
    Port ( clock : 				in STD_LOGIC;
           steering_bit_in : 	in STD_LOGIC_VECTOR (7 downto 0);
           steering_pwm_out : out STD_LOGIC);
end component;

component transmission_detection is
	 generic (DETECT_RANGE : integer := 3);
    Port ( clock : in STD_LOGIC;
           steering_in : in STD_LOGIC;
           speed_in : in STD_LOGIC;
			  enable_transm : in STD_LOGIC;
           detected_trans : out STD_LOGIC);
end component;

component state_machine is
    Port ( clk : in STD_LOGIC;
           detected_trans : in STD_LOGIC;
           enable : in STD_LOGIC;
           sel : out STD_LOGIC);
end component;

signal steering_pwm_out,speed_pwm_out: std_logic;
signal steering_trans,speed_trans: STD_LOGIC;
signal detected_trans : std_logic;
signal enable_semi_auto,select_output: std_logic;

begin
   --------------------------- Steering PWM Generator Process ------------------------------------
	STE: steering_pwm_generator
	generic map(LIMIT_MAX => 63, LIMIT_MIN => -40)
	Port map( clock => clock,	
				 steering_bit_in => steering_bit_in,
				 steering_pwm_out => steering_pwm_out
			  );	
   --------------------------- Speed PWM Generator Process ------------------------------------
	SPE: speed_pwm_generator
	generic map(LIMIT_MAX => 60, LIMIT_MIN => -40)
   Port map(  clock => clock,
              speed_bit_in => speed_bit_in,
              speed_pwm_out => speed_pwm_out
			  );			  			  			
   ---------------------- Monitoring incoming signal from Transmitter ---------------------
	enable_semi_auto <= mode(1) and not mode(0);
	
	TRANS_DETECT: transmission_detection 
	generic map(DETECT_RANGE => 5)
   Port map(  clock 			  => clock,
              steering_in 	  => steering_in,
              speed_in 		  => speed_in,
				  enable_transm  => enable_semi_auto,
              detected_trans => detected_trans
			  );			    
   ---------------------- Moore State machine to implement semi-automatic driving--------------------
	SM: state_machine
   port map( 
				  clk            => clock,
				  detected_trans => detected_trans,
				  enable         => mode(2),
				  sel            => select_output
	 		  );			
			  
	MUX_STATEM:process(select_output, steering_in, speed_in, steering_pwm_out, speed_pwm_out) 		  
	begin
		if(select_output = '0')then
			steering_trans <= steering_pwm_out;
			speed_trans    <= speed_pwm_out;
		else
			steering_trans <= steering_in;
			speed_trans    <= speed_in;
		end if;
	end process;
   ----------------------------- Outputs Multiplexing ------------------------------------	
	steering_out <= (mode(1) and not mode(0) and steering_trans) or (not mode(1) and mode(0) and steering_in) or (not mode(1) and not mode(0) and steering_pwm_out);
	speed_out    <= (mode(1) and not mode(0) and speed_trans)    or (not mode(1) and mode(0) and speed_in)    or (not mode(1) and not mode(0) and speed_pwm_out);		
end Behavioral;


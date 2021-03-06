------------------------------------------------------------------------------
-- user_logic.vhd - entity/architecture pair
------------------------------------------------------------------------------
--
-- ***************************************************************************
-- ** Copyright (c) 1995-2011 Xilinx, Inc.  All rights reserved.            **
-- **                                                                       **
-- ** Xilinx, Inc.                                                          **
-- ** XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"         **
-- ** AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND       **
-- ** SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,        **
-- ** OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,        **
-- ** APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION           **
-- ** THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,     **
-- ** AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE      **
-- ** FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY              **
-- ** WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE               **
-- ** IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR        **
-- ** REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF       **
-- ** INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS       **
-- ** FOR A PARTICULAR PURPOSE.                                             **
-- **                                                                       **
-- ***************************************************************************
--
------------------------------------------------------------------------------
-- Filename:          user_logic.vhd
-- Version:           1.00.a
-- Description:       User logic.
-- Date:              Tue Jan 15 17:00:41 2013 (by Create and Import Peripheral Wizard)
-- VHDL Standard:     VHDL'93
------------------------------------------------------------------------------
-- Naming Conventions:
--   active low signals:                    "*_n"
--   clock signals:                         "clk", "clk_div#", "clk_#x"
--   reset signals:                         "rst", "rst_n"
--   generics:                              "C_*"
--   user defined types:                    "*_TYPE"
--   state machine next state:              "*_ns"
--   state machine current state:           "*_cs"
--   combinatorial signals:                 "*_com"
--   pipelined or register delay signals:   "*_d#"
--   counter signals:                       "*cnt*"
--   clock enable signals:                  "*_ce"
--   internal version of output port:       "*_i"
--   device pins:                           "*_pin"
--   ports:                                 "- Names begin with Uppercase"
--   processes:                             "*_PROCESS"
--   component instantiations:              "<ENTITY_>I_<#|FUNC>"
------------------------------------------------------------------------------

-- DO NOT EDIT BELOW THIS LINE --------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library proc_common_v3_00_a;
use proc_common_v3_00_a.proc_common_pkg.all;

-- DO NOT EDIT ABOVE THIS LINE --------------------

--USER libraries added here

------------------------------------------------------------------------------
-- Entity section
------------------------------------------------------------------------------
-- Definition of Generics:
--   C_MST_NATIVE_DATA_WIDTH      -- Internal bus width on user-side
--   C_LENGTH_WIDTH               -- Master interface data bus width
--   C_MST_AWIDTH                 -- Master-Intf address bus width
--   C_NUM_REG                    -- Number of software accessible registers
--   C_SLV_DWIDTH                 -- Slave interface data bus width
--
-- Definition of Ports:
--   Bus2IP_Clk                   -- Bus to IP clock
--   Bus2IP_Resetn                -- Bus to IP reset
--   Bus2IP_Data                  -- Bus to IP data bus
--   Bus2IP_BE                    -- Bus to IP byte enables
--   Bus2IP_RdCE                  -- Bus to IP read chip enable
--   Bus2IP_WrCE                  -- Bus to IP write chip enable
--   IP2Bus_Data                  -- IP to Bus data bus
--   IP2Bus_RdAck                 -- IP to Bus read transfer acknowledgement
--   IP2Bus_WrAck                 -- IP to Bus write transfer acknowledgement
--   IP2Bus_Error                 -- IP to Bus error response
--   ip2bus_mstrd_req             -- IP to Bus master read request
--   ip2bus_mstwr_req             -- IP to Bus master write request
--   ip2bus_mst_addr              -- IP to Bus master read/write address
--   ip2bus_mst_be                -- IP to Bus byte enable
--   ip2bus_mst_length            -- Ip to Bus master transfer length
--   ip2bus_mst_type              -- Ip to Bus burst assertion control
--   ip2bus_mst_lock              -- Ip to Bus bus lock
--   ip2bus_mst_reset             -- Ip to Bus master reset
--   bus2ip_mst_cmdack            -- Bus to Ip master command ack
--   bus2ip_mst_cmplt             -- Bus to Ip master trans complete
--   bus2ip_mst_error             -- Bus to Ip master error
--   bus2ip_mst_rearbitrate       -- Bus to Ip master re-arbitrate for bus ownership
--   bus2ip_mst_cmd_timeout       -- Bus to Ip master command time out
--   bus2ip_mstrd_d               -- Bus to Ip master read data
--   bus2ip_mstrd_rem             -- Bus to Ip master read data rem
--   bus2ip_mstrd_sof_n           -- Bus to Ip master read start of frame
--   bus2ip_mstrd_eof_n           -- Bus to Ip master read end of frame
--   bus2ip_mstrd_src_rdy_n       -- Bus to Ip master read source ready
--   bus2ip_mstrd_src_dsc_n       -- Bus to Ip master read source dsc
--   ip2bus_mstrd_dst_rdy_n       -- Ip to Bus master read dest. ready
--   ip2bus_mstrd_dst_dsc_n       -- Ip to Bus master read dest. dsc
--   ip2bus_mstwr_d               -- Ip to Bus master write data
--   ip2bus_mstwr_rem             -- Ip to Bus master write data rem
--   ip2bus_mstwr_src_rdy_n       -- Ip to Bus master write source ready
--   ip2bus_mstwr_src_dsc_n       -- Ip to Bus master write source dsc
--   ip2bus_mstwr_sof_n           -- Ip to Bus master write start of frame
--   ip2bus_mstwr_eof_n           -- Ip to Bus master write end of frame
--   bus2ip_mstwr_dst_rdy_n       -- Bus to Ip master write dest. ready
--   bus2ip_mstwr_dst_dsc_n       -- Bus to Ip master write dest. ready
------------------------------------------------------------------------------

entity user_logic is
  generic
  (
    -- ADD USER GENERICS BELOW THIS LINE ---------------
    --USER generics added here
    C_DEBUG_DWIDTH                 : integer              := 32;	
    C_OPERATION_MODE               : integer              := 0;	
    C_NUM_PU_CONFIG_REGS           : integer              := 1;	
    C_NUM_PU_STATUS_REGS           : integer              := 1;	
    C_FIFO_IMPLEMENTATION_TYPE	   : integer             := 1;   -- 0 = Any, 1 = BRAM, 2 = Dist.Memory, 3 = Shift Regs
    C_FIFO_DEPTH                   : integer              := 1024;
    -- ADD USER GENERICS ABOVE THIS LINE ---------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol parameters, do not add to or delete
    C_MST_NATIVE_DATA_WIDTH        : integer              := 32;
    C_MST_AWIDTH                   : integer              := 32;
    C_NUM_REG                      : integer              := 10;
    C_NUM_INTR                     : integer              := 32;
    C_SLV_DWIDTH                   : integer              := 32
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );
  port
  (
    -- ADD USER PORTS BELOW THIS LINE ------------------
    --USER ports added here
    DEBUG_D_I  	   					: in std_logic_vector (C_DEBUG_DWIDTH-1 downto 0);    
    DEBUG_D_O  	   					: out std_logic_vector (C_DEBUG_DWIDTH-1 downto 0);    
    DEBUG_D_T  	   					: out std_logic_vector (C_DEBUG_DWIDTH-1 downto 0);    
	 
    -- SDI Interface ------------------------------------------------------------------
    stream_out_clk						: in	std_logic;
    stream_out_stop						: in	std_logic;
    stream_out_valid					: out	std_logic;
    stream_out_data						: out	std_logic_vector(C_MST_NATIVE_DATA_WIDTH-1 downto 0);
    stream_in_clk							: in	std_logic;
    stream_in_stop						: out	std_logic;
    stream_in_valid						: in	std_logic;
    stream_in_data						: in	std_logic_vector(C_MST_NATIVE_DATA_WIDTH-1 downto 0);
    debug:out std_logic_vector(69 downto 0);
    trig0:out std_logic_vector(0 downto 0);
    pu_reset                  : out	std_logic;	
    pu_sof                    : out std_logic;
    pu_sof_ack                : in std_logic;
    pu_eof_in                    : in std_logic;
    pu_sof_in                     : in std_logic;
    pu_eof_ack                : out std_logic;
    request_frame             : in std_logic;
    request_frame_ack         : out std_logic;

    -- PU Registers ---------------------------------------------------------------------
    pu_config_regs_vector 				    : out	std_logic_vector(C_NUM_PU_CONFIG_REGS*C_SLV_DWIDTH -1 downto 0);
    pu_status_regs_vector 		        : in	std_logic_vector(C_NUM_PU_STATUS_REGS*C_SLV_DWIDTH -1 downto 0);
    -- ADD USER PORTS ABOVE THIS LINE ------------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol ports, do not add to or delete
    Bus2IP_Clk                     : in  std_logic;
    Bus2IP_Resetn                  : in  std_logic;
    Bus2IP_Data                    : in  std_logic_vector(0 to C_SLV_DWIDTH-1);
    Bus2IP_BE                      : in  std_logic_vector(0 to C_SLV_DWIDTH/8-1);
    Bus2IP_RdCE                    : in  std_logic_vector(0 to C_NUM_REG-1);
    Bus2IP_WrCE                    : in  std_logic_vector(0 to C_NUM_REG-1);
    IP2Bus_Data                    : out std_logic_vector(0 to C_SLV_DWIDTH-1);
    IP2Bus_RdAck                   : out std_logic;
    IP2Bus_WrAck                   : out std_logic;
    IP2Bus_Error                   : out std_logic;
    ip2bus_mstrd_req               : out std_logic;
    ip2bus_mstwr_req               : out std_logic;
    ip2bus_mst_addr                : out std_logic_vector(0 to C_MST_AWIDTH-1);
    ip2bus_mst_be                  : out std_logic_vector(0 to (C_MST_NATIVE_DATA_WIDTH/8)-1 );
    ip2bus_mst_length              : out std_logic_vector(0 to 11);
    ip2bus_mst_type                : out std_logic;
    ip2bus_mst_lock                : out std_logic;
    ip2bus_mst_reset               : out std_logic;
    bus2ip_mst_cmdack              : in  std_logic;
    bus2ip_mst_cmplt               : in  std_logic;
    bus2ip_mst_error               : in  std_logic;
    bus2ip_mst_rearbitrate         : in  std_logic;
    bus2ip_mst_cmd_timeout         : in  std_logic;
    bus2ip_mstrd_d                 : in  std_logic_vector( 0 to C_MST_NATIVE_DATA_WIDTH-1);
    bus2ip_mstrd_rem               : in  std_logic_vector(0 to (C_MST_NATIVE_DATA_WIDTH/8)-1 );
    bus2ip_mstrd_sof_n             : in  std_logic;
    bus2ip_mstrd_eof_n             : in  std_logic;
    bus2ip_mstrd_src_rdy_n         : in  std_logic;
    bus2ip_mstrd_src_dsc_n         : in  std_logic;
    ip2bus_mstrd_dst_rdy_n         : out std_logic;
    ip2bus_mstrd_dst_dsc_n         : out std_logic;
    ip2bus_mstwr_d                 : out std_logic_vector( 0 to C_MST_NATIVE_DATA_WIDTH-1);
    ip2bus_mstwr_rem               : out std_logic_vector(0 to (C_MST_NATIVE_DATA_WIDTH/8)-1 );
    ip2bus_mstwr_src_rdy_n         : out std_logic;
    ip2bus_mstwr_src_dsc_n         : out std_logic;
    ip2bus_mstwr_sof_n             : out std_logic;
    ip2bus_mstwr_eof_n             : out std_logic;
    bus2ip_mstwr_dst_rdy_n         : in  std_logic;
    bus2ip_mstwr_dst_dsc_n         : in  std_logic;
    IP2Bus_IntrEvent               : out std_logic_vector(0 to C_NUM_INTR-1)
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );

  attribute MAX_FANOUT : string;
  attribute SIGIS : string;

  attribute SIGIS of Bus2IP_Clk    : signal is "CLK";
  attribute SIGIS of Bus2IP_Resetn : signal is "RST";
  attribute SIGIS of ip2bus_mst_reset: signal is "RST";

end entity user_logic;

------------------------------------------------------------------------------
-- Architecture section
------------------------------------------------------------------------------

architecture IMP of user_logic is

  --USER signal declarations added here, as needed for user logic
function max(L, R: INTEGER) return INTEGER is
  begin
    if L > R then
      return L;
    else
      return R;
    end if;
  end;
 function log2 (x : positive) return natural is 
  begin
    if x <= 1 then
      return 0;
    else
      return log2 (x / 2) + 1;
    end if;
  end function log2;
  ------------------------------------------
  -- Signals for user logic slave model s/w accessible register example
  ------------------------------------------
   function generate_pattern (SIZE, INDEX : integer) return std_logic_vector is
    variable result : std_logic_vector(0 to SIZE-1) := (others => '0');
  begin
    result(INDEX) := '1';
    return result;
  end function;

  
 
function resize (ARG: std_logic_vector; NEW_SIZE: NATURAL) return std_logic_vector is
    constant NAU: std_logic_vector(0 downto 1) := (others => '0');
    constant ARG_LEFT: INTEGER := ARG'LENGTH-1;
    alias XARG: std_logic_vector(ARG_LEFT downto 0) is ARG;
    variable RESULT: std_logic_vector(NEW_SIZE-1 downto 0) := (others => '0');
  begin
    if (NEW_SIZE < 1) then return NAU;
    end if;
    if XARG'LENGTH =0 then return RESULT;
    end if;
    if (RESULT'LENGTH < ARG'LENGTH) then
      RESULT(RESULT'LEFT downto 0) := XARG(RESULT'LEFT downto 0);
    else
      RESULT(RESULT'LEFT downto XARG'LEFT+1) := (others => '0');
      RESULT(XARG'LEFT downto 0) := XARG;
    end if;
    return RESULT;
  end resize;

  constant HARDWARE_VERSION : std_logic_vector := X"0001";
  constant MAGIC_WORD : std_logic_vector := X"ABCD";
  constant BURST_LENGTH : integer := 16;  -- burst length counter in beats!

  constant PU_NUM_REG : integer := max(C_NUM_PU_CONFIG_REGS, C_NUM_PU_STATUS_REGS);
  
  constant ALIGNMENT_BITS : integer := log2(C_MST_NATIVE_DATA_WIDTH/8);
 component fifo
    generic (
      DATA_WIDTH : integer := 32;
      FIFO_DEPTH_BITS : integer := 10
    );
    port (
      din: IN std_logic_VECTOR(DATA_WIDTH-1 downto 0);
      rd_clk: IN std_logic;
      rd_en: IN std_logic;
      rst: IN std_logic;
      wr_clk: IN std_logic;
      wr_en: IN std_logic;
      dout: OUT std_logic_VECTOR(DATA_WIDTH-1 downto 0);
      empty: OUT std_logic;
      full: OUT std_logic;
      rd_data_count: OUT std_logic_VECTOR(FIFO_DEPTH_BITS downto 0);
      wr_data_count: OUT std_logic_VECTOR(FIFO_DEPTH_BITS downto 0)
    );
  end component;
 component fifo_generator_v4_4_16_64
  PORT (
    rst : IN STD_LOGIC;
    wr_clk : IN STD_LOGIC;
    rd_clk : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(15 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(63 DOWNTO 0);
    full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC;
    valid : OUT STD_LOGIC;
    rd_data_count : OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
    wr_data_count : OUT STD_LOGIC_VECTOR(8 DOWNTO 0)
  );
END component;
   subtype REG is std_logic_vector(0 to C_SLV_DWIDTH-1);

  type USER_PARAM_WRITE_REG_TYPE is array(0 to C_NUM_PU_CONFIG_REGS-1) of REG;
  signal pu_config_regs           : USER_PARAM_WRITE_REG_TYPE;

  type USER_STATUS_READ_REG_TYPE is array(0 to C_NUM_PU_STATUS_REGS-1) of REG;
  signal pu_status_regs           : USER_STATUS_READ_REG_TYPE;




  type SCHEDULER_STATES is (SCHEDULE_IDLE, SCHEDULE_READER, SCHEDULE_WRITER);
  signal scheduler_state                : SCHEDULER_STATES;
      ---------------------------------------------------
  -- register table for IM Data Module slave section
  ---------------------------------------------------
  signal hardware_reg                   : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal status_reg                     : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal reader_start_address_reg       : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal reader_end_address_reg         : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal writer_start_address_reg       : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal writer_end_address_reg         : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal reader_trigger_address_reg     : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal writer_trigger_address_reg     : std_logic_vector(0 to C_SLV_DWIDTH-1);
 signal start_writer_g: std_logic;
  --------------------------------------
  -- control register bitwise definition
  --------------------------------------

   signal tick_counter : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal tick_counter_is_running : std_logic;
  signal start_tick_counter : std_logic;
  signal clear_tick_counter : std_logic;
  signal stop_tick_counter : std_logic;
  signal stop_tick_counter_on_reader_intr : std_logic;
  signal stop_tick_counter_on_writer_intr : std_logic;
  signal clear_writer_fifo : std_logic;
  signal start_reader       : std_logic;
  signal reader_is_active   : std_logic;
  signal reader_continous_mode : std_logic;
  signal status_reader_done : std_logic;
  signal status_reader_done_irq : std_logic;
  signal reader_fifo_is_flushed_irq : std_logic;
  signal reader_trigger_addr_irq : std_logic;

  signal start_writer       : std_logic;
  signal writer_is_active   : std_logic;
  signal writer_continous_mode : std_logic;
  signal flush_writer_fifo : std_logic;
  signal status_writer_done_irq : std_logic;
  signal writer_trigger_addr_irq : std_logic;

  signal current_reader_address : std_logic_vector(0 to C_MST_AWIDTH-1);
  signal current_writer_address : std_logic_vector(0 to C_MST_AWIDTH-1);
  signal reader_end_address     : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal writer_end_address     : std_logic_vector(0 to C_SLV_DWIDTH-1);

--  signal reader_fifo_wr_count   : std_logic_vector(0 to log2(FIFO_SIZE)-1);
 signal writer_fifo_rd_count1   : std_logic_vector(0 to log2(C_FIFO_DEPTH)-1);

  signal reader_fifo_wr_count   : std_logic_vector(0 to log2(C_FIFO_DEPTH));
  --signal writer_fifo_rd_count   : std_logic_vector(0 to log2(C_FIFO_DEPTH));
  signal writer_fifo_rd_count   : std_logic_vector(0 to 6);
  
  signal status_reader_start_addr_error : std_logic;
  signal status_reader_end_addr_error : std_logic;
  signal status_writer_start_addr_error : std_logic;
  signal status_writer_end_addr_error : std_logic;

  -- signals to control fifo
  signal reader_fifo_reset		  : std_logic;
  signal writer_fifo_reset		  : std_logic;
  signal reader_fifo_empty      : std_logic;
  signal writer_fifo_empty      : std_logic;
  signal writer_fifo_full       : std_logic;
  --signal start_writer_g: std_logic;
  signal clear_reader_fifo : std_logic;
  signal reader_fifo_rd_en : std_logic;
  signal reader_fifo_full  : std_logic;
  signal mst_wr_d               : std_logic_vector(0 to C_MST_NATIVE_DATA_WIDTH-1);



  signal slv_reg_write_sel          : std_logic_vector(0 to C_NUM_REG-1);
  signal slv_reg_read_sel           : std_logic_vector(0 to C_NUM_REG-1);
  signal slv_ip2bus_data        : std_logic_vector(0 to C_SLV_DWIDTH-1);
  signal reg_read_ack                   : std_logic;
  signal reg_write_ack                  : std_logic;

  ------------------------------------------
  -- Signals for user logic master model example
  ------------------------------------------
  signal mst_cntl_burst                 : std_logic;
  signal mst_ip2bus_addr                : std_logic_vector(0 to C_MST_AWIDTH-1);
  signal mst_xfer_length                : std_logic_vector(0 to 11);
  
  -- signals for master model command interface state machine
  type CMD_CNTL_SM_TYPE is (CMD_IDLE, CMD_PREPARE_READ, CMD_PREPARE_WRITE,CMD_RUN, CMD_WAIT_FOR_DATA);
  signal mst_cmd_sm_state               : CMD_CNTL_SM_TYPE;
  signal mst_cmd_sm_set_done            : std_logic;
  signal mst_cmd_sm_set_error           : std_logic;
  signal mst_cmd_sm_set_timeout         : std_logic;
  signal mst_cmd_sm_busy                : std_logic;
  signal mst_cmd_sm_clr_go              : std_logic;
  signal mst_cmd_sm_rd_req              : std_logic;
  signal mst_cmd_sm_wr_req              : std_logic;
  signal mst_cmd_sm_reset               : std_logic;
  signal mst_cmd_sm_bus_lock            : std_logic;
  signal mst_cmd_sm_ip2bus_addr         : std_logic_vector(0 to C_MST_AWIDTH-1);
  signal mst_cmd_sm_ip2bus_be           : std_logic_vector(0 to C_MST_NATIVE_DATA_WIDTH/8-1);
  signal mst_cmd_sm_xfer_type           : std_logic;
  signal mst_cmd_sm_xfer_length         : std_logic_vector(0 to 11);
  signal mst_cmd_sm_start_rd_llink      : std_logic;
  signal mst_cmd_sm_start_wr_llink      : std_logic;
  -- signals for master model read locallink interface state machine
  type RD_LLINK_SM_TYPE is (LLRD_IDLE, LLRD_GO);
  signal mst_llrd_sm_state              : RD_LLINK_SM_TYPE;
  signal mst_llrd_sm_dst_rdy            : std_logic;
  -- signals for master model write locallink interface state machine
  type WR_LLINK_SM_TYPE is (LLWR_IDLE, LLWR_SNGL_INIT, LLWR_SNGL, LLWR_BRST_INIT, LLWR_BRST, LLWR_BRST_LAST_BEAT);
  signal mst_llwr_sm_state              : WR_LLINK_SM_TYPE;
  signal mst_llwr_sm_src_rdy            : std_logic;
  signal mst_llwr_sm_sof                : std_logic;
  signal mst_llwr_sm_eof                : std_logic;
  signal mst_llwr_byte_cnt              : integer;
  signal mst_fifo_valid_write_xfer      : std_logic;
  signal mst_fifo_valid_read_xfer       : std_logic;
  signal Bus2IP_Reset                   : std_logic;
  signal remaining_words_test         : std_logic_vector(0 to 11);
attribute SIGIS of Bus2IP_Reset   : signal is "RST";
begin

  --USER logic implementation added here

  ------------------------------------------
  -- Example code to read/write user logic slave model s/w accessible registers
  -- 
  -- Note:
  -- The example code presented here is to show you one way of reading/writing
  -- software accessible registers implemented in the user logic slave model.
  -- Each bit of the Bus2IP_WrCE/Bus2IP_RdCE signals is configured to correspond
  -- to one software accessible register by the top level template. For example,
  -- if you have four 32 bit software accessible registers in the user logic,
  -- you are basically operating on the following memory mapped registers:
  -- 
  --    Bus2IP_WrCE/Bus2IP_RdCE   Memory Mapped Register
  --                     "1000"   C_BASEADDR + 0x0
  --                     "0100"   C_BASEADDR + 0x4
  --                     "0010"   C_BASEADDR + 0x8
  --                     "0001"   C_BASEADDR + 0xC
  -- 
  ------------------------------------------
  slv_reg_write_sel <= Bus2IP_WrCE(0 to C_NUM_REG-1);
  slv_reg_read_sel  <= Bus2IP_RdCE(0 to C_NUM_REG-1);
  reg_read_ack <= '0' when (conv_integer(Bus2IP_RdCE(0 to C_NUM_REG-1)) = 0) else '1';  -- one bit is set?
  reg_write_ack <= '0' when (conv_integer(Bus2IP_WrCE(0 to C_NUM_REG-1)) = 0) else '1';  -- one bit is set?

  writer_fifo_rd_count1<="00" & writer_fifo_rd_count;
trig0(0)<=stream_in_valid;
debug<=  '0'  & writer_fifo_full & writer_fifo_reset & mst_fifo_valid_read_xfer & writer_is_active & stream_in_valid  & current_writer_address & writer_end_address;

  -- implement slave model software accessible register(s)
  hardware_reg(16 to 31) <= HARDWARE_VERSION;
  hardware_reg(0 to 15) <= MAGIC_WORD;

--display : 0x 31 30 29 .... 0
  SLAVE_REG_WRITE_PROC : process(Bus2IP_Clk) is
  begin

    if Bus2IP_Clk'event and Bus2IP_Clk = '1' then
      if Bus2IP_Resetn = '0' then
        reader_start_address_reg <= (others => '0');
        reader_end_address_reg <= (others => '0');
        writer_start_address_reg <=(others => '0');
        writer_end_address_reg <=(others => '0');
        start_reader <= '0';
        start_writer <= '0';
        reader_continous_mode <= '0';
        writer_continous_mode <= '0';
        reader_fifo_reset <= '0';
        writer_fifo_reset <= '0';
        pu_reset <= '0';
        start_tick_counter <= '0';
        clear_tick_counter <= '0';
        stop_tick_counter <= '0';
        stop_tick_counter_on_reader_intr <= '0';
        stop_tick_counter_on_writer_intr <= '0';
        for index in 0 to (C_NUM_PU_CONFIG_REGS-1) loop
          pu_config_regs(index) <= (others => '0');
        end loop;
      else
        start_reader <= '0';
        start_writer <= '0';--'0';
        reader_fifo_reset <= '0';
        writer_fifo_reset <= '0';
        pu_reset <= '0';
        start_tick_counter <= '0';
        clear_tick_counter <= '0';
        stop_tick_counter <= '0';

        -- control register
        if slv_reg_write_sel = generate_pattern(C_NUM_REG, 1) then
           start_reader <= Bus2IP_Data(0);
          start_writer <= Bus2IP_Data(1);
          reader_fifo_reset <= Bus2IP_Data(2);
          writer_fifo_reset <= Bus2IP_Data(3);
          pu_reset <= Bus2IP_Data(4);
          start_tick_counter <= Bus2IP_Data(5);
          clear_tick_counter <= Bus2IP_Data(6);
          stop_tick_counter <= Bus2IP_Data(7);
          stop_tick_counter_on_reader_intr <= Bus2IP_Data(8);
          stop_tick_counter_on_writer_intr <= Bus2IP_Data(9);
          reader_continous_mode <= Bus2IP_Data(10);
          writer_continous_mode <= Bus2IP_Data(11);
        end if;

        if slv_reg_write_sel = generate_pattern(C_NUM_REG, 2) then
          reader_start_address_reg <= Bus2IP_Data;--X"C01641D8";--
        end if;
        if slv_reg_write_sel = generate_pattern(C_NUM_REG, 3) then
          reader_end_address_reg   <= Bus2IP_Data;--X"C01BC3D7";-- 
        end if;
        if slv_reg_write_sel = generate_pattern(C_NUM_REG, 4) then
          reader_trigger_address_reg   <= Bus2IP_Data;
        end if;
        if slv_reg_write_sel = generate_pattern(C_NUM_REG, 5) then
          writer_start_address_reg <= Bus2IP_Data;--X"C01641D8";--
        end if;
        if slv_reg_write_sel = generate_pattern(C_NUM_REG, 6) then
          writer_end_address_reg <= Bus2IP_Data;--X"C01BC3D7";--
        end if;
        if slv_reg_write_sel = generate_pattern(C_NUM_REG, 7) then
          writer_trigger_address_reg   <= Bus2IP_Data;
        end if;

        for index in 0 to C_NUM_PU_CONFIG_REGS-1 loop
          if ( slv_reg_write_sel = generate_pattern(C_NUM_REG, C_NUM_REG-PU_NUM_REG+index) ) then
            for byte_index in 0 to (C_SLV_DWIDTH/8)-1 loop
              if ( Bus2IP_BE(byte_index) = '1' ) then
               pu_config_regs(index)(byte_index*8 to byte_index*8+7) <= Bus2IP_Data(byte_index*8 to byte_index*8+7);
              end if;
            end loop;
          end if;
        end loop;
      end if;

    end if;
  end process SLAVE_REG_WRITE_PROC;

  -- implement slave model software accessible register(s) read mux
  REG_READ_PROC : process( slv_reg_read_sel, hardware_reg, status_reg, current_writer_address , tick_counter, reader_start_address_reg, reader_end_address_reg, writer_start_address_reg, writer_end_address_reg, pu_status_regs, reader_trigger_address_reg, writer_trigger_address_reg ) is
  begin
    slv_ip2bus_data <= (others => '0');
    if slv_reg_read_sel = generate_pattern(C_NUM_REG, 0) then
      slv_ip2bus_data <= hardware_reg;
    end if;
    if slv_reg_read_sel = generate_pattern(C_NUM_REG, 1) then
      slv_ip2bus_data <= status_reg;
    end if;
    if slv_reg_read_sel = generate_pattern(C_NUM_REG, 2) then
      slv_ip2bus_data <= reader_start_address_reg;
    end if;
    if slv_reg_read_sel = generate_pattern(C_NUM_REG, 3) then
      slv_ip2bus_data <= reader_end_address_reg;
    end if;
    if slv_reg_read_sel = generate_pattern(C_NUM_REG, 4) then
      slv_ip2bus_data <= reader_trigger_address_reg;
    end if;
    if slv_reg_read_sel = generate_pattern(C_NUM_REG, 5) then
      slv_ip2bus_data <= writer_start_address_reg;
    end if;
    if slv_reg_read_sel = generate_pattern(C_NUM_REG, 6) then
      slv_ip2bus_data <= writer_end_address_reg;
    end if;
    if slv_reg_read_sel = generate_pattern(C_NUM_REG, 7) then
      slv_ip2bus_data <= writer_trigger_address_reg;
    end if;
    if slv_reg_read_sel = generate_pattern(C_NUM_REG, 8) then
      slv_ip2bus_data <= tick_counter;
    end if;

    for index in 0 to C_NUM_PU_STATUS_REGS-1 loop
      if slv_reg_read_sel = generate_pattern(C_NUM_REG, C_NUM_REG-PU_NUM_REG+index) then
        slv_ip2bus_data <= pu_status_regs(index);
      end if;
    end loop;

  end process REG_READ_PROC;
 -- status register write operation
  STATUS_REG_WRITE_PROC : process(Bus2IP_Clk) is
  begin
    if ( Bus2IP_Clk'event and Bus2IP_Clk = '1' ) then
      if Bus2IP_Resetn = '0' then
        status_reg <= (others => '0');
      else
        -- ctrl_clear_status_reg (bit number 31) 
        -- ctrl_clear_status_reg (bit number 31) 
        if ( Bus2IP_Data(31) = '1' and slv_reg_write_sel = generate_pattern(C_NUM_REG, 1) ) then
          -- allows a clear of the 'Done'/'error'/'timeout/...'
	        status_reg <= (others => '0');		  
        else
          -- 'Done'/'error'/'timeout/...' from master control state machine
          status_reg(0) 			 <= status_reader_done_irq;-- or status_reg(0);
          status_reg(1) 			 <= reader_is_active;
          status_reg(2) 			 <= status_writer_done_irq;-- or status_reg(2);
          status_reg(3) 			 <= writer_is_active;
          status_reg(4) 			 <= reader_fifo_is_flushed_irq or status_reg(4);
          status_reg(5) 			 <= reader_trigger_addr_irq or status_reg(5);
          status_reg(6) 			 <= writer_trigger_addr_irq or status_reg(6);
          status_reg(24) 			 <= mst_cmd_sm_set_error or status_reg(24);
          status_reg(25) 			 <= mst_cmd_sm_set_timeout or status_reg(25);
          status_reg(26) 			 <= status_reader_start_addr_error or status_reg(26);
          status_reg(27) 			 <= status_reader_end_addr_error or status_reg(27);
          status_reg(28) 			 <= status_writer_start_addr_error or status_reg(28);
          status_reg(29) 			 <= status_writer_end_addr_error or status_reg(29);
          status_reg(30 to 31) <= (others => '0');
        end if;
      end if;
    end if;
  end process STATUS_REG_WRITE_PROC;

 -- PU config register array multiplexer
  PU_CONFIG_REG_ARRAY_MUX_PROC : process(pu_config_regs) is
  begin
    for index in 0 to C_NUM_PU_CONFIG_REGS-1 loop
      pu_config_regs_vector((index+1)*C_SLV_DWIDTH-1 downto index*C_SLV_DWIDTH) <= pu_config_regs(index);
    end loop;
  end process PU_CONFIG_REG_ARRAY_MUX_PROC;

  -- PU status register array multiplexer
  PU_STATUS_REG_ARRAY_MUX_PROC : process( pu_status_regs_vector ) is
  begin
    for index in 0 to C_NUM_PU_STATUS_REGS-1 loop
      pu_status_regs(index) <= pu_status_regs_vector((index+1)*C_SLV_DWIDTH-1 downto index*C_SLV_DWIDTH);
    end loop;
  end process PU_STATUS_REG_ARRAY_MUX_PROC;

  READER_FIFO_IS_FLUSHED : process ( Bus2IP_Clk ) is
  begin
    if (rising_edge(Bus2IP_Clk)) then
      if Bus2IP_Resetn = '0' then
        reader_fifo_is_flushed_irq <= '0';
        status_reader_done <= '0';
      else
        -- check if reader FIFO is flushed
        reader_fifo_is_flushed_irq <= '0';
        status_reader_done <= status_reader_done or status_reader_done_irq;
        if (status_reader_done and reader_fifo_empty) = '1' then
          reader_fifo_is_flushed_irq <= '1';
          status_reader_done <= '0';
        end if;
      end if;
    end if;
  end process READER_FIFO_IS_FLUSHED;
        
  HANDLE_PU_EOF : process (Bus2IP_Clk) is
  begin
    if (rising_edge(Bus2IP_Clk)) then
      if Bus2IP_Resetn = '0' then
        flush_writer_fifo <= '0';
        pu_eof_ack <= '0';
      else
        -- latch pu_eof signal
        pu_eof_ack <= pu_eof_in;
        flush_writer_fifo <= flush_writer_fifo or pu_eof_in;
--        if start_writer = '1' or status_writer_done_irq = '1' then
        if status_writer_done_irq = '1' then
          flush_writer_fifo <= '0';
        end if;
      end if;
    end if;
  end process HANDLE_PU_EOF;

MASTER_ADDRESS_SCHEDULER_PROC : process (Bus2IP_Clk) is
    variable ALIGNMENT_ONES : std_logic_vector(0 to ALIGNMENT_BITS-1) := (others => '1');
    variable ALIGNMENT_ZEROS : std_logic_vector(0 to ALIGNMENT_BITS-1) := (others => '0');
    variable next_address : std_logic_vector(0 to C_SLV_DWIDTH-1);
  begin
    if (rising_edge(Bus2IP_Clk)) then
       if Bus2IP_Resetn = '0' then
        reader_is_active <= '0';
        writer_is_active <= '0';
        current_reader_address <= (others => '0');
        current_writer_address <= (others => '0');
        reader_end_address <= (others => '0');
        writer_end_address <= (others => '0');
        tick_counter <= (others => '0');
        status_reader_start_addr_error <= '0';
        status_reader_end_addr_error <= '0';
        status_writer_start_addr_error <= '0';
        status_writer_end_addr_error <= '0';
        reader_trigger_addr_irq <= '0';
        writer_trigger_addr_irq <= '0';
        pu_sof <= '0';
        request_frame_ack <= '0';
        next_address := (others => '0');
        clear_reader_fifo <= '0';
        --start_writer_g<='0';
      else
        status_reader_start_addr_error <= '0';
        status_reader_end_addr_error <= '0';
        status_writer_start_addr_error <= '0';
        status_writer_end_addr_error <= '0';
        reader_trigger_addr_irq <= '0';
        writer_trigger_addr_irq <= '0';
        clear_reader_fifo <= '0';
        clear_writer_fifo <= '0';
        if pu_sof_ack = '1' then
          pu_sof <= '0';
        end if;

        if request_frame = '0' then
          request_frame_ack <= '0';
        end if;

        if status_reader_done_irq = '1' then
          reader_is_active <= '0';
        -- start reader and prevent double activation
       elsif (reader_is_active = '0'  and reader_fifo_empty = '0') then
           clear_reader_fifo <= '1';
		  elsif ((start_reader = '1') or (reader_continous_mode = '1')) and (reader_is_active = '0') and (request_frame='1') and (reader_fifo_empty = '1') then
          -- check alignment (to PLB master transaction size) of start and end address
          if reader_start_address_reg(C_SLV_DWIDTH-ALIGNMENT_BITS to C_SLV_DWIDTH-1) /= ALIGNMENT_ZEROS then
             status_reader_start_addr_error <= '1'; 
          elsif reader_end_address_reg(C_SLV_DWIDTH-ALIGNMENT_BITS to C_SLV_DWIDTH-1) /= ALIGNMENT_ONES then
             status_reader_end_addr_error <= '1';
          else
             --if reader_continous_mode = '1' then  -- TODO: double check
               if reader_fifo_empty = '1' then
                 reader_is_active <= '1';
                 pu_sof <= '1';
                 request_frame_ack <= '1';
                 current_reader_address <= reader_start_address_reg;
                 reader_end_address <= reader_end_address_reg;
               else
                 clear_reader_fifo <= '1';
               end if;
             --else
              --   reader_is_active <= '1';
               --  pu_sof <= '1';
               --  request_frame_ack <= '1';
               --  current_reader_address <= reader_start_address_reg;
              --   reader_end_address <= reader_end_address_reg;
              --end if;
          end if;
        -- increment address counter while FIFO is written
        elsif mst_fifo_valid_write_xfer = '1' then
          current_reader_address <= current_reader_address + (C_MST_NATIVE_DATA_WIDTH/8);
          next_address := current_reader_address + (C_MST_NATIVE_DATA_WIDTH/8);
--          current_reader_address <= next_address;
          if reader_trigger_address_reg(0 to C_SLV_DWIDTH-ALIGNMENT_BITS-1) = next_address(0 to C_SLV_DWIDTH-ALIGNMENT_BITS-1) then
            --reader_trigger_addr_irq <= '1';
          end if;
        end if;
        
        if status_writer_done_irq = '1' then
          writer_is_active <= '0';
         		  elsif((writer_fifo_empty='0') and (writer_is_active='0')) then 
		      clear_writer_fifo <= '1';
        -- start writer and prevent double activation
         elsif (start_writer = '1' or start_writer_g='1' or writer_continous_mode = '1') and ( writer_is_active = '0')  and (writer_fifo_empty='1')then
          -- check alignment (to PLB master transaction size) of start and end address
         if writer_start_address_reg(C_SLV_DWIDTH-3 to C_SLV_DWIDTH-1) /= ALIGNMENT_ZEROS then
             status_writer_start_addr_error <= '1'; 
          elsif writer_end_address_reg(C_SLV_DWIDTH-3 to C_SLV_DWIDTH-1) /= ALIGNMENT_ONES then
             status_writer_end_addr_error <= '1';
          else
              start_writer_g<='1';
             if(pu_sof_in='1')then
				     writer_is_active <= '1' ;
                                    start_writer_g<='0';
	    end if;
             current_writer_address <= writer_start_address_reg;
             writer_end_address <= writer_end_address_reg;
          end if;
        -- increment address counter while FIFO is read
        elsif mst_fifo_valid_read_xfer = '1' then
          current_writer_address <= current_writer_address + (C_MST_NATIVE_DATA_WIDTH/8);
          next_address := current_writer_address + (C_MST_NATIVE_DATA_WIDTH/8);
--          current_writer_address <= next_address;
         if writer_trigger_address_reg(0 to C_SLV_DWIDTH-ALIGNMENT_BITS-1) = next_address(0 to C_SLV_DWIDTH-ALIGNMENT_BITS-1) then
            --writer_trigger_addr_irq <= '1';
          end if;
        end if;
        
        if (stop_tick_counter = '1') or 
           ((stop_tick_counter_on_reader_intr and status_reader_done_irq) = '1') or 
           ((stop_tick_counter_on_writer_intr and status_writer_done_irq) = '1') then
          tick_counter_is_running <= '0';
        elsif start_tick_counter = '1' then
          tick_counter_is_running <= '1';
        end if;

        if clear_tick_counter = '1' then
          tick_counter <= (others => '0');
        elsif tick_counter_is_running = '1' then
          tick_counter <= tick_counter + 1;
        end if;
      end if;
    end if;
  end process MASTER_ADDRESS_SCHEDULER_PROC;  

  -- user logic master command interface assignments
  IP2Bus_MstRd_Req  <= mst_cmd_sm_rd_req;
  IP2Bus_MstWr_Req  <= mst_cmd_sm_wr_req;
  IP2Bus_Mst_Addr   <= mst_cmd_sm_ip2bus_addr;
  IP2Bus_Mst_BE     <= (others => '1');--mst_cmd_sm_ip2bus_be;
  IP2Bus_Mst_Type   <= mst_cmd_sm_xfer_type;
  IP2Bus_Mst_Length <= mst_cmd_sm_xfer_length;
  IP2Bus_Mst_Lock   <= '0';--mst_cmd_sm_bus_lock;
  IP2Bus_Mst_Reset  <= '0';--mst_cmd_sm_reset;
  IP2Bus_MstWr_d <= mst_wr_d;

  --implement master command interface state machine
  MASTER_CMD_SM_PROC : process( Bus2IP_Clk ) is
    variable remaining_words : std_logic_vector(0 to C_MST_AWIDTH-4);
    variable beats : integer range 0 to (BURST_LENGTH*C_MST_NATIVE_DATA_WIDTH/8);
    variable next_scheduler_state : SCHEDULER_STATES;
    variable reader_reached_end_addr : std_logic;
    variable writer_reached_end_addr : std_logic;
  begin

    if ( Bus2IP_Clk'event and Bus2IP_Clk = '1' ) then
      if ( Bus2IP_Resetn = '0' ) then

       -- reset condition
   mst_cmd_sm_state          <= CMD_IDLE;
        mst_cmd_sm_clr_go         <= '0';
        mst_cmd_sm_rd_req         <= '0';
        mst_cmd_sm_wr_req         <= '0';
        mst_cmd_sm_bus_lock       <= '0';
        mst_cmd_sm_reset          <= '0';
        mst_cmd_sm_ip2bus_addr    <= (others => '0');
        --mst_cmd_sm_ip2bus_be      <= (others => '0');
        mst_cmd_sm_xfer_type      <= '0';
        mst_cmd_sm_xfer_length    <= (others => '0');
        mst_cmd_sm_set_done       <= '0';
        mst_cmd_sm_set_error      <= '0';
        mst_cmd_sm_set_timeout    <= '0';
        mst_cmd_sm_busy           <= '0';
        mst_cmd_sm_start_rd_llink <= '0';
        mst_cmd_sm_start_wr_llink <= '0';
        mst_ip2bus_addr   <= (others => '0');
        mst_xfer_length   <= (others => '0');
        status_reader_done_irq <= '0';
        status_writer_done_irq <= '0';       
        remaining_words_test <= (others => '0');
      else

        -- default condition
        mst_cmd_sm_clr_go         <= '0';
        mst_cmd_sm_rd_req         <= '0';
        mst_cmd_sm_wr_req         <= '0';
        mst_cmd_sm_bus_lock       <= '0';
        mst_cmd_sm_reset          <= '0';
        mst_cmd_sm_ip2bus_addr    <= (others => '0');
        --mst_cmd_sm_ip2bus_be      <= (others => '0');
        mst_cmd_sm_xfer_type      <= '0';
        mst_cmd_sm_xfer_length    <= (others => '0');
        mst_cmd_sm_set_done       <= '0';
        mst_cmd_sm_set_error      <= '0';
        mst_cmd_sm_set_timeout    <= '0';
        mst_cmd_sm_busy           <= '1';
        mst_cmd_sm_start_rd_llink <= '0';
        mst_cmd_sm_start_wr_llink <= '0';
        remaining_words_test <= (others => '0');
        --status_reader_done_irq <= '0';
        --status_writer_done_irq <= '0';
        mst_ip2bus_addr   <= mst_ip2bus_addr;
        mst_xfer_length   <= mst_xfer_length;
        if (status_reader_done_irq = '1') and (reader_is_active = '0') then         
          status_reader_done_irq<='0';
        end if;
        if (status_writer_done_irq = '1') and (writer_is_active = '0') then         
          status_writer_done_irq<='0';
        end if;
        if (current_reader_address >= reader_end_address) then
          reader_reached_end_addr := '1';
        else
          reader_reached_end_addr := '0';
        end if;
        if ( (current_writer_address >= writer_end_address) or (flush_writer_fifo = '1' and writer_fifo_empty = '1') ) then
          writer_reached_end_addr := '1';
        else
          writer_reached_end_addr := '0';
        end if;

        -- state transition
        case mst_cmd_sm_state is

          when CMD_IDLE =>
           -- if ( mst_go = '1' ) then
             -- mst_cmd_sm_state  <= CMD_RUN;
            --  mst_cmd_sm_clr_go <= '1';
              --if ( mst_cntl_rd_req = '1' ) then
              --  mst_cmd_sm_start_rd_llink <= '1';
             -- elsif ( mst_cntl_wr_req = '1' ) then
              --  mst_cmd_sm_start_wr_llink <= '1';
              --end if;
            if (status_reader_done_irq /= '1') and (status_writer_done_irq /= '1') then    
             case scheduler_state is
              when SCHEDULE_IDLE =>
                if writer_is_active = '1' then
                  scheduler_state <= SCHEDULE_WRITER;
                  mst_cmd_sm_state <= CMD_PREPARE_WRITE;
                elsif reader_is_active = '1' then
                  scheduler_state <= SCHEDULE_READER;
                  mst_cmd_sm_state <= CMD_PREPARE_READ;
                end if;
              when SCHEDULE_READER =>
                if writer_is_active = '1' then
                  scheduler_state <= SCHEDULE_WRITER;
                  mst_cmd_sm_state <= CMD_PREPARE_WRITE;
                elsif reader_is_active = '1' then
                  scheduler_state <= SCHEDULE_READER;
                  mst_cmd_sm_state <= CMD_PREPARE_READ;
                else
                  scheduler_state <= SCHEDULE_IDLE;
                end if;
              when SCHEDULE_WRITER =>
                if reader_is_active = '1' then
                  scheduler_state <= SCHEDULE_READER;
                  mst_cmd_sm_state <= CMD_PREPARE_READ;
                elsif writer_is_active = '1' then
                  scheduler_state <= SCHEDULE_WRITER;
                  mst_cmd_sm_state <= CMD_PREPARE_WRITE;
                else
                  scheduler_state <= SCHEDULE_IDLE;
                end if;
              when others =>
                scheduler_state <= SCHEDULE_IDLE;
            end case;
          else
                  mst_cmd_sm_state <= CMD_IDLE;
                 scheduler_state <= SCHEDULE_IDLE;
           end if;

          --  else
           --   mst_cmd_sm_state  <= CMD_IDLE;
           --   mst_cmd_sm_busy   <= '0';
           -- end if;
          
           when CMD_PREPARE_READ =>

              remaining_words := resize(SHR(reader_end_address - current_reader_address, X"3"), remaining_words'length);  -- FIXME: 3 is bad => log2(C_MST_NATIVE_DATA_WIDTH/8)
--              remaining_words := resize(SHR(reader_end_address + 1 - current_reader_address, X"3"), remaining_words'length);  -- FIXME: 3 is bad => log2(C_MST_NATIVE_DATA_WIDTH/8)
              
              if conv_integer(remaining_words) >= BURST_LENGTH then
                beats := BURST_LENGTH;
              else
                beats := conv_integer(remaining_words);
              end if;
              
              if beats > 1 then
                mst_cntl_burst <= '1';
              else
               mst_cntl_burst <= '0';
              end if;
              
              mst_xfer_length <= conv_std_logic_vector(beats * (C_MST_NATIVE_DATA_WIDTH/8), 12);
                 if ( reader_is_active = '1' and reader_reached_end_addr = '1' ) then
                status_reader_done_irq <= '1';
              end if;
              if reader_reached_end_addr = '1' then
                mst_cmd_sm_state <= CMD_IDLE;
              else
                if conv_integer(reader_fifo_wr_count) < (C_FIFO_DEPTH-beats) then
                  -- start reader FSM
                  mst_cmd_sm_start_rd_llink <= '1';
                  mst_ip2bus_addr <= current_reader_address;
                  mst_cmd_sm_state <= CMD_RUN;
               else
                 mst_cmd_sm_state <= CMD_IDLE;
               end if;
              end if;

              remaining_words_test <= remaining_words(C_MST_AWIDTH-1-14 to C_MST_AWIDTH-4);

          when CMD_PREPARE_WRITE =>
              if flush_writer_fifo = '0' then 
                remaining_words := resize(SHR(writer_end_address - current_writer_address, X"3"), remaining_words'length);  -- FIXME: 3 is bad
              else
               remaining_words := resize(writer_fifo_rd_count1, remaining_words'length);
              end if;
              if conv_integer(remaining_words) >= BURST_LENGTH then
                beats := BURST_LENGTH;
              else
                beats := conv_integer(remaining_words);
              end if;
              
              if beats > 1 then
                mst_cntl_burst <= '1';
              else
                mst_cntl_burst <= '0';
             end if;
              
              mst_xfer_length <= conv_std_logic_vector(beats * (C_MST_NATIVE_DATA_WIDTH/8), 12);
              if ( writer_is_active = '1' and writer_reached_end_addr = '1' ) then
                status_writer_done_irq <= '1';
              end if; 
              if writer_reached_end_addr = '1' then
                mst_cmd_sm_state <= CMD_IDLE;
              else
                if (conv_integer(writer_fifo_rd_count) >= beats) then
                  -- start write FSM
                  mst_cmd_sm_start_wr_llink <= '1';
                  mst_ip2bus_addr <= current_writer_address;
                  mst_cmd_sm_state <= CMD_RUN;
                else
                  mst_cmd_sm_state <= CMD_IDLE;
                end if;
              end if;

              remaining_words_test <= remaining_words(C_MST_AWIDTH-1-14 to C_MST_AWIDTH-4);

          when CMD_RUN =>
            if ( Bus2IP_Mst_CmdAck = '1' and Bus2IP_Mst_Cmplt = '0' ) then
              mst_cmd_sm_state <= CMD_WAIT_FOR_DATA;
            elsif ( Bus2IP_Mst_Cmplt = '1' ) then
              mst_cmd_sm_state <= CMD_IDLE;
              if ( Bus2IP_Mst_Cmd_Timeout = '1' ) then
                -- AXI4LITE address phase timeout
                mst_cmd_sm_set_error   <= '1';
                mst_cmd_sm_set_timeout <= '1';
              elsif ( Bus2IP_Mst_Error = '1' ) then
                -- AXI4LITE data transfer error
                mst_cmd_sm_set_error   <= '1';
              end if;
            else
              mst_cmd_sm_state       <= CMD_RUN;
              mst_cmd_sm_rd_req      <= (mst_cmd_sm_rd_req or mst_cmd_sm_start_rd_llink) and not Bus2IP_Mst_CmdAck;
              mst_cmd_sm_wr_req      <= (mst_cmd_sm_wr_req or mst_cmd_sm_start_wr_llink) and not Bus2IP_Mst_CmdAck;
             -- mst_cmd_sm_rd_req      <= mst_cntl_rd_req;
             -- mst_cmd_sm_wr_req      <= mst_cntl_wr_req;
              mst_cmd_sm_ip2bus_addr <= mst_ip2bus_addr;
             -- mst_cmd_sm_ip2bus_be   <= mst_ip2bus_be(15 downto 16-C_MST_NATIVE_DATA_WIDTH/8 );
              mst_cmd_sm_xfer_type   <= mst_cntl_burst;
              mst_cmd_sm_xfer_length <= mst_xfer_length;
              --mst_cmd_sm_bus_lock    <= mst_cntl_bus_lock;
            end if;

          when CMD_WAIT_FOR_DATA =>
            if ( Bus2IP_Mst_Cmplt = '1' ) then
              mst_cmd_sm_state    <= CMD_IDLE;
              mst_cmd_sm_set_done <= '1';
              mst_cmd_sm_busy     <= '0';
              if ( reader_is_active = '1' and reader_reached_end_addr = '1' ) then
                status_reader_done_irq <= '1';
              end if;
              if ( writer_is_active = '1' and writer_reached_end_addr = '1' ) then
                status_writer_done_irq <= '1';
              end if;
              if ( Bus2IP_Mst_Cmd_Timeout = '1' ) then
                -- AXI4LITE address phase timeout
                mst_cmd_sm_set_error   <= '1';
                mst_cmd_sm_set_timeout <= '1';
              elsif ( Bus2IP_Mst_Error = '1' ) then
                -- AXI4LITE data transfer error
                mst_cmd_sm_set_error   <= '1';
              end if;
            else
              mst_cmd_sm_state <= CMD_WAIT_FOR_DATA;
            end if;
          when others =>
            mst_cmd_sm_state    <= CMD_IDLE;
            mst_cmd_sm_busy     <= '0';

        end case;

      end if;
    end if;


  end process MASTER_CMD_SM_PROC;

  -- user logic master read locallink interface assignments
  IP2Bus_MstRd_dst_rdy_n <= not(mst_llrd_sm_dst_rdy);
  IP2Bus_MstRd_dst_dsc_n <= '1'; -- do not throttle data

  -- implement a simple state machine to enable the
  -- read locallink interface to transfer data
  LLINK_RD_SM_PROCESS : process( Bus2IP_Clk ) is
  begin

    if ( Bus2IP_Clk'event and Bus2IP_Clk = '1' ) then
      if ( Bus2IP_Resetn = '0' ) then

        -- reset condition
        mst_llrd_sm_state   <= LLRD_IDLE;
        mst_llrd_sm_dst_rdy <= '0';

      else

        -- default condition
        mst_llrd_sm_state   <= LLRD_IDLE;
        mst_llrd_sm_dst_rdy <= '0';

        -- state transition
        case mst_llrd_sm_state is

          when LLRD_IDLE =>
            if ( mst_cmd_sm_start_rd_llink = '1') then
              mst_llrd_sm_state <= LLRD_GO;
            else
              mst_llrd_sm_state <= LLRD_IDLE;
            end if;

          when LLRD_GO =>
            -- done, end of packet
            if ( mst_llrd_sm_dst_rdy    = '1' and
                 Bus2IP_MstRd_src_rdy_n = '0' and
                 Bus2IP_MstRd_eof_n     = '0' ) then
              mst_llrd_sm_state   <= LLRD_IDLE;
            -- not done yet, continue receiving data
            else
              mst_llrd_sm_state   <= LLRD_GO;
              mst_llrd_sm_dst_rdy <= '1';
            end if;

          when others =>
            mst_llrd_sm_state <= LLRD_IDLE;

        end case;

      end if;
    else
      null;
    end if;

  end process LLINK_RD_SM_PROCESS;

  -- user logic master write locallink interface assignments
  IP2Bus_MstWr_src_rdy_n <= not(mst_llwr_sm_src_rdy);
  IP2Bus_MstWr_src_dsc_n <= '1'; -- do not throttle data
  IP2Bus_MstWr_rem       <= (others => '0');
  IP2Bus_MstWr_sof_n     <= not(mst_llwr_sm_sof);
  IP2Bus_MstWr_eof_n     <= not(mst_llwr_sm_eof);

  -- implement a simple state machine to enable the
  -- write locallink interface to transfer data
  LLINK_WR_SM_PROC : process( Bus2IP_Clk ) is
    constant BYTES_PER_BEAT : integer := C_MST_NATIVE_DATA_WIDTH/8;
  begin

    if ( Bus2IP_Clk'event and Bus2IP_Clk = '1' ) then
      if ( Bus2IP_Resetn = '0' ) then

        -- reset condition
        mst_llwr_sm_state   <= LLWR_IDLE;
        mst_llwr_sm_src_rdy <= '0';
        mst_llwr_sm_sof     <= '0';
        mst_llwr_sm_eof     <= '0';
        mst_llwr_byte_cnt   <= 0;

      else

        -- default condition
        mst_llwr_sm_state   <= LLWR_IDLE;
        mst_llwr_sm_src_rdy <= '0';
        mst_llwr_sm_sof     <= '0';
        mst_llwr_sm_eof     <= '0';
        mst_llwr_byte_cnt   <= 0;

        -- state transition
        case mst_llwr_sm_state is

          when LLWR_IDLE =>
            if ( mst_cmd_sm_start_wr_llink = '1' and mst_cntl_burst = '0' ) then
              mst_llwr_sm_state <= LLWR_SNGL_INIT;
            elsif ( mst_cmd_sm_start_wr_llink = '1' and mst_cntl_burst = '1' ) then
              mst_llwr_sm_state <= LLWR_BRST_INIT;
            else
              mst_llwr_sm_state <= LLWR_IDLE;
            end if;

          when LLWR_SNGL_INIT =>
            mst_llwr_sm_state   <= LLWR_SNGL;
            mst_llwr_sm_src_rdy <= '1';
            mst_llwr_sm_sof     <= '1';
            mst_llwr_sm_eof     <= '1';

          when LLWR_SNGL =>
            -- destination discontinue write
            if ( Bus2IP_MstWr_dst_dsc_n = '0' and Bus2IP_MstWr_dst_rdy_n = '0' ) then
              mst_llwr_sm_state   <= LLWR_IDLE;
              mst_llwr_sm_src_rdy <= '0';
              mst_llwr_sm_eof     <= '0';
            -- single data beat transfer complete
            elsif ( mst_fifo_valid_read_xfer = '1' ) then
              mst_llwr_sm_state   <= LLWR_IDLE;
              mst_llwr_sm_src_rdy <= '0';
              mst_llwr_sm_sof     <= '0';
              mst_llwr_sm_eof     <= '0';
            -- wait on destination
            else
              mst_llwr_sm_state   <= LLWR_SNGL;
              mst_llwr_sm_src_rdy <= '1';
              mst_llwr_sm_sof     <= '1';
              mst_llwr_sm_eof     <= '1';
            end if;

          when LLWR_BRST_INIT =>
            mst_llwr_sm_state   <= LLWR_BRST;
            mst_llwr_sm_src_rdy <= '1';
            mst_llwr_sm_sof     <= '1';
            mst_llwr_byte_cnt   <= CONV_INTEGER(mst_xfer_length);

          when LLWR_BRST =>
            if ( mst_fifo_valid_read_xfer = '1' ) then
              mst_llwr_sm_sof <= '0';
            else
              mst_llwr_sm_sof <= mst_llwr_sm_sof;
            end if;
            -- destination discontinue write
            if ( Bus2IP_MstWr_dst_dsc_n = '0' and
                 Bus2IP_MstWr_dst_rdy_n = '0' ) then
              mst_llwr_sm_state   <= LLWR_IDLE;
              mst_llwr_sm_src_rdy <= '1';
              mst_llwr_sm_eof     <= '1';
            -- last data beat write
            elsif ( mst_fifo_valid_read_xfer = '1' and
              (mst_llwr_byte_cnt-BYTES_PER_BEAT) <= BYTES_PER_BEAT ) then
              mst_llwr_sm_state   <= LLWR_BRST_LAST_BEAT;
              mst_llwr_sm_src_rdy <= '1';
              mst_llwr_sm_eof     <= '1';
            -- wait on destination
            else
              mst_llwr_sm_state   <= LLWR_BRST;
              mst_llwr_sm_src_rdy <= '1';
              -- decrement write transfer counter if it's a valid write
              if ( mst_fifo_valid_read_xfer = '1' ) then
                mst_llwr_byte_cnt <= mst_llwr_byte_cnt - BYTES_PER_BEAT;
              else
                mst_llwr_byte_cnt <= mst_llwr_byte_cnt;
              end if;
            end if;

          when LLWR_BRST_LAST_BEAT =>
            -- destination discontinue write
            if ( Bus2IP_MstWr_dst_dsc_n = '0' and
                 Bus2IP_MstWr_dst_rdy_n = '0' ) then
              mst_llwr_sm_state   <= LLWR_IDLE;
              mst_llwr_sm_src_rdy <= '0';
            -- last data beat done
            elsif ( mst_fifo_valid_read_xfer = '1' ) then
              mst_llwr_sm_state   <= LLWR_IDLE;
              mst_llwr_sm_src_rdy <= '0';
            -- wait on destination
            else
              mst_llwr_sm_state   <= LLWR_BRST_LAST_BEAT;
              mst_llwr_sm_src_rdy <= '1';
              mst_llwr_sm_eof     <= '1';
            end if;

          when others =>
            mst_llwr_sm_state <= LLWR_IDLE;

        end case;

      end if;
    else
      null;
    end if;

  end process LLINK_WR_SM_PROC;

  -- local srl fifo for data storage
  mst_fifo_valid_write_xfer <= not(Bus2IP_MstRd_src_rdy_n) and mst_llrd_sm_dst_rdy;
  mst_fifo_valid_read_xfer  <= not(Bus2IP_MstWr_dst_rdy_n) and mst_llwr_sm_src_rdy;

  ----------------------------------------------------------------------------------------------------------------------
	-- READ ONLY
  ----------------------------------------------------------------------------------------------------------------------
  reader_fifo_rd_en <= (not stream_out_stop) or clear_reader_fifo;

	r: if C_OPERATION_MODE = 0 or C_OPERATION_MODE = 2 generate
		-- READ BUFFER (PLB master attachment could write to the FIFO and the SDI interface read it)
	DATA_CAPTURE_FIFO_READER : fifo
    generic map (
      DATA_WIDTH => C_MST_NATIVE_DATA_WIDTH,
      FIFO_DEPTH_BITS => log2(C_FIFO_DEPTH) )
		port map (
			din => Bus2IP_MstRd_d,
			rd_clk => stream_out_clk,
			rd_en => reader_fifo_rd_en, 
			rst => reader_fifo_reset,
			wr_clk => Bus2IP_Clk,
			wr_en => mst_fifo_valid_write_xfer,
			dout => stream_out_data,
			empty => reader_fifo_empty,
			full =>open,
                        rd_data_count => open,
                        wr_data_count => reader_fifo_wr_count );
	end generate;

   stream_out_valid <= (not reader_fifo_empty) and (not clear_reader_fifo);
	----------------------------------------------------------------------------------------------------------------------
	-- WRITE ONLY
	----------------------------------------------------------------------------------------------------------------------
	w: if C_OPERATION_MODE = 1 or C_OPERATION_MODE = 2 generate
		-- WRITE BUFFER (PLB master attachment could read from the FIFO and SDI write to it)
	--DATA_CAPTURE_FIFO_WRITER : fifo
    --generic map (
      --DATA_WIDTH => C_MST_NATIVE_DATA_WIDTH,
     -- FIFO_DEPTH_BITS => log2(C_FIFO_DEPTH) )
	--	port map (
		--	din =>stream_in_data,
        DATA_CAPTURE_FIFO_WRITER : fifo_generator_v4_4_16_64
		port map (
			din => stream_in_data(39 downto 32) & stream_in_data(7 downto 0),
			rd_clk => Bus2IP_Clk,
			rd_en => mst_fifo_valid_read_xfer or clear_writer_fifo,
			rst => writer_fifo_reset,
			wr_clk => stream_in_clk,
			wr_en => stream_in_valid and writer_is_active ,
			dout => mst_wr_d,
			empty => writer_fifo_empty,
			full => writer_fifo_full, 
      rd_data_count => writer_fifo_rd_count,
      wr_data_count => open );
	end generate;

  stream_in_stop <= writer_fifo_full or not writer_is_active;
  
  IP2Bus_IntrEvent <= (
    0 => status_reader_done_irq,
    1 => status_writer_done_irq,
    2 => reader_fifo_is_flushed_irq,
    3 => reader_trigger_addr_irq,
    4 => writer_trigger_addr_irq,
    others => '0');
  ------------------------------------------
  -- Example code to drive IP to Bus signals
  ------------------------------------------
  IP2Bus_Data  <= slv_ip2bus_data when reg_read_ack = '1' else
                  (others => '0');

  IP2Bus_WrAck <= reg_write_ack;
  IP2Bus_RdAck <= reg_read_ack;
  IP2Bus_Error <= '0';
-------------------------------------------------------------------------------------

	DEBUG_D_T <= (others => '1');	-- High Z, Input mode

	DEBUG_D_O(0) <= Bus2IP_Clk;
	DEBUG_D_O(1) <= mst_cmd_sm_rd_req;
	DEBUG_D_O(2) <= mst_cmd_sm_wr_req;
	DEBUG_D_O(3) <= mst_cmd_sm_xfer_type;
	DEBUG_D_O(4) <= Bus2IP_Mst_CmdAck;
	DEBUG_D_O(5) <= Bus2IP_Mst_Cmplt;

--	DEBUG_D_O(6) <= Bus2IP_MstRd_sof_n;
--	DEBUG_D_O(7) <= Bus2IP_MstRd_eof_n;
--	DEBUG_D_O(8) <= Bus2IP_MstRd_src_rdy_n;
--	DEBUG_D_O(9) <= not(mst_llrd_sm_dst_rdy);
--	DEBUG_D_O(10) <= not(mst_llwr_sm_sof);
--	DEBUG_D_O(11) <= not(mst_llwr_sm_eof);
--	DEBUG_D_O(12) <= not(mst_llwr_sm_src_rdy);
--	DEBUG_D_O(13) <= Bus2IP_MstWr_dst_rdy_n;
--	DEBUG_D_O(14) <= mst_fifo_valid_read_xfer;
--	DEBUG_D_O(15) <= mst_fifo_valid_write_xfer;
--
--	DEBUG_D_O(14 downto 6) <= writer_fifo_rd_count;
   DEBUG_D_O(14 downto 6) <= reader_fifo_wr_count(0 to 8);
	DEBUG_D_O(17 downto 15) <= conv_std_logic_vector(CMD_CNTL_SM_TYPE'pos(mst_cmd_sm_state), 3);
	DEBUG_D_O(29 downto 18) <= remaining_words_test;
	DEBUG_D_O(30) <= pu_eof_in;
	DEBUG_D_O(31) <= flush_writer_fifo;
   DEBUG_D_O(32) <= writer_is_active;
	DEBUG_D_O(33) <= status_writer_done_irq;
	DEBUG_D_O(34) <= reader_is_active;
	DEBUG_D_O(35) <= status_reader_done_irq;
	DEBUG_D_O(36) <= reader_fifo_is_flushed_irq;
--	DEBUG_D_O(44 downto 37) <= mst_xfer_length(4 to 11);

	DEBUG_D_O(37) <= stream_out_clk;
	DEBUG_D_O(38) <= stream_out_stop;
	DEBUG_D_O(39) <= not reader_fifo_empty; -- stream_out_valid
	DEBUG_D_O(40) <= '0';
  
	DEBUG_D_O(41) <= stream_in_clk;
	DEBUG_D_O(42) <= '0';
	DEBUG_D_O(43) <= stream_in_valid;
	DEBUG_D_O(44) <= stream_in_data(C_MST_NATIVE_DATA_WIDTH-1);

end IMP;

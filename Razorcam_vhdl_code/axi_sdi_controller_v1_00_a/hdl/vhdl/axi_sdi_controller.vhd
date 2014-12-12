------------------------------------------------------------------------------
-- axi_sdi_controller.vhd - entity/architecture pair
------------------------------------------------------------------------------
-- IMPORTANT:
-- DO NOT MODIFY THIS FILE EXCEPT IN THE DESIGNATED SECTIONS.
--
-- SEARCH FOR --USER TO DETERMINE WHERE CHANGES ARE ALLOWED.
--
-- TYPICALLY, THE ONLY ACCEPTABLE CHANGES INVOLVE ADDING NEW
-- PORTS AND GENERICS THAT GET PASSED THROUGH TO THE INSTANTIATION
-- OF THE USER_LOGIC ENTITY.
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
-- Filename:          axi_sdi_controller.vhd
-- Version:           1.00.a
-- Description:       Top level design, instantiates library components and user logic.
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

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library proc_common_v3_00_a;
use proc_common_v3_00_a.proc_common_pkg.all;
use proc_common_v3_00_a.ipif_pkg.all;

library axi_lite_ipif_v1_01_a;
use axi_lite_ipif_v1_01_a.axi_lite_ipif;

library axi_master_burst_v1_00_a;
use axi_master_burst_v1_00_a.axi_master_burst;

library axi_sdi_controller_v1_00_a;
use axi_sdi_controller_v1_00_a.user_logic;

------------------------------------------------------------------------------
-- Entity section
------------------------------------------------------------------------------
-- Definition of Generics:
--   C_S_AXI_DATA_WIDTH           -- 
--   C_S_AXI_ADDR_WIDTH           -- 
--   C_S_AXI_MIN_SIZE             -- 
--   C_USE_WSTRB                  -- 
--   C_DPHASE_TIMEOUT             -- 
--   C_BASEADDR                   -- AXI4LITE slave: base address
--   C_HIGHADDR                   -- AXI4LITE slave: high address
--   C_FAMILY                     -- 
--   C_NUM_REG                    -- Number of software accessible registers
--   C_NUM_MEM                    -- Number of address-ranges
--   C_SLV_AWIDTH                 -- Slave interface address bus width
--   C_SLV_DWIDTH                 -- Slave interface data bus width
--   C_M_AXI_ADDR_WIDTH           -- Master-Intf address bus width
--   C_M_AXI_DATA_WIDTH           -- Master-Intf data bus width
--   C_MAX_BURST_LEN              -- Max no. of data-beats allowed in burst
--   C_NATIVE_DATA_WIDTH          -- Internal bus width on user-side
--   C_LENGTH_WIDTH               -- Master interface data bus width
--   C_ADDR_PIPE_DEPTH            -- Depth of Address pipelining
--
-- Definition of Ports:
--   S_AXI_ACLK                   -- 
--   S_AXI_ARESETN                -- 
--   S_AXI_AWADDR                 -- 
--   S_AXI_AWVALID                -- 
--   S_AXI_WDATA                  -- 
--   S_AXI_WSTRB                  -- 
--   S_AXI_WVALID                 -- 
--   S_AXI_BREADY                 -- 
--   S_AXI_ARADDR                 -- 
--   S_AXI_ARVALID                -- 
--   S_AXI_RREADY                 -- 
--   S_AXI_ARREADY                -- 
--   S_AXI_RDATA                  -- 
--   S_AXI_RRESP                  -- 
--   S_AXI_RVALID                 -- 
--   S_AXI_WREADY                 -- 
--   S_AXI_BRESP                  -- 
--   S_AXI_BVALID                 -- 
--   S_AXI_AWREADY                -- 
--   m_axi_aclk                   -- 
--   m_axi_aresetn                -- 
--   md_error                     -- 
--   m_axi_arready                -- 
--   m_axi_arvalid                -- 
--   m_axi_araddr                 -- 
--   m_axi_arlen                  -- 
--   m_axi_arsize                 -- 
--   m_axi_arburst                -- 
--   m_axi_arprot                 -- 
--   m_axi_arcache                -- 
--   m_axi_rready                 -- 
--   m_axi_rvalid                 -- 
--   m_axi_rdata                  -- 
--   m_axi_rresp                  -- 
--   m_axi_rlast                  -- 
--   m_axi_awready                -- 
--   m_axi_awvalid                -- 
--   m_axi_awaddr                 -- 
--   m_axi_awlen                  -- 
--   m_axi_awsize                 -- 
--   m_axi_awburst                -- 
--   m_axi_awprot                 -- 
--   m_axi_awcache                -- 
--   m_axi_wready                 -- 
--   m_axi_wvalid                 -- 
--   m_axi_wdata                  -- 
--   m_axi_wstrb                  -- 
--   m_axi_wlast                  -- 
--   m_axi_bready                 -- 
--   m_axi_bvalid                 -- 
--   m_axi_bresp                  -- 
------------------------------------------------------------------------------

entity axi_sdi_controller is
  generic
  (
    -- ADD USER GENERICS BELOW THIS LINE ---------------
    --USER generics added here
    -- ADD USER GENERICS ABOVE THIS LINE ---------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol parameters, do not add to or delete
    C_S_AXI_DATA_WIDTH             : integer              := 32;
    C_S_AXI_ADDR_WIDTH             : integer              := 32;
    C_S_AXI_MIN_SIZE               : std_logic_vector     := X"000001FF";
    C_USE_WSTRB                    : integer              := 0;
    C_DPHASE_TIMEOUT               : integer              := 8;
    C_BASEADDR                     : std_logic_vector     := X"FFFFFFFF";
    C_HIGHADDR                     : std_logic_vector     := X"00000000";
    C_FAMILY                       : string               := "virtex6";
    C_DEBUG_DWIDTH                 : integer              := 45;
    C_OPERATION_MODE               : integer              := 0;	
    C_NUM_PU_CONFIG_REGS           : integer              := 1;	
    C_NUM_PU_STATUS_REGS           : integer              := 1;	
    C_FIFO_IMPLEMENTATION_TYPE	   : integer              := 1;   -- 0 = Any, 1 = BRAM, 2 = Dist.Memory, 3 = Shift Regs
    C_FIFO_DEPTH						       : integer              := 1024;
    --USER generics added here
    C_NUM_MEM                      : integer              := 1;
    C_SLV_AWIDTH                   : integer              := 32;
    C_SLV_DWIDTH                   : integer              := 32;
    C_M_AXI_ADDR_WIDTH             : integer              := 32;
    C_M_AXI_DATA_WIDTH             : integer              := 32;
    C_MAX_BURST_LEN                : integer              := 16;
    C_NATIVE_DATA_WIDTH            : integer              := 32;
    C_LENGTH_WIDTH                 : integer              := 12;
    C_ADDR_PIPE_DEPTH              : integer              := 1
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );
  port
  (
    -- ADD USER PORTS BELOW THIS LINE ------------------
    --USER ports added here
    DEBUG_D_I  	   					: in std_logic_vector (C_DEBUG_DWIDTH-1 downto 0) := '0' & X"00000000000";   
    DEBUG_D_O  	   					: out std_logic_vector (C_DEBUG_DWIDTH-1 downto 0);    
    DEBUG_D_T  	   					: out std_logic_vector (C_DEBUG_DWIDTH-1 downto 0);    
	 
    -- SDI Interface ------------------------------------------------------------------
    stream_out_clk						: in	std_logic :='0';
    stream_out_stop						: in	std_logic :='0';
    stream_out_valid					: out	std_logic;
    stream_out_data						: out	std_logic_vector(C_M_AXI_DATA_WIDTH-1 downto 0);
    stream_in_clk							: in	std_logic :='0';
    stream_in_stop						: out	std_logic;
    stream_in_valid						: in	std_logic :='0';
    stream_in_data						: in	std_logic_vector(C_M_AXI_DATA_WIDTH-1 downto 0) := X"00000000";
    debug:out std_logic_vector(69 downto 0);
    trig0:out std_logic_vector(0 downto 0);
    pu_reset                  : out	std_logic;	
    pu_sof                    : out std_logic;
    pu_sof_ack                : in std_logic :='0';
    pu_eof_in                    : in std_logic :='0';
    pu_sof_in                     : in std_logic :='0';
    pu_eof_ack                : out std_logic;
    request_frame             : in std_logic :='0';
    request_frame_ack         : out std_logic;

    -- PU Registers ---------------------------------------------------------------------
    pu_config_regs				    : out	std_logic_vector(C_NUM_PU_CONFIG_REGS*C_S_AXI_DATA_WIDTH-1 downto 0);
    pu_status_regs		        : in	std_logic_vector(C_NUM_PU_STATUS_REGS*C_S_AXI_DATA_WIDTH-1 downto 0):= X"00000000";
    IP2INTC_Irpt                   : out std_logic;
    -- ADD USER PORTS ABOVE THIS LINE ------------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol ports, do not add to or delete
    S_AXI_ACLK                     : in  std_logic;
    S_AXI_ARESETN                  : in  std_logic;
    S_AXI_AWADDR                   : in  std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
    S_AXI_AWVALID                  : in  std_logic;
    S_AXI_WDATA                    : in  std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
    S_AXI_WSTRB                    : in  std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
    S_AXI_WVALID                   : in  std_logic;
    S_AXI_BREADY                   : in  std_logic;
    S_AXI_ARADDR                   : in  std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
    S_AXI_ARVALID                  : in  std_logic;
    S_AXI_RREADY                   : in  std_logic;
    S_AXI_ARREADY                  : out std_logic;
    S_AXI_RDATA                    : out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
    S_AXI_RRESP                    : out std_logic_vector(1 downto 0);
    S_AXI_RVALID                   : out std_logic;
    S_AXI_WREADY                   : out std_logic;
    S_AXI_BRESP                    : out std_logic_vector(1 downto 0);
    S_AXI_BVALID                   : out std_logic;
    S_AXI_AWREADY                  : out std_logic;
    m_axi_aclk                     : in  std_logic;
    m_axi_aresetn                  : in  std_logic;
    md_error                       : out std_logic;
    m_axi_arready                  : in  std_logic;
    m_axi_arvalid                  : out std_logic;
    m_axi_araddr                   : out std_logic_vector(C_M_AXI_ADDR_WIDTH-1 downto 0);
    m_axi_arlen                    : out std_logic_vector(7 downto 0);
    m_axi_arsize                   : out std_logic_vector(2 downto 0);
    m_axi_arburst                  : out std_logic_vector(1 downto 0);
    m_axi_arprot                   : out std_logic_vector(2 downto 0);
    m_axi_arcache                  : out std_logic_vector(3 downto 0);
    m_axi_rready                   : out std_logic;
    m_axi_rvalid                   : in  std_logic;
    m_axi_rdata                    : in  std_logic_vector(C_M_AXI_DATA_WIDTH-1 downto 0);
    m_axi_rresp                    : in  std_logic_vector(1 downto 0);
    m_axi_rlast                    : in  std_logic;
    m_axi_awready                  : in  std_logic;
    m_axi_awvalid                  : out std_logic;
    m_axi_awaddr                   : out std_logic_vector(C_M_AXI_ADDR_WIDTH-1 downto 0);
    m_axi_awlen                    : out std_logic_vector(7 downto 0);
    m_axi_awsize                   : out std_logic_vector(2 downto 0);
    m_axi_awburst                  : out std_logic_vector(1 downto 0);
    m_axi_awprot                   : out std_logic_vector(2 downto 0);
    m_axi_awcache                  : out std_logic_vector(3 downto 0);
    m_axi_wready                   : in  std_logic;
    m_axi_wvalid                   : out std_logic;
    m_axi_wdata                    : out std_logic_vector(C_M_AXI_DATA_WIDTH-1 downto 0);
    m_axi_wstrb                    : out std_logic_vector((C_M_AXI_DATA_WIDTH)/8 - 1 downto 0);
    m_axi_wlast                    : out std_logic;
    m_axi_bready                   : out std_logic;
    m_axi_bvalid                   : in  std_logic;
    m_axi_bresp                    : in  std_logic_vector(1 downto 0)
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );

  attribute MAX_FANOUT : string;
  attribute SIGIS : string;
  attribute MAX_FANOUT of S_AXI_ACLK       : signal is "10000";
  attribute MAX_FANOUT of S_AXI_ARESETN       : signal is "10000";
  attribute SIGIS of S_AXI_ACLK       : signal is "Clk";
  attribute SIGIS of S_AXI_ARESETN       : signal is "Rst";

  attribute MAX_FANOUT of m_axi_aclk       : signal is "10000";
  attribute MAX_FANOUT of m_axi_aresetn       : signal is "10000";
  attribute SIGIS of m_axi_aclk       : signal is "Clk";
  attribute SIGIS of m_axi_aresetn       : signal is "Rst";
  attribute SIGIS of IP2INTC_Irpt  : signal is "INTR_LEVEL_HIGH";

end entity axi_sdi_controller;

------------------------------------------------------------------------------
-- Architecture section
------------------------------------------------------------------------------

architecture IMP of axi_sdi_controller is
  function max(L, R: INTEGER) return INTEGER is
  begin
    if L > R then
      return L;
    else
      return R;
    end if;
  end;
  constant USER_SLV_DWIDTH                : integer              := C_S_AXI_DATA_WIDTH;

  constant IPIF_SLV_DWIDTH                : integer              := C_S_AXI_DATA_WIDTH;

  constant ZERO_ADDR_PAD                  : std_logic_vector(0 to 31) := (others => '0');
  constant CONTROLLER_BASEADDR            : std_logic_vector     := C_BASEADDR or X"00000000";
  constant CONTROLLER_HIGHADDR            : std_logic_vector     := C_BASEADDR or X"000000FF";
  constant INTR_BASEADDR                  : std_logic_vector     := C_BASEADDR or X"00000100";
  constant INTR_HIGHADDR                  : std_logic_vector     := C_BASEADDR or X"000001FF";

 constant IPIF_ARD_ADDR_RANGE_ARRAY      : SLV64_ARRAY_TYPE     := 
    (
      ZERO_ADDR_PAD & CONTROLLER_BASEADDR,  -- controller space base address
      ZERO_ADDR_PAD & CONTROLLER_HIGHADDR,  -- controller space high address
      ZERO_ADDR_PAD & INTR_BASEADDR,        -- interrupt control space base address
      ZERO_ADDR_PAD & INTR_HIGHADDR         -- interrupt control space high address
    );
  constant PU_NUM_REG                     : integer              := max(C_NUM_PU_CONFIG_REGS, C_NUM_PU_STATUS_REGS);
  constant CONTROLLER_NUM_REG             : integer              := 9 + PU_NUM_REG;
  constant INTR_NUM_CE                    : integer              := 16;


  constant IPIF_ARD_NUM_CE_ARRAY          : INTEGER_ARRAY_TYPE   := 
    (
        0  => (pad_power2(CONTROLLER_NUM_REG)), -- number of ce for controller space
        1  => INTR_NUM_CE                     -- number of ce for interrupt control space
    );
  constant IPIF_INHIBIT_CC_BLE_INCLUSION  : integer              := 0;
  ------------------------------------------
  -- Number of device level interrupts
  ------------------------------------------
  constant INTR_NUM_IPIF_IRPT_SRC         : integer              := 4;
  ------------------------------------------
  ------------------------------------------
  -- Width of triggered reset in bus clocks
  ------------------------------------------
  constant RESET_WIDTH                    : integer              := 8;
  constant USER_NUM_INTR                  : integer              := 32;
  constant USER_INTR_CAPTURE_MODE         : integer              := 5;
  constant INTR_IP_INTR_MODE_ARRAY        : INTEGER_ARRAY_TYPE   := 
    (
      0  => USER_INTR_CAPTURE_MODE,
      1  => USER_INTR_CAPTURE_MODE,
      2  => USER_INTR_CAPTURE_MODE,
      3  => USER_INTR_CAPTURE_MODE,
      4  => USER_INTR_CAPTURE_MODE,
      5  => USER_INTR_CAPTURE_MODE,
      6  => USER_INTR_CAPTURE_MODE,
      7  => USER_INTR_CAPTURE_MODE,
      8  => USER_INTR_CAPTURE_MODE,
      9  => USER_INTR_CAPTURE_MODE,
      10 => USER_INTR_CAPTURE_MODE,
      11 => USER_INTR_CAPTURE_MODE,
      12 => USER_INTR_CAPTURE_MODE,
      13 => USER_INTR_CAPTURE_MODE,
      14 => USER_INTR_CAPTURE_MODE,
      15 => USER_INTR_CAPTURE_MODE,
      16 => USER_INTR_CAPTURE_MODE,
      17 => USER_INTR_CAPTURE_MODE,
      18 => USER_INTR_CAPTURE_MODE,
      19 => USER_INTR_CAPTURE_MODE,
      20 => USER_INTR_CAPTURE_MODE,
      21 => USER_INTR_CAPTURE_MODE,
      22 => USER_INTR_CAPTURE_MODE,
      23 => USER_INTR_CAPTURE_MODE,
      24 => USER_INTR_CAPTURE_MODE,
      25 => USER_INTR_CAPTURE_MODE,
      26 => USER_INTR_CAPTURE_MODE,
      27 => USER_INTR_CAPTURE_MODE,
      28 => USER_INTR_CAPTURE_MODE,
      29 => USER_INTR_CAPTURE_MODE,
      30 => USER_INTR_CAPTURE_MODE,
      31 => USER_INTR_CAPTURE_MODE 
    );

  ------------------------------------------
  -- Width of the master address bus (32 only)
  ------------------------------------------
  constant USER_MST_AWIDTH                : integer              := C_M_AXI_ADDR_WIDTH;

  ------------------------------------------
  -- Width of the master data bus 
  ------------------------------------------
  constant USER_MST_DWIDTH                : integer              := C_M_AXI_DATA_WIDTH;

  ------------------------------------------
  -- Width of data-bus going to user-logic
  ------------------------------------------
  constant USER_MST_NATIVE_DATA_WIDTH     : integer              := C_NATIVE_DATA_WIDTH;

  ------------------------------------------
  -- Width of the master data bus (12-20 )
  ------------------------------------------
  constant USER_LENGTH_WIDTH              : integer              := C_LENGTH_WIDTH;

  ------------------------------------------
  -- Index for CS/CE
  ------------------------------------------
  -- Index for CS/CE
  ------------------------------------------
  constant CONTROLLER_CS_INDEX            : integer              := 0;
  constant CONTROLLER_CE_INDEX            : integer              := calc_start_ce_index(IPIF_ARD_NUM_CE_ARRAY, CONTROLLER_CS_INDEX);
  constant INTR_CS_INDEX                  : integer              := 1;
  constant INTR_CE_INDEX                  : integer              := calc_start_ce_index(IPIF_ARD_NUM_CE_ARRAY, INTR_CS_INDEX);
  constant IPIF_BUS2CORE_CLK_RATIO        : integer              := 1;
  
  ------------------------------------------
  -- Device priority encoder feature inclusion/omission
  -- true  = include priority encoder
  -- false = omit priority encoder
  ------------------------------------------
  constant INTR_INCLUDE_DEV_PENCODER      : boolean              := true;

  ------------------------------------------
  -- Device ISC feature inclusion/omission
  -- true  = include device ISC
  -- false = omit device ISC
  ------------------------------------------
  constant INTR_INCLUDE_DEV_ISC           : boolean              := true;

  ------------------------------------------
  -- IP Interconnect (IPIC) signal declarations
  ------------------------------------------
  signal ipif_Bus2IP_Clk                : std_logic;
  signal ipif_Bus2IP_Resetn             : std_logic;
  signal ipif_Bus2IP_Addr               : std_logic_vector(0 to C_S_AXI_ADDR_WIDTH-1);
  signal ipif_Bus2IP_RNW                : std_logic;
  signal ipif_Bus2IP_BE                 : std_logic_vector(0 to IPIF_SLV_DWIDTH/8-1);
  signal ipif_Bus2IP_CS                 : std_logic_vector(0 to (IPIF_ARD_ADDR_RANGE_ARRAY'LENGTH)/2-1);
  signal ipif_Bus2IP_RdCE               : std_logic_vector(0 to calc_num_ce(IPIF_ARD_NUM_CE_ARRAY)-1 );
  signal ipif_Bus2IP_WrCE               : std_logic_vector(0 to calc_num_ce(IPIF_ARD_NUM_CE_ARRAY)-1 );
  signal ipif_Bus2IP_Data               : std_logic_vector(0 to IPIF_SLV_DWIDTH-1);
  signal ipif_IP2Bus_WrAck              : std_logic;
  signal ipif_IP2Bus_RdAck              : std_logic;
  signal ipif_IP2Bus_Error              : std_logic;
  signal ipif_IP2Bus_Data               : std_logic_vector(0 to IPIF_SLV_DWIDTH-1);
  signal ipif_ip2bus_mstrd_req          : std_logic;
  signal ipif_ip2bus_mstwr_req          : std_logic;
  signal ipif_ip2bus_mst_addr           : std_logic_vector(0 to C_M_AXI_ADDR_WIDTH-1);
  signal ipif_ip2bus_mst_be             : std_logic_vector(0 to (C_NATIVE_DATA_WIDTH)/8-1);
  signal ipif_ip2bus_mst_length         : std_logic_vector(0 to C_LENGTH_WIDTH-1);
  signal ipif_ip2bus_mst_type           : std_logic;
  signal ipif_ip2bus_mst_lock           : std_logic;
  signal ipif_ip2bus_mst_reset          : std_logic;
  signal ipif_bus2ip_mst_cmdack         : std_logic;
  signal ipif_bus2ip_mst_cmplt          : std_logic;
  signal ipif_bus2ip_mst_error          : std_logic;
  signal ipif_bus2ip_mst_rearbitrate    : std_logic;
  signal ipif_bus2ip_mst_cmd_timeout    : std_logic;
  signal ipif_bus2ip_mstrd_d            : std_logic_vector(0 to C_NATIVE_DATA_WIDTH-1);
  signal ipif_bus2ip_mstrd_rem          : std_logic_vector(0 to (C_NATIVE_DATA_WIDTH)/8-1);
  signal ipif_bus2ip_mstrd_sof_n        : std_logic;
  signal ipif_bus2ip_mstrd_eof_n        : std_logic;
  signal ipif_bus2ip_mstrd_src_rdy_n    : std_logic;
  signal ipif_bus2ip_mstrd_src_dsc_n    : std_logic;
  signal ipif_ip2bus_mstrd_dst_rdy_n    : std_logic;
  signal ipif_ip2bus_mstrd_dst_dsc_n    : std_logic;
  signal ipif_ip2bus_mstwr_d            : std_logic_vector(0 to C_NATIVE_DATA_WIDTH-1);
  signal ipif_ip2bus_mstwr_rem          : std_logic_vector(0 to (C_NATIVE_DATA_WIDTH)/8-1);
  signal ipif_ip2bus_mstwr_src_rdy_n    : std_logic;
  signal ipif_ip2bus_mstwr_src_dsc_n    : std_logic;
  signal ipif_ip2bus_mstwr_sof_n        : std_logic;
  signal ipif_ip2bus_mstwr_eof_n        : std_logic;
  signal ipif_bus2ip_mstwr_dst_rdy_n    : std_logic;
  signal ipif_bus2ip_mstwr_dst_dsc_n    : std_logic;
  signal ipif_Bus2IP_Reset              : std_logic;
  signal rst_Bus2IP_Reset               : std_logic;
  signal rst_IP2Bus_WrAck               : std_logic;
  signal rst_IP2Bus_Error               : std_logic;
  signal rst_Bus2IP_Reset_tmp           : std_logic;
  signal user_Bus2IP_RdCE               : std_logic_vector(0 to CONTROLLER_NUM_REG-1);
  signal user_Bus2IP_WrCE               : std_logic_vector(0 to CONTROLLER_NUM_REG-1);
  signal user_IP2Bus_Data               : std_logic_vector(0 to USER_SLV_DWIDTH-1);
  signal user_IP2Bus_RdAck              : std_logic;
  signal user_IP2Bus_WrAck              : std_logic;
  signal user_IP2Bus_Error              : std_logic;

  signal intr_IPIF_Reg_Interrupts       : std_logic_vector(0 to 1);
  signal intr_IPIF_Lvl_Interrupts       : std_logic_vector(0 to INTR_NUM_IPIF_IRPT_SRC-1);
  signal intr_IP2Bus_Data               : std_logic_vector(0 to IPIF_SLV_DWIDTH-1);
  signal intr_IP2Bus_WrAck              : std_logic;
  signal intr_IP2Bus_RdAck              : std_logic;
  signal intr_IP2Bus_Error              : std_logic;
  signal user_IP2Bus_IntrEvent          : std_logic_vector(0 to USER_NUM_INTR-1);
begin

  ------------------------------------------
  -- instantiate axi_lite_ipif
  ------------------------------------------
  AXI_LITE_IPIF_I : entity axi_lite_ipif_v1_01_a.axi_lite_ipif
    generic map
    (
      C_S_AXI_DATA_WIDTH             => IPIF_SLV_DWIDTH,
      C_S_AXI_ADDR_WIDTH             => C_S_AXI_ADDR_WIDTH,
      C_S_AXI_MIN_SIZE               => C_S_AXI_MIN_SIZE,
      C_USE_WSTRB                    => C_USE_WSTRB,
      C_DPHASE_TIMEOUT               => C_DPHASE_TIMEOUT,
      C_ARD_ADDR_RANGE_ARRAY         => IPIF_ARD_ADDR_RANGE_ARRAY,
      C_ARD_NUM_CE_ARRAY             => IPIF_ARD_NUM_CE_ARRAY,
      C_FAMILY                       => C_FAMILY
    )
    port map
    (
      S_AXI_ACLK                     => S_AXI_ACLK,
      S_AXI_ARESETN                  => S_AXI_ARESETN,
      S_AXI_AWADDR                   => S_AXI_AWADDR,
      S_AXI_AWVALID                  => S_AXI_AWVALID,
      S_AXI_WDATA                    => S_AXI_WDATA,
      S_AXI_WSTRB                    => S_AXI_WSTRB,
      S_AXI_WVALID                   => S_AXI_WVALID,
      S_AXI_BREADY                   => S_AXI_BREADY,
      S_AXI_ARADDR                   => S_AXI_ARADDR,
      S_AXI_ARVALID                  => S_AXI_ARVALID,
      S_AXI_RREADY                   => S_AXI_RREADY,
      S_AXI_ARREADY                  => S_AXI_ARREADY,
      S_AXI_RDATA                    => S_AXI_RDATA,
      S_AXI_RRESP                    => S_AXI_RRESP,
      S_AXI_RVALID                   => S_AXI_RVALID,
      S_AXI_WREADY                   => S_AXI_WREADY,
      S_AXI_BRESP                    => S_AXI_BRESP,
      S_AXI_BVALID                   => S_AXI_BVALID,
      S_AXI_AWREADY                  => S_AXI_AWREADY,
      Bus2IP_Clk                     => ipif_Bus2IP_Clk,
      Bus2IP_Resetn                  => ipif_Bus2IP_Resetn,
      Bus2IP_Addr                    => ipif_Bus2IP_Addr,
      Bus2IP_RNW                     => ipif_Bus2IP_RNW,
      Bus2IP_BE                      => ipif_Bus2IP_BE,
      Bus2IP_CS                      => ipif_Bus2IP_CS,
      Bus2IP_RdCE                    => ipif_Bus2IP_RdCE,
      Bus2IP_WrCE                    => ipif_Bus2IP_WrCE,
      Bus2IP_Data                    => ipif_Bus2IP_Data,
      IP2Bus_WrAck                   => ipif_IP2Bus_WrAck,
      IP2Bus_RdAck                   => ipif_IP2Bus_RdAck,
      IP2Bus_Error                   => ipif_IP2Bus_Error,
      IP2Bus_Data                    => ipif_IP2Bus_Data
    );

  ------------------------------------------
  -- instantiate axi_master_burst
  ------------------------------------------
  AXI_MASTER_BURST_I : entity axi_master_burst_v1_00_a.axi_master_burst
    generic map
    (
      C_M_AXI_ADDR_WIDTH             => C_M_AXI_ADDR_WIDTH,
      C_M_AXI_DATA_WIDTH             => C_M_AXI_DATA_WIDTH,
      C_MAX_BURST_LEN                => C_MAX_BURST_LEN,
      C_NATIVE_DATA_WIDTH            => C_NATIVE_DATA_WIDTH,
      C_LENGTH_WIDTH                 => C_LENGTH_WIDTH,
      C_ADDR_PIPE_DEPTH              => C_ADDR_PIPE_DEPTH,
      C_FAMILY                       => C_FAMILY
    )
    port map
    (
      m_axi_aclk                     => m_axi_aclk,
      m_axi_aresetn                  => m_axi_aresetn,
      md_error                       => md_error,
      m_axi_arready                  => m_axi_arready,
      m_axi_arvalid                  => m_axi_arvalid,
      m_axi_araddr                   => m_axi_araddr,
      m_axi_arlen                    => m_axi_arlen,
      m_axi_arsize                   => m_axi_arsize,
      m_axi_arburst                  => m_axi_arburst,
      m_axi_arprot                   => m_axi_arprot,
      m_axi_arcache                  => m_axi_arcache,
      m_axi_rready                   => m_axi_rready,
      m_axi_rvalid                   => m_axi_rvalid,
      m_axi_rdata                    => m_axi_rdata,
      m_axi_rresp                    => m_axi_rresp,
      m_axi_rlast                    => m_axi_rlast,
      m_axi_awready                  => m_axi_awready,
      m_axi_awvalid                  => m_axi_awvalid,
      m_axi_awaddr                   => m_axi_awaddr,
      m_axi_awlen                    => m_axi_awlen,
      m_axi_awsize                   => m_axi_awsize,
      m_axi_awburst                  => m_axi_awburst,
      m_axi_awprot                   => m_axi_awprot,
      m_axi_awcache                  => m_axi_awcache,
      m_axi_wready                   => m_axi_wready,
      m_axi_wvalid                   => m_axi_wvalid,
      m_axi_wdata                    => m_axi_wdata,
      m_axi_wstrb                    => m_axi_wstrb,
      m_axi_wlast                    => m_axi_wlast,
      m_axi_bready                   => m_axi_bready,
      m_axi_bvalid                   => m_axi_bvalid,
      m_axi_bresp                    => m_axi_bresp,
      ip2bus_mstrd_req               => ipif_ip2bus_mstrd_req,
      ip2bus_mstwr_req               => ipif_ip2bus_mstwr_req,
      ip2bus_mst_addr                => ipif_ip2bus_mst_addr,
      ip2bus_mst_be                  => ipif_ip2bus_mst_be,
      ip2bus_mst_length              => ipif_ip2bus_mst_length,
      ip2bus_mst_type                => ipif_ip2bus_mst_type,
      ip2bus_mst_lock                => ipif_ip2bus_mst_lock,
      ip2bus_mst_reset               => ipif_ip2bus_mst_reset,
      bus2ip_mst_cmdack              => ipif_bus2ip_mst_cmdack,
      bus2ip_mst_cmplt               => ipif_bus2ip_mst_cmplt,
      bus2ip_mst_error               => ipif_bus2ip_mst_error,
      bus2ip_mst_rearbitrate         => ipif_bus2ip_mst_rearbitrate,
      bus2ip_mst_cmd_timeout         => ipif_bus2ip_mst_cmd_timeout,
      bus2ip_mstrd_d                 => ipif_bus2ip_mstrd_d,
      bus2ip_mstrd_rem               => ipif_bus2ip_mstrd_rem,
      bus2ip_mstrd_sof_n             => ipif_bus2ip_mstrd_sof_n,
      bus2ip_mstrd_eof_n             => ipif_bus2ip_mstrd_eof_n,
      bus2ip_mstrd_src_rdy_n         => ipif_bus2ip_mstrd_src_rdy_n,
      bus2ip_mstrd_src_dsc_n         => ipif_bus2ip_mstrd_src_dsc_n,
      ip2bus_mstrd_dst_rdy_n         => ipif_ip2bus_mstrd_dst_rdy_n,
      ip2bus_mstrd_dst_dsc_n         => ipif_ip2bus_mstrd_dst_dsc_n,
      ip2bus_mstwr_d                 => ipif_ip2bus_mstwr_d,
      ip2bus_mstwr_rem               => ipif_ip2bus_mstwr_rem,
      ip2bus_mstwr_src_rdy_n         => ipif_ip2bus_mstwr_src_rdy_n,
      ip2bus_mstwr_src_dsc_n         => ipif_ip2bus_mstwr_src_dsc_n,
      ip2bus_mstwr_sof_n             => ipif_ip2bus_mstwr_sof_n,
      ip2bus_mstwr_eof_n             => ipif_ip2bus_mstwr_eof_n,
      bus2ip_mstwr_dst_rdy_n         => ipif_bus2ip_mstwr_dst_rdy_n,
      bus2ip_mstwr_dst_dsc_n         => ipif_bus2ip_mstwr_dst_dsc_n
    );
  ------------------------------------------
  -- instantiate interrupt_control
  ------------------------------------------
  INTERRUPT_CONTROL_I : entity axi_sdi_controller_v1_00_a.interrupt_control
    generic map
    (
      C_NUM_CE                       => INTR_NUM_CE,
      C_NUM_IPIF_IRPT_SRC            => INTR_NUM_IPIF_IRPT_SRC,
      C_IP_INTR_MODE_ARRAY           => INTR_IP_INTR_MODE_ARRAY,
      C_INCLUDE_DEV_PENCODER         => INTR_INCLUDE_DEV_PENCODER,
      C_INCLUDE_DEV_ISC              => INTR_INCLUDE_DEV_ISC,
      C_IPIF_DWIDTH                  => IPIF_SLV_DWIDTH
    )
    port map
    (
      Bus2IP_Clk                     => ipif_Bus2IP_Clk,
      Bus2IP_Reset                   => not ipif_Bus2IP_Resetn,
      Bus2IP_Data                    => ipif_Bus2IP_Data,
      Bus2IP_BE                      => ipif_Bus2IP_BE,
      Interrupt_RdCE                 => ipif_Bus2IP_RdCE(INTR_CE_INDEX to INTR_CE_INDEX+INTR_NUM_CE-1),
      Interrupt_WrCE                 => ipif_Bus2IP_WrCE(INTR_CE_INDEX to INTR_CE_INDEX+INTR_NUM_CE-1),
      IPIF_Reg_Interrupts            => intr_IPIF_Reg_Interrupts,
      IPIF_Lvl_Interrupts            => intr_IPIF_Lvl_Interrupts,
      IP2Bus_IntrEvent               => user_IP2Bus_IntrEvent,
      Intr2Bus_DevIntr               => IP2INTC_Irpt,
      Intr2Bus_DBus                  => intr_IP2Bus_Data,
      Intr2Bus_WrAck                 => intr_IP2Bus_WrAck,
      Intr2Bus_RdAck                 => intr_IP2Bus_RdAck,
      Intr2Bus_Error                 => intr_IP2Bus_Error,
      Intr2Bus_Retry                 => open,
      Intr2Bus_ToutSup               => open
    );
   -- feed registered and level-pass-through interrupts into Device ISC if exists, otherwise ignored
  intr_IPIF_Reg_Interrupts(0) <= '0';
  intr_IPIF_Reg_Interrupts(1) <= '0';
  intr_IPIF_Lvl_Interrupts(0) <= '0';
  intr_IPIF_Lvl_Interrupts(1) <= '0';
  intr_IPIF_Lvl_Interrupts(2) <= '0';
  intr_IPIF_Lvl_Interrupts(3) <= '0';
  ------------------------------------------
 
  ------------------------------------------
  -- instantiate User Logic
  ------------------------------------------
  USER_LOGIC_I : entity axi_sdi_controller_v1_00_a.user_logic
    generic map
    (
      -- MAP USER GENERICS BELOW THIS LINE ---------------
      --USER generics mapped here
      C_DEBUG_DWIDTH              => C_DEBUG_DWIDTH,
      C_OPERATION_MODE            => C_OPERATION_MODE,	
      C_NUM_PU_CONFIG_REGS        => C_NUM_PU_CONFIG_REGS,	
      C_NUM_PU_STATUS_REGS        => C_NUM_PU_STATUS_REGS,
      C_FIFO_IMPLEMENTATION_TYPE  => C_FIFO_IMPLEMENTATION_TYPE,
      C_FIFO_DEPTH                => C_FIFO_DEPTH,	
      -- MAP USER GENERICS ABOVE THIS LINE ---------------

      C_MST_NATIVE_DATA_WIDTH        => USER_MST_NATIVE_DATA_WIDTH,
      C_MST_AWIDTH                   => USER_MST_AWIDTH,
      C_NUM_REG                      => CONTROLLER_NUM_REG,
      C_NUM_INTR                     => USER_NUM_INTR, 
      C_SLV_DWIDTH                   => USER_SLV_DWIDTH
    )
    port map
    (
      -- MAP USER PORTS BELOW THIS LINE ------------------
      --USER ports mapped here
      DEBUG_D_I => DEBUG_D_I,
      DEBUG_D_O => DEBUG_D_O,
      DEBUG_D_T => DEBUG_D_T,
	 
      stream_out_clk => stream_out_clk,
      stream_out_stop => stream_out_stop,
      stream_out_valid => stream_out_valid, 						
      stream_out_data => stream_out_data, 						
      stream_in_clk => stream_in_clk,
      stream_in_stop => stream_in_stop, 						
      stream_in_valid => stream_in_valid,						
      stream_in_data => stream_in_data,						
            debug=>debug,
      trig0=>trig0,
      pu_reset => pu_reset,
      pu_sof => pu_sof,
      pu_sof_ack => pu_sof_ack,
      pu_eof_in => pu_eof_in,
      pu_sof_in => pu_sof_in,
      pu_eof_ack => pu_eof_ack,
      request_frame => request_frame,
      request_frame_ack => request_frame_ack,

      pu_config_regs_vector => pu_config_regs,
      pu_status_regs_vector => pu_status_regs,
      -- MAP USER PORTS ABOVE THIS LINE ------------------

      Bus2IP_Clk                     => ipif_Bus2IP_Clk,
      Bus2IP_Resetn                  => ipif_Bus2IP_Resetn,
      Bus2IP_Data                    => ipif_Bus2IP_Data,
      Bus2IP_BE                      => ipif_Bus2IP_BE,
      Bus2IP_RdCE                    => user_Bus2IP_RdCE,
      Bus2IP_WrCE                    => user_Bus2IP_WrCE,
      IP2Bus_Data                    => user_IP2Bus_Data,
      IP2Bus_RdAck                   => user_IP2Bus_RdAck,
      IP2Bus_WrAck                   => user_IP2Bus_WrAck,
      IP2Bus_Error                   => user_IP2Bus_Error,
      ip2bus_mstrd_req               => ipif_ip2bus_mstrd_req,
      ip2bus_mstwr_req               => ipif_ip2bus_mstwr_req,
      ip2bus_mst_addr                => ipif_ip2bus_mst_addr,
      ip2bus_mst_be                  => ipif_ip2bus_mst_be,
      ip2bus_mst_length              => ipif_ip2bus_mst_length,
      ip2bus_mst_type                => ipif_ip2bus_mst_type,
      ip2bus_mst_lock                => ipif_ip2bus_mst_lock,
      ip2bus_mst_reset               => ipif_ip2bus_mst_reset,
      bus2ip_mst_cmdack              => ipif_bus2ip_mst_cmdack,
      bus2ip_mst_cmplt               => ipif_bus2ip_mst_cmplt,
      bus2ip_mst_error               => ipif_bus2ip_mst_error,
      bus2ip_mst_rearbitrate         => ipif_bus2ip_mst_rearbitrate,
      bus2ip_mst_cmd_timeout         => ipif_bus2ip_mst_cmd_timeout,
      bus2ip_mstrd_d                 => ipif_bus2ip_mstrd_d,
      bus2ip_mstrd_rem               => ipif_bus2ip_mstrd_rem,
      bus2ip_mstrd_sof_n             => ipif_bus2ip_mstrd_sof_n,
      bus2ip_mstrd_eof_n             => ipif_bus2ip_mstrd_eof_n,
      bus2ip_mstrd_src_rdy_n         => ipif_bus2ip_mstrd_src_rdy_n,
      bus2ip_mstrd_src_dsc_n         => ipif_bus2ip_mstrd_src_dsc_n,
      ip2bus_mstrd_dst_rdy_n         => ipif_ip2bus_mstrd_dst_rdy_n,
      ip2bus_mstrd_dst_dsc_n         => ipif_ip2bus_mstrd_dst_dsc_n,
      ip2bus_mstwr_d                 => ipif_ip2bus_mstwr_d,
      ip2bus_mstwr_rem               => ipif_ip2bus_mstwr_rem,
      ip2bus_mstwr_src_rdy_n         => ipif_ip2bus_mstwr_src_rdy_n,
      ip2bus_mstwr_src_dsc_n         => ipif_ip2bus_mstwr_src_dsc_n,
      ip2bus_mstwr_sof_n             => ipif_ip2bus_mstwr_sof_n,
      ip2bus_mstwr_eof_n             => ipif_ip2bus_mstwr_eof_n,
      bus2ip_mstwr_dst_rdy_n         => ipif_bus2ip_mstwr_dst_rdy_n,
      bus2ip_mstwr_dst_dsc_n         => ipif_bus2ip_mstwr_dst_dsc_n,
      IP2Bus_IntrEvent               => user_IP2Bus_IntrEvent
    );

  ------------------------------------------
  -- connect internal signals
  ------------------------------------------
------------------------------------------
  -- connect internal signals
  ------------------------------------------
  IP2BUS_DATA_MUX_PROC : process( ipif_Bus2IP_CS, user_IP2Bus_Data, intr_IP2Bus_Data ) is
  begin

    case ipif_Bus2IP_CS is
      when "10" => ipif_IP2Bus_Data <= user_IP2Bus_Data;
      when "01" => ipif_IP2Bus_Data <= intr_IP2Bus_Data;
      when others => ipif_IP2Bus_Data <= (others => '0');
    end case;

  end process IP2BUS_DATA_MUX_PROC;

  ipif_IP2Bus_WrAck <= user_IP2Bus_WrAck or intr_IP2Bus_WrAck;
  ipif_IP2Bus_RdAck <= user_IP2Bus_RdAck or intr_IP2Bus_RdAck;
  ipif_IP2Bus_Error <= user_IP2Bus_Error or intr_IP2Bus_Error;

  user_Bus2IP_RdCE <= ipif_Bus2IP_RdCE(CONTROLLER_CE_INDEX to CONTROLLER_CE_INDEX+CONTROLLER_NUM_REG-1);
  user_Bus2IP_WrCE <= ipif_Bus2IP_WrCE(CONTROLLER_CE_INDEX to CONTROLLER_CE_INDEX+CONTROLLER_NUM_REG-1);

end IMP;
//////////////////////////////////////////////////////////////////////////////
// Filename:          drivers/plb_sdi_controller_v1_00_a/src/plb_sdi_controller.c
// Version:           1.00.a
// Description:       plb_sdi_controller Driver Source File
// Date:              Thu Mar 26 21:03:20 2009 
// By:                University of Potsdam - Department of Computer Sceince 
//                    Felix & akzare
//////////////////////////////////////////////////////////////////////////////


/***************************** Include Files *******************************/
#include <linux/types.h>	

#include "plb_sdi_controller.h"

/************************** Function Definitions ***************************/
volatile int IM_PLB_Data_Modul_Status_Reader_Done_IRQ_Flg = 0;
volatile int IM_PLB_Data_Modul_Status_Writer_Done_IRQ_Flg = 0;
volatile int IM_PLB_Data_Modul_Status_Reader_Flushed_IRQ_Flg = 0;
volatile int IM_PLB_Data_Modul_Status_Reader_Trigger_Addr_IRQ_Flg = 0;
volatile int IM_PLB_Data_Modul_Status_Writer_Trigger_Addr_IRQ_Flg = 0;
/*
*********************************************************************************************************
*  data types
*********************************************************************************************************
*/
static volatile struct hw_regs_t *hw_regs = NULL;  // FIXME: delete

inline int PLB_SDI_CONTROLLER_Check_ReaderDone_Flg() {
  return IM_PLB_Data_Modul_Status_Reader_Done_IRQ_Flg;
}

inline void PLB_SDI_CONTROLLER_Reset_ReaderDone_Flg() {
  IM_PLB_Data_Modul_Status_Reader_Done_IRQ_Flg = 0;
}

inline int PLB_SDI_CONTROLLER_Check_WriterDone_Flg() {
  return IM_PLB_Data_Modul_Status_Writer_Done_IRQ_Flg;
}

inline void PLB_SDI_CONTROLLER_Reset_WriterDone_Flg() {
  IM_PLB_Data_Modul_Status_Writer_Done_IRQ_Flg = 0;
}

inline int PLB_SDI_CONTROLLER_Check_ReaderFlushed_Flg() {
  return IM_PLB_Data_Modul_Status_Reader_Flushed_IRQ_Flg;
}

inline void PLB_SDI_CONTROLLER_Reset_ReaderFlushed_Flg() {
  IM_PLB_Data_Modul_Status_Reader_Flushed_IRQ_Flg = 0;
}

inline int PLB_SDI_CONTROLLER_Check_ReaderTriggerAdrr_Flg() {
  return IM_PLB_Data_Modul_Status_Reader_Trigger_Addr_IRQ_Flg;
}

inline void PLB_SDI_CONTROLLER_Reset_Reader_TriggerAdrr_Flg() {
  IM_PLB_Data_Modul_Status_Reader_Trigger_Addr_IRQ_Flg = 0;
}

inline int PLB_SDI_CONTROLLER_Check_WriterTriggerAdrr_Flg() {
  return IM_PLB_Data_Modul_Status_Writer_Trigger_Addr_IRQ_Flg;
}

inline void PLB_SDI_CONTROLLER_Reset_Writer_TriggerAdrr_Flg() {
  IM_PLB_Data_Modul_Status_Writer_Trigger_Addr_IRQ_Flg = 0;
}

inline void PLB_SDI_CONTROLLER_Set_BaseAddress(uint32_t BaseAddress) {
  hw_regs = (struct hw_regs_t *)(BaseAddress);
}

inline int PLB_SDI_CONTROLLER_Writer_Done() {
  return hw_regs->control_status_reg & PLB_SDI_CONTROLLER_WRITER_DONE_MASK;
}

inline int PLB_SDI_CONTROLLER_Reader_Done() {
  return hw_regs->control_status_reg & PLB_SDI_CONTROLLER_READER_DONE_MASK;
}

inline int PLB_SDI_CONTROLLER_Reader_Done_ReaderFIFO_Empty() {
  return hw_regs->control_status_reg & PLB_SDI_CONTROLLER_RD_DONE_RD_FIFOEMPTY_MASK;
}

inline int PLB_SDI_CONTROLLER_Reader_End_of_Block_Done() {
  return hw_regs->control_status_reg & PLB_SDI_CONTROLLER_READER_END_OF_BLOCK_MASK;
}

inline int PLB_SDI_CONTROLLER_Writer_End_of_Block_Done() {
  return hw_regs->control_status_reg & PLB_SDI_CONTROLLER_WRITER_END_OF_BLOCK_MASK;
}

inline int PLB_SDI_CONTROLLER_Is_MST_Error() {
  return hw_regs->control_status_reg & PLB_SDI_CONTROLLER_ERROR_MST_ERROR_MASK;
}

inline int PLB_SDI_CONTROLLER_Is_MST_TIMEOUT_Error() {
  return hw_regs->control_status_reg & PLB_SDI_CONTROLLER_ERROR_MST_TIMEOUT_MASK;
}

inline int PLB_SDI_CONTROLLER_Is_Reader_StartAddr_Error() {
  return hw_regs->control_status_reg & PLB_SDI_CONTROLLER_ERROR_READER_START_ADDR_MASK;
}

inline int PLB_SDI_CONTROLLER_Is_Reader_EndAddr_Error() {
  return hw_regs->control_status_reg & PLB_SDI_CONTROLLER_ERROR_READER_END_ADDR_MASK;
}

inline int PLB_SDI_CONTROLLER_Is_Writer_StartAddr_Error() {
  return hw_regs->control_status_reg & PLB_SDI_CONTROLLER_ERROR_WRITER_START_ADDR_MASK;
}

inline int PLB_SDI_CONTROLLER_Is_Writer_EndAddr_Error() {
  return hw_regs->control_status_reg & PLB_SDI_CONTROLLER_ERROR_WRITER_END_ADDR_MASK;
}

inline uint32_t PLB_SDI_CONTROLLER_Get_HDWR_Ver() {
  return hw_regs->hw_version;
}

inline uint32_t PLB_SDI_CONTROLLER_Get_Src_Start_Addr() {
  return hw_regs->src_start_addr;
}

inline uint32_t PLB_SDI_CONTROLLER_Get_Src_End_Addr() {
  return hw_regs->src_end_addr;
}

inline uint32_t PLB_SDI_CONTROLLER_Get_Dst_Start_Addr() {
  return hw_regs->dest_start_addr;
}

inline uint32_t PLB_SDI_CONTROLLER_Get_Dst_End_Addr() {
  return hw_regs->dest_end_addr;
}

inline uint32_t PLB_SDI_CONTROLLER_Get_Tick_Cntr() {
  return hw_regs->tick_counter;
}

inline void PLB_SDI_CONTROLLER_Set_Src_Start_Addr(uint32_t SrcAddress_Reader) {
  hw_regs->src_start_addr = SrcAddress_Reader;
}

inline void PLB_SDI_CONTROLLER_Set_Src_End_Addr(uint32_t SrcAddress_Reader) {
  hw_regs->src_end_addr = SrcAddress_Reader;
}
  
inline void PLB_SDI_CONTROLLER_Set_Dst_Start_Addr(uint32_t DstAddress_Reader) {
  hw_regs->dest_start_addr = DstAddress_Reader;
}

inline void PLB_SDI_CONTROLLER_Set_Dst_End_Addr(uint32_t DstAddress_Reader) {
  hw_regs->dest_end_addr = DstAddress_Reader;
}

inline void PLB_SDI_CONTROLLER_Reset_Reader_FIFO() {
  hw_regs->control_status_reg = PLB_SDI_CONTROLLER_CLR_READER_FIFO_MASK;
}

inline void PLB_SDI_CONTROLLER_Reset_Writer_FIFO() {
  hw_regs->control_status_reg = PLB_SDI_CONTROLLER_CLR_WRITER_FIFO_MASK;
}

inline void PLB_SDI_CONTROLLER_Master_StartReaderWriter_SnapShot() {
  hw_regs->control_status_reg = PLB_SDI_CONTROLLER_STRT_READER_SNGLE_MASK | PLB_SDI_CONTROLLER_STRT_WRITER_SNGLE_MASK | 
    PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER |
    PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ;
}

inline void PLB_SDI_CONTROLLER_Master_StartWriter_SnapShot() {
  hw_regs->control_status_reg = PLB_SDI_CONTROLLER_STRT_WRITER_SNGLE_MASK |
    PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER |
    PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ;
}
        
inline void PLB_SDI_CONTROLLER_Master_StartReader_SnapShot() {
  hw_regs->control_status_reg = PLB_SDI_CONTROLLER_STRT_READER_SNGLE_MASK |
    PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER |
    PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ;
}

inline void PLB_SDI_CONTROLLER_Master_StartReaderWriter_Continous() {
  hw_regs->control_status_reg = PLB_SDI_CONTROLLER_STRT_READER_CONTINUOS_MASK | PLB_SDI_CONTROLLER_STRT_WRITER_CONTINUOS_MASK | 
    PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER |
    PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ;
}

inline void PLB_SDI_CONTROLLER_Master_StartWriter_Continous() {
  hw_regs->control_status_reg = PLB_SDI_CONTROLLER_STRT_WRITER_CONTINUOS_MASK |
    PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER |
    PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ;
}
        
inline void PLB_SDI_CONTROLLER_Master_StartReader_Continous() {
  hw_regs->control_status_reg = PLB_SDI_CONTROLLER_STRT_READER_CONTINUOS_MASK |
    PLB_SDI_CONTROLLER_CLR_STATUS_REG_MASK | PLB_SDI_CONTROLLER_START_TICK_COUNTER | PLB_SDI_CONTROLLER_CLEAR_TICK_COUNTER |
    PLB_SDI_CONTROLLER_STOP_TICK_COUNTER_ON_WR_IRQ;
}
    
/**
 *
 * Enable all possible interrupts from PLB_SDI_CONTROLLER device.
 *
 * @param   baseaddr_p is the base address of the PLB_SDI_CONTROLLER device.
 *
 * @return  None.
 *
 * @note    None.
 *
 */
void PLB_SDI_CONTROLLER_EnableInterrupt(void * baseaddr_p)
{
  uint32_t baseaddr;
  baseaddr = (uint32_t) baseaddr_p;

  /*
   * Enable all interrupt source from user logic.
   */
  PLB_SDI_CONTROLLER_mWriteReg(baseaddr, PLB_SDI_CONTROLLER_INTR_IPIER_OFFSET, 0xFFFFFFFF);

  /*
   * Enable all possible interrupt sources from device.
   */
  PLB_SDI_CONTROLLER_mWriteReg(baseaddr, PLB_SDI_CONTROLLER_INTR_DIER_OFFSET,
    INTR_TERR_MASK
    | INTR_DPTO_MASK
    | INTR_IPIR_MASK
    );

  /*
   * Set global interrupt enable.
   */
  PLB_SDI_CONTROLLER_mWriteReg(baseaddr, PLB_SDI_CONTROLLER_INTR_DGIER_OFFSET, INTR_GIE_MASK);
}

void PLB_SDI_CONTROLLER_DisableInterrupt(void * baseaddr_p)
{
  uint32_t baseaddr;
  baseaddr = (uint32_t) baseaddr_p;

  /*
   * Enable all interrupt source from user logic.
   */
  PLB_SDI_CONTROLLER_mWriteReg(baseaddr, PLB_SDI_CONTROLLER_INTR_IPIER_OFFSET, 0);

  /*
   * Enable all possible interrupt sources from device.
   */
  PLB_SDI_CONTROLLER_mWriteReg(baseaddr, PLB_SDI_CONTROLLER_INTR_DIER_OFFSET, 0);

  /*
   * Set global interrupt enable.
   */
  PLB_SDI_CONTROLLER_mWriteReg(baseaddr, PLB_SDI_CONTROLLER_INTR_DGIER_OFFSET, 0);
}

/**
 *
 * Example interrupt controller handler for PLB_SDI_CONTROLLER device.
 * This is to show example of how to toggle write back ISR to clear interrupts.
 *
 * @param   baseaddr_p is the base address of the PLB_SDI_CONTROLLER device.
 *
 * @return  None.
 *
 * @note    None.
 *
 */
void PLB_SDI_CONTROLLER_Intr_DefaultHandler(void * baseaddr_p)
{
  uint32_t baseaddr;
  uint32_t IntrStatus;
  uint32_t IpStatus;
  baseaddr = (uint32_t) baseaddr_p;

  /*
   * Get status from Device Interrupt Status Register.
   */
  IntrStatus = PLB_SDI_CONTROLLER_mReadReg(baseaddr, PLB_SDI_CONTROLLER_INTR_DISR_OFFSET);

  /*
   * Verify the source of the interrupt is the user logic and clear the interrupt
   * source by toggle write baca to the IP ISR register.
   */
  if ( (IntrStatus & INTR_IPIR_MASK) == INTR_IPIR_MASK )
  {
    IpStatus = PLB_SDI_CONTROLLER_mReadReg(baseaddr, PLB_SDI_CONTROLLER_INTR_IPISR_OFFSET);
    PLB_SDI_CONTROLLER_mWriteReg(baseaddr, PLB_SDI_CONTROLLER_INTR_IPISR_OFFSET, IpStatus);
  }

}

void PLB_SDI_CONTROLLER_Intr_Handler(void * baseaddr_p)
{
	uint32_t baseaddr;
	uint32_t IntrStatus;
	uint32_t IpStatus;

	baseaddr = (uint32_t) baseaddr_p;

	/*
	* Get status from Device Interrupt Status Register.
	*/
	IntrStatus = PLB_SDI_CONTROLLER_mReadReg(baseaddr, PLB_SDI_CONTROLLER_INTR_DISR_OFFSET);

	/*
	* Verify the source of the interrupt is the user logic and clear the interrupt
	* source by toggle write baca to the IP ISR register.
	*/
	if ( (IntrStatus & INTR_IPIR_MASK) == INTR_IPIR_MASK )
	{
		
		IpStatus = PLB_SDI_CONTROLLER_mReadReg(baseaddr, PLB_SDI_CONTROLLER_INTR_IPISR_OFFSET);
		if (IpStatus & PLB_SDI_CONTROLLER_IRQ_READER_DONE_MASK) {
                	IM_PLB_Data_Modul_Status_Reader_Done_IRQ_Flg = 1;
		}
		if (IpStatus & PLB_SDI_CONTROLLER_IRQ_READER_FLUSHED_MASK) {
			IM_PLB_Data_Modul_Status_Reader_Flushed_IRQ_Flg = 1;
		}
		if (IpStatus & PLB_SDI_CONTROLLER_IRQ_READER_TRIGGER_ADDR_MASK) {
			IM_PLB_Data_Modul_Status_Reader_Trigger_Addr_IRQ_Flg = 1;
		}
		if (IpStatus & PLB_SDI_CONTROLLER_IRQ_WRITER_DONE_MASK) {
			IM_PLB_Data_Modul_Status_Writer_Done_IRQ_Flg = 1;
		}
		if (IpStatus & PLB_SDI_CONTROLLER_IRQ_WRITER_TRIGGER_ADDR_MASK) {
			IM_PLB_Data_Modul_Status_Writer_Trigger_Addr_IRQ_Flg = 1;
		}
		PLB_SDI_CONTROLLER_mWriteReg(baseaddr, PLB_SDI_CONTROLLER_INTR_IPISR_OFFSET, IpStatus);
		
	}
}


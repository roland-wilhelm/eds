/*
 * $RCSfile: init_appl.c,v $
 * $Revision: 1.11 $
 * $Date: 2006/04/06 12:19:41 $
 *
 * Abstract:
 *   
 *
 * Copyright Vodafone Global Resarch and Devellopment - 2007
 * Ronald Baker
 * All Rights Reserved
 *
 * Project:  MAID
 *
 */


/* MAC Configuration Register 1 */
#define MAC1_REC_EN         0x00000001  /* Receive Enable                    */
#define MAC1_PASS_ALL       0x00000002  /* Pass All Receive Frames           */
#define MAC1_RX_FLOWC       0x00000004  /* RX Flow Control                   */
#define MAC1_TX_FLOWC       0x00000008  /* TX Flow Control                   */
#define MAC1_LOOPB          0x00000010  /* Loop Back Mode                    */
#define MAC1_RES_TX         0x00000100  /* Reset TX Logic                    */
#define MAC1_RES_MCS_TX     0x00000200  /* Reset MAC TX Control Sublayer     */
#define MAC1_RES_RX         0x00000400  /* Reset RX Logic                    */
#define MAC1_RES_MCS_RX     0x00000800  /* Reset MAC RX Control Sublayer     */
#define MAC1_SIM_RES        0x00004000  /* Simulation Reset                  */
#define MAC1_SOFT_RES       0x00008000  /* Soft Reset MAC                    */
/* Command Register */
#define CR_RX_EN            0x00000001  /* Enable Receive                    */
#define CR_TX_EN            0x00000002  /* Enable Transmit                   */
#define CR_REG_RES          0x00000008  /* Reset Host Registers              */
#define CR_TX_RES           0x00000010  /* Reset Transmit Datapath           */
#define CR_RX_RES           0x00000020  /* Reset Receive Datapath            */
#define CR_PASS_RUNT_FRM    0x00000040  /* Pass Runt Frames                  */
#define CR_PASS_RX_FILT     0x00000080  /* Pass RX Filter                    */
#define CR_TX_FLOW_CTRL     0x00000100  /* TX Flow Control                   */
#define CR_RMII             0x00000200  /* Reduced MII Interface             */
#define CR_FULL_DUP         0x00000400  /* Full Duplex                       */


#include "lpc23xx.h"

void init_eth(void)
{
	
   	int i;

	  /* Enable P1 Ethernet Pins. */
  	  PINSEL2 = 0x55555555;
	  PINSEL3 = (PINSEL3 & ~0x0000000F) | 0x00000005;

	  /* Power Up the EMAC controller. */
	  PCONP |= 0x40000000;
	  
      for ( i=10000; i; i--);   
      
	   /* Reset all EMAC internal modules. */
	  MAC_MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX |
	             MAC1_SIM_RES | MAC1_SOFT_RES;
//	  MAC_COMMAND = CR_REG_RES | CR_TX_RES | CR_RX_RES;

	  for ( i=10000; i; i--);
	  
	
	
	  
	
  
  }

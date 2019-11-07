#include "hardfault.h"

// hard fault handler in C,
// with stack frame location as input parameter
// called from HardFault_Handler in file xxx.s
/*
void HardFault_Handler(void)
{
asm volatile(  
"movs r0, #4\t\n"  
"mov  r1, lr\t\n"  
"tst  r0, r1\t\n" 
"beq 1f\t\n"  
"mrs r0, psp\t\n"  
"ldr r1,=hard_fault_handl_c\t\n"  
"bx r1\t\n"  
"1:mrs r0,msp\t\n"  
"ldr r1,=hard_fault_handl_c\t\n"  
:
:
: "r0" 
);  
}
*/
void __attribute__((optimize("O0"))) hard_fault_handl_c (unsigned int * hardfault_args){
  
  unsigned int stacked_r0;
  unsigned int stacked_r1;
  unsigned int stacked_r2;
  unsigned int stacked_r3;
  unsigned int stacked_r12;
  unsigned int stacked_lr;
  unsigned int stacked_pc;
  unsigned int stacked_psr;
 
  stacked_r0 = ((unsigned long) hardfault_args[0]);
  stacked_r1 = ((unsigned long) hardfault_args[1]);
  stacked_r2 = ((unsigned long) hardfault_args[2]);
  stacked_r3 = ((unsigned long) hardfault_args[3]);
 
  stacked_r12 = ((unsigned long) hardfault_args[4]);
  stacked_lr = ((unsigned long) hardfault_args[5]);
  stacked_pc = ((unsigned long) hardfault_args[6]);
  stacked_psr = ((unsigned long) hardfault_args[7]);
 
  printf ("[Hard fault handler - all numbers in hex]\n");
  printf ("R0 = %x\n", stacked_r0);
  printf ("R1 = %x\n", stacked_r1);
  printf ("R2 = %x\n", stacked_r2);
  printf ("R3 = %x\n", stacked_r3);
  printf ("R12 = %x\n", stacked_r12);
  printf ("LR [R14] = %x  subroutine call return address\n", stacked_lr);
  printf ("PC [R15] = %x  program counter\n", stacked_pc);
  printf ("PSR = %x\n", stacked_psr);
  printf ("BFAR = %lx\n", (*((volatile unsigned long *)(0xE000ED38))));
  printf ("CFSR = %lx\n", (*((volatile unsigned long *)(0xE000ED28))));
  printf ("HFSR = %lx\n", (*((volatile unsigned long *)(0xE000ED2C))));
  printf ("DFSR = %lx\n", (*((volatile unsigned long *)(0xE000ED30))));
  printf ("AFSR = %lx\n", (*((volatile unsigned long *)(0xE000ED3C))));
  printf ("SCB_SHCSR = %lx\n", SCB->SHCSR);
  
  while (1);
}


bool cpu_check_address(volatile const char *address)
{
    /* Cortex-M3, Cortex-M4, Cortex-M4F, Cortex-M7 are supported */
    static const uint32_t BFARVALID_MASK = (0x80 << SCB_CFSR_BUSFAULTSR_Pos);
    bool is_valid = true;

    /* Clear BFARVALID flag by writing 1 to it */
    SCB->CFSR |= BFARVALID_MASK;

    /* Ignore BusFault by enabling BFHFNMIGN and disabling interrupts */
    uint32_t mask = __get_FAULTMASK();
    __disable_fault_irq();
    SCB->CCR |= SCB_CCR_BFHFNMIGN_Msk;

    /* probe address in question */
    *address;

    /* Check BFARVALID flag */
    if ((SCB->CFSR & BFARVALID_MASK) != 0)
    {
        /* Bus Fault occured reading the address */
        is_valid = false;
    }

    /* Reenable BusFault by clearing  BFHFNMIGN */
    SCB->CCR &= ~SCB_CCR_BFHFNMIGN_Msk;
    __set_FAULTMASK(mask);

    return is_valid;
}

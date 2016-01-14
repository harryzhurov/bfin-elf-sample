
#include <sys/platform.h>

//#include <cplb.h>
#include <sys/anomaly_macros_rtl.h>
//#include <defBF533.h>
//#include <def_LPBlackfin.h>

//------------------------------------------------------------------------------
//
//   Data and code global and extern labels
//
//

/*
    .section data1;                      
    .align 4;                            
                                         
    .extern __stack_end;                 
                                         
    .extern __install_default_handlers;  
    .extern __unknown_exception_occurred;
                                         
    .extern _HardwareInit;               
                                         
                                         
    .section ctor;                       
    .align 4;                            
    ___ctor_table:                       
    .int;                                
    .global ___ctor_table;               
    .type   ___ctor_table, STT_OBJECT;   
    .extern ___ctorloop;                 
*/
//------------------------------------------------------------------------------
//
//   The code
//
/*
    //.section program;         
    .text;                      
    .align 2;                   
    .global __start;            
    .type __start, STT_FUNC;    
    //.global .start.end;       
    //.type .start.end,STT_FUNC;
    .extern _main;              
    .type _main, STT_FUNC;      
*/
    //.section .startup;
    .text
    .align 2;
    .global __start;
    .type __start, STT_FUNC;
__start:
    //-------------------------------------------------------------------------
    // Avoid BF532 Anomaly #42 and turn on cycle conter
    r1     = 0x32; // default state + cycle counter enable
    syscfg = r1;

#if WA_05000229
   // Avoid Anomaly 05-00-0229: DMA5_CONFIG and SPI_CTL not cleared on reset.
   R1 = 0x400;
#if defined(__ADSPBF538__) || defined(__ADSPBF539__)
   P0.L = SPI0_CTL & 0xFFFF;
   P0.H = SPI0_CTL >> 16;
   W[P0] = R1.L;
#else
   P0.L = SPI_CTL & 0xFFFF;
   P0.H = SPI_CTL >> 16;
   W[P0] = R1.L;
#endif
   P0.L = DMA5_CONFIG & 0xFFFF;
   P0.H = DMA5_CONFIG >> 16;
   R1 = 0;
   W[P0] = R1.L;
#endif

    //-------------------------------------------------------------------------
    //
    // Set loop counters to zero, to make sure that hw loops are disabled 
    //                      and
    // Set DAG Length register to zero to avoid wrapping around with I-regs using
    //
    r0  = 0;
    lc0 = r0;
    lc1 = r0;
    l0  = r0;
    l1  = r0;
    l2  = r0;
    l3  = r0;
    //-------------------------------------------------------------------------
    //
    // Initialize the Event Vector table.
    //
    p0.h = EVT0 >> 16;
    p0.l = EVT0 & 0xffff;
    //-------------------------------------------------------------------------
    //
    // Install __unknown_exception_occurred in EVT so that 
    // there is defined behaviour.
    //
    p0 += 2*4;      // Skip Emulation and Reset
    p1 = 13;
    r0.l = __unknown_exception_occurred;
    r0.h = __unknown_exception_occurred;
    lsetup (load_def_handler, load_def_handler) lc0 = p1;
load_def_handler:   
    [p0++] = r0;
    //-------------------------------------------------------------------------
    //
    // Switch to supervisor mode at the lowest priority
    // 
    p1.h = LPSM;
    p1.l = LPSM;
    [p0] = p1;

    // Initialise the stack.
    sp.l=__stack_end;
    sp.h=__stack_end;

    // We're still in supervisor mode at the moment, so the FP
    // needs to point to the supervisor stack.
    fp = sp;

    // Move the processor into user mode.
    p0.l = WaitForContinue;
    p0.h = WaitForContinue;
    reti = p0;
    r0.l = 0x8000; // IVG 15 mask
    sti r0;
    raise 15;

WaitForContinue:
    rti;
                                                  
    //-------------------------------------------------------------------------
    //
    // The Lowest Priority Supervisor Mode                 
    //
LPSM:    
    //-------------------------------------------------------------------------
    //
    // Zero fill .bss section
    // 
    r0.l = __bss_start;
    r0.h = __bss_start;
    r1.l = __bss_end;
    r1.h = __bss_end;
    r1   = r1 - r0;
    p1   = r1;
    p0   = r0;
    r0   = 0;
    
    lsetup (zerofill_bss, zerofill_bss) lc0 = p1;
zerofill_bss:    
    B[p0++] = r0;

    [--sp] = reti;  // re-enables the interrupt system
    sp += 4;        // increment on one 32-bit word
    sp += -12;      // And leave space for incoming "parameters"

    r0 = 0;         // load up R0 and R1 and call _main()
    r1 = r0;
    call.x ___libc_init_array;

    sp += 12;       // Restore space for incoming parameters
    jump.x _main;


//------------------------------------------------------------------------------
//
//
//
    .align 2
    .type __unknown_exception_occurred, STT_FUNC;
__unknown_exception_occurred:
    // This function is invoked by the default exception
    // handler, if it does not recognise the kind of
    // exception that has occurred. In other words, the
    // default handler only handles some of the system's
    // exception types, and it does not expect any others
    // to occur. If your application is going to be using
    // other kinds of exceptions, you must replace the
    // default handler with your own, that handles all the
    // exceptions you will use.
    //
    // Since there's nothing we can do, we just loop here
    // at what we hope is a suitably informative label.
    IDLE;
    CSYNC;
    JUMP __unknown_exception_occurred;
    RTS;
    .size __unknown_exception_occurred, .-__unknown_exception_occurred
//------------------------------------------------------------------------------

#if defined(__ADSPLPBLACKFIN__)
#if SET_CLOCK_SPEED == 1

/*
** CLKIN == 27MHz on the EZ-Kits.
** D==0 means CLKIN is passed to PLL without dividing.
** MSEL==0x16 means VCO==27*0x16 == 594MHz
** CSEL==0 means CCLK==VCO == 594MHz
** SSEL==5 means SCLK==VCO/5 == 118MHz
*/

// int pll_set_system_clocks(int csel, int ssel)
// returns 0 for success, -1 for error.

    .align 2
    .type __pll_set_system_clocks, STT_FUNC;
__pll_set_system_clocks:
    P0.H = PLL_DIV >> 16;
    P0.L = PLL_DIV & 0xFFFF;
    R2 = W[P0] (Z);

    // Plant CSEL and SSEL
    R0 <<= 16;
    R0.L = (4 << 8) | 2;    // 2 bits, at posn 4
    R1 <<= 16;
    R1.L = 4;       // 4 bits, at posn 0
    R2 = DEPOSIT(R2, R0);

#if defined(__WORKAROUND_DREG_COMP_LATENCY)
        // Work around anomaly 05-00-0209 which affects the DEPOSIT
        // instruction (and the EXTRACT, SIGNBITS, and EXPADJ instructions)
        // if the previous instruction created any of its operands
        NOP;
#endif

    R2 = DEPOSIT(R2, R1);

    W[P0] = R2;
    SSYNC;
    RTS;
    .size __pll_set_system_clocks, .-__pll_set_system_clocks

// int pll_set_system_vco(int msel, int df, lockcnt)
    .align 2
    .type __pll_set_system_vco, STT_FUNC;
__pll_set_system_vco:
    P0.H = PLL_CTL >> 16;
    P0.L = PLL_CTL & 0xFFFF;
    R3 = W[P0] (Z);
    P2 = R3;        // Save copy
    R3 >>= 1;       // Drop old DF
        R1 = ROT R1 BY -1;      // Move DF into CC
    R3 = ROT R3 BY 1;   // and into ctl space.
    R0 <<= 16;      // Set up pattern reg
    R0.L = (9<<8) | 6;  // (6 bits at posn 9)
        R1 = P2;                // Get the old version
    R3 = DEPOSIT(R3, R0);
    CC = R1 == R3;      // and if we haven't changed
    IF CC JUMP L$done;  // Anything, return

    CC = R2 == 0;       // Use default lockcount if
    IF CC JUMP L$wakeup;    // user one is zero.
    P2.H = PLL_LOCKCNT >> 16;
    P2.L = PLL_LOCKCNT & 0xFFFF;
    W[P2] = R2;     // Set the lock counter
L$wakeup:
    P2.H = SIC_IWR >> 16;
    P2.L = SIC_IWR & 0xFFFF;
    R2 = [P2];
    BITSET(R2, 0);      // enable PLL Wakeup
    [P2] = R2;

    W[P0] = R3;     // Update PLL_CTL
    SSYNC;

    CLI R2;         // Avoid unnecessary interrupts
    IDLE;           // Wait until PLL has locked
    STI R2;         // Restore interrupts.

L$done:
    RTS;
    .size __pll_set_system_vco, .-__pll_set_system_vco
#endif
#endif /* ADSPLPBLACKFIN */
//------------------------------------------------------------------------------



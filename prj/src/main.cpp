//******************************************************************************
//*
//*     ADSP-BF533/bfin-elf standalone sample program
//
//*     Copyright (c) 2015-2016, Harry E. Zhurov
//*
//*
//******************************************************************************

#include <stdint.h>
#include "device_def.h"
#include "macro.h"

//---------------------------------------------------------------------------
//
//      Types
//

class TSlon
{
public:
    TSlon() : a(0), b(0) { }
    TSlon(int x) : a(x), b(5) { }
    
    int get_a() const { return a; }
    int get_b() const { return b; }
    
    void set_a(int x) { a = x; }
    void set_b(int x) { b = a + x; }
    
    
private:
    int a;
    int b;
};


int f(int);
int g(int);

volatile uint16_t Array[] __attribute__ ((section(".sdram"))) = { 1, 2, 3, 4, 5, 6, 7, 8 };


//---------------------------------------------------------------------------
//
//      Objects
//
int a = 1;
int b;
volatile int c;
volatile int d;

TSlon Slon(2);

float          af;
float          bf;
volatile float cf;

//---------------------------------------------------------------------------

EX_INTERRUPT_HANDLER(timer0_isr) __attribute__ ((interrupt_handler));

//---------------------------------------------------------------------------
int main()
{     
    d = ({ b; a; });
    
    d = __builtin_cli();
    c = read_cycles_reg();
    
    if(af > bf)
    {
        cf = cf / af + af * bf;
    }
    //----------------------------------------------------------------------
    //
    //    Set Pcocessor Core clock to 200 MHz, peripheral clock - to 100 MHz
    //    Set Processor Core voltage at 0.85 v
    //
    pll_set_system_vco(8, 0, 0x300);
    pll_set_system_clocks(0, 2);
    ssync();
    
    /////////////////////////////////////////////////////////////////////
    //
    //    System resources setup
    // 
    //-------------------------------------------------------------------
    //
    //  System Timer setup and start
    //
    MMR32(TCNTL)   = 1;        // turn on the timer
    MMR32(TSCALE)  = 0;        //
    MMR32(TPERIOD) = 200000;   // 5ns * 200 000 = 1 ms
    MMR32(TCNTL)   = 0x07;     // run timer 
    //-------------------------------------------------------------------
    //
    //    Register System Interrupt Handlers
    //
    //
 //   register_handler_ex(ik_timer, system_timer_isr, 1);
    //-------------------------------------------------------------------
    /////////////////////////////////////////////////////////////////////


    MMR16(FIO_DIR) |= (1 << 8) + (1 << 9);

    register_handler_ex(ik_ivg11, timer0_isr, 1);
    MMR32(SIC_IMASK)     = (1 << 16);             // enable Timer0 interrupt
    MMR16(TIMER0_CONFIG) = PWM_OUT  
                         + IRQ_ENA 
                         + PERIOD_CNT 
                         + OUT_DIS; 

    MMR32(TIMER0_PERIOD) = 10011;
    MMR32(TIMER0_WIDTH)  = 5000;
    MMR16(TIMER_ENABLE)  = TIMEN0;                 // timer0 enable

    for(;;)
    {
        ++a;
        if( a & 0x1ul )
        {
            c = a - f(b++);
            Slon.set_a(c);
        }
        else
        {
            d = Slon.get_b();
        }
    }
    return Slon.get_a();  
}
//---------------------------------------------------------------------------
EX_INTERRUPT_HANDLER(timer0_isr)
{
    MMR16(TIMER_STATUS) = TIMIL0;

    //MMR16(FIO_FLAG_S) = (1 << 9);
    MMR16(FIO_FLAG_T) = (1 << 9);
}
//---------------------------------------------------------------------------
int f(int x)
{
    return x*3; 
}
//---------------------------------------------------------------------------
int g(int x)
{
     return x - 2;
}
//---------------------------------------------------------------------------
extern "C" void _init() 
{
}
//---------------------------------------------------------------------------




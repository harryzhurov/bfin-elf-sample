//******************************************************************************
//*
//*     FULLNAME:  Single-Chip Microcontroller Real-Time Operating System
//*
//*     NICKNAME:  scmRTOS
//*
//*     PROCESSOR: ADSP-BF533 (Analog Devices)
//*
//*     TOOLKIT:   VDSP (Analog Devices)
//*
//*     PURPOSE:   Port Test File
//*
//*     Version: v5.0.1
//*
//*
//*     Copyright (c) 2003-2015, scmRTOS Team
//*
//*     Permission is hereby granted, free of charge, to any person 
//*     obtaining  a copy of this software and associated documentation 
//*     files (the "Software"), to deal in the Software without restriction, 
//*     including without limitation the rights to use, copy, modify, merge, 
//*     publish, distribute, sublicense, and/or sell copies of the Software, 
//*     and to permit persons to whom the Software is furnished to do so, 
//*     subject to the following conditions:
//*
//*     The above copyright notice and this permission notice shall be included 
//*     in all copies or substantial portions of the Software.
//*
//*     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
//*     EXPRESS  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
//*     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
//*     IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
//*     CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
//*     TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
//*     THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//*
//*     =================================================================
//*     Project sources: https://github.com/scmrtos/scmrtos
//*     Documentation:   https://github.com/scmrtos/scmrtos/wiki/Documentation
//*     Wiki:            https://github.com/scmrtos/scmrtos/wiki
//*     Sample projects: https://github.com/scmrtos/scmrtos-sample-projects
//*     =================================================================
//*
//******************************************************************************
//*     ADSP-BF533/VisualDSP++ sample by Harry E. Zhurov, Copyright (c) 2005-2015

#include <stdint.h>
#include "device_def.h"
#include "macro.h"
#include "usrlib.h"

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




//---------------------------------------------------------------------------
//
//      Objects
//
int a = 1;
int b;
volatile int c;
volatile int d;

TSlon Slon(2);

usr::ring_buffer<uint16_t, 16> buf;
//---------------------------------------------------------------------------

EX_INTERRUPT_HANDLER(timer0_isr) __attribute__ ((interrupt_handler));

//---------------------------------------------------------------------------
int main()
{     
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
            buf.push( Slon.get_a() );
            Slon.set_b( buf.pop() );
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




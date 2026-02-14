
volatile uint8_t &TWB = *(volatile uint8_t *)0xB8;  // TWBR bit rate
volatile uint8_t &TWS = *(volatile uint8_t *)0xB9;  // TWSR status
volatile uint8_t &TWA = *(volatile uint8_t *)0xBA;  // TWAR address
volatile uint8_t &TWD = *(volatile uint8_t *)0xBB;  // TWDR data
volatile uint8_t &TWC = *(volatile uint8_t *)0xBC;  // TWCR control

constexpr uint8_t START = 0xA4;   // (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)
constexpr uint8_t STOP  = 0x14;   // STOP condition
constexpr uint8_t WRITE = 0x0;    // SLA+W
constexpr uint8_t READ  = 0x1;    // SLA+R

inline void CLRTWINT()  { TWC |= 0x70; }  // Clear TWINT
inline void WRITEBYTE() { TWC = 0x84; }  // Write byte, clears TWINT & TWEN

int write(int filediscriptor, uint8_t *data, uint8_t len)/*ADDR must be first byte*/
{
  if((filediscriptor&0x20000000)==0x0)
  {
    return -1;
  }
  TWC = START;/*write START coindition*/
  uint8_t flags = TWS;/*Check bits to make sure START good. does 2x but no check just 
  if((TWS&0xF8)!=0x18)
  {
    TWC = STOP; /*Error out and send STOP condition*/
    return -1;/*emulating linux i2c return values, so this means no good*/
  }
  TWD=((uint8_t)((filediscriptor&0x7F)<<1))|WRITE;/*load addr to SLA+W*/
  
  WRITEBYTE(); /*write byte macro. clears TWINT and TWEN bits*/
  while(!(TWC&0x80);
  if((TWS&0xF8)!=0x08)/*error out*/
  {
    TWC = STOP;
    return -1;
  }

  for(uint8_t i=0; i<len; i++)/*send out data bytes*/
  {
    TWD = data[i];
    TWC = 0x84
    if((TWS&0xF8)!=0x28)/*error out if no ACk and good data write*/ 
    {
      TWC = STOP;
      return -1;
    }
  }
  TWC = STOP;
  return len;
}

int read(int filediscriptor, uint8_t *data, uint8_t len)
{
    if((filediscriptor&0x10000000)==0x0)
  {
    return -1;
  }
  TWC = START;/*write START coindition*/
  while(!(TWC&0x80));
  if((TWS&0xF8)!=0x08)
  {
    TWC = STOP; /*Error out and send STOP condition*/
    return -1;/*emulating linux i2c return values, so this means no good*/
  }
  TWD=((uint8_t)((filediscriptor&0x7F)<<1))|READ;/*load addr to SLA+R*/
  TWC = 0x84; /*write byte macro. clears TWINT and TWEN bits*/
  while(!(TWC&0x80));
  if((TWS&0xF8)!=0x40)/*error out*/
  {
    TWC = STOP;
    return -1;
  }
  
  for(uint8_t i=0; i<len; i++)/*send out data bytes*/
  {
    if(i < len - 1)
      TWC = 0xC4; // Read with ACK
    else
      TWC = 0x84; // Read with NACK (last byte)
    while(!(TWC & 0x80));
    data[i] = TWD;  // Actually READ the data
    if(i < len - 1 && (TWS&0xF8)!=0x50)/*error out if no ACK*/
    {
      TWC = STOP;
      return -1;
    }
  }
  TWC = STOP;
  return len;
}

/*##############################################################################*/
/*# SMCR definitions for sleeping/waking etc                                   #*/
/*##############################################################################*/
volatile uint8_t &SMC = *(volatile uint8_t *)0x33; // SMCR register

constexpr uint8_t SLEEP     = 0b0001; // Sleep mode enabled
constexpr uint8_t WAKE      = 0b0000; // Sleep mode disabled
constexpr uint8_t IDLE      = 0b0000; // Idle mode
constexpr uint8_t ADCNOISE  = 0b0010; // ADC Noise Reduction
constexpr uint8_t POWERDOWN = 0b0100; // Power-down
constexpr uint8_t POWERSAVE = 0b0110; // Power-save
constexpr uint8_t STANDBY   = 0b1100; // Standby

/*##############################################################################*/
/*# Interrupt definitions                                                        #*/
/*##############################################################################*/
volatile uint8_t &SREG_  = *(volatile uint8_t *)0x3F;  // Status register
constexpr uint8_t ENABLEINTERRUPT  = 0x80;
constexpr uint8_t DISABLEINTERRUPT = 0x00;

volatile uint8_t &EICRA_ = *(volatile uint8_t *)0x69;  // External Interrupt Control Register A
constexpr uint8_t RISINGINTERRUPT  = 0x3; // Rising edge
constexpr uint8_t FALLINGINTERRUPT = 0x2; // Falling edge

volatile uint8_t &EIMSK_   = *(volatile uint8_t *)0x3D; // External Interrupt Mask
volatile uint8_t &PCMSK_0  = *(volatile uint8_t *)0x6D; // Pin Change Mask 0-7
volatile uint8_t &PCMSK_1  = *(volatile uint8_t *)0x6C; // Pin Change Mask 8-14
volatile uint8_t &PCMSK_2  = *(volatile uint8_t *)0x6B; // Pin Change Mask 16-23
volatile uint8_t &PCICR_   = *(volatile uint8_t *)0x68; // Pin Change Interrupt Control

/*##############################################################################*/
/*# Port definitions                                                            #*/
/*##############################################################################*/
volatile uint8_t &PORTD_ = *(volatile uint8_t *)0x0B;
volatile uint8_t &DDRD_  = *(volatile uint8_t *)0x0A;
volatile uint8_t &PIND_  = *(volatile uint8_t *)0x09;

volatile uint8_t &PORTC_ = *(volatile uint8_t *)0x08;
volatile uint8_t &DDRC_  = *(volatile uint8_t *)0x07;
volatile uint8_t &PINC_  = *(volatile uint8_t *)0x06;

volatile uint8_t &PORTB_ = *(volatile uint8_t *)0x05;
volatile uint8_t &DDRB_  = *(volatile uint8_t *)0x04;
volatile uint8_t &PINB_  = *(volatile uint8_t *)0x03;

/*##############################################################################*/
/*# PWM Measurement Setup                                                       #*/
/*##############################################################################*/
void configForPwmMeasure(void)
{
  DDRD_  = 0x00; // set PORTD input
  PORTD_ = 0x00; // disable pull-ups
}

volatile unsigned long rise[6];       // PWM rise times
volatile unsigned long pulseWidth[6]; // PWM pulse widths

uint8_t PCINT_0_flags = 0x00; // Track last known state of pins

/*##############################################################################*/
/*# Pin Change Interrupt Vector                                                 #*/
/*##############################################################################*/
#define __PCINT_0_vector__ __vector_7
void __PCINT_0_vector__(void) __attribute__((signal, used, externally_visible));
void __PCINT_0_vector__(void)
{
  unsigned long MICROS = micros();
  uint8_t bt = 1;
  uint8_t PINDR = PIND_;
  for(uint8_t i = 0; i < 5; i++)
  {
    if((PINDR & bt) != (PCINT_0_flags & bt))
    {
      PCINT_0_flags = (PCINT_0_flags & ~bt) | (PINDR & bt);
      if((PINDR & bt) == bt)
        rise[i] = MICROS;
      else
        pulseWidth[i] = MICROS - rise[i];
    }
    bt <<= 1;
  }
}

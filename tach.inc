;   Tachometer
;   Using pulse timing with CCP3 Capture mode and Timer3. 
;   PIN  15: RC0
;    
;   Using a 4MHz Oscillator, 
;   Fosc/4 = 1MHz, 1 instruction cycle = 1us
;   Each count = 2us 1:2 Prescalar
;   Pulse Period T = Currrent Rising edge - Previous Rising edge (1/2 revolution)
;   rpm = (60 x timer freq)/ (4 x Tach period)
    
;   PPS Routing REGISTER CPP3PPS 
;   CCP3  inpt RC0 via PPS
;   RC0 = 0x10 
;    
;   REGISTER T3CLK 
;	BIT 3-0 : 0001 Fosc/4
;    
;   REGISTER T3CON
;	Bit 5-4:11 1:8 Prescaler
;	Bit 2: 1 SYNC Do not synchronize external clock input
;	Bit 1: 1 RD16 All 16 bits of Timer1 can be read simultaneously (TMR1H is buffered)
;	Bit 0: 1 ENABLE

;   Reset Timer3  
    
;   REGISTER CCP3CON: CCPx CONTROL REGISTER
;	Bit 7: 1 CCP3 Enabled
;	Bit 5:OUT
;       Bit 4: unsed in capture mode
;	Bit 3-0:0101  Capture mode: every rising edge of CCPx input
    
;   REGISTER CCP3CAP: CAPTURE INPUT SELECTION REGISTER
;	Bit 2-0: 000 CTS = CCP3PPS input
    
;    REGISTER PIR6: PERIPHERAL INTERRUPT REQUEST REGISTER 6
;	Bit 2: CCP3IF FLAG
;	If 0: no rising edge captured, 1: rising edge captured 
;	Clear flag after every capture
    
;    On rising edge, copy TMR3H:TMR

init_tach
    
    BANKSEL CCP3PPS 
    MOVLW 0x10
    MOVWF CCP3PPS
    
    BANKSEL T3CLK
    MOVLW B'00000001' 
    MOVWF T3CLK
    
    BANKSEL T3CON 
    MOVLW Bb'00010111' 
    MOVWF T3CON
    
    
    BANKSEL TMR3H 
    CLRF TMR3H 
    CLRF TMR3L
  
    BANKSEL CCP3CON 
    MOVLW b'10000101' 
    MOVWF CCP3CON
    
    BANKSEL ??????? 
    CLRF ???????
    
    BANKSEL PIR6 
    BCF PIR6, 2
    
    RETURN
    
get_fan_speed
    
    BANKSEL PIR6 
    BTFSS PIR6, 2   ;Check if rising edge captured
    RETURN ; 0 - NO
    ; 1 - Yes clr flag to detect next edge

calc_period    ; Current - Previous 
   BANKSEL CCPR3L 
   MOVF CCPR3L, W 
   MOVWF TACH_PERIODL
   MOVF CCPR3H, W 
   MOVWF TACH_PERIODH
   
   BANKSEL TACH_PREVH
   BTFSS   TACH_PREVH, 7  
   GOTO    save_pulse
   
   MOVF    TACH_PREVL, W
   SUBWF   TACH_PERIODL, F ; TACH_PERIODL = Current - PrevL
   
    
    

	   
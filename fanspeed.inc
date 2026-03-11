;   FANSPEED & 7 SEGMENT BRIGHTNESS CONTROL
;   Pin 16: RC1
    
;    FAN RATED SPEED 4800 +- 15%
    
;   Similar to PWM used for Buzzer circuit 
;   Using Timer4 with CCP2 module for pulse generation in PWM mode

;   Programmable Prescalar 1:4 
;   PWM Period = [(T4PR) + 1] x 4 x TOSC x (TMR2 Prescale Value)
;	0.001s = [(T4PR) +1] x 4 x (1/4,000,000) x (4)
;   T4PR = 249  
    
;   T4CON REGISTER 
;	Bit 7: 1 (ON) 
;	Bit 6-4: 010 (1:4) Prescaler
    
    
;    CCP2CON REGISTER 
;	Bit 7: 1 CCP2 is enabled 
;	Bit 4: 1 LEFT!!! Aligned FMT	
;	Bit 3-0: 1111  PWM Mode	
    
;   PWM DUTY CYLE CCPRxH:CCPRxL (500)
;    bits 15:8 = 01 7:6 = 1111 0100
;    CCPR2H = ADRESH
;    CCPR2L = ADRESL
    
;    PERIPHERAL PIN SELECT OUTPUT: RxyPPS REGISTER 
;    RC1PPS Bits 5-0 = 0x0A
    
;    T4CLKCON REGISTER
;    Bit 3-0; 0001 = FOSC/4
    
    
;     CCPTMRS0 REGISTER (HANDLES CCP1/TIMER2 AND CCP2/TIMER4)
;	C3TSETl[5;4] =10	    TACH CCP3 uses timer1
;	C2TSEL[3:2] = 10      CCP2 uses Timer4
;	C1TSEL[1:0] = 01    CCP1 uses Timer2
    
    
;   SETUP FOR PWM OPERATION
;   1. RxyPPS REGISTER -> Route CCP2 to RC1
;   2. PR2 REGISTER
;   3. CCPR1H:CCPR1L
;   4. CCP2CON REGISTER
;   5. T4CLKCON, T4CON
;   6. CCP2CON CLR bit 7
;    
    
init_fanspeed_PWM
    
    BANKSEL RC1PPS
    MOVLW 0x0A
    MOVWF RC1PPS
    
    BANKSEL T4PR
    MOVLW D'249'
    MOVWF T4PR 
    
    BANKSEL CCP2CON 
    MOVLW B'10011111' 
    MOVWF CCP2CON
    
    BANKSEL CCPTMRS0
    MOVLW   B'00101001'     
    MOVWF   CCPTMRS0
    
    BANKSEL T4CLKCON
    MOVLW b'00000001'
    MOVWF T4CLKCON
    
    BANKSEL T4CON
    MOVLW b'10100000'
    MOVWF T4CON
    

    ;  Set to MAX by default for 1000 milliseconds
    ; Necessary since fan startup voltage is 4V    
    BANKSEL CCPR2H 
    MOVLW   0xFF            ; 255 = 100% Duty Cycle
    MOVWF   CCPR2H 
    MOVLW   0x00            ; Low bits 0
    MOVWF   CCPR2L
    CALL Delay_1000milli
    RETURN
     
adjust_fanspeed
    BANKSEL ADC_H8BIT   ; <15:8> MSB
    MOVF    ADC_H8BIT, W
    BANKSEL CCPR2H
    MOVWF   CCPR2H

    ; <7:6> LSB
    BANKSEL ADC_L2BIT
    MOVF    ADC_L2BIT, W
    BANKSEL CCPR2L
    MOVWF   CCPR2L
    RETURN
    
    
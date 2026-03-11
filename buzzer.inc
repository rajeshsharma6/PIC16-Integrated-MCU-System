;   Buzzer Config 
;   Pin 38 : RB5 
;   If ADC_4BIT >= 12
;	RB1: High (Yellow LED ON)
;	RB5: High 1s, Low 1s
    
;   Driving Circuit
;   Speaker Resistance = 8ohms , V/R = 625mA
;   Maximum Pin output current = 25mA, 
;   NPN Transistor 2N4401: Ic = B x Ib
;   Ib = (5V - 0.7V) / 1k ? 4.3 mA

;   Audible Frequencies 20hz- 20Hz
;   For 4MHz Oscillator, each instruction cycle is 1µs:
;   Output Sound at 1kHz
;   Therefore frequency divider -> Period = 1ms
;   Toggle High: 0.5ms  (500 instruction cycles each)
;   Toggle Low: 0.5ms  (500 instruction cycles each)
    
    
;   Using Timer2 with CCP module for pulse generation in PWM mode

;   Programmable Prescalar 1:4 
;   PWM Period = [(T2PR) + 1] x 4 x TOSC x (TMR2 Prescale Value)
;	0.001s = [(T2PR) +1] x 4 x (1/4,000,000) x (4)
;   T2PR = 249  
    
;   T2CON REGISTER 
;	Bit 7: 1 (ON) 
;	Bit 6-4: 010 (1:4)
    
;    CCP1CON REGISTER 
;	Bit 7: 1 CCP1 is enabled 
;	Bit 4: 0 Right Aligned FMT	
;	Bit 3-0: 1111  PWM Mode	
    
;   PWM DUTY CYLE CCPRxH:CCPRxL (500)
;    bits 9:8 = 01 7:0 = 1111 0100
;    CCPR1H = 00000001
;    CCPR1L = 11110100
    
;    PERIPHERAL PIN SELECT OUTPUT: RxyPPS REGISTER 
;    RB5PPS Bits 5-0 = 0x09
    
;    T2CLKCON REGISTER
;    Bit 3-0; 0001 = FOSC/4
    
;   SETUP FOR PWM OPERATION
;   1. RxyPPS REGISTER -> Route CCP1 to RB5
;   2. PR2 REGISTER
;   3. CCPR1H:CCPR1L
;   4. CCP1CON REGISTER
;   5. T2CLKCON, T2CON
;   6. CCP1CON CLR bit 7
;    
    
init_buzzer_PWM
    
    BANKSEL RB5PPS
    MOVLW 0x09
    MOVWF RB5PPS
    
    BANKSEL T2PR
    MOVLW D'249'
    MOVWF T2PR
    
    BANKSEL CCPR1H 
    MOVLW B'00000001'
    MOVWF CCPR1H 
    
    MOVLW B'11110100'
    MOVWF CCPR1L
    
    BANKSEL CCP1CON 
    MOVLW B'10001111' 
    MOVWF CCP1CON
    
    BANKSEL T2CLKCON
    MOVLW b'00000001'
    MOVWF T2CLKCON
    
    BANKSEL T2CON
    MOVLW b'10100000'
    MOVWF T2CON
    
    BANKSEL CCP1CON ; Bit 7: 0 CCP1 is disabled by default 
    BCF CCP1CON, 7
    RETURN
     
; 12 = 1100, 13 = 1101, 14 = 1110, 15 = 1111
; Any number >= 12 will have Bit 3 and Bit 2 set    
check_alarmState
    BANKSEL ADC_4BIT
    BTFSS ADC_4BIT, 3 
    GOTO alarm_off
    
    BTFSS ADC_4BIT, 2
    GOTO alarm_off

alarm_on
;   LED ON
    BANKSEL LATB
    BSF     LATB, 1 
    
;   Check if 1 second passed
    BANKSEL TIMER_TOGGLE_BUZZER
    BTFSS TIMER_TOGGLE_BUZZER , 0   
    GOTO buzz_off		; 0
    
buzz_on				; 1
    BANKSEL CCP1CON ; Bit 7: 1 CCP1 enabled
    BSF CCP1CON, 7
    RETURN

buzz_off    
    BANKSEL CCP1CON 
    BCF CCP1CON, 7
    RETURN
    
alarm_off    
;   LED OFF
    BANKSEL LATB
    BCF     LATB, 1 
    
    BANKSEL CCP1CON ; Bit 7: 0 CCP1 is disabled by default 
    BCF CCP1CON, 7
    
    BANKSEL TIMER_TOGGLE_BUZZER
    CLRF TIMER_TOGGLE_BUZZER	    ;reset 
    RETURN
    
    
    
   



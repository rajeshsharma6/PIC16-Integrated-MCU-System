;   Configure Timer1 16-bit for 1s Delay
;   16-bit = 65,536 counts 
;   OSC: 4MHz
;   OSC/4 : 1MHz
;   1:8 Prescalar: 125,000 Hz 
;   Each timer1 increment = 8us
;   Maximum time before overflow = 65,536 × 8µs = 524,288 µs
;   
;   Use 2 x 500,000us counts:
;   500,000us = 62,500 counts
;   Preload timer with 65,536 - 62,500 = 3,036
;   After 2 overflows: 1s 

;   REGISTERS
;    
;   T1CLK
;	 Bit 3-0: 0001 (T1CS:FOSC/4)

;   TMR1H:TMR1L (PRELOAD Timer) 
;	TMR1H: 00001011
;	TMR1L: 11011100 
    
;    PIR4   reset overflow flag
;	Bit 0: 0  (0: No TMR1 overflow occurred, 1: No TMR1 overflow occurred)
    
;   T1CON
;	 Bit 5-4: 11 (1:8 Prescaler)
;	 Bit 0: 1 (ON)
    
    
    
init_timer1
    
    BANKSEL T1CLK       
    MOVLW   b'00000001'           
    MOVWF   T1CLK
   
    MOVLW   b'00001011' 
    MOVWF   TMR1H       
    MOVLW   b'11011100'       
    MOVWF   TMR1L      
    
    BANKSEL PIR4        
    BCF     PIR4, 0     
    
    BANKSEL T1CON   
    MOVLW  b'00110001'
    MOVWF   T1CON
    
;   Set counter and overflow counter = 0
    BANKSEL TIMER_TOGGLE_BUZZER
    CLRF TIMER_TOGGLE_BUZZER
    BANKSEL TIMER_TOGGLE_UART
    CLRF TIMER_TOGGLE_UART
    CLRF TIMER_OVERFLOW_COUNTER
    
    RETURN
    
timer1_run
    ; Check for overflow
    BANKSEL PIR4        
    BTFSS   PIR4, 0    
    RETURN				 ;no overflow, exit immediately
    
    ;After overflow, clear overflow flag and preload timer1 again
    BCF     PIR4, 0			; Clear the hardware flag so it can trigger again
    
;   28.6.1: Stop timer before preloading 
    BANKSEL T1CON
    BCF     T1CON, 0			; ON: 0 (Timer1 stopped)
    
    BANKSEL TMR1H
    MOVLW   b'00001011' 
    MOVWF   TMR1H       
    MOVLW   b'11011100'       
    MOVWF   TMR1L    
    
;   Resume timer
    BANKSEL T1CON
    BSF     T1CON, 0			; ON: 1

    BANKSEL TIMER_OVERFLOW_COUNTER	; tracks number of overflows
    INCF    TIMER_OVERFLOW_COUNTER, F    

;   Check for 2 overflows
    MOVLW   d'2'
    SUBWF   TIMER_OVERFLOW_COUNTER, W	; 2 - TIMER_OVERFLOW_COUNTER
    BTFSS   STATUS, Z			; Z flag is set if the result is zero,
    RETURN				; Not 2 overflows yet

;   2 overflows 
    CLRF    TIMER_OVERFLOW_COUNTER      ; Reset  oveflow counter
    BANKSEL TIMER_TOGGLE_BUZZER
    MOVLW   b'00000001'
    XORWF   TIMER_TOGGLE_BUZZER, F	; Toggle
    BANKSEL TIMER_TOGGLE_UART
    MOVLW   b'00000001'
    XORWF   TIMER_TOGGLE_UART, F	
    RETURN             



    
    
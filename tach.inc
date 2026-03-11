;   Tachometer
;   Initally used pulse timing with CCP3 Capture mode and Timer3 external clock pulse. 
;   
;   PULSE COUNTING: 
;   PIN  6: RA4
;   10k? pull-up resistor to +5V to produce clean pulses.
;   The tach signal on RC0 connected to timer3 input via PPS
;   Timer 3 configured as counter, increments by 1 for every rising edge on RC0
;   Integrate with timer1 TIMER_TOGGLE for 1s intervals
    
;   get_fanSpeed 
;	Detect when TIMER_TOGGLE != 0, 
;	stop timer3, 
;	count copied to pulse_H:pulse_L 
;	restart timer   
    
;   convert_to_rpm 
;   Revolutions per second = number of pulses counted in 1 second/2
;   Revolutions per minutes = RPS x 60 = pulses counted x 30    
;	This can be done by left shift 5 times (x32) (LSLF and RLF)
;	    Returns RPM that is always 2/32 higher 
;	Repeated Addition: Loop 30 times and add value to itself
    
    
; CONFIG
;   PPS Routing to clock input T3CKI 
;   REGISTER T3CKIPPS 
;   RA4 = 0x04 
;    
;   REGISTER T3CLK 
;	BIT 3-0 : 0000 T3CKIPPS
;    
;   REGISTER T3CON
;	Bit 5-4: 00 1:1 Prescaler 
;	Bit 2: 0  synchronize external clock input
;	Bit 1: 1 RD16 All 16 bits of Timer1 can be read simultaneously (TMR1H is buffered)
;	Bit 0: 1 ENABLE

;   Reset counter and overflow flag  
;   TMR3H:TMR3L = 0
    
;   Clear TRM3IF:Timer3 Overflow Interrupt Flag bit PIR4 bit 2
;   Reset Variables  

init_tach
    ; Unlock PPS before writing T3CKIPPS
    BANKSEL PPSLOCK
    MOVLW   0x55
    MOVWF   PPSLOCK
    MOVLW   0xAA
    MOVWF   PPSLOCK
    BCF     PPSLOCK, 0          ; PPSLOCKED = 0 (writable)
    
    BANKSEL T3CKIPPS 
    MOVLW 0x04
    MOVWF T3CKIPPS
    
     ; Lock PPS
    BANKSEL PPSLOCK
    MOVLW   0x55
    MOVWF   PPSLOCK
    MOVLW   0xAA
    MOVWF   PPSLOCK
    BSF     PPSLOCK, 0          ; PPSLOCKED = 1
    
    BANKSEL T3CLK
    MOVLW B'00000000' 
    MOVWF T3CLK
    
    BANKSEL T3GCON
    clrf    T3GCON
    
    BANKSEL T3CON 
    MOVLW B'00000011' 
    MOVWF T3CON
    
    BANKSEL TMR3H 
    CLRF TMR3H 
    CLRF TMR3L
    
    BANKSEL PIR4
    BCF     PIR4, 2
    
    BANKSEL PULSE_H
    CLRF    PULSE_H
    CLRF    PULSE_L

    BANKSEL TACH_RPMH
    CLRF    TACH_RPMH
    CLRF    TACH_RPML

    
    
    RETURN
    
get_fan_speed

    BANKSEL TIMER_TOGGLE_UART
    BTFSS   TIMER_TOGGLE_UART, 0   
    RETURN			; 0
    GOTO    read_rps		; 1 (1 sec passed)
    
read_rps
    
    BANKSEL T3CON
    BCF     T3CON, 0		; Stop timer T3COn bit 0 = 0 
    
    ; Small delay for Timer3 to settle
    CALL Delay_500micro
    
;   Store in register
    BANKSEL TMR3L
    MOVF    TMR3L, W
    BANKSEL PULSE_L
    MOVWF   PULSE_L

    BANKSEL TMR3H
    MOVF    TMR3H, W
    BANKSEL PULSE_H
    MOVWF   PULSE_H  
    
;   Reset counter    
    BANKSEL TMR3H
    CLRF    TMR3H         
    CLRF    TMR3L
    
    BANKSEL T3CON
    BSF     T3CON, 0        ; restart Timer3 

    CALL convert_to_rpm
    RETURN
    
fan_stopped
    BANKSEL TACH_RPMH
    CLRF    TACH_RPMH
    CLRF    TACH_RPML
    RETURN
    
    
convert_to_rpm
    
    ; FAN MAX RATED SPEED 4800RPM +- 15%
    ; Clear previous tach_rpm value
    BANKSEL TACH_RPMH
    CLRF    TACH_RPMH       
    CLRF    TACH_RPML

    ;x30 using repeated addition
    BANKSEL LOOP_COUNT
    MOVLW   D'30'
    MOVWF   LOOP_COUNT
    
rpm_add
    BANKSEL PULSE_L
    MOVF    PULSE_L, W
    BANKSEL TACH_RPML
    ADDWF   TACH_RPML, F   

    BANKSEL PULSE_H
    MOVF    PULSE_H, W
    BANKSEL TACH_RPMH
    ADDWFC  TACH_RPMH, F    

    BANKSEL LOOP_COUNT
    DECFSZ  LOOP_COUNT, F 
    GOTO    rpm_add     ; skip if LOOP_count = 0
    
    RETURN

    

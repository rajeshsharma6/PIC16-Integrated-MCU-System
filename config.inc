;   PIN ASSIGNMENT
;   TRISx: Input (1), Output (0)
;   ANSELx: Analog (1), Digital (0) 
;   PORTA 
;	RA0 - Input (Analog) : ADC Channel 0
;   PORTB
;	RB0 - Output (Digital): Active-low LED (RED) : DIP - ON, ADC - OFF
;	RB1 - Output (Digital): Active-high LED (YELLOW) : Alarm - ON    
;	RB2 - Output (Digital): Active-high LED (GREEN) : Transmitting - ON    
;	RB5 - Output (Dgital): Buzzer
;   PORTC
;	RC0 - Input : tachymeter
;	RC1 - Output : PWM fan speed and 7 SEG brightness 
;	RC2 - Input (Digital): DIP Pin 1 (LSB)
;	RC3 - Input (Digital): DIP Pin 2  
;	RC4 - Input (Digital): DIP Pin 3     
;	RC5 - Input (Digital): DIP Pin 4 (MSB)
;	RC6 - Ouput (Digital): UART TX
;   PORTD
;	RD0 - Output (Digital): 7-SEG A	
;	RD1 - Output (Digital): 7-SEG B
;	RD2 - Output (Digital): 7-SEG C
;	RD3 - Output (Digital): 7-SEG D
;	RD4 - Output (Digital): 7-SEG E
;	RD5 - Output (Digital): 7-SEG F
;	RD6 - Output (Digital): 7-SEG G
;	RD7 - Output (Digital): 7-SEG DP
;   PORTE
;	RE0 - Input (Digital): 
;	RE1 - Input (Digital): Active-low Control Mode Select
    
    
    
init_ports    
;   PORTA
    BANKSEL TRISA
    BSF TRISA,0		; Set RA0 to input
    BANKSEL ANSELA
    BSF ANSELA, 0	; Set RA0 to analog

;   PORT B
    BANKSEL LATB
    CLRF LATB
    BANKSEL TRISB
    CLRF TRISB		; Make all PORTB pins outputs
    BANKSEL ANSELB	
    CLRF ANSELB		; Make all PORTB pins digital
    
;   PORTC
    BANKSEL TRISC
    MOVLW B'10111101'	 ; Set RC1 & RC6 as only Output
    MOVWF TRISC
    BANKSEL ANSELC
    CLRF ANSELC		 ; Make all PORTC pins digital
    
;   PORTD - 7 SEG
    BANKSEL TRISD
    CLRF TRISD		 ; Make all PORTD pins outputs
    BANKSEL ANSELD
    CLRF ANSELD		 ; Make all PORTD pins digital
    BANKSEL LATD		    
    CLRF LATD		 ; Turn OFF 7-Seg
    
;   PORTE
    BANKSEL TRISE
    MOVLW B'00000011'	 ; Set RE0 & RE1 to input
    MOVWF TRISE
    BANKSEL ANSELE
    CLRF ANSELE		 ; Make all PORTE pins digital
    
    RETURN
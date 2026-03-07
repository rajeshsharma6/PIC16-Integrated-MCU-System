; DIP 1 - PORTC2 | DIP 2 - PORTC3 | DIP 3 - PORTC4 | DIP 4 - PORTC5 
; PORTC = xx xx RC5 RC4 RC3 RC2 xx xx
; DIP_4BIT = xx xx xx xx RC5 RC4 RC3 RC2
    
    
  
read_dip
    BANKSEL PORTC
    MOVF PORTC, W
    ANDLW B'00111100'  ; Zero non-DIP bits
    MOVWF DIP_4BIT
    
    ; Shift right 2 times
    lsrf    DIP_4BIT, f      
    lsrf    DIP_4BIT, f      
    RETURN





 
disp_seven_seg
    MOVF  SEVEN_SEG_4BIT, W
    CALL seven_seg_decoder ; Place 7 segment pattern into W
    
    BANKSEL PORTD             
    MOVWF LATD          ; Send the bit pattern to the 7-segment pins
    RETURN
   
seven_seg_decoder
    ADDWF PCL, f         ; Jump forward by the amount in SEVEN_SEG_4BIT
    
;   Common Cathode display (1 = Segment ON)
    RETLW b'00111111'    ; 0
    RETLW b'00000110'    ; 1
    RETLW b'01011011'    ; 2
    RETLW b'01001111'    ; 3
    RETLW b'01100110'    ; 4
    RETLW b'01101101'    ; 5
    RETLW b'01111101'    ; 6
    RETLW b'00000111'    ; 7
    RETLW b'01111111'    ; 8
    RETLW b'01101111'    ; 9
    RETLW b'01110111'    ; A (10)
    RETLW b'01111100'    ; B (11)
    RETLW b'00111001'    ; C (12)
    RETLW b'01011110'    ; D (13)
    RETLW b'01111001'    ; E (14)
    RETLW b'01110001'    ; F (15)
    RETURN
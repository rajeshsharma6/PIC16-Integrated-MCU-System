
;Outer Loop - 1000ms (100*(10,100+3)+2 overhead+2 return)
Delay_1000milli
    MOVLW   d'100'          ; 1 cycle
    MOVWF   DELAY_S            ; 1 cycle - Address 0x21 counter
d_outerloop
    CALL    Delay_10milli   ; 10,100us
    DECFSZ  DELAY_S, 1         ; 1 cycle
    goto    d_outerloop     ; 2 cycles
    return                  ; 2 cycles
	
;Middle Loop - 10.1ms (20 iterations * (502 + 3) + 2 cycles overhead +2 return
Delay_10milli
    MOVLW   d'20'           ; 1 cycle
    MOVWF   DELAY_MS            ; 1 cycle - Address 0x22 counter
d_middleloop
    CALL    Delay_500micro  ; 502us 
    DECFSZ  DELAY_MS, 1         ; 1 cycle
    goto    d_middleloop    ; 2 cycles
    return		    ; 2 cycles
	
	
; Inner Loop - 500µs  (168 iterations * 3 cycles) + 2 cycles overhead +2 return
Delay_500micro
    MOVLW d'163'
    MOVWF DELAY_US    
d_innerloop
    DECFSZ DELAY_US,1 
    goto d_innerloop
    return



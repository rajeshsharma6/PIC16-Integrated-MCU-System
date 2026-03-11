;   RE1 - Active-low (High, Low when pressed)


 
press_button
    BANKSEL PORTE
    BTFSC PORTE, 1	; Check if button pressed (Pressed: RE1 = 0)
    RETURN		; (Not Pressed)
    
;   Debounce Delay
    CALL Delay_10milli	
    
    BANKSEL PORTE
    BTFSC PORTE, 1	; Check if still pressed
    RETURN		; False trigger
    BANKSEL MODE
    MOVLW   0x01
    XORWF   MODE, F	; Toggle MODE
   
release
    BANKSEL PORTE
    BTFSS PORTE, 1	; Check if button released (RE1=1)
    GOTO release
    CALL Delay_10milli
    RETURN
    
    
mode_select
    BANKSEL MODE
    MOVF    MODE, W
    BTFSC   STATUS, Z    ; Is MODE = 0? (DIP Mode)
    GOTO    mode_dip


mode_adc                ; If MODE = 1 
    BANKSEL LATB
    BSF     LATB, 0      ; RB0 HIGH = LED OFF 
    MOVF    ADC_4BIT, W  ; Load ADC result
    MOVWF   SEVEN_SEG_4BIT
    RETURN

mode_dip                ; If MODE = 0 
    BANKSEL LATB
    BCF     LATB, 0      ; RB0 LOW = LED ON 
    MOVF    DIP_4BIT, W		
    MOVWF   SEVEN_SEG_4BIT      
    RETURN


    
; check_reset
; RE0 is active-HIGH: pin = 1 when button pressed, 0 when released.
; Uses RESET instruction for a clean full restart (all SFRs = POR values).
check_reset
    BANKSEL PORTE
    BTFSS   PORTE, 0        ; skip if RE0 = 1 (button pressed, active-HIGH)
    RETURN                  ; RE0 = 0 -> not pressed -> return
    RESET                   ; software reset: restarts from ORG 0x00
;   ADCON0 Register
;	Bit 7: ADON: ADC Enable (1 - Enabled)
;	Bit 4: ADCS: ADC Clock Selection (0 = Clock supplied from oscillator pin 13)
;	Bit 2: ADFRMO: ADC results alignment (0 = ADRESH <7:0>, ADRESL <1:0>) LEFT JUSTIFIED       
;	Bit 0: ADGO: ADC Conversion Status (0 = conversion completed , 1 = Start conversion cycle)

;   ADPCH: ADC POSITIVE CHANNEL SELECTION REGISTER (000000 = ANA0) 
   
;   ADREF: ADC REFERENCE SELECTION REGISTER 
;	Bit 1-0: ADPREF:  Positive Voltage Reference  (00 = VDD)
;	Bit 4: ADNREF:  Negative Voltage Reference (0 = VSS)

   
    
init_adc
;   ADCON0 
    BANKSEL ADCON0
    MOVLW b'10000000'
    MOVWF ADCON0
    
;   ADPCH
    BANKSEL ADPCH
    CLRF ADPCH
    
;   ADREF
    BANKSEL ADREF
    CLRF ADREF
    
    RETURN
    
adc_convert
    BANKSEL ADCON0
    BSF     ADCON0, 0	; Set Bit 0 (ADGO) to 1 to start conversion

adc_conversionCycle
    BTFSC ADCON0, 0	; Check if ADC still converting (Is Bit 0 (ADGO) = 1)
    GOTO adc_conversionCycle ; yes
    
    BANKSEL ADRESH	 ;contains converted 8 Bit ADC value
    MOVF    ADRESH, W
    MOVWF ADC_H8BIT	 ;For PWM CCPR2H
    MOVF ADRESL , W
    MOVWF ADC_L2BIT	 ;For PWM CCPR2L

    RETURN

adc_4bit
    BANKSEL ADRESH	
    MOVF    ADRESH, W
    MOVWF ADC_4BIT
    
    ; Shift right 4 times (Divide by 16)
    lsrf    ADC_4BIT, f      
    lsrf    ADC_4BIT, f      
    lsrf    ADC_4BIT, f      
    lsrf    ADC_4BIT, f
    RETURN
    
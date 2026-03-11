; Transmission: 
;  TX1REG (write buffer) to TSR (transmit shift register)
;  TSR takes one bit from TX1REG, adds start and stop bit and?clocks each bit onto the TX pin at baud rate 
    
; Start bit (LOW), STOP bit (HIGH) 
    
;  Send "RPM=XXXX\r\n" where XXXX is the current RPM value.
;  Called from main loop when TIMER_TOGGLE_UART != 0.
;  Also turns the TX LED (RB2, active-high) on during transmission.
     
;  COnvert 16 bit RPM to 4 BCD digits 
;  Repeated subtraction  
;    1. Count how many times 1000 fits -> DIGIT_1000
;	Subtract 1000
;	Check if carry flag = 1
;	    No: No borrow, increament 
;	    Yea: undo last subtraction, decrement
;    2. how many times  100 fits = DIGIT_100
;    3. how many times   10 fits = DIGIT_10
;    4. remainder         = DIGIT_1

; Covert bin to ASCII : Add 0x30    
    
;   Fosc = 4MHz    
;   SYNC =0
;   high baud rate (BRGH = 1)
;   16-bit BRG (BRG16 = 0) 
    
; Refer to Table 33-4   
;   Baud Rate: 9600
;   Actual rate 9615
;   % error: 0.16
;   SPBRG: d'25' 
    
 
;   PIN	CONFIG
;   PIC TX1: RC6 (connected to PC USB-serial adapter RXD)
;   PIC RX1: RC7 (connected to PC USB-serial adapter TXD)
    
;   REGISTERS

;   PPS Routing  
;   RC6PPS   - PPS output register for RC6
;	set to 0x10 = TX/CK
;   RX1PPS   - PPS input register for RX1
;	set to 0x17 = RC7

;    SPBRG: 25 = 9600 baud at 4MHz with BRGH=1, BRG16=0
;	SPBRGL = d'25'
;	SPBRGH  = 0
    
;   TX1STA: TRANSMIT STATUS AND CONTROL REGISTER
;	bit 5: 1 enabled TXEN: Transmit Enable bit
;	bit 4: 0 asynchronous mode
;	bit 2: 1 BRGH: High Baud Rate Select bit

;    RC1STA: RECEIVE STATUS AND CONTROL REGISTER
;	bit 7: 1 enabled SPEN: Serial Port Enable bit  (needed for Tx and Rx to operate)
    
        
    
init_uart

   ; Unlock PPS 
    BANKSEL PPSLOCK
    MOVLW   0x55
    MOVWF   PPSLOCK
    MOVLW   0xAA
    MOVWF   PPSLOCK
    BCF     PPSLOCK, 0      ; Clear PPSLOCKED bit to enable PPS changes

   ; Route UART TX to RC6 (PPS output)
    BANKSEL RC6PPS
    MOVLW   0x10            ; 0x10 = TX/CK output (Table 13-3)
    MOVWF   RC6PPS
    

   ; Lock PPS 
    BANKSEL PPSLOCK
    MOVLW   0x55
    MOVWF   PPSLOCK
    MOVLW   0xAA
    MOVWF   PPSLOCK
    BSF     PPSLOCK, 0      ; Set PPSLOCKED bit to lock PPS settings


    ; Baud rate = 9600 bps
    BANKSEL SP1BRGL
    MOVLW   D'25'
    MOVWF   SP1BRGL
    CLRF    SP1BRGH

    ; TX1STA: bit5 TXEN=1, bit4 SYNC=0, bit2 BRGH=1
    BANKSEL TX1STA
    MOVLW   b'00100100'
    MOVWF   TX1STA

    ; RC1STA: bit7 SPEN=1
    BANKSEL RC1STA
    MOVLW   b'10000000'
    MOVWF   RC1STA
    RETURN
    
    BANKSEL BAUD1CON
    BCF     BAUD1CON, 3

uart_send_byte
    BANKSEL PIR3
    
uart_tx_wait
    ; Double-buffering architecture
    ; Check TXIF (EUSART Transmit Interrupt Flag (read-only) bit)
    ; 0 = transmit buffer full , 1 = ready to accept next byte

    BTFSS PIR3, 4 
    GOTO uart_tx_wait ;0
    
    BANKSEL TX1REG  ; holding register 33.1.1.2
    ; load next byte for transmission while the hardware sends previous   
    ; Clears TX1IF (because TX1REG is now occupied)
    MOVWF   TX1REG  
    
    RETURN
    
uart_test
    MOVLW   'M'
    CALL    uart_send_byte
    MOVLW   'e'
    CALL    uart_send_byte
    MOVLW   'a'
    CALL    uart_send_byte
    MOVLW   's'
    CALL    uart_send_byte
    MOVLW   'u'
    CALL    uart_send_byte
    MOVLW   'r'
    CALL    uart_send_byte
    MOVLW   'i'
    CALL    uart_send_byte
    MOVLW   'n'
    CALL    uart_send_byte
    MOVLW   'g'
    CALL    uart_send_byte
    MOVLW   0x20
    CALL    uart_send_byte
    MOVLW   'F'
    CALL    uart_send_byte
    MOVLW   'a'
    CALL    uart_send_byte
    MOVLW   'n'
    CALL    uart_send_byte
    MOVLW   0x20
    CALL    uart_send_byte
    MOVLW   'S'
    CALL    uart_send_byte
    MOVLW   'p'
    CALL    uart_send_byte
    MOVLW   'e'
    CALL    uart_send_byte
    MOVLW   'e'
    CALL    uart_send_byte
    MOVLW   'd'
    CALL    uart_send_byte

    
    ; Send line end '\r\n'
    MOVLW   0x0D            
    CALL    uart_send_byte
    MOVLW   0x0A           
    CALL    uart_send_byte
uart_send_rpm
    
    BANKSEL TIMER_TOGGLE_UART
    BTFSS   TIMER_TOGGLE_UART, 0    
    RETURN ;  1 sec did not pass
    
    ;Yes - 1s passed
    ; turn on led
    BANKSEL LATB
    BSF     LATB, 2
    
    ; Convert RPM (16bit) to 4 BCD digits
    CALL rpm_to_BCD
    
    ; SEND 'R' 'P' 'M' '=' (ASCII)
    MOVLW   'R'
    CALL    uart_send_byte
    MOVLW   'P'
    CALL    uart_send_byte
    MOVLW   'M'
    CALL    uart_send_byte
    MOVLW   '='
    CALL    uart_send_byte
    
    
    ; SEND 4 BCD digits as ACSII (add 0x30)
    BANKSEL DIGIT_1000
    MOVF    DIGIT_1000, W
    ADDLW   0x30
    CALL    uart_send_byte
 
    BANKSEL DIGIT_100
    MOVF    DIGIT_100, W
    ADDLW   0x30
    CALL    uart_send_byte
 
    BANKSEL DIGIT_10
    MOVF    DIGIT_10, W
    ADDLW   0x30
    CALL    uart_send_byte
 
    BANKSEL DIGIT_1
    MOVF    DIGIT_1, W
    ADDLW   0x30
    CALL    uart_send_byte
    
    ; Send line end '\r\n'
    MOVLW   0x0D            
    CALL    uart_send_byte
    MOVLW   0x0A           
    CALL    uart_send_byte


    ; Turn off led 
    BANKSEL LATB
    BCF     LATB, 2

;    clear TIMER_toggle
    BANKSEL TIMER_TOGGLE_UART
    CLRF    TIMER_TOGGLE_UART

    RETURN

 

    
rpm_to_BCD
    
    BANKSEL TACH_RPMH 
    ; Clear BCD digit registers
    BANKSEL DIGIT_1000
    CLRF    DIGIT_1000
    CLRF    DIGIT_100
    CLRF    DIGIT_10
    CLRF    DIGIT_1
    
    
    ;Store in temp registers
    MOVF    TACH_RPMH, W
    MOVWF   TACH_toBCDH
    MOVF    TACH_RPML, W
    MOVWF   TACH_toBCDL
    
    
; Thousands
    
;   LOW(D'1000') = 232 (0xE8)
;   HIGH(D'1000') = 768 (0x03)
div_1000    
    MOVLW   LOW(D'1000')
    SUBWF   TACH_toBCDL, F
    MOVLW   HIGH(D'1000')
    SUBWFB  TACH_toBCDH, F      ; Subtract high byte, check if the previous  needed a borrow
    
    BTFSS   STATUS, C           ; Check carry flag STATUS reg (C=0 means borrow/negative, c=1 subtraction succsessful)
    GOTO    div_1000_undo	; 0 
    INCF    DIGIT_1000, F	; 1 increment
    GOTO    div_1000

    
div_1000_undo  ; undo final subtraction
    MOVLW   LOW(D'1000')      
    ADDWF   TACH_toBCDL, F
    MOVLW   HIGH(D'1000')
    ADDWFC  TACH_toBCDH, F      ; Add with carry
    
; Hundreds
   
div_100    
    MOVLW   LOW(D'100')
    SUBWF   TACH_toBCDL, F
    MOVLW   HIGH(D'100')
    SUBWFB  TACH_toBCDH, F      
    
    BTFSS   STATUS, C            
    GOTO    div_100_undo	; 0 
    INCF    DIGIT_100, F	; 1 increment
    GOTO    div_100
    
div_100_undo  
    MOVLW   LOW(D'100')      
    ADDWF   TACH_toBCDL, F
    MOVLW   HIGH(D'100')
    ADDWFC  TACH_toBCDH, F    
    
; Tens
    
div_10
    MOVLW   D'10'
    SUBWF   TACH_toBCDL, F
    MOVLW   D'0'
    SUBWFB  TACH_toBCDH, F      ; 16-bit subtraction for the 10s too
    
    BTFSS   STATUS, C
    GOTO    div_10_undo
    
    INCF    DIGIT_10, F
    GOTO    div_10
    
div_10_undo
    MOVLW   D'10'
    ADDWF   TACH_toBCDL, F
    MOVLW   D'0'
    ADDWFC  TACH_toBCDH, F
 
;   Ones   
    BANKSEL TACH_toBCDL
    MOVF    TACH_toBCDL, W
    BANKSEL DIGIT_1
    MOVWF   DIGIT_1
    
    RETURN
    
    
    
    
    
LIST    p=PIC16F18877		
    #INCLUDE <p16f18877.inc>
    
    ; CONFIG1
    __CONFIG _CONFIG1, _FEXTOSC_HS & _RSTOSC_EXT1X & _CLKOUTEN_OFF & _CSWEN_OFF & _FCMEN_OFF
    ; CONFIG2
    __CONFIG _CONFIG2, _MCLRE_ON & _PWRTE_OFF & _LPBOREN_OFF & _BOREN_OFF & _BORV_LO & _ZCD_OFF & _PPS1WAY_OFF & _STVREN_ON
    ; CONFIG3
    __CONFIG _CONFIG3, _WDTCPS_WDTCPS_31 & _WDTE_OFF & _WDTCWS_WDTCWS_7 & _WDTCCS_SC
    ; CONFIG4
    __CONFIG _CONFIG4, _WRT_OFF & _SCANE_not_available & _LVP_ON
    ; CONFIG5
    __CONFIG _CONFIG5, _CP_OFF & _CPD_OFF
    

ADC_4BIT       EQU 0x70
ADC_H8BIT      EQU 0x71   
ADC_L2BIT      EQU 0x72    
DIP_4BIT       EQU 0x73  
SEVEN_SEG_4BIT EQU 0x74
DELAY_S	       EQU 0x75
DELAY_MS       EQU 0x76
DELAY_US       EQU 0x77
TIMER_OVERFLOW_COUNTER EQU 0x78
TIMER_TOGGLE   EQU 0x79
MODE           EQU 0x7A

TACH_PERIODL   EQU 0x20    
TACH_PERIODH   EQU 0x21    
TACH_PREVL     EQU 0x22    
TACH_PREVH     EQU 0x23    
TACH_RPML      EQU 0x24    
TACH_RPMH      EQU 0x25    

DIGIT_1000     EQU 0x26    
DIGIT_100      EQU 0x27    
DIGIT_10       EQU 0x28    
DIGIT_1        EQU 0x29	   
	   
	   

   
   
	   
 
    ORG 0x00
    GOTO begin
 
    #INCLUDE <adc.inc>
    #INCLUDE <buzzer.inc>
    #INCLUDE <config.inc>
    #INCLUDE <delays.inc>
    #INCLUDE <dip.inc>
    #INCLUDE <fanspeed.inc>
    #INCLUDE <mode.inc>
    #INCLUDE <seven_seg_disp.inc>
    #INCLUDE <timer.inc>
	

begin
    CALL    init_ports      
    CALL    init_adc        
    CALL    init_buzzer_PWM
    CALL    init_timer1
    CALL    init_fanspeed_PWM
    
    BANKSEL MODE
    CLRF    MODE            ; Default to DIP Switch mode (0)
    CLRF    SEVEN_SEG_4BIT
    
    BANKSEL LATB
    BCF     LATB, 0         ; Status LED ON (Active-Low for DIP Mode)

MAIN_LOOP
    CALL    check_reset	    ; emergency reset
    CALL    adc_convert     ; POT to 10-bit (Left justified to 8-bit)
    CALL    adc_4bit        ; 8-bit ADRESH to 4-bit
    CALL    read_dip        ; Read PORTC and process DIP_4BIT
    CALL    press_button    ; Check RE1 for mode toggle
    CALL    mode_select     ; Select between ADC_4BIT and DIP_4BIT
    CALL    disp_seven_seg  ; Update 7-segment display
    CALL    timer1_run	    ; TIMER_TOGGLE from 0 to 1 every second
    CALL    check_alarmState; Check if ADC_4BIT >= 12
    CALL    adjust_fanspeed ; ADRESH to CCPR2H, ADRESL to CCPR2L to modify pulse width
    GOTO    MAIN_LOOP       
    
    END
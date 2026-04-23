.section ".data"
.equ	DDRB,0x04
.equ	PORTB,0x05
.equ	CLK,5	;Clock
.equ	DIN,3	;Data Input
.equ	CS,2	;Chip Select
.equ	BL,1	;Backlight
.equ	RST,0	;Reset
    
.equ	DDRD,0x0A
.equ	PORTD,0x0B
.equ	DC,7	;Data Control 0 for commands 1 for DIN/screen update
    
.equ	SREG,0x3F
.equ	CARRY,0
.equ	ZERO,1

.global xh    
.global xl
    
.global yh 
.global yl 
    
.global xh2  
.global xl2
    
.global yh2 
.global yl2 
    
.global colorh
.global colorl

.section ".text"

.global pinInit
pinInit:
    sbi DDRD, DC    ;configures DC as output and sets it as outputting
    clr PORTD	    ;clears PORTD
    
    sbi DDRB, CLK   ;configuring all PORTB outputs as output registers
    sbi DDRB, DIN
    sbi DDRB, CS
    sbi DDRB, BL
    sbi DDRB, RST
    
    clr PORTB	    ;ensures CS is off so LCD will recieve input
    sbi PORTB, BL   ;turns on backlight
    cbi PORTB, RST  ;reset must be held high for proper function, Set low for restart
    nop
    nop
    ret

.global softReset
softReset:
    sbi PORTB, RST  ;Reset terminated
    ldi r17, 0x01   ;Soft Reset
    call sendCommand
    ret
 
.global sleepOut
sleepOut:
    ldi r17, 0x11   ;Sleep out
    call sendCommand
    ret
    
.global screenInit    
screenInit:
    ldi r17, 0xC0   ;power control
    call sendCommand
    ldi r17, 0x23
    call sendData
    
    ;power control2
    ldi r17, 0xC1
    call sendCommand
    ldi r17, 0x10
    call sendData
    
    ;VCOM Control
    ldi r17, 0xC5
    call sendCommand
    ldi r17, 0x3E
    call sendData
    ldi r17, 0x28
    call sendData
    
    ;MADCTL
    ldi r17, 0x36
    call sendCommand
    ldi r17, 0x88
    call sendData
    
    ;16bit RGB
    ldi r17, 0x3A
    call sendCommand
    ldi r17, 0x55
    call sendData
    
    ;Frame Control
    ldi r17, 0xB1
    call sendCommand
    ldi r17, 0x00
    call sendData
    ldi r17, 0x1F
    call sendData
    
    ldi r17, 0x13   ;Display mode on
    call sendCommand
    ldi r17, 0x29   ;Display on
    call sendCommand
    
    sbi PORTD, DC   ;sets DC on for DIN digestion
    ret
    
.global drawPixel
drawPixel:
    ldi r17, 0x2A   ;Set Horizontal values
    call sendCommand
    lds r17, xh
    call sendData
    lds r17, xl
    call sendData
    lds r17, xh
    call sendData
    lds r17, xl
    inc r17
    call sendData
    
    ldi r17, 0x2B   ;Set Horizontal values
    call sendCommand
    lds r17, yh
    call sendData
    lds r17, yl
    call sendData
    
    cpi r17, 255
    brne not255
    
    lds r17, yh
    inc r17
    call sendData
    ldi r17, 0
    call sendData
    jmp sendColors
    
not255:
    lds r17, yh
    call sendData
    lds r17, yl
    inc r17
    call sendData
sendColors:
    
    ldi r17, 0x2C   ;pixel data write command
    call sendCommand
    
    lds r17, colorh ;colorh register
    call sendData
    lds r17, colorl ;colorl register
    call sendData

    ret
    
.global drawBackground
drawBackground:
    ldi r17, 0x2A   ;Set Horizontal values
    call sendCommand
    ldi r17, 0
    call sendData
    ldi r17, 0
    call sendData
    ldi r17, 0
    call sendData
    ldi r17, 0xEF
    call sendData
    
    ldi r17, 0x2B   ;Set Horizontal values
    call sendCommand
    ldi r17, 0
    call sendData
    ldi r17, 0
    call sendData
    ldi r17, 0x01
    call sendData
    ldi r17, 0x3F
    call sendData
    
    ldi r17, 0x2C   ;pixel data write command
    call sendCommand
    
    
    ldi r18,240
drawback3:
    ldi r19,160
drawback2:
    ldi r20,2
drawback1:
    lds r17, colorh ;colorh register
    call sendData
    lds r17, colorl ;colorl register
    call sendData
    dec r20
    brne drawback1
    dec r19
    brne drawback2
    dec r18
    brne drawback3
    ret
    
.global drawBlock
drawBlock:
    ldi r17, 0x2A   ;Set Horizontal values
    call sendCommand
    lds r17, xh
    call sendData
    lds r17, xl
    call sendData
    lds r17, xh2
    call sendData
    lds r17, xl2
    ldi r16, 10
    add r17, r16
    call sendData
    
    ldi r17, 0x2B   ;Set Horizontal values
    call sendCommand
    lds r17, yh
    call sendData
    lds r17, yl
    call sendData
    lds r17, yh2
    call sendData
    lds r17, yl2
    ldi r16, 10
    add r17, r16
    call sendData
    
    ldi r17, 0x2C   ;pixel data write command
    call sendCommand
    
    ldi r18, 100 
pixelloop:
    lds r17, colorh ;colorh register
    call sendData
    lds r17, colorl ;colorl register
    call sendData
    dec r18
    brne pixelloop

    ret

sendData:
    sbi PORTD, DC
    call sendbyte
    ret
    
sendCommand:
    cbi PORTD, DC
    call sendbyte
    ret
    
sendbyte:
    ldi r16, 8
inputloop:
    /*cbi PORTB, DIN
    lsl r17
    in r1, SREG
    sbrc r1, CARRY
    sbi PORTB, DIN
    call clockcycle
    dec r16
    brne inputloop*/
    
    cbi PORTB, DIN
    sbrc r17, 7
    sbi PORTB, DIN
    call clockcycle
    cbi PORTB, DIN
    sbrc r17, 6
    sbi PORTB, DIN
    call clockcycle
    cbi PORTB, DIN
    sbrc r17, 5
    sbi PORTB, DIN
    call clockcycle
    cbi PORTB, DIN
    sbrc r17, 4
    sbi PORTB, DIN
    call clockcycle
    cbi PORTB, DIN
    sbrc r17, 3
    sbi PORTB, DIN
    call clockcycle
    cbi PORTB, DIN
    sbrc r17, 2
    sbi PORTB, DIN
    call clockcycle
    cbi PORTB, DIN
    sbrc r17, 1
    sbi PORTB, DIN
    call clockcycle
    cbi PORTB, DIN
    sbrc r17, 0
    sbi PORTB, DIN
    call clockcycle
    ret
    
clockcycle:
    sbi PORTB,CLK
    cbi PORTB,CLK
    ret
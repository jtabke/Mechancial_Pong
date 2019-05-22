'****************************************************************
'*  Name    : pic_pong.BAS                                      *
'*  Author  : MECH 307 Group 23                                 *
'*  Notice  : Copyright (c) 2019 [select VIEW...EDITOR OPTIONS] *
'*          : All Rights Reserved                               *
'*  Date    : 5/8/2019                                          *
'*  Version : 1.0                                               *
'*  Notes   :                                                   *
'*          :                                                   *
'****************************************************************
'IdentIFy and set the internal oscillaTOr clock speed (required FOR the PIC16F88)
DEFINE OSC 8
OSCCON.4 = 1
OSCCON.5 = 1
OSCCON.6 = 1

' Turn off the A/D converter (required FOR the PIC16F88)
ANSEL = 0

P1_goal var PORTA.0
P2_goal var PORTA.1
P1_paddle var PORTA.2
P2_paddle var PORTA.3
Blue_ON var PORTA.4
Green_ON var PORTA.7


TRISA = %11111111 'set RA0-RA7 TO inputs

i var BYTE 'counter variable

P1_green var PORTB.0
P1_red var PORTB.1
P1_blue var PORTB.2

P2_green var PORTB.7
P2_red var PORTB.6
P2_blue var PORTB.5

TRISB = %00000000 'set RB0-RB7 TO outputs


main:
	IF P1_goal = 1 THEN
    GOTO P1
    ENDIF
    IF P2_goal = 1 THEN
    GOTO P2
    ENDIF
	IF P1_paddle = 1 THEN 
	GOTO Paddle_1
	ENDIF
	IF P2_paddle = 1 THEN 
	GOTO Paddle_2
	ENDIF
	WHILE Blue_ON = 1
	HIGH P1_blue
	HIGH P2_blue
	WEND
    WHILE Green_ON = 1
	HIGH P1_green
	HIGH P2_green
	WEND      
    GOTO main
    END

P1:
    HIGH P2_red
    FOR i = 1 TO 3
        HIGH P1_blue
        PAUSE 200
        LOW P1_blue
        PAUSE 200
        NEXT i
    LOW P2_red
    GOTO main
    END

P2:
    HIGH P1_red
    FOR i = 1 TO 3
        HIGH P2_blue
        PAUSE 200
        LOW P2_blue
        PAUSE 200
        NEXT i
    LOW P1_red
    GOTO main
    END
	
Paddle_1:
	HIGH P1_green
	PAUSE 200
	LOW P1_green
	GOTO main
	END
	
Paddle_2:
	HIGH P2_green
	PAUSE 200
	LOW P2_green
	GOTO main
	END
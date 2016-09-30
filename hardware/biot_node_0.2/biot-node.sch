EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:atmelSAM
LIBS:valves
LIBS:biot-node-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Biot IMU Node"
Date ""
Rev "0"
Comp "Motion Capture Systems"
Comment1 "Jon. Kelly"
Comment2 "Modular Construction"
Comment3 "Biot Node 0.2"
Comment4 "Motion Capture Systems (c)2016"
$EndDescr
$Comp
L CONN_01X06 P5
U 1 1 57B543CA
P 7375 5025
F 0 "P5" H 7375 5375 50  0000 C CNN
F 1 "FTDI/Serial" V 7475 5025 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06" H 7375 5025 50  0001 C CNN
F 3 "" H 7375 5025 50  0000 C CNN
	1    7375 5025
	1    0    0    -1  
$EndComp
$Comp
L Led_Small D2
U 1 1 57B546DC
P 6475 3600
F 0 "D2" H 6425 3725 50  0000 L CNN
F 1 "PWR" H 6300 3500 50  0000 L CNN
F 2 "LEDs:LED_1206" V 6475 3600 50  0001 C CNN
F 3 "" V 6475 3600 50  0000 C CNN
	1    6475 3600
	0    1    -1   0   
$EndComp
$Comp
L Led_Small D1
U 1 1 57B58E2E
P 5575 3800
F 0 "D1" H 5525 3925 50  0000 L CNN
F 1 "SIGNAL" H 5400 3700 50  0000 L CNN
F 2 "LEDs:LED_1206" V 5575 3800 50  0001 C CNN
F 3 "" V 5575 3800 50  0000 C CNN
	1    5575 3800
	-1   0    0    -1  
$EndComp
Text GLabel 5175 3400 2    60   Input ~ 0
SDA
Text GLabel 5175 3500 2    60   Input ~ 0
SCLK
Text GLabel 5175 3600 2    60   Input ~ 0
RX
Text GLabel 5175 3700 2    60   Input ~ 0
TX
Text GLabel 3400 3725 0    60   Input ~ 0
SDA
$Comp
L MPU9250-Breakout U1
U 1 1 57B66AB0
P 3825 3925
F 0 "U1" H 3875 3250 60  0000 C CNN
F 1 "MPU9250-Breakout" V 4025 3950 60  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x10" V 3775 3775 60  0001 C CNN
F 3 "" V 3775 3775 60  0001 C CNN
	1    3825 3925
	1    0    0    -1  
$EndComp
Text GLabel 3400 3600 0    60   Input ~ 0
SCLK
$Comp
L CONN_02X05 P4
U 1 1 57B66BB2
P 7325 4175
F 0 "P4" H 7325 4475 50  0000 C CNN
F 1 "ATMEL-CORTEX" H 7325 3875 50  0000 C CNN
F 2 "atmelSAM:ATMEL_cortex_header_smd" H 7325 2975 50  0001 C CNN
F 3 "" H 7325 2975 50  0000 C CNN
	1    7325 4175
	1    0    0    -1  
$EndComp
Text GLabel 7575 3975 2    60   Input ~ 0
SWDIO
Text GLabel 7575 4075 2    60   Input ~ 0
SWDCLK
Text GLabel 7575 4375 2    60   Input ~ 0
NRST
Text GLabel 7175 5175 0    60   Input ~ 0
RX
Text GLabel 7175 5075 0    60   Input ~ 0
TX
$Comp
L R R2
U 1 1 57B66E31
P 5825 3800
F 0 "R2" V 5905 3800 50  0000 C CNN
F 1 "39" V 5825 3800 50  0000 C CNN
F 2 "Resistors_SMD:R_1210" V 5755 3800 50  0001 C CNN
F 3 "" H 5825 3800 50  0000 C CNN
	1    5825 3800
	0    1    1    0   
$EndComp
$Comp
L R R3
U 1 1 57B66E9C
P 6475 3350
F 0 "R3" V 6555 3350 50  0000 C CNN
F 1 "39" V 6475 3350 50  0000 C CNN
F 2 "Resistors_SMD:R_1210" V 6405 3350 50  0001 C CNN
F 3 "" H 6475 3350 50  0000 C CNN
	1    6475 3350
	1    0    0    -1  
$EndComp
Text GLabel 5000 5050 0    60   Input ~ 0
NRST
$Comp
L GND #PWR01
U 1 1 57B671DE
P 6475 3800
F 0 "#PWR01" H 6475 3550 50  0001 C CNN
F 1 "GND" H 6475 3650 50  0000 C CNN
F 2 "" H 6475 3800 50  0000 C CNN
F 3 "" H 6475 3800 50  0000 C CNN
	1    6475 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 57B6728D
P 6150 3800
F 0 "#PWR02" H 6150 3550 50  0001 C CNN
F 1 "GND" H 6150 3650 50  0000 C CNN
F 2 "" H 6150 3800 50  0000 C CNN
F 3 "" H 6150 3800 50  0000 C CNN
	1    6150 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR03
U 1 1 57B673C1
P 6825 4375
F 0 "#PWR03" H 6825 4125 50  0001 C CNN
F 1 "GND" H 6825 4225 50  0000 C CNN
F 2 "" H 6825 4375 50  0000 C CNN
F 3 "" H 6825 4375 50  0000 C CNN
	1    6825 4375
	1    0    0    -1  
$EndComp
Wire Wire Line
	6475 3700 6475 3800
Wire Wire Line
	5975 3800 6150 3800
Wire Wire Line
	5175 3800 5475 3800
Wire Wire Line
	5175 3300 6150 3300
Wire Wire Line
	6825 4375 7075 4375
NoConn ~ 7075 4275
NoConn ~ 7575 4175
NoConn ~ 7575 4275
$Comp
L GND #PWR04
U 1 1 57B674CA
P 6775 4775
F 0 "#PWR04" H 6775 4525 50  0001 C CNN
F 1 "GND" H 6775 4625 50  0000 C CNN
F 2 "" H 6775 4775 50  0000 C CNN
F 3 "" H 6775 4775 50  0000 C CNN
	1    6775 4775
	1    0    0    -1  
$EndComp
Wire Wire Line
	6775 4775 7175 4775
$Comp
L GND #PWR05
U 1 1 57B67543
P 2700 3475
F 0 "#PWR05" H 2700 3225 50  0001 C CNN
F 1 "GND" H 2700 3325 50  0000 C CNN
F 2 "" H 2700 3475 50  0000 C CNN
F 3 "" H 2700 3475 50  0000 C CNN
	1    2700 3475
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 3475 3400 3475
$Comp
L GND #PWR06
U 1 1 57B675A9
P 5925 5200
F 0 "#PWR06" H 5925 4950 50  0001 C CNN
F 1 "GND" H 5925 5050 50  0000 C CNN
F 2 "" H 5925 5200 50  0000 C CNN
F 3 "" H 5925 5200 50  0000 C CNN
	1    5925 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5925 5050 5925 5200
NoConn ~ 3400 3850
NoConn ~ 3400 3975
NoConn ~ 3400 4100
NoConn ~ 3400 4225
NoConn ~ 3400 4350
NoConn ~ 3400 4475
$Comp
L +3.3V #PWR07
U 1 1 57B676BA
P 5600 2700
F 0 "#PWR07" H 5600 2550 50  0001 C CNN
F 1 "+3.3V" H 5600 2840 50  0000 C CNN
F 2 "" H 5600 2700 50  0000 C CNN
F 3 "" H 5600 2700 50  0000 C CNN
	1    5600 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 3200 5175 3200
Wire Wire Line
	5600 2700 5600 3200
$Comp
L +3.3V #PWR08
U 1 1 57B67717
P 6775 3975
F 0 "#PWR08" H 6775 3825 50  0001 C CNN
F 1 "+3.3V" H 6775 4115 50  0000 C CNN
F 2 "" H 6775 3975 50  0000 C CNN
F 3 "" H 6775 3975 50  0000 C CNN
	1    6775 3975
	1    0    0    -1  
$EndComp
Wire Wire Line
	6775 3975 7075 3975
$Comp
L +3.3V #PWR09
U 1 1 57B677C3
P 6475 4850
F 0 "#PWR09" H 6475 4700 50  0001 C CNN
F 1 "+3.3V" H 6475 4990 50  0000 C CNN
F 2 "" H 6475 4850 50  0000 C CNN
F 3 "" H 6475 4850 50  0000 C CNN
	1    6475 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6475 2900 6475 3200
Wire Wire Line
	6475 4850 6475 4975
Wire Wire Line
	6475 4975 7175 4975
Wire Wire Line
	3275 3300 3275 3350
Wire Wire Line
	3275 3350 3400 3350
Wire Wire Line
	7075 4175 6825 4175
Wire Wire Line
	6825 4075 6825 4375
Wire Wire Line
	7075 4075 6825 4075
Connection ~ 6825 4175
Wire Wire Line
	6150 3300 6150 3800
NoConn ~ 7175 4875
NoConn ~ 7175 5275
$Comp
L PWR_FLAG #FLG010
U 1 1 57B69434
P 5900 2925
F 0 "#FLG010" H 5900 3020 50  0001 C CNN
F 1 "PWR_FLAG" H 5900 3105 50  0000 C CNN
F 2 "" H 5900 2925 50  0000 C CNN
F 3 "" H 5900 2925 50  0000 C CNN
	1    5900 2925
	1    0    0    -1  
$EndComp
Connection ~ 5600 2925
Connection ~ 5900 2925
Connection ~ 6475 2925
$Comp
L GND #PWR011
U 1 1 57B69BD1
P 6725 3025
F 0 "#PWR011" H 6725 2775 50  0001 C CNN
F 1 "GND" H 6725 2875 50  0000 C CNN
F 2 "" H 6725 3025 50  0000 C CNN
F 3 "" H 6725 3025 50  0000 C CNN
	1    6725 3025
	1    0    0    -1  
$EndComp
Wire Wire Line
	6725 3025 7100 3025
$Comp
L PWR_FLAG #FLG012
U 1 1 57B69C57
P 7100 3025
F 0 "#FLG012" H 7100 3120 50  0001 C CNN
F 1 "PWR_FLAG" H 7100 3205 50  0000 C CNN
F 2 "" H 7100 3025 50  0000 C CNN
F 3 "" H 7100 3025 50  0000 C CNN
	1    7100 3025
	-1   0    0    1   
$EndComp
Wire Wire Line
	5600 2925 7100 2925
$Comp
L CONN_01X07 P1
U 1 1 57C62951
P 4975 3500
F 0 "P1" H 4975 3900 50  0000 C CNN
F 1 "ATSAMR21B" V 5075 3500 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x07_Pitch2.00mm" H 4975 3500 50  0001 C CNN
F 3 "" H 4975 3500 50  0000 C CNN
	1    4975 3500
	-1   0    0    -1  
$EndComp
$Comp
L CONN_01X05 P2
U 1 1 57C62A9D
P 4975 4200
F 0 "P2" H 4975 4500 50  0000 C CNN
F 1 "ATSAMR21B-TEST-POINTs" H 5575 4200 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x05" H 4975 4200 50  0001 C CNN
F 3 "" H 4975 4200 50  0000 C CNN
	1    4975 4200
	1    0    0    -1  
$EndComp
Text GLabel 4775 4400 0    60   Input ~ 0
NRST
Text GLabel 4775 4100 0    60   Input ~ 0
SWDIO
Text GLabel 4775 4000 0    60   Input ~ 0
SWDCLK
$Comp
L +3.3V #PWR013
U 1 1 57B677F3
P 3275 3300
F 0 "#PWR013" H 3275 3150 50  0001 C CNN
F 1 "+3.3V" H 3275 3440 50  0000 C CNN
F 2 "" H 3275 3300 50  0000 C CNN
F 3 "" H 3275 3300 50  0000 C CNN
	1    3275 3300
	1    0    0    -1  
$EndComp
Text Notes 4525 4250 0    60   ~ 0
TP4
Text Notes 4525 4350 0    60   ~ 0
TP5
NoConn ~ 4775 4200
NoConn ~ 4775 4300
$Comp
L CONN_01X03 P3
U 1 1 57C7C389
P 7300 2925
F 0 "P3" H 7300 3125 50  0000 C CNN
F 1 "UP_CONVERTOR" V 7400 2925 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 7300 2925 50  0001 C CNN
F 3 "" H 7300 2925 50  0000 C CNN
	1    7300 2925
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR014
U 1 1 57C7C7D9
P 8375 3125
F 0 "#PWR014" H 8375 2875 50  0001 C CNN
F 1 "GND" H 8375 2975 50  0000 C CNN
F 2 "" H 8375 3125 50  0000 C CNN
F 3 "" H 8375 3125 50  0000 C CNN
	1    8375 3125
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P6
U 1 1 57C8AF85
P 7875 2525
F 0 "P6" H 7875 2725 50  0000 C CNN
F 1 "On/Off Header" V 7975 2525 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 7875 2525 50  0001 C CNN
F 3 "" H 7875 2525 50  0000 C CNN
	1    7875 2525
	0    1    1    0   
$EndComp
Wire Wire Line
	8375 2825 8375 2325
Wire Wire Line
	8375 2325 7975 2325
Wire Wire Line
	7100 2325 7100 2825
Wire Wire Line
	7100 2325 7875 2325
Connection ~ 7775 2325
Text Notes 8100 2850 0    60   ~ 0
1.5V
$Comp
L SW_PUSH SW1
U 1 1 57D0BB8E
P 5625 5050
F 0 "SW1" H 5775 5160 50  0000 C CNN
F 1 "RESET" H 5625 4970 50  0000 C CNN
F 2 "Buttons_Switches_SMD:SW_SPST_EVPBF" H 5625 5050 50  0001 C CNN
F 3 "" H 5625 5050 50  0000 C CNN
	1    5625 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5325 5050 5000 5050
Wire Wire Line
	5325 5050 5325 4875
$Comp
L R R1
U 1 1 57D0BE3E
P 5325 4725
F 0 "R1" V 5405 4725 50  0000 C CNN
F 1 "10k" V 5325 4725 50  0000 C CNN
F 2 "Resistors_SMD:R_1210" V 5255 4725 50  0001 C CNN
F 3 "" H 5325 4725 50  0000 C CNN
	1    5325 4725
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR015
U 1 1 57D0C048
P 5325 4575
F 0 "#PWR015" H 5325 4425 50  0001 C CNN
F 1 "+3.3V" H 5325 4715 50  0000 C CNN
F 2 "" H 5325 4575 50  0000 C CNN
F 3 "" H 5325 4575 50  0000 C CNN
	1    5325 4575
	1    0    0    -1  
$EndComp
$Comp
L Battery BT1
U 1 1 57D0C172
P 8375 2975
F 0 "BT1" H 8475 3025 50  0000 L CNN
F 1 "1.5V" H 8475 2925 50  0000 L CNN
F 2 "atmelSAM:AAA_x1_Battery_Holder" V 8375 3015 50  0001 C CNN
F 3 "" V 8375 3015 50  0000 C CNN
	1    8375 2975
	1    0    0    -1  
$EndComp
$EndSCHEMATC
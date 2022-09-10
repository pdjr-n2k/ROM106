EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 9
Title "SIM108 Switch Input Module"
Date "2022-06-11"
Rev "2.0.0"
Comp "PDJR <preeve@pdjr.eu>"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Isolator:SFH617A-1 U5
U 1 1 62EBA913
P 6100 3400
AR Path="/62FB7846/62EBA913" Ref="U5"  Part="1" 
AR Path="/63089671/62EBA913" Ref="U6"  Part="1" 
AR Path="/63103CAF/62EBA913" Ref="U7"  Part="1" 
AR Path="/62F64647/62EBA913" Ref="U8"  Part="1" 
AR Path="/62FD589D/62EBA913" Ref="U9"  Part="1" 
AR Path="/63041ECA/62EBA913" Ref="U10"  Part="1" 
AR Path="/62F45EE0/62EBA913" Ref="U11"  Part="1" 
AR Path="/62FB890E/62EBA913" Ref="U12"  Part="1" 
F 0 "U7" H 6100 3200 50  0000 C CNN
F 1 "SFH617A-1" H 6100 3600 50  0000 C CNN
F 2 "Package_DIP:DIP-4_W7.62mm" H 5900 3200 50  0001 L CIN
F 3 "http://www.vishay.com/docs/83740/sfh617a.pdf" H 6100 3400 50  0001 L CNN
	1    6100 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 3500 5700 3500
$Comp
L Device:R R12
U 1 1 62ED7963
P 5300 3500
AR Path="/62FB7846/62ED7963" Ref="R12"  Part="1" 
AR Path="/63089671/62ED7963" Ref="R14"  Part="1" 
AR Path="/63103CAF/62ED7963" Ref="R16"  Part="1" 
AR Path="/62F64647/62ED7963" Ref="R18"  Part="1" 
AR Path="/62FD589D/62ED7963" Ref="R20"  Part="1" 
AR Path="/63041ECA/62ED7963" Ref="R22"  Part="1" 
AR Path="/62F45EE0/62ED7963" Ref="R24"  Part="1" 
AR Path="/62FB890E/62ED7963" Ref="R26"  Part="1" 
F 0 "R16" H 5350 3500 50  0000 L CNN
F 1 "10K 0.5W" V 5300 3500 24  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 5230 3500 50  0001 C CNN
F 3 "~" H 5300 3500 50  0001 C CNN
F 4 "0.125W" V 5400 3500 50  0001 C CNN "Power"
	1    5300 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 3350 5300 3300
Wire Wire Line
	5300 3300 5800 3300
Wire Wire Line
	5300 3650 5300 3700
Wire Wire Line
	5300 3700 5400 3700
Wire Wire Line
	5300 3300 5100 3300
Connection ~ 5300 3300
$Comp
L Device:R R13
U 1 1 62F493CD
P 5700 4050
AR Path="/62FB7846/62F493CD" Ref="R13"  Part="1" 
AR Path="/63089671/62F493CD" Ref="R15"  Part="1" 
AR Path="/63103CAF/62F493CD" Ref="R17"  Part="1" 
AR Path="/62F64647/62F493CD" Ref="R19"  Part="1" 
AR Path="/62FD589D/62F493CD" Ref="R21"  Part="1" 
AR Path="/63041ECA/62F493CD" Ref="R23"  Part="1" 
AR Path="/62F45EE0/62F493CD" Ref="R25"  Part="1" 
AR Path="/62FB890E/62F493CD" Ref="R27"  Part="1" 
F 0 "R17" H 5750 4050 50  0000 L CNN
F 1 "100R" V 5700 4050 39  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 5630 4050 50  0001 C CNN
F 3 "~" H 5700 4050 50  0001 C CNN
F 4 "0.125W" V 5800 4050 50  0001 C CNN "Power"
	1    5700 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 4100 5300 4200
$Comp
L Transistor_BJT:BC337 Q2
U 1 1 62EA87F7
P 5600 3700
AR Path="/62FB7846/62EA87F7" Ref="Q2"  Part="1" 
AR Path="/63089671/62EA87F7" Ref="Q4"  Part="1" 
AR Path="/63103CAF/62EA87F7" Ref="Q6"  Part="1" 
AR Path="/62F64647/62EA87F7" Ref="Q8"  Part="1" 
AR Path="/62FD589D/62EA87F7" Ref="Q10"  Part="1" 
AR Path="/63041ECA/62EA87F7" Ref="Q12"  Part="1" 
AR Path="/62F45EE0/62EA87F7" Ref="Q14"  Part="1" 
AR Path="/62FB890E/62EA87F7" Ref="Q16"  Part="1" 
F 0 "Q6" H 5791 3746 50  0000 L CNN
F 1 "BC337" H 5791 3655 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 5800 3625 50  0001 L CIN
F 3 "https://diotec.com/tl_files/diotec/files/pdf/datasheets/bc337.pdf" H 5600 3700 50  0001 L CNN
	1    5600 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 3900 5700 3900
Connection ~ 5700 3900
Wire Wire Line
	5700 4200 5300 4200
Connection ~ 5300 4200
$Comp
L Simulation_SPICE:DIODE D10
U 1 1 62FE9B9E
P 5100 3500
AR Path="/62FB7846/62FE9B9E" Ref="D10"  Part="1" 
AR Path="/63089671/62FE9B9E" Ref="D11"  Part="1" 
AR Path="/63103CAF/62FE9B9E" Ref="D12"  Part="1" 
AR Path="/62F64647/62FE9B9E" Ref="D13"  Part="1" 
AR Path="/62FD589D/62FE9B9E" Ref="D14"  Part="1" 
AR Path="/63041ECA/62FE9B9E" Ref="D15"  Part="1" 
AR Path="/62F45EE0/62FE9B9E" Ref="D16"  Part="1" 
AR Path="/62FB890E/62FE9B9E" Ref="D17"  Part="1" 
F 0 "D12" V 5150 3600 50  0000 L CNN
F 1 "IN4148" V 5050 3600 50  0000 L CNN
F 2 "Diode_THT:D_A-405_P2.54mm_Vertical_AnodeUp" H 5100 3500 50  0001 C CNN
F 3 "~" H 5100 3500 50  0001 C CNN
F 4 "Y" H 5100 3500 50  0001 L CNN "Spice_Netlist_Enabled"
F 5 "D" H 5100 3500 50  0001 L CNN "Spice_Primitive"
	1    5100 3500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5100 3350 5100 3300
Wire Wire Line
	5000 4200 5100 4200
Connection ~ 5100 3300
Wire Wire Line
	5100 3300 5000 3300
Wire Wire Line
	5100 3650 5100 4200
Connection ~ 5100 4200
Wire Wire Line
	5100 4200 5300 4200
Wire Wire Line
	6400 3500 6500 3500
Wire Wire Line
	6400 3300 6500 3300
$Comp
L Transistor_BJT:BC547 Q1
U 1 1 62FB7374
P 5400 3900
AR Path="/62FB7846/62FB7374" Ref="Q1"  Part="1" 
AR Path="/63089671/62FB7374" Ref="Q3"  Part="1" 
AR Path="/63103CAF/62FB7374" Ref="Q5"  Part="1" 
AR Path="/62F64647/62FB7374" Ref="Q7"  Part="1" 
AR Path="/62FD589D/62FB7374" Ref="Q9"  Part="1" 
AR Path="/63041ECA/62FB7374" Ref="Q11"  Part="1" 
AR Path="/62F45EE0/62FB7374" Ref="Q13"  Part="1" 
AR Path="/62FB890E/62FB7374" Ref="Q15"  Part="1" 
F 0 "Q5" H 5591 3946 50  0000 L CNN
F 1 "BC547" H 5591 3855 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 5600 3825 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/BC550-D.pdf" H 5400 3900 50  0001 L CNN
	1    5400 3900
	-1   0    0    -1  
$EndComp
Connection ~ 5300 3700
Text HLabel 5000 3300 0    50   Input ~ 0
OII_IN
Text HLabel 5000 4200 0    50   UnSpc ~ 0
OII_IN_GND
Text HLabel 6500 3300 2    50   Output ~ 0
OII_OUT
Text HLabel 6500 3500 2    50   UnSpc ~ 0
OII_OUT_GND
$EndSCHEMATC

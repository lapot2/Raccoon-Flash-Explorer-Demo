;String table for messages

;	IC Info labels (for all modes)

	CSEG AT 0F000H
	
; struct{
;		  char all_length;
;		  char label1_len;
;		  char label1_str[];
;		  char param1_len;
;		  char param1_str[];
;		  ...
;		};

	PUBLIC text_label_main
text_label_main:
	DB	5; total strings count
	DB	9,"HW MODE: ",	39; _at_ 0xf000;//AUTO 1.8v Spi Flash
	DB	9,"IC Type: ",	23; _at_ 0xf040;//MX25U512   
	DB	9,"IC ID--: ",	23; _at_ 0xf060;//C2253A
	DB	9,"IC Misc: ",	23; _at_ 0xf080;//SPI/DSPI/QSPI/QPI
	DB	0;\r\n
		
	PUBLIC text_label_warning
text_label_warning:
	;;;;;;; new method
	DB	15,"----Warning----"

		PUBLIC text_label_ic_size
text_label_ic_size:
	DB	6
	DB	9,"IC Size: ",	23; _at_ 0xf0a0;//by ID
	DB	7,"MB---: ",	17; _at_ 0xf0c0;//64 
	DB	7,"KiB--: ",	17; _at_ 0xf100;//65536
	DB	7,"Bytes: ",	17; _at_ 0xf120;//67108864
	DB	7,"Hex--: ",	17; _at_ 0xf140;//0x4000000
	DB	0;\r\n
	
	;char code Reg1Set1[8][9] 328b
	PUBLIC Reg1Set1
Reg1Set1:
	DB	10
	DB	9,"Reg 1: 0x", 2
	DB	8," BUSY = ",1
	DB	8," WEL  = ",1
	DB	8," BP0  = ",1
	DB	8," BP1  = ",1
	DB	8," BP2  = ",1
	DB	8," TB   = ",1
	DB	8," SEC  = ",1
	DB	8," SRP0 = ",1
	DB	0;\r\n
		
	;char code Reg1Set2[8][9]
	PUBLIC Reg1Set2
Reg1Set2:	; for MXIC
	DB	10
	DB	9,"Reg 1: 0x", 2
	DB	8," BUSY = ",1
	DB	8," WEL  = ",1
	DB	8," BP0  = ",1
	DB	8," BP1  = ",1
	DB	8," BP2  = ",1
	DB	8," BP3  = ",1
	DB	8," QE   = ",1
	DB	8," SRP  = ",1
	DB	0;\r\n

	;char code Reg2Set1[8][9]
	PUBLIC Reg2Set1
Reg2Set1:
	DB	10
	DB	9,"Reg 2: 0x", 2
	DB	8," SRP1 = ",1
	DB	8," QE   = ",1
	DB	8," LB0  = ",1
	DB	8," LB1  = ",1
	DB	8," LB2  = ",1
	DB	8," LB3  = ",1
	DB	8," CMP  = ",1
	DB	8," SUS  = ",1
	DB	0;\r\n

	;char code Reg3Set1[8][9]
	PUBLIC Reg3Set1
Reg3Set1:
	DB	10
	DB	9,"Reg 3: 0x", 2
	DB	8," Bit0 = ",1
	DB	8," Bit1 = ",1
	DB	8," Bit2 = ",1
	DB	8," Bit3 = ",1
	DB	8," Bit4 = ",1
	DB	8," Bit5 = ",1
	DB	8," Bit6 = ",1
	DB	8," Bit7 = ",1
	DB	0;\r\n
		
label_sfdp_info:
	DB	2
	DB	11,"SFDP Info: ",10
	DB	0
		
label_speed:
	DB	2
	DB	7,"Speed: ",10
	DB	0

//for i2c & spi eeprom
		PUBLIC text_label_i2c_mem_layout
text_label_i2c_mem_layout:
	DB	4
	;DB	15,"Memory Layout: ",8
	DB	7, "Banks: ",8
	DB	7, "Bytes: ",8
	DB	7, "Pages: ",8
	DB	0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; PARAM Labels ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; struct{
;		  char label_len;
;		  char label_text[];
;		}

		PUBLIC label_pin
		PUBLIC label_nc_text
		PUBLIC label_shorted
label_pin:
	DB	3,"Pin"
label_nc_text:
	DB	14," Not connected"
label_shorted:
	DB	9, " Short to"
		
		PUBLIC label_auto
		PUBLIC label_manual
label_auto:
	DB	4,"ONLY"
label_manual:
	DB	6,"Manual"
		
		PUBLIC label_mhz
		PUBLIC label_mv
label_mhz:
	DB	3,"MHz"
label_mv:
	DB	2,"mV"
		
		PUBLIC label_v18
		PUBLIC label_v33
label_v18:
	DB	4,"1.8v"
label_v33:
	DB	4,"3.3v"
		
		PUBLIC label_autoneg
		PUBLIC label_id_error
		PUBLIC label_ver_error
label_autoneg:
	DB	16,"Autoset speed by"
label_id_error:
	DB	8,"ID Error"
label_ver_error:
	DB	12,"Verify Error"
		
		PUBLIC label_at
		PUBLIC label_hex_0x
label_at:
	DB	2,"at"
label_hex_0x:
	DB	2,"0x"
		
		PUBLIC label_magic
		PUBLIC label_ping
		PUBLIC label_sfdp
		PUBLIC label_id
label_magic:
	DB	5,"Magic"
label_ping:
	DB	4,"Ping"
label_sfdp:
	DB	4,"SFDP"
label_id:
	DB	2,"ID"
		
		PUBLIC label_qpi
		PUBLIC label_qspi
		PUBLIC label_dspi
label_qpi:
	DB	3,"QPI"
label_qspi:
	DB	4,"QSPI"
label_dspi:
	DB	4,"DSPI"
		
		PUBLIC label_spi
		PUBLIC label_i2c
		PUBLIC label_twi
		PUBLIC label_mmc
		PUBLIC label_flash
		PUBLIC label_eeprom
		PUBLIC label_emmc
		PUBLIC label_spinand
label_spi:
	DB	3,"SPI"
label_i2c:
	DB	3,"I2C"
label_twi:
	DB	3,"TWI"
label_mmc:
	DB	3,"MMC"
label_flash:
	DB	5,"Flash"
label_eeprom:
	DB	6,"Eeprom"
label_emmc:
	DB	4,"eMMC"
label_spinand:
	DB	4,"NAND"
		
label_fuck:
	DB	19,"This fucking bit is"; always on!"
label_always:
	DB	6,"always on!"
label_fuckon:
	DB	3,"on!"

	PUBLIC label_overload
	PUBLIC label_chip180
	PUBLIC label_bus_error
label_overload:
	;DB	19,"OVERLOAD PROTECTION"
	DB	27,"!!! OVERLOAD PROTECTION !!!"
label_chip180:
	DB	26,"CHIP MAYBE IS WRAP TO 180°"
label_bus_error:
	DB	9,"BUS Error"
		
	;PUBLIC label_null
;label_null:
	;DB	0
		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Pinout ;;;;;;;;;;;;;;;;;;;;;;;;;;;
	PUBLIC label_pinout
label_pinout:
	DB	11
	DB	22,"     _____   _____    ",0
	DB	22,"    | o   |_|     |   ",0;	pin_ptr[0] = scsibuf+buf_ptr + 5;	pin_ptr[7] = pin_ptr[0] + 8;
	DB	22," 1--| Yes     Yes |--8",0;
	DB	22,"    |             |   ",0;	pin_ptr[1] = scsibuf+buf_ptr + 5;	pin_ptr[6] = pin_ptr[1] + 8;
	DB	22," 2--| Yes     Yes |--7",0;
	DB	22,"    |             |   ",0;	pin_ptr[2] = scsibuf+buf_ptr + 5;	pin_ptr[5] = pin_ptr[2] + 8;
	DB	22," 3--| Yes     Yes |--6",0;
	DB	22,"    |             |   ",0;	pin_ptr[3] = scsibuf+buf_ptr + 5;	pin_ptr[4] = pin_ptr[3] + 8;
	DB	22," 4--| Yes     Yes |--5",0;
	DB	22,"    |_____________|   ",0;
	DB	0


		PUBLIC label_ok
		PUBLIC label_nc
		PUBLIC label_error
		PUBLIC label_vcc
		PUBLIC label_gnd
label_ok:
	DB	5," Yes "
label_nc:
	DB	5,"*N/C*"
label_error:
	DB	5,"Error"
label_vcc:
	DB	3,"VCC"
label_gnd:
	DB	3,"GND"

;;;;;;;;;;;;;;;;;;;;;;; old style spi
	;PUBLIC label_spi_cs
	;PUBLIC label_spi_mosi
	;PUBLIC label_spi_wp
	;PUBLIC label_spi_gnd
	;PUBLIC label_spi_miso
	;PUBLIC label_spi_sclk
	;PUBLIC label_spi_hold
	;PUBLIC label_spi_vcc
;label_spi_cs:
	;DB	3,"/CS"
;label_spi_mosi:
	;DB	4,"MOSI"
;label_spi_wp:
	;DB	3,"/WP"
;label_spi_gnd:
	;DB	3,"GND"
;label_spi_miso:
	;DB	5," MISO"
;label_spi_sclk:
	;DB	5," SCLK"
;label_spi_hold:
	;DB	5,"/HOLD"
;label_spi_vcc:
	;DB	5,"  VCC"
		
;;;;;;;;;;;;;;;;;;;;;;; 8x6 array
	PUBLIC label_spi_pins_arr
label_spi_pins_arr:
	DB	5,"/CS  "
	DB	5,"MOSI "
	DB	5,"/WP  "
	DB	5,"GND  "
	DB	5," MISO"
	DB	5," SCLK"
	DB	5,"/HOLD"
	DB	5,"  VCC"
	
;label_twi_cs:
	;DB	2,"CS"
;label_twi_sclk:
	;DB	2,"SK"
;label_twi_mosi:
	;DB	2,"DI"
;label_miso:
	;DB	2,"DO"
;;label_twi_gnd:
	;;DB	3,"GND"
;label_twi_org:
	;DB	3,"ORG"
;label_twi_wp:
	;DB	2,"NC"
;;label_twi_vcc:
	;;DB	3,"VCC"
	
;;;;;;;;;;;;;;;;;;;;;;; old style i2c
;label_i2c_a0:
	;DB	2,"A0"
;label_i2c_a1:
	;DB	2,"A1"
;label_i2c_a2:
	;DB	2,"A2"
;;label_i2c_gnd:
	;;DB	3,"GND"
;label_i2c_sda:
	;DB	3,"SDA"
;label_i2c_scl:
	;DB	3,"SCL"
;label_twi_wp:
	;DB	2,"WP"
;;label_twi_vcc:
	;;DB	3,"VCC"
	
;;;;;;;;;;;;;;;;;;;;;;; 8x6 array
	PUBLIC label_i2c_pins_arr
label_i2c_pins_arr:
	DB	5,"A0   "
	DB	5,"A1   "
	DB	5,"A2   "
	DB	5,"GND  "
	DB	5,"  SDA"
	DB	5,"  SCL"
	DB	5,"   WP"
	DB	5,"  VCC"
		
;;label_mmc_cs:;--------pin 1 NC
	;;DB	3,"/CS"
;label_mmc_dat0_ret:;---pin 2 Dat0 return
	;DB	5,"DATA0"
;label_mmc_cmd:;--------pin 3 Cmd
	;DB	3,"CMD"
;;label_mmc_gnd:;-------pin 4 GND
	;;DB	3,"GND"
;label_mmc_dat0:;-------pin 5 Dat0
	;DB	5,"DATA0"
;label_mmc_sclk:;-------pin 6 Clk
	;DB	3,"CLK"
;label_mmc_vccq:;------pin 8 1.8v
	;DB	4,"VCCQ"
;label_mmc_vcc:;-------ext 3.3v from any pin
	;DB	3,"VCC"
	
;	IO test labels
	PUBLIC label_io_test1
	PUBLIC label_io_test2
label_io_test1:
	DB	15,"----Test IO----"
label_io_test2:
	DB	15,"Level: High/Low"
	;DB	18,"Level: High  | Low"
;label_io_test3:
	;DB	21,"-------------|-------"
;label_io_spacer1:
	;DB	1,":"
;label_io_spacer2:
	;DB	1,"|"
;label_io_spacer3:
	;DB	1,": "

		
;	Other labels..
	PUBLIC label_about
	PUBLIC label_host
	PUBLIC label_windows
	PUBLIC label_linux
	PUBLIC label_apple
	PUBLIC label_build
	PUBLIC label_version
	PUBLIC label_serial_number
	PUBLIC label_hello
	PUBLIC label_copyright
label_about:
	;DB	15,"----Warning----"
	DB	15,"-----About-----"
label_host:
	DB	5,"Host:"
label_windows:
	DB	7,"Windows"
label_linux:
	DB	13,"Linux/Android"
label_apple:
	DB	5,"Apple"
label_build:
	DB	24,"Build: ", __DATE2__, " ", __TIME__
label_version:
	DB	18,"FW Ver: 10D0 Demo "
label_serial_number:
	DB	4,"S/N:"
label_hello:
	DB	37,"You are using a ra©©oon programmer :D"
label_copyright:
	DB	21,"Raccoon(S)© 2024-2025"
		
	
	END
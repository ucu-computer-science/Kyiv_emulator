# Kyiv_emulator

## Description
This project aims to create an emulator of one of the oldest computers in Europe - "ЕОМ Київ"(Kyiv).
"Kyiv" was developed in 1957. The developers team included Kateryna Yushchenko, Viktor Glushkov, Boris Gnatenko, Kateryna Shkabara and others.
Though, in terms of memory and reliability "Kyiv" wasn't the best for its times, but its architecture, in terms of ease of writing programs, was advanced and actually paved the way for new types of computers that are much more powerful than its peers, as computers of that time were intended exclusively for calculations and programming in machine code and low-level languages.

"Kyiv" is the world's first computer, which from the very beginning laid down the needs of programmers, in particular: the ability to easily implement compilers, text processing, asynchronous control, image processing, the ability to solve logical problems and implement information systems.

Kateryna Yushchenko, one of the "Kyiv"'s inventors, according to pr. Yuiriy Yushchenko, implemented poiners in the machine before it is officially considered to. Also, according to A.P. Oleh Fareniuk, "Kyiv" was the first machine that implemented RISC ISA.

## Installation
In future we want to host emulator as web application, till then you can compile source code or install application
```{bash}
$ git clone https://github.com/oliuba/Kyiv_emulator.git
$ cd Kyiv_emulator
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./kyivemu
```

## Kyiv Programs
Kyiv has 29 operations:

Just for k1,2,3 :

k1 = '(A1+E1A)

'k1 = A1

|            Opcode           	| instruction (en) 	|                description               	|                                                                                                                       	| STOP         	|
|:---------------------------:	|:----------------:	|:----------------------------------------:	|-----------------------------------------------------------------------------------------------------------------------	|--------------	|
| Arithmetic operations       	|                  	|                                          	|                                                                                                                       	|              	|
|              01             	|        add       	| addition                                 	| k1+k2=>k3 'C+1=>C                                                                                                     	| \|res\| ≥ 1  	|
|              02             	|        sub       	| substruction                             	| k1-k2=>k3 'C+1=>C                                                                                                     	| \|res\| ≥ 1  	|
|              03             	|        adc       	| addition of commands                     	| \| k1 + \|k2\| \| V sign of k2 => k3 'C+1=>C                                                                          	|              	|
|              06             	|       suba       	| modulus substraction                     	| \|k1\| - \|k2\| => k3 'C+1=>C                                                                                         	|              	|
|              07             	|      addcyc      	| cyclical addition                        	| k1+k2=>k3 if res ≥ 1 : k3 & k1 >> 40 ( carry from the sign bit goes to the least significant bit of the res ) 'C+1=>1 	|              	|
|              10             	|        mul       	| multiplication without rounding          	| k1*k2=>k3 res>>40 'C+1=>C                                                                                             	|              	|
|              11             	|       rmul       	| multiplication with rounding             	| k1*k2=>k3 res += 1<<39 res>>40 (1 is added to the highest of the discarded bits.) 'C+1=>C                             	|              	|
|              12             	|        div       	| division                                 	| k1/k2=>k3 'C+1=>C                                                                                                     	|              	|
| Logical operations          	|                  	|                                          	|                                                                                                                       	|              	|
|              13             	|        sh        	| logical shift                            	| if k2 < 0    >> else    << k1 >>/<< n => k3 n -- the number of orders of 3rd address of k2 'C+1=>C                    	|              	|
|              14             	|        and       	| digital logical addition                 	| k1 & k2 => k3 'C+1=>C                                                                                                 	|              	|
|              15             	|        or        	| digital logical "multiplication"         	| k1 \| k2 => k3 'C+1=>C                                                                                                	|              	|
|              17             	|        xor       	| digital logical operation nonequivalence 	| k1 XOR k2 => k3 'C+1=>C                                                                                               	|              	|
|              35             	|       norm       	| normalization                            	| k1 -- normalized exponent => k2 mantissa => k3 'C+1=>C                                                                	|              	|
| Control transfer operations 	|                  	|                                          	|                                                                                                                       	|              	|
|              04             	|        jle       	| if 1  ≤  2                               	| if k1 ≤ k2: k3=>C else: 'C+1=>C                                                                                       	|              	|
|              05             	|       jlea       	| if \| 1 \|  ≤  \| 2 \|                   	| if \|k2\| ≤ \|k2\|: k3=>C else: 'C+1=>C                                                                               	|              	|
|              16             	|        je        	| if 1  =  2                               	| if k1 == k2: k3=>C else: 'C+1=>C                                                                                      	|              	|
|              30             	|       nfork      	| if 1 ≤ -0                                	| if k1 ≤ -0: k3=>C k2=>P else: 'C+1=>C                                                                                 	|              	|
|              31             	|       ncall      	| 1 ≤ -0                                   	| if k1 ≤ -0: k3=>C else k2=>C                                                                                          	|              	|
|              32             	|        ret       	| return                                   	|                                                                                                                       	|              	|
| Modification of addresses   	|                  	|                                          	|                                                                                                                       	|              	|
|              26             	|        gob       	| beginning of a group operation           	| k1=>Ц(loop) k2=>A if A=Ц: k3=>C else: 'C+1=>C                                                                         	|              	|
|              27             	|        goe       	| end of a group operation                 	| A+k1=>A if A=Ц: k3=>C else: k2=>C                                                                                     	|              	|
|              34             	|        fop       	| call on the retainer (F)                 	| second address of k1 ==> A 'A=>k3 'C+1=>C                                                                             	|              	|
| Input/Output                	|                  	|                                          	|                                                                                                                       	|              	|
|              20             	|        rdt       	| input of data from punched card          	| read data to 'k1, 'k1+1,..,'k2. Start reading with offset k3                                                          	|              	|
|              21             	|        rbn       	| input of command from punched card       	| read command to 'k1, 'k1+1,...,'k2. Start reading with offset k3                                                      	|              	|
|              22             	|        wbn       	| output to punched card                   	| write 'k1, 'k1+1,...'k2 to card                                                                                       	|              	|
|              23             	|        wmd       	| write to magnetic drum                   	| write 'k1, 'k1+1,...'k2 to drum                                                                                       	|              	|
|              24             	|        rmd       	| read from magnetic drum                  	| read from drum and write to 'k1, 'k1+1,..., 'k2                                                                        	|              	|
|              25             	|        imd       	| init magnetic dram                       	| k2 -- number of magnetic dram (3 max) number of road k3 -- offset k1 -- 0 if read, 1 -- of write 'C+1=>C              	|              	|
|              33             	|      ostanov     	| stop                                     	|                                                                                                                       	|              	|
## GUI Usage

GUI consists of such main parts:

1. Signalization panel

It demonstrates the inner work of "Kyiv" at the time of command execution.
1. The first row shows binary result of the command execution - data that is written to the third address of the command.
2. The second row shows the content of the number register - the number that is transported from or to a magnetic drum. Works with the commands 023 (write magnetic deum) or 024 (read magnetic drum).
3. The left part of third and forth rows (KOп) is a set of bulbs that are responsible for the executed opcode. When a command is executed, one of 29 bulbs lights up.
4. The bulb "Тр Выб" ligths up if there is an exchange with punch tape, punch card or magnetic drum.
5. The middle part of the third row is the indicator of the address register - it works in the takt mode and shows the address of a certain takt's address.
6. The middle part of the forth row shows the content of the return register.
7. The bulp "Тр Пр" light up if there is the command of jumps if the jump is made.
8. The last part of the third row is meant for takt control of "Kyiv": it demonstrates the start and end of most important parts of command execution.
9. The last part of the forth row shows the content of the command counter register.
10. The fifth row shows the content of the command register in the binary-octal mode, usual for "Kyiv" where first 5 bulbs stand for opcode, then each 12 represent first, second and third addresses respectively.
11. The sixth row shows the content of intrmediate register to which the content of number register is written.
12. The seventh row contains three parts. Unfortunately, we don't know the purpose of the most left one. The middle one contains the address register's contant, and the rigth part - the contant of the loop register. There are also three buttons: "Нач" and "Гот" for beginning and start of a command execution, and "Авт" for the work of the machine in an automatic mode.

![image](https://user-images.githubusercontent.com/50978411/149560770-1e69b03f-a726-4977-bf7f-51dc73fdf1d0.png)

2. Control panel

It emulates the work of the control panel, from which it is possible to change the settings of the machine, change ROM and execute an own command.

There is an upper and lower part of the panel:

1. The upper part contains some of the set codes - three cells 03000-03002 of ROM which may be chosen by setting the first button on and choosing the values for each digit (in binary-octal) of three buttons.
2. The lower part contains some triggers, tumblers, addresses and command:
- the first row contains buttons for a command. If the first button is checked, it is possible to execute the chosen command (in binary-octal).
- the trigger of emergency stop is checked when the machine works in the usual state, where if there is a stop in "Kyiv", T register is enabled and one command is skipped. Otherwise T register remains the same, and the two commands are skipped.
- The next trigger is disabled when there is a need to block command counter register (for example, to execute the same command many times for debugging).
- The trigger "Пч" is used for print but is currently not implemented.
- The next trigger blocks registers of address, loop and return.
- The trigger "Переск - ОСТ" blocks stop, so that there never occurs stop of machine when this trigger is enabled, even when it should occur in normal mode.
- Tumbler "Авт-Цикл-Такт" is set to "Авт" when machine works in its usual automatic mode, "Цикл" - when each command is executed one by one, and "Такт" when each command is executed by 4 tats one by one. Each new step occurs when the key "K" is pressed.
- Tumbler "Норм раб - Ввод - Уст с ПУ" is set to "Норм раб" when the machin works in a normal mode, "Ввод" - when the user wants to change the content of the set codes in the upper panel or the command in the lower panel, and "Уст с ПУ" to load these changes into machine (execute the command or save set codes to ROM).
- The next trigger sets all registers to zero.
- Button "Останов" stops the machine if the stop block is disabled.
- Tumbler "Нейтральное - По N команды - По III адресу" is set to "Нейтральное" when the normal stop works (during program execution), "По N команды" - machine stops when it executes the same command as on the lower panel, and "По III адресу" - machine stops when the executed command's third address is the same as in the nearby cells.

![image](https://user-images.githubusercontent.com/50978411/149560985-c8facf84-4fe7-4b3a-90f7-4b98925563c2.png)

3. Set codes of ROM (cells 03000 - 03007)

It contains the set codes - eight cells 03000-03007 of ROM. By default contains zeros. If one of the cells is chosen on the control panel, machine works with it, otherwise - with the cell from here.

![image](https://user-images.githubusercontent.com/50978411/149561110-79695334-4dda-49bd-8e1a-2135b614bfd4.png)

4. Assembly input / opening from file

It contains two parts:

1. The place for assember code. It may be entered directly in the field or chosen from text file using the below button.
2. The place for assembled code. It is impossible to write there, it only shows the "Kyiv" direct commands from the assembler code after clicking on button "Assembly".

![image](https://user-images.githubusercontent.com/50978411/149561230-f6c63bc9-6052-4b58-bd3d-e18417d3dccc.png)

5. Three magnetic drums

![image](https://user-images.githubusercontent.com/50978411/149561296-743bcaf5-944d-4f77-9de5-d18570147177.png)

6. Punch cards

## Authors
* [Anna Korabliova](https://github.com/anika02)
* [Diana Hromiak](https://github.com/Diana-Doe)
* [Daria Minieieva](https://github.com/DariaMinieieva)
* [Olha Liuba](https://github.com/oliuba)
* [Hanna Yershova](https://github.com/hannusia)

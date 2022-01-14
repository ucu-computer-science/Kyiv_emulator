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

## GUI Usage

GUI consists of such main parts:

1. Signalization panel

![image](https://user-images.githubusercontent.com/50978411/149560770-1e69b03f-a726-4977-bf7f-51dc73fdf1d0.png)

2. Control panel

![image](https://user-images.githubusercontent.com/50978411/149560985-c8facf84-4fe7-4b3a-90f7-4b98925563c2.png)

3. Set codes of ROM (cells 03000 - 03007)

![image](https://user-images.githubusercontent.com/50978411/149561110-79695334-4dda-49bd-8e1a-2135b614bfd4.png)

4. Assembly input / opening from file

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

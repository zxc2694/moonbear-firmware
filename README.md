STM32F429_Quadrotor
======================
This is a quadcopter flight control project based on [QCopterFlightControl2.0](https://github.com/Hom19910422/QCopterFlightControl).	The software is being heavily modified in order to adapt FreeRTOS derived multi-threaded and priority-based computing environment. That means we could manage manay task and implement mult-thread application. User can see the sensor status when you try to design or monitor you rotor. A special designed shell enironment could help user to send some command or do some actions on rotors.

[DEMO] : [Youtube Video](https://www.youtube.com/watch?v=uV2TDQUWQ_A)

<img src = "https://lh5.googleusercontent.com/-ox-Q230u9II/VVwGDFMxlaI/AAAAAAAABPo/IFKrzYnNl2M/w1032-h595-no/%25E6%259C%25AA%25E5%2591%25BD%25E5%2590%258D.png" width=600>

License
======================
* The [QCopterFlightControl](https://github.com/Hom19910422/QCopterFlightControl) project is copyrighted by [Hom19910422](Hom19910422@gmail.com) and governed by MIT license.


* The [lineoise](https://github.com/antirez/linenoise) is copyrighted under BSD license and release by [antirez](antirez@gmail.com)

* Detailed license file can be found in `LICENSE`.

* The [STM32F429_Quadrotor](https://github.com/zxc2694/ICLab_Quadrotor) is released as MIT License and welcome to send any feedback to us!

Features
======================
* Support three commercial R/C remoters(Futaba R/C, devention R/C, WFLY R/C)
* Support FreeRTOS and linenoise text editing environment with a Shell
* Real time plotting data from GUI by Python on ground station  
* Display data from LCD ili9341
* Online tuning PID parameters


Shell commands
======================
* clear - Clear the screen
* help - The informations of the shell
* monitor - A QuadCopter Status Monitor
* ps - Show all processes [Block]
* sdinfo - Show SD card information
* sdsave - Storage control parameter to SD card
* showData - Display all the data of flight from reading IMU 
* tuning - Online tuning PID parameters

QuadCopter Status Monitor
======================
After enable the command line, you can type :
* resume - Disable the commandline mode
* reset - Drop the unsaved settings
	- reset [quadcopter parameter] / reset all
* quit - Quit the monitor
* help - The QuadCopter Status Monitor Manual
* set - Set the parameter of the quadcopter
      - set [quadcopter parameter] [value] / set update

Linenoise hotkeys
======================
* To get more informations, please read the LINENOISE.md

TODO
======================
* Filter improvement
* Height control implementation
* Position control
* Speed controll
* GPS navigation and auto-pilot

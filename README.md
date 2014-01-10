QuadcopterFlightControl
======================
This is a quadcopter flight control board base on [QCopterFlightControl2.0](https://github.com/Hom19910422/QCopterFlightControl).	In this version, we design new software which include FreeRTOS, a kind of real-time OS. That means we could manage manay task and implement mult-thread application. User can see the sensor status when you try to design or monitor you rotor. A special designed shell enironment could help user to send some command or do some actions on rotors.

License
======================
* The [QCopterFlightControl](https://github.com/Hom19910422/QCopterFlightControl) project is auorized by [Hom19910422](Hom19910422@gmail.com)

* The [lineoise](https://github.com/antirez/linenoise) is auorized under BSD License and release by [antirez](antirez@gmail.com)

Features
======================
* Support commercial R/C remoter(Futaba R/C)
* Support linenoise text editing environment with a Shell
* Online tuning PID parameters
* Report the status of the QuadCopter

Shell commands
======================
* clear - Clear the screen
* help - The informations of the shell
* monitor - A QuadCopter Status Monitor
* ps - Show all processes [Block]

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

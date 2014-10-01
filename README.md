QuadcopterFlightControl
======================
This is a quadcopter flight control project based on [QCopterFlightControl2.0](https://github.com/Hom19910422/QCopterFlightControl).	The software is being heavily modified in order to adapt FreeRTOS derived multi-threaded and priority-based computing environment. That means we could manage manay task and implement mult-thread application. User can see the sensor status when you try to design or monitor you rotor. A special designed shell enironment could help user to send some command or do some actions on rotors.

[Developing Note](https://zoo.hackpad.com/ep/pad/static/Y4388OmZRRA)

<img src = "https://lh6.googleusercontent.com/-30ZaooIhtY0/UlT09AWvWmI/AAAAAAAAER0/mvbLm6LFEQ4/w958-h539-no/DSC_1506.jpg" width=600>

License
======================
* The [QCopterFlightControl](https://github.com/Hom19910422/QCopterFlightControl) project is copyrighted by [Hom19910422](Hom19910422@gmail.com) and governed by MIT license.


* The [lineoise](https://github.com/antirez/linenoise) is copyrighted under BSD license and release by [antirez](antirez@gmail.com)

* Detailed license file can be found in `LICENSE`.

* The [QuadcopterFlightControl](https://github.com/QuadCopterTainan/QuadcopterFlightControl) is released as MIT License and welcome to send any feedback to us!

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
* Filter improvement

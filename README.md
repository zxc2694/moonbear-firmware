QuadcopterFlightControl
======================
This is a quadcopter flight control project which based on [QCopterFlightControl2.0](https://github.com/Hom19910422/QCopterFlightControl). 

In this version, we redesign the software and import the FreeRTOS(A kind of Real-Time OS which provide a multi-thread programming environment).

Also, A specail designed Shell provide for adjusting and optimizing the Quad is also being provide in the system.The User can tune the PID, test motors or even get the whole system status easily for building up the project.

<img src = "https://lh6.googleusercontent.com/-30ZaooIhtY0/UlT09AWvWmI/AAAAAAAAER0/mvbLm6LFEQ4/w958-h539-no/DSC_1506.jpg" width=450>

License
======================
**Origin Source**
* The [QCopterFlightControl](https://github.com/Hom19910422/QCopterFlightControl) project is auorized auorized under MIT License and release by [Hom19910422](Hom19910422@gmail.com)

* The [lineoise](https://github.com/antirez/linenoise) is auorized under BSD License and release by [antirez](antirez@gmail.com)

**Fork version**
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
* Attitudes stable by using optical flow

QuadrotorFlightControl
======================
The enhanced version of [QCopterFlightControl 2.0](https://github.com/Hom19910422/QCopterFlightControl)

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
* ps - Show all processes

QuadCopter Status Monitor
======================
After enable the command line, you can type :
* resume - Disable the commandline mode
* quit - Quit the monitor
* help - The QuadCopter Status Monitor Manual
* set - Set the parameter of the quadcopter
      - set [quadcopter parameter] [value] / set update

Linenoise hotkeys
======================
* Ctrl+d : Remove char at right of cursor, or of the line is empty, act as end-of-file
* Ctrl+u : Delete the whole line
* Ctrl+k : Delete from current to end of line
* Ctrl+a : Go to the start of the line
* Ctrl+e : Go to the end of the line
* Ctrl+l : Clear screen
* Ctrl+w : Delete previous words
* Ctrl+b : Move to left for one offset
* Ctrl+f : Move to right for one offset
* Ctrl+p : Move to previous record
* Ctrl+n : Move to next record
* Ctrl+t : Swaps current character with previous
* TAB : autocomplete the text

TODO
======================

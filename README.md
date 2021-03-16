# Project 2: Command Line Shell

By Jerome Reduta

This program is intended to be a replica of a shell (particularly the BASH shell).

# Shell? What's that?

You can think of an OS as having layers (it is short for Onion System after all). The outermost layer of that OS is the shell, which is usually just an interface that lets the user tell the OS to do things.

For example, if you open up terminal or powershell or even this program and type:
<pre>
	ls -a
</pre>

you've essentially told the OS to list all the files in your current directory, even the hidden ones. The output you received (a list of filenames) is the result of that command.

# Included Files

The following files are included in this project:

<ol>
	<li>Makefile: For compiling and testing the project.</li>
	<li>this_leaks_how_much_oh_sh: The executable. Complies with shell naming standards.</li>
	<li>shell.c: The driver file.</li>
	<li>history.c: For storing previous commands</li>
	<li>job_list.c: For storing a list of background jobs</li>
	<li>pipe.c: For running most commands and connecting them through a pipe as necessary.</li>
	<li>''$'\370\261\370''|'$'\375\177': I don't know what I've done to get these files. I am scared.</li>
	<li>process_io.c: For anything to do with processing input and output files</li>
	<li>README.md: Hi!</li>
	<li>signal.c: For anything to do with signal handling</li>
	<li>tests.txt: A collection of tests I made, just in case I need to use them again</li>
	<li>util.c: A collection of useful functions that didn't make it to any of the other, cooler files</li>
	<li>Many, many header files associated with most of the above files. If it ends with .c, and isn't shell.c, it has a header file.</li>
</ol>


# Compiling and Running

To compile and run:
<pre>
	Compile: make
	Run: ./this_leaks_how_much_oh_sh
</pre>

# Sample Output

<pre>
	[jrreduta@KevinM P2-JeromeRReduta]$ ./this_leaks_how_much_oh_sh
	You're li-shell-ing to KRASH at [jrreduta@KevinM:       ~/P2-JeromeRReduta]ls
	''$'\370\261\370''|'$'\375\177'   logger.h.gch                     signal.h.gch
	''$'\250\031\n\a\376\177'         Makefile                         signal.o
	''$'\230\360\321''1'$'\374\177'  ''$'\270\276\201''n'$'\376\177'   tests
	 42.txt                           pipe.c                           tests.txt
	 a.out                            pipe.h                           this_leaks_how_much_oh_sh
	 docs                             pipe.h.gch                      ''$'\257\345\227''['$'\274''U'
	''$'\257''e'$'\361\314''YU'       pipe.o                           ui.c
	 history.c                        process_io.c                     ui.h
	 history.h                        process_io.h                     ui.h.gch
	 history.h.gch                    process_io.h.gch                 ui.o
	 history.o                        process_io.o                    ''$'\257''Ů*JV'
	 job_list.c                       README.md                        util.c
	 job_list.h                       scripting.txt                    util.h
	 job_list.h.gch                   shell.c                          util.h.gch
	 job_list.o                       shell.o                          util.o
	 libshell.so                      signal.c
	 logger.h                         signal.h
	You're li-shell-ing to KRASH at [jrreduta@KevinM:       ~/P2-JeromeRReduta]^C
</pre>

# Special Thanks

<pre>
	To my TAs, without whom I would have finished like, less than half the test cases.
	To my teacher, who won't grade too harshly, right? Right?
	To my parents, who taught me how to type fast. It was crucial in this project.
	To my CS 221 teacher, for teaching me the basics of C.

</pre>

# Edit
I hope this doesn't count but I kind of wanted to see how much code I wrote, so I ran CLOC (suggested by https://stackoverflow.com/questions/26881441/can-you-get-the-number-of-lines-of-code-from-a-github-repository#:~:text=In%20a%20GitHub%20repository%20you,code%20the%20project%20consists%20of.). I'll put the results below, for science.


<pre>
[jrreduta@KevinM counting-code]$ cloc P2-JeromeRReduta/
      21 text files.
      21 unique files.
       4 files ignored.

github.com/AlDanial/cloc v 1.88  T=0.02 s (971.2 files/s, 127118.1 lines/s)
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                8            512            504           1006
C/C++ Header                     8             35             62            107
Markdown                         1             18              0             63
make                             1             16              4             29
-------------------------------------------------------------------------------
SUM:                            18            581            570           1205
-------------------------------------------------------------------------------
</pre>




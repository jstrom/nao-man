 INSTALLING LIBRARIES AND CROSS COMPILER

In order to compile the man binaries, you must first have the NaoQI libraries  and associated cross compiler installed on your machine. These libraries are the property of Aldebaran Robotics, but are mirrored on our server for those with a Bowdoin login. You may contact Aldebaran for the libraries.

 The default configuration is to install to /usr/local/nao.

NOTE: The given commands should be executed from the directory where you downloaded the archives. Also, depending on your system setup, you may need root privileges to install to /usr/local/nao

LINUX:
First, get the NaoQI libraries by retrieving the file <https://robocup.bowdoin.edu/files/software/nao/NaoQi/1.2.0/linux/NaoQiRobocup-1.2.0-Linux.tar.gz>. You should then extract the contents to /usr/local/nao (i.e. there should be a file /usr/local/nao/Release-notes.txt)

	sudo tar -xvzf NaoQiRobocup-1.2.0-Linux.tar.gz -C /usr/local

If the directory /usr/local/nao already exists you should move it first

        sudo mv /usr/local/nao /usr/local/nao-old

Then you should move the NaoQi directory to /usr/local/nao

	sudo mv /usr/local/NaoQiRobocup-1.2.0-Linux /usr/local/nao

You will also need the cross compiler, so start by creating its directory:

	sudo mkdir /usr/local/nao/crosstoolchain

Next, the new (1.0) cross tool chain is available from <https://robocup.bowdoin.edu/files/software/nao/NaoQi/1.2.0/ctc-robocup-1.2..tar.bz2>

Double check to make sure you have folders named 'cross' and 'staging' in $AL_DIR/crosstoolchain

DARWIN (MAC OS X):
Download the Northern Bites Nao tools from
<https://robocup.bowdoin.edu/files/software/nao/NaoQi/1.2.0/darwin/darwin-nao-1.2.tar.bz2>

Extract the archive using the following command:

tar -xvvjf darwin-nao-1.2.tar.bz2

NOTE: if you currently have /usr/local/nao you should move it with the following command if you do not want to lose the contents:

sudo mv /usr/local/nao /usr/local/nao-old

Now move the archive to /usr/local/nao:

sudo mv ./nao-darwin /usr/local/nao

COMPILING & INSTALLING

We use CMake for compiling and installing our code.  It has most of GNU Make's
features along with some other niceties.

How you are going to use the code greatly determines how you should configure
the build.  For most applications you will be compiling code to be run on the
robot.  Thus you want to execute the following commands to cross compile for the
robot's OS.  Do so with the following commands (where $ROBOCUP_HOME is the path
to your robocup directories):

   cd $ROBOCUP_HOME/man
   make cross

This will take you to a configuration screen.  If you want to simply test if
your code compiles, than press c to begin configuration and then press g to
generate the configuration files.

If you are compiling for the robot you should make sure @MAN_IS_REMOTE@ is set
to OFF.  If it is set to OFF, you can change it by moving over the OFF text area
and hitting the enter key.  Here you should also set the appropriate player for
the robot and the IP address. Help for the configuration tool is accessible by
typing h while the tool is running.

Now to compile the code simply run:

    make

If there are errors compiling and you need to change configuration files you can
delete the configuration files and make the code base ready for recompiling via:

   make clean

If you have an error compiling and need to clean your code base, but do not wish
to delete your configuration files you can delete the binaries only by using the
command:

    make cclean

Once you have built the binaries they can be installed on the robot with the
following command:

    make install


RUNNING THE CODE
Set the IP address of the remote robot on the configuration screen as described above.
Next, ssh into the robot as root and edit the file /opt/naoqi/modules/lib/autoload.ini using `nano autoload.ini`  This file lists all the modules that are loaded by NaoQi on boot time. If you are running as a dynamically linked library, you must ensure that the line
man is present in that list. Additionally, you must disable pythonbridge for the Python to run correctly in our code (if you don't do this, you will get a segfault).


SIMULATOR:
See the directions at http://robocup.bowdoin.edu/trac/wiki/Webots


TROUBLESHOOTING:
Still having trouble?  Take a look at our wiki at http://robocup.bowdoin.edu/trac. Particularly, the page http://robocup.bowdoin.edu/trac/wiki/PracticalNao might give links to more detailed instructions then those shown here.


KNOWN BUGS:
SIMULATOR (in robostadium contest mode without NaoQi):
	  -angleX and angleY are calculated incorrectly (they are just 0.0)
	  -GameController (i.e. Comm) is not working, so switching into playing is not possible
	  -Use the "webots.mtb" table

 LocalWords:  NaoQI robocup

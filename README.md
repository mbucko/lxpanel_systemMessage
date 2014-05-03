lxpanel_systemMessage
==============

LXDE Panel Plugin that displays an the latest message from dmesg

I created this on 32-bit CrunchBang Linux 11 "Waldorf" and have not tested anywhere else.

CrunchBang Linux 11
------------

Quick build instructions (once you have the dependencies):

- Install
 1. make
 2. sudo make install
- Uninstall
 1. sudo make uninstall

The plugin should be accessible from the "Add/Remove Panel Items" option in LXDE
You might want to tick the "Stretch" property for the label to get as much space as possible.

Dependencies
--------

You need the following packages:
* libglib2.0-dev
* libmenu-cache1-dev 
* libgtk2.0-dev

Command:

`sudo apt-get install libglib2.0-dev libmenu-cache1-dev libgtk2.0-dev`

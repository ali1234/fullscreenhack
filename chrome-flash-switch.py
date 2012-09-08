#!/usr/bin/python

# author: Sagar Karandikar, apps@sagark.org

import os
import sys

def switch(contents):
    # disable PepperFlash and enable the usual libflashplayer.so
    for x in range(len(contents)):
        if '"path": "/opt/google/chrome/PepperFlash/libpepflashplayer.so",' in contents[x]:
            contents[x-2] = '         "enabled": false,\n'
        if '"path": "/usr/lib/flashplugin-installer/libflashplayer.so",' in contents[x]:
            contents[x-2] = '         "enabled": true,\n'

print("Attempting to enable the external flash plugin in chrome.")

filelocation = os.path.expanduser("~/")
filelocation += ".config/google-chrome/Default/Preferences"

if not os.path.exists(filelocation):
    print("It does not appear that you have google-chrome installed. Exiting.")
    sys.exit(1)

prefs = file(filelocation) # open user's chrome preferences file
contents = prefs.readlines() # get its contents
prefs.close()

externalflash = False # if this is False at the end of the for loop, we'll tell 
# the user that they do not have the required external flash plugin
for x in range(len(contents)):
    if '"path": "/usr/lib/flashplugin-installer/libflashplayer.so",' in contents[x]:
        # if we see that libflashplayer.so is installed, call switch to enable it
        switch(contents)
        externalflash = True

if not externalflash:
        print("It does not appear that you have an external flash plugin installed")
        print("Exiting.")
        sys.exit(1)

# write preferences to file
prefsout = file(filelocation, "w")
for x in contents:
    prefsout.write(x)
prefsout.close()

# instruct user to restart chrome
print("chrome-preferences have been modified so that chrome will use the")
print("external flash plugin instead of PepperFlash.")
print("Please restart chrome for this to take effect.")

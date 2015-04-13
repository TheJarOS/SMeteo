import os
import time 
print "3, 2, 1, Start"
os.system('sudo python /home/pi/SMeteo/smeteoN.py')
print "Opend SMeteo"
print "Moving photo"
os.system('sudo python /home/pi/SMeteo/moveSnap.py')
print "Sleeping..."

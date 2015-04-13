import serial 
import MySQLdb
import time
import os
import glob
from datetime import datetime
import sys

dbConn = MySQLdb.connect("localhost","root","TheJarOS2000","smeteo") or die ("could not connect to database")
cursor = dbConn.cursor()

device = '/dev/ttyUSB0'

os.system('modprobe w1-gpio')
os.system('modprobe w1-therm')

os.system('modprobe w1-gpio')
os.system('modprobe w1-therm')

base_dir = '/sys/bus/w1/devices/'
thermometer_folder = glob.glob(base_dir + '28*')[0]
thermometer_file = thermometer_folder + '/w1_slave'

def read_temp_raw():
  f = open(thermometer_file, 'r')
  lines = f.readlines()
  f.close()
  return lines

try:
  print "Trying..."
  Arduino = serial.Serial(device, 9600) 
  time.sleep(2)
  print "Connected to: ", device
  print "Welcome Arduino :D"
except: 
  print "Failed to connect on: ", device    
  sys.exit() 

print "Speaking to: ", device
Arduino.write('1') 
data = Arduino.readline()  
pieces = data.split("\t")
print "Data from Arduino" 
print "Temperature: ", pieces[0]
print "Humidity: ", pieces[1]
print "Preasure: ", pieces[2]
print "Light: ", pieces[3]
print "Rain: ", pieces[4]
  
print "Reading from indoor thermometer"
lines = read_temp_raw()
while lines[0].strip()[-3:] != 'YES':
  time.sleep(0.2)
  lines = read_temp_raw()
equals_pos = lines[1].find('t=')
if equals_pos != -1:
  temp_string = lines[1][equals_pos+2:]
  temperatureC = float(temp_string) / 1000.0
print "Temperature in home: ", temperatureC
  
print "Getting date and time"
dataCzas = datetime.now().strftime('%Y-%m-%d')
godzinaCzas = datetime.now().strftime('%H:%M:%S')
print dataCzas, "	", godzinaCzas

print "Getting temperature of CPU"
CPUtempG = os.popen('cat /sys/class/thermal/thermal_zone0/temp').readline()
CPUtemp =  float(CPUtempG) 
CPUtemp = CPUtemp /1000.0
print "CPU temperature: ", CPUtemp
print "Sending data to MySQL" 
cursor.execute("INSERT INTO dane (data,godzina,temperatura1,temperatura2,wilgotnosc,cisnienie,jasnosc,opady,CPU) VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s)", (dataCzas,godzinaCzas,temperatureC,pieces[0],pieces[1],pieces[2],pieces[3],pieces[4],CPUtemp))
dbConn.commit()
print ""

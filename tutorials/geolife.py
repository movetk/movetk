# This is a script to preprocess the Geolife trajectories to be loaded into tulib
# Place this file in the Geolife root directory, and/or adjust the geolife_directory and output_directory variables

import csv
from datetime import date, time, datetime
import os

#global constants
geolife_directory = "Geolife Trajectories 1.3/Data/"
output_directory = "out/"
dateformat_label = "%Y/%m/%d %H:%M:%S"
dateformat_probe = "%Y-%m-%d %H:%M:%S"
dateformat_out = "%Y/%m/%d %H:%M:%S"
seperate_output = True
only_labelled_users = True
only_labelled_probes = True

mode_start, mode_end, mode_id = (datetime.min, datetime.max, "unknown")

#Function to read trough the Labels file
def getNextMode(labels_reader):
    mode_line = next(labels_reader, None)
    if mode_line is None:
        return (datetime.min, datetime.max, "unknown")
    else:
        mode_start = datetime.strptime(mode_line["START"], dateformat_label)
        mode_end = datetime.strptime(mode_line["END"], dateformat_label)
        mode_id = mode_line["MODE"]
        print(f"Next mode is <{mode_id}>, from {mode_start} to {mode_end}")
        return (mode_start, mode_end, mode_id)

def handleProbe(probe, traj_id, out_writer, labels_reader):
    global mode_start, mode_end, mode_id
    datetime_probe = datetime.strptime(probe["DATE"] + " " + probe["TIME"], dateformat_probe)
    #print(f"Starting probe at {datetime_probe}")
    while datetime_probe >= mode_end:
        mode_start, mode_end, mode_id = getNextMode(labels_reader)
    if datetime_probe >= mode_start:
        mode = mode_id
    else:
        mode = "unknown"

    #convert altitude to metres, if it's not -777
    altitude = float(probe["ALTITUDE"])
    if(altitude != -777):
        altitude = altitude / 3.2808

    if only_labelled_probes and mode is not "unknown":
        out_writer.writerow({
            "LAT": probe["LAT"], 
            "LONG": probe["LONG"],
            "ALTITUDE": probe["ALTITUDE"], 
            "DATETIME": int(datetime_probe.timestamp()), 
            "TRAJID": traj_id,
            "MODEID": mode
            })

def handleTrajectory(traj_path, traj_id, out_writer, labels_reader):
    with open(traj_path) as traj:
        traj_reader = csv.DictReader(traj, delimiter = ',', fieldnames = ["LAT", "LONG", "UNKNOWN", "ALTITUDE", "DATETIME", "DATE", "TIME"])
        #Skip the first 6 lines
        for _ in range(0, 6):
            next(traj_reader)
        probe = next(traj_reader)

        #Iterate trough all probes in the trajectory
        while probe is not None:
            handleProbe(probe, traj_id, out_writer, labels_reader)
            probe = next(traj_reader, None)

def handleUser(user_path, user_id, out_writer):
    global mode_start, mode_end, mode_id
    print(f"Starting user {user_id}")

    #Read Labels
    labels_path = f"{user_path}/labels.txt"
    labels_reader = None
    mode_start, mode_end, mode_id = (datetime.min, datetime.max, "unknown")
    if os.path.isfile(labels_path):
        labels = open(labels_path)
        labels_reader = csv.DictReader(labels, delimiter = '\t', fieldnames = ["START", "END", "MODE"])
        #Skip the first line
        next(labels_reader)
        mode_end = datetime.min
        if(seperate_output):
            out = open(f"{output_directory}/{user_id}.csv", mode='w+', newline='')
            out_writer = csv.DictWriter(out, delimiter = ',', fieldnames = ["LAT", "LONG", "ALTITUDE", "DATETIME", "TRAJID", "MODEID"])
            out_writer.writeheader()
    else:
        if only_labelled_users:
            return

    #Each file is a seperate trajectory
    i = 0
    for fname in os.listdir(f"{user_path}/Trajectory"):
        traj_path = f"{user_path}/Trajectory/{fname}"
        traj_id = f"{user_id}_{i}"
        handleTrajectory(traj_path, traj_id, out_writer, labels_reader)
        i = i + 1

    if labels_reader is not None:
        labels.close()
    if(seperate_output):
        out.close()

#Create output file
if not os.path.exists(output_directory):
    os.makedirs(output_directory)

out_writer = None
if(not seperate_output):
    out = open(output_directory + "out.csv", mode='w+', newline='')
    out_writer = csv.DictWriter(out, delimiter = ',', fieldnames = ["LAT", "LONG", "ALTITUDE", "DATETIME", "TRAJID", "MODEID"])
    out_writer.writeheader()

#Traverse filesystem
for user_id in os.listdir(geolife_directory):
    #Each directory is a seperate user
    handleUser(f"{geolife_directory}/{user_id}", user_id, out_writer)

if(not seperate_output):
    out.close()
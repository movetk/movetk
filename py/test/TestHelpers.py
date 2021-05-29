from PyMoveTk.geometry import Polyline
import numpy as np

APPROXIMATE_EQUAL_TOLERANCE = 1e-6


def points_approximately_equal(point0, point1, tolerance = APPROXIMATE_EQUAL_TOLERANCE):
    diff = point0-point1
    return abs(diff.norm()) < tolerance

def _skipSpace(string, currentOffset):
    strLen = len(string)
    for i in range(currentOffset,strLen):
        if not string[i].isspace():
            return i
    return -1

def parseIpePath(ipePathString):
    # 
    stringLen = len(ipePathString)
    i = 0
    # Find '<path' tag for start of path.
    while True:
        bracketLoc = ipePathString.find('<',i)
        if bracketLoc == -1:
            raise Exception("Could not find start tag")
        offset = _skipSpace(ipePathString,bracketLoc+1)
        if offset == -1:
            raise Exception("Invalid ipe path given, could not find closing >")
        isPath = ipePathString.startswith('path',offset)
        bracketEndLoc = ipePathString.find('>',bracketLoc+1)
        if isPath:
            i = bracketEndLoc + 1 # Skip just past the end bracket
            break
        i = bracketEndLoc + 1

    # Read until next tag, and parse operators while reading
    prevPosition = i
    newPos = ipePathString.find('<',prevPosition)
    if newPos == -1:
        raise Exception("Could not find end of path tag")
    # Operators
    data = ipePathString[prevPosition:newPos].strip().split()

    # Parse operators. Only supports 'm' and 'l' for now.
    readStart = 0
    locations = []
    for j in range(len(data)):
        if data[j] == 'm' or data[j] == 'l':
            assert(readStart == j-2)
            locations.append([float(data[j-2]),float(data[j-1])])
            readStart = j+1

    dataArray = np.array(locations)
    return Polyline(dataArray)
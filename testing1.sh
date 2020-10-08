!/bin/sh
# Testing script for AMStartup.c
# Author: Antony Guzman
# Inspired by CS50 given example of testing.sh
# Date: Feb 29, 2020
#
# usage: bash -v testing.sh


#####################################
### These tests should fail ###

# 1 argument
./AMStartup 


# 4 argument
./AMStartup -n -d -h


# 6 argument
./AMStartup 4 -d 0 -h flume.cs.dartmouth.edu



# 7 argument + noninterger input
./AMStartup -n hey -d 0 -h flume.cs.dartmouth.edu



# 7 argument + bigger nAvatars
./AMStartup -n 11 -d hey -h flume.cs.dartmouth.edu



# 7 argument + bigger nAvatars
./AMStartup -n 4 -d hey -h flume.cs.dartmouth.edu



# 7 argument + fake email
./AMStartup -n 5 -d 4 -h nonexistent.cs.darmtouth.edu



#####################################
### These tests should pass ###

# 2 avatars and 0 difficulty 
./AMStartup -n 2 -d 0 -h flume.cs.dartmouth.edu


# 5 avatars and 5 difficulty 
./AMStartup -n 5 -d 5 -h flume.cs.dartmouth.edu

# 8 avatars and 3 difficulty 
./AMStartup -n 8 -d 3 -h flume.cs.dartmouth.edu


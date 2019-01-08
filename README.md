# 2018-Winter-Semester-Theory-of-Computer-Games
## Course Introducton
Learn all kinds of algorithms and techniques to design various computer games. 
These algorithms and techniques include recent very successful machine learning methods by AlphaGo, such as reinforcement learning and deep learning.

## Course Outline
1.	9/11 	Syllabus & Introduction 
2.	9/18 	Survey – Game solved 
3.	9/25 	Reinforcement learning  
4.	10/2 	TD learning and Deep Learning 
5.	10/9 	Alpha-beta pruning and its analysis 
6.	10/16 	Scout/NegaScout and SSS* 
7.	10/23 	Enhancements to alpha-beta search 
8.	10/30 	Monte-Carlo tree search  
9.	11/6 	Threat-space search 
10.	11/13 	Proof number search  
11.	11/20 	Single-player games and A* algorithm 
12.	11/27 	Parallel game tree search 
13.	12/4 	Combinatorial game theory 
14.	12/11 	Paper Presentation 
15.	12/18 	Paper Presentation 
16.	12/25 	Paper Presentation 
17.	1/1 	No class (New Year's Day) 
18.	1/8 	Final exam

## Project 1: Write a player to play Threes! with Random ways to play. (Change 2048 game ot Threes!)
```shell
make
./2048
```

## Project 2: Write a player to play Threes! with high win rates.
[Introduction](./pj-2-v1.pdf)
```shell
make
./2048 --train --total=50000 --block=1000 --limit=1000
./2048
```

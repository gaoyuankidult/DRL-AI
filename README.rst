DRL-AI
==========

This project is a CUDA implementation of the paper 

| **Playing Atari with Deep Reinforcement Learning**
| *Volodymyr Mnih, Koray Kavukcuoglu, David Silver, Alex Graves, Ioannis Antonoglou, Daan Wierstra, Martin Riedmiller*
| NIPS workshop 2013
| 
| However we have not got the performance reported by the paper yet.

Authors
---------------
Yuan Gao & Yao Lu

Required Libraries
---------------------
rl-glue, SDL, SDL-image, CUDA, cublas

Compile
----------------
| cd ENV/src/dnq_gpu
| make
| cd ../..
| make

Run 
--------------------
| For example, to play game Breakout

| ./ale -player_agent deep_q_agent -display_screen true breakout.bin


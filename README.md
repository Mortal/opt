Optimization
============

Assigning people to destinations. Around 50 people each need to move to one of
24 destinations. Each destination has a maximum capacity, however the sum of
these capacities is around twice the number of people.

Each person has a prioritized list of people with whom they would like to go to
the same destination.

We have to assign people to destinations so that their priorities are
fulfilled. Note the fuzzy object function!

This project is an attempt at approximating an optimal solution.

Algorithm
---------

Brute force (repeatedly generating random solutions) seems to work pretty well.

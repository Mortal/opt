Optimization.

Ideas.

Greedy + probabilistic

Initially, we assign people to room buffers by their 1st priority.

Sort rooms by number of people in buffers.

For each room in decreasing order, assign people at random to the room, where
the probability of a person being chosen depends on the person's priority of
the room. People in surplus are assigned to their next prioritized room.

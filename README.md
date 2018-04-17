# Homework 7 STL and Graph Algorithms
## By Alessandro Mendola

### P1. Building the Graph
To build the graph, I began with two options. Option 1: I could iterate over each
group in the database (G elements), connecting each member within the group
(K members) which would result in approximately O(G*K!) time to add all the
connections.
The second option was to iterate over each member in the database (M elements),
then iterate over each group the current member is in (L elements), then iterate
over each other member in the current group (K elements), connecting the pair.
This approach would result in approximately O(M*L*K) time. I chose the second
approach to avoid the K! factor in the first approach.

#### Result:
```Time to build member graph 3.40642```

### P2 and P3, BFS vs IDDFS
For this section, I implemented a Breadth-First-Search and Depth-First-Search to 
connect to a given node. I then compared the time and space required to run each.

#### Results:
##### BFS:
```Time to find path to member 0.5991```
WITH pathfinder:
```Maximum resident set size (kbytes): 126888```
WITHOUT pathfinder:
```Maximum resident set size (kbytes): 126828```
Pathfinder space usage: 60 kb

##### IDDFS:
```Time to find path to member 0.0057299```
WITH Pathfinder:
```Maximum resident set size (kbytes): 126888```
WITHOUT Pathfinder:
```Maximum resident set size (kbytes): 126876```
Pathfinder space usage: 12 kb

##### Comments:
The IDDFS was faster and used less memory than BFS. IDDFS combines the speed in
finding close neighbors of the BFS with the space efficiency of DFS.
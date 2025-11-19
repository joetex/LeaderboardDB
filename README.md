# LeaderboardDB
MinMaxBTree, a modified B+Tree that tracks rank of a score. 

See [MinMaxTree.h](https://github.com/joetex/LeaderboardDB/blob/main/src/db/datastructures/MinMaxTree.h) and [MinMaxNode.h](https://github.com/joetex/LeaderboardDB/blob/main/src/db/datastructures/MinMaxNode.h)

<img width="1212" height="719" alt="image" src="https://github.com/user-attachments/assets/fab43c44-c7a1-4927-b2a6-80489ef3a6a8" />


This was inspired by Redis Skiplist for Sorted Sets, but I wanted something that fits into filesystem better.  

#### Supported
- Custom leaf count (128 is optimal in memory)
- Ascending or Descending scoring
- Retreiving range or reverse range of scores

#### Todo
- Optimize the MinMaxTree to serialize into filesystem storage
- Reduce node memory
- Improve searching through lists inside a leaf or node to find score in O(log N)

#### Sample Output

```
=== LeaderboardDB v1.0 ===
Insert 1000001 : 86ms
Show top ranks, lower is better
Rank [1]: score=0: id=1000000
Rank [2]: score=1: id=999999
Rank [3]: score=2: id=999998
Rank [4]: score=3: id=999997
Rank [5]: score=4: id=999996
Rank [6]: score=5: id=999995
Rank [7]: score=6: id=999994
Rank [8]: score=7: id=999993
Rank [9]: score=8: id=999992
Rank [10]: score=9: id=999991
Iterate MinMax 10: 0ms
=====================
Insert 1000001 : 73ms
Show top ranks, higher is better
Rank [1]: score=1000000: id=0
Rank [2]: score=999999: id=1
Rank [3]: score=999998: id=2
Rank [4]: score=999997: id=3
Rank [5]: score=999996: id=4
Rank [6]: score=999995: id=5
Rank [7]: score=999994: id=6
Rank [8]: score=999993: id=7
Rank [9]: score=999992: id=8
Rank [10]: score=999991: id=9
Iterate MinMax 10: 2ms
```


### More Information

This is a thought experiment to make a faster statistics based database.  

Advanced leaderboards need to track more than just rank.  
They need a history of rank movement, direct access to full metadata of owner, timestamps, and more.  
We should be able to slice subsets of ranks by different metrics like time windows or metadata from the owners (i.e. Country, Age, etc)

Traditional relational databases can do these things, but ranking is not optimized on BTree at all.  Storage is cheap, so data redudancy or duplication is part of the plan.

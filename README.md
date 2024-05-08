# Pseudoarobiricty Computation

# Datasets

Because some datasets used in the paper are too large to be uploaded to GitHub, we have summarized the download links for the dataset in the table below.

Datasets used for performance studies.

| Dataset | Link |
| --- | --- |
| Catster | https://networkrepository.com/soc-catster.php |
| Berkstan | https://networkrepository.com/web-BerkStan-dir.php |
| IT | https://networkrepository.com/web-it-2004.php |
| WikiEO | http://www.konect.cc/networks/wikipedia_link_eo/ |
| HuGene2 | https://networkrepository.com/bio-human-gene2.php |
| Hollywood | https://networkrepository.com/ca-hollywood-2009.php |
| Weibo | https://networkrepository.com/soc-sinaweibo.php |
| Arabic | https://networkrepository.com/arabic-2005.php |
| ITALL | https://networkrepository.com/web-it-2004-all.php |
| SKALL | https://networkrepository.com/web-sk-2005-all.php |

Datasets used for temporal graphs experiments.

| Dataset | Link |
| --- | --- |
| WikiElec | https://networkrepository.com/soc-wiki-elec.php |
| Epinions | https://networkrepository.com/soc-epinions-trust-dir.php |
| HepTh | https://networkrepository.com/ca-cit-HepTh.php |

Datasets used for case studies are uploaded to the “Case Study Datasets” folder. 

Datasets download links for all 195 graphs used for pseudoarboricity empirical analyses are uploaded to “195 Datasets Download Link.txt”.

# Preprocess

The datasets need to be preprocessed, since our paper only focuses on undirected simple graph. Some datasets may be directed graphs, or contain loops, or contain multiple edges. A preprocessing is needed to convert such datasets into undirected simple graph by ignoring the direction of edges, deleting loops, and considering multiple edges as one edge. The preprocessing can be done with the simple program “preprocess.cpp” in the “Graph” folder. We preprocessed the SK dataset in the paper as an example, stored in "Graphs/sk.txt".

# Compile

```
g++ main.cpp -o main -std=c++11 -O3
```

# Usage of Static Algorithms

Datasets need to be stored in the “Graphs” folder. In “dataset.txt”, enter the graph name as our example shown. To run the program, run the following command. 

```
./main
```

# Usage of Dynamic Algorithms

Our dynamic algorithms take input in the form of unreversible orientations rather than undirected graphs. An unreversible orientation is stored in a file format where each line "from to" represents a directed edge <from, to> (see example in Graphs/sk_unreversible.txt). To compute an unreversible orientation from an undirected graph, one can use the get_unreversible.cpp program located in the Graphs folder. This program implements the proposed INDEGREE+Retest algorithm. The compilation process is the same as that for static algorithms, and it is executed using the command ``./get_unreversible <undirected_graph_address> <output_address>``. Once the unreversible orientation is obtained, it should be placed in the Graphs folder, and then the orientation name should be entered in "dataset.txt" as demonstrated in our example. Finally, the dynamic algorithms can be run by executing the command ``./main``.

Note that for dynamic algorithms, the number of edges to be updated can be changed by the "insert_num" variable, and this number must be more than the number of edges of the whole graph. 

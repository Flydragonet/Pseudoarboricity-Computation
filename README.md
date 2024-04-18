# Pseudoarobiricty Computation

# Datasets

Because some datasets used in the paper are too large to be uploaded to GitHub, we have summarized the download links for the dataset in the table below.

Datasets used for performance studies.

| Catster | https://networkrepository.com/soc-catster.php |
| --- | --- |
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

| WikiElec | https://networkrepository.com/soc-wiki-elec.php |
| --- | --- |
| Epinions | https://networkrepository.com/soc-epinions-trust-dir.php |
| HepTh | https://networkrepository.com/ca-cit-HepTh.php |

Datasets used for case studies are uploaded to the “Case Study Datasets” folder. 

Datasets download links for all 195 graphs used for pseudoarboricity empirical analyses are uploaded to “195 Datasets Download Link.txt”.

# Preprocess

The datasets need to be preprocessed, since our paper only focuses on undirected simple graph. Some datasets may be directed graphs, or contain loops, or contain multiple edges. A preprocessing is needed to convert such datasets into undirected simple graph by ignoring the direction of edges, deleting loops, and considering multiple edges as one edge. The preprocessing can be done with the simple program “preprocess.cpp” in the “Graph” folder. 

# Compile

```
g++ main.cpp -o main -std=c++11 -O3
```

# Usage

Datasets need to be stored in the “Graphs” folder. In “dataset.txt”, enter the graph name as our example shown. To run the program, run the following command. 

```
./main
```
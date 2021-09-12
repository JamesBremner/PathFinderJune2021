# PathFinder

An application to discover optimum path through graphs of costed links. 

## Edit

Menu item `File | Edit` reads a space delimited text file, displays it and allows user to edit it.  Note that the editor is extremely simple, intended only for quick fixes or adjustments.  For more comples tasks, use a fully featured text editor

## Save

Menu item `File | Save` saves file displayed in editor.

## Calculate

Menu item `File | Calulate` reads and calculates path through graph specified in space delimited text file.  

The exact calculation performed is specified by the first line in the file.  

If the calculation is succesful, a graphical display of the graph with the path picked out in red will be shown.

Calculation Options are:

 - __Costs.__ Input specifies undirected links, link costs, starting and ending nodes, then uses the Dijsktra algorithm to find optimal path. [Detailed Documentation](https://github.com/JamesBremner/PathFinder2/wiki/Costs)
 - __Spans__ Input specifies undirected links, with costs then uses Pim's algorithm to find minimum cost links that connect all nodes together. [Detailed Documentation](https://github.com/JamesBremner/PathFinder2/wiki/Spans)
 - __Sales__  Input specifies undirected links, with costs then finds a route that will visit every node once. [Detailed Documentation](https://github.com/JamesBremner/PathFinder2/wiki/Sales)
 - __Cams__ Find a set of nodes that cover every link.  Input specifies undirected links.  [Detailed Documentation](https://github.com/JamesBremner/PathFinder2/wiki/Cams)
 - __Cliques__ Find the set of maximal cliques in a graph. [Detailed Documentation](https://github.com/JamesBremner/PathFinder2/wiki/Cliques)
 - __Flows__ Find maximum flow through a graph.  [Detailed Documentation](https://github.com/JamesBremner/PathFinder2/wiki/Flows)
 - __Hills__ Find easiest path through hilly terrain. [Detailed Documentation](https://github.com/JamesBremner/PathFinder2/wiki/Hills)
 - __Prerequistes__ This option finds paths through ordered requirements. "Before you can do B, you must have done A" [Detailed Documentation](https://github.com/JamesBremner/PathFinder2/wiki/Prerequisites)

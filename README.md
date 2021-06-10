# PathFinder2

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

![image](https://user-images.githubusercontent.com/2046227/120937017-05a78680-c6d9-11eb-9571-57d70303c1c1.png)

 - __Spans__ Input specifies undirected links, with costs then uses Pim's algorithm to find minimum cost links that connect all nodes together. [Detailed Documentation](https://github.com/JamesBremner/PathFinder2/wiki/Spans)

![image](https://user-images.githubusercontent.com/2046227/120936975-d6911500-c6d8-11eb-9bc2-c67df948fbde.png)

 - __Sales__  Input specifies undirected links, with costs then finds a route that will visit every node once. [Detailed Documentation](https://github.com/JamesBremner/PathFinder2/wiki/Sales)
 
 ![image](https://user-images.githubusercontent.com/2046227/120936942-a77aa380-c6d8-11eb-9f4d-6b2c59e4edc3.png)

 - __Cams__ Find a set of nodes that cover every link.  Input specifies undirected links.  [Detailed Documentation](https://github.com/JamesBremner/PathFinder2/wiki/Cams)

![cams](https://user-images.githubusercontent.com/2046227/120936885-4fdc3800-c6d8-11eb-966a-577e1c81b1dd.png)

 - __Cliques__ Find the set of maximal cliques in a graph. [Detailed Documentation](https://github.com/JamesBremner/PathFinder2/wiki/Cliques)

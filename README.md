# PathFinder2

An application to discover optimum path through graphs of costed links. 

## Edit

Menu item `File | Edit` reads a space delimited text file, displays it and allows user to edit it.  Note that the editor is extremely simple, intended only for quick fixes or adjustments.  For more comples tasks, use a fully featured text editor

## Save

Menu item `File | Save` saves file displayed in editor.

## Calculate

Menu item `File | Calulate` reads and calculates path through graph specified in space delimited text file.  The exact calculation performed is specified by the first line in the file.  Options are:


 - __Costs.__ Input specifies undirected links, link costs, starting and ending nodes, then uses the Dijsktra algorithm to find optimal path.

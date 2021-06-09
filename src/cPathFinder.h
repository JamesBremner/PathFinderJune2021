#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "cGraph.h"

/** @brief general purpose path finder
 * 
 **** Usage: Node indices
 * 
 * Use addLink( int u, int v, float cost )
 * to add a costed link between nodes referred by their index
 * 
 * Use start() and end()
 * to specify starting and ending node indices
 * 
 * Use path() 
 * to calculate optimim path from start to end
 * 
 * Use pathText()
 * to get a string listing the node indices visited
 * 
 **** Usage: Node names
 *
 * To refer to nodes by name,
 * rather than keeping track of their indices,
 * use findoradd( std::string& name )
 * PathText() will return a list of nodes visited by their name
 * 
 * <pre>
 
   cPathFinder f;
   f.addLink(
       f.findoradd("A"),
       f.findoradd("B"),
       cost );
    ...
    f.start(f.find("A"));
    f.end(f.find("B"));
    f.path();
    std::cout << f.pathText() << "\n";
  </pre>
 * 
 */
class cPathFinder
{
public:
    void clear();

    /** Add costed link between two nodes
 *
 * @param[in] u node index
 * @param[in] v node index
 * @param[in] cost link cost
 *
 * If the nodes do not exist, they will be added.
 *
 */
    void addLink(
        int u,
        int v,
        double cost = 1 );

    void addLink(
        const std::string &su,
        const std::string &sv,
        double cost = 1);

    int addNode(const std::string &name);

    /** Find or add node by name
 * 
 * @param[in] name
 * @return node index
 * 
 * If a node of specified name does not exist, it is added.
 */
    int findoradd(const std::string &name);

    void deleteNode(int n);

    /// set starting node
    void start(int start);
    void start(const std::string &start);
    int start() const;

    /// set ending node, index
    void end(int end)
    {
        myEnd = end;
    }

    /// set ending node, name
    void end(const std::string &end)
    {
        myEnd = find(end);
    }

    int linkCost( int u, int v );

    /** @brief Find optimum path from start to end node
 *
 * The path from attributes myStart to myEnd
 * is saved into myPath
 * The path can be listed by call to pathText()
 */
    void path();

    /** Find paths from start to all nodes
 */
    void paths(int start);

    /** @brief Find path to end node, after call to paths( int start )
     * 
     * @param[in] end index of end vertex
     * 
     * The path is stored in attribute myPath.
     * The path can be listed by call to pathText()
     * 
     * If no path exists, an exception is thrown.
 */
    std::vector<int> pathPick(int end);

    int distance(int end);

    /// Human readable list of links
    std::string linksText();

    /// Human readable path list
    std::string pathText();

    /// Human readable list of edges that connects all nodes together
    std::string spanText();

    /** @brief graphical display of graph with path in red.
     * 
     * @return display in graphviz dot format
     * 
     * render at https://dreampuf.github.io/GraphvizOnline
     * 
     * or install graphviz and use command
     * 
     * dot -Kfdp -n -Tpng -o sample.png sample.dot
     * 
     */
    std::string pathViz();

    std::string pathViz(
        const std::vector<int> &vp,
        bool all = true);

    /** graphical display of graph with spanning tree
     * 
     * @param[in] all true to see all links, spanning tree in red
     * @param[in] all false to see spanning tree
     * @return display in graphviz dot format
     */
    std::string spanViz(bool all = true);

    std::string camsViz();

    /** @brief Find node by name
 * 
 * @param[in] name
 * @return node index, -1 if named node does not exist
 */
    int find(const std::string &name);

    /// true if link between nodes
    bool IsAdjacent(int u, int v);

    /// true if all nodes are connected together
    bool IsConnected();

    int nodeCount();
    int linkCount();
    int islandCount();

    std::string nodeColor(int n) const;
    void nodeColor(int n, const std::string &color);
    std::string nodeName(int n);

    /** @brief set graph links type
     * 
     * @param[in] f true for directed, false for undirected, default directed
     * 
     * If not called, the graph will be undirected
     * 
     */
    void directed(bool f = true)
    {
        myGraph.directed( f );
    }

    /// add expensive ( INT_MAX ) links between unlinked nodes
    void makeComplete();

    /** make costs positive
     * @param[in] cost the maximum negative cost
     * 
     * The absolute value of cost will be added to every cost
     * cost will be saved to to attribute myMaxNegCost
     */
    void makeCostsPositive(int cost);

    /// Find minimum edge set that connects all nodes together
    void span();

    /// Find path that visits every node
    void tsp();

    /// Find nodes that cover all links
    void cams();

    /// Find path in grid that minimizes changes in height
    void hills(
        const std::vector<std::vector<float>> &gheight);

    void gsingh();

    void shaun();

    void flows();


private:

    graph::cGraph myGraph;

    int              myStart;             // starting node index
    int              myEnd;               // ending node index
    std::vector<int> myPath;              // vector of node indices visited
    std::vector<int> myDist;              // cost to reach each node from start
    std::vector<int> myPred;              // previous node to each node from start
    graph::cGraph    mySpanTree;          // minimum spanning tree
    double           myPathCost;          // total cost of links in path
    int              myMaxNegCost;

    //cEdge& linkProps( int u, int v );

    void depthFirst( int v );
    void depthRecurse( int v );

    void copyNodes( const graph::cGraph& other );

    // template <typename T>
    // std::string linksTextT(T &g);

    // template <typename T>
    // void pathsT(int start, T &g);

    // template <typename T>
    // std::string pathVizT(
    //     const std::vector<int> &vp,
    //     bool all,
    //     T &g);
};
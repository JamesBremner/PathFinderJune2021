#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <functional>
#include "cGraph.h"

namespace raven
{
    namespace graph
    {
        enum class eCalculation
        {
            none,
            not_open,
            costs,
            spans,
            sales,
            hills,
            cams,
            gsingh,
            shaun,
            islands,
            flows,
            multiflows,
            maze_ascii_art,
            cliques,
            reqs,
        };

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
        class cPathFinder : public cGraph
        {
        public:
            /////////////////  Class constructors ///////////////////

            cPathFinder()
            {
            }
            cPathFinder(const graph::cGraph &g)
            {
                myG = g.graph();
                myfDirected = g.isDirected();
            }

            /////////////////////////////////// setters //////////////////////////

            void clear();

            /// starting node
            void start(int start);
            void start(const std::string &start);
            int start() const;
            void addSource(const std::string &source);

            /// ending node
            void end(int end)
            {
                myEnd = end;
            }
            void end(const std::string &end)
            {
                myEnd = find(end);
            }

            ///////////////////////// methods applying algorithms //////////////////////

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

            /// true if all nodes are connected together
            bool IsConnected();

            int islandCount();

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

            /** Find path that visits every requested node
             * @parasm[in] v vector of node indices to be visited
             * 
             * If v is empty, all nodes must be visited
             */
            void tsp(const std::vector<int>& v);

            /// Find path that visits every node
            std::vector<int> tsp();

            /// Find nodes that cover all links
            void cams();

            /// Find path in grid that minimizes changes in height
            void hills(
                const std::vector<std::vector<float>> &gheight);

            void gsingh();

            void shaun();

            /// Find maximum flow through graph
            void flows();
            void multiflows();
            void equiflows();

            void waterValves(
                const std::vector<int> &valveTimes,
                bool pulse);

            /// Find maximal cliques in graph
            void cliques();

            /// Find prerequisites
            void PreReqs(
                const std::vector<std::string> &va);

            /** Given a bi-partite graph with node weights for both type A and type B nodes
1 For each node of type B we sum over node weights of type A that this node has an edge with and multiply the sum with its own node weight to get the node value.
2 Select the node from type B which has the highest value and append it to the output set S.
3 Delete the selected node from type B and all the nodes it had an edge to from type A.
4 Go back to step 1 until any node in type B is left with an edge to a node in type A.
5 Append any remaining node of type B to the output set in order of its node weight.

https://stackoverflow.com/q/62338424/16582

The output, an ordered vector of node indices, is stored in myPath attribute
*/
            void karup();

            /////////////////////// get text output ///////////////////////////////////////////

            /// Human readable path list
            std::string pathText();

            std::string resultsText()
            {
                return myResults;
            }

            /// Human readable list of edges that connects all nodes together
            std::string spanText();

            ////////////////// get graphViz output ////////////////////////////////////////////

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

            /** Depth First search
     * @param[in] v index of starting node
     * @param[in] visitor function to call on each new node visited
     */
            void depthFirst(int v, std::function<void(int v)> visitor);

        private:
            int myStart; // starting node index
            std::vector<int> mySource;
            int myEnd;                // ending node index
            std::vector<int> myPath;  // vector of node indices visited
            std::vector<int> myDist;  // cost to reach each node from start
            std::vector<int> myPred;  // previous node to each node from start
            graph::cGraph mySpanTree; // minimum spanning tree
            double myPathCost;        // total cost of links in path
            int myMaxNegCost;
            std::string myResults;

            void depthRecurse(int v, std::function<void(int v)> visitor);

            /** Breadth First Search
     * @param[in] visitor function to call on each new node visited
     */
            void breadth(std::function<void(int v, int p)> visitor);
        };
    }
}

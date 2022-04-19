#include <sstream>
#include <algorithm>
#include <queue>
#include <set>
#include <list>
#include "cPathFinder.h"
#include "cRunWatch.h"

namespace raven
{
    namespace graph
    {

        void cPathFinder::clear()
        {
            cGraph::clear();
            myPath.clear();
            mySource.clear();
            myPathCost = 0;
        }

        std::string cPathFinder::pathViz()
        {
            std::string graphvizgraph = "graph";
            std::string graphvizlink = "--";
            if (isDirected())
            {
                graphvizgraph = "digraph";
                graphvizlink = "->";
            }

            std::stringstream f;
            f << graphvizgraph << " G {\n";
            for (auto n : nodes())
            {
                f << n.second.myName
                  << " [color=\"" << n.second.myColor << "\"  penwidth = 3.0 ];\n";
            }

            // loop over links
            for (auto &e : links())
            {
                if (!isDirected())

                    /* an undirected graph has a pair of directed links
                    beteen every pair of linked nodes. Suppress the display
                    of the second link of the pair
                    */
                    if (e.first.first > e.first.second)
                        continue;

                // check if link between two nodes on path
                bool onpath = false;
                auto pathItsrc = std::find(myPath.begin(), myPath.end(), e.first.first);
                auto pathItdst = std::find(myPath.begin(), myPath.end(), e.first.second);
                if (pathItsrc != myPath.end() && pathItdst != myPath.end())
                    if (pathItsrc == pathItdst + 1 || pathItsrc == pathItdst - 1)
                        onpath = true;

                f << node(e.first.first).myName << graphvizlink
                  << node(e.first.second).myName;
                if (onpath)
                    f << "[color=\"red\"] ";
                f << "\n";
            }

            f << "}\n";
            return f.str();
        }

        std::string cPathFinder::camsViz()
        {
            std::string graphvizgraph = "graph";
            std::string graphvizlink = "--";
            if (isDirected())
            {
                graphvizgraph = "digraph";
                graphvizlink = "->";
            }

            std::stringstream f;
            f << graphvizgraph << " G {\n";
            for (auto &n : nodes())
            {
                f << n.second.myName
                  << " [color=\"" << n.second.myColor << "\"  penwidth = 3.0 ];\n";
            }

            // loop over links
            for (auto &e : links())
            {
                f << userName(e.first.first) << graphvizlink
                  << userName(e.first.second)
                  << "\n";
            }

            f << "}\n";
            return f.str();
        }

        std::string cPathFinder::spanViz(bool all)
        {
            std::string graphvizgraph = "graph";
            std::string graphvizlink = "--";
            if (isDirected())
            {
                graphvizgraph = "digraph";
                graphvizlink = "->";
            }

            std::stringstream f;
            f << graphvizgraph << " G {\n";
            for (auto &n : nodes())
            {
                f << n.second.myName
                  << " [color=\"" << n.second.myColor << "\"  penwidth = 3.0 ];\n";
            }

            // loop over links
            for (auto &e : links())
            {
                f << node(e.first.first).myName << graphvizlink
                  << node(e.first.second).myName;
                if (mySpanTree.includes_link(e.first))
                    f << "[color=\"red\"] ";
                f << "\n";
            }

            f << "}\n";
            return f.str();
        }
        void cPathFinder::start(int n)
        {
            if (0 > n || n >= myG.size())
                throw std::runtime_error(
                    "cPathFinder::bad start node");
            myStart = n;
        }
        void cPathFinder::start(const std::string &start)
        {
            myStart = find(start);
            if (myStart < 0)
                throw std::runtime_error("cPathFinder::bad start node");
        }

        void cPathFinder::addSource(const std::string &source)
        {
            int s = find(source);
            if (s < 0)
                throw std::runtime_error(
                    "cPathFinder::bad source node");
            mySource.push_back(s);
        }

        void cPathFinder::path()
        {
            paths(myStart);
            pathPick(myEnd);
        }
        void cPathFinder::paths(int start)
        {
            int V = nodeCount();

            myDist.clear();
            myDist.resize(V);
            myPred.clear();
            myPred.resize(V, -1);

            bool sptSet[V]; // sptSet[i] will be true if vertex i is included in shortest
            // path tree or shortest distance from src to i is finalized

            // Initialize all distances as INFINITE and stpSet[] as false
            for (int i = 0; i < V; i++)
                myDist[i] = INT_MAX, sptSet[i] = false;

            // Distance of source vertex from itself is always 0
            myDist[start] = 0;
            myPred[start] = 0;

            // Find shortest path for all vertices
            for (int count = 0; count < V - 1; count++)
            {
                // Pick the minimum distance vertex from the set of vertices not
                // yet processed. u is always equal to src in the first iteration.
                int min = INT_MAX, u;

                for (int v = 0; v < V; v++)
                    if (sptSet[v] == false && myDist[v] <= min)
                        min = myDist[v], u = v;
                if (min == INT_MAX)
                {
                    // no more nodes connected to start
                    break;
                }

                // Mark the picked vertex as processed
                sptSet[u] = true;

                // Update dist value of the adjacent vertices of the picked vertex.
                for (int v : adjacent(u))
                {
                    if (sptSet[v])
                        continue; // already processed

                    // Update dist[v] only if total weight of path from src to  v through u is
                    // smaller than current value of dist[v]
                    int linkcost = cost(u, v);
                    if (myDist[u] + linkcost < myDist[v])
                    {
                        myDist[v] = myDist[u] + linkcost;
                        myPred[v] = u;
                    }
                }
            }
        }

        std::vector<int> cPathFinder::pathPick(int end)
        {
            myPath.clear();
            std::cout << "->cPathFinder::pathPick "
                      << myStart << " " << end << "\n";
            for (int d : myPred)
            {
                std::cout << d << " ";
            }
            std::cout << "\n";

            if (end < 0)
                throw std::runtime_error("cPathFinder::pathPick bad end node");
            if (myPred[end] == end || myPred[end] == -1)
                return myPath; // there is no path

            // pick out path, starting at goal and finishing at start
            myPath.push_back(end);
            int prev = end;
            while (1)
            {
                // std::cout << prev << " " << myPred[prev] << ", ";
                int next = myPred[prev];
                myPath.push_back(next);
                if (next == myStart)
                    break;
                prev = next;
            }

            // reverse so path goes from start to goal
            std::reverse(myPath.begin(), myPath.end());

            std::cout << "\n dbg " << myPath.size() << " " << myDist.size()
                      << " " << myPath.back() << "\n";
            for (auto d : myDist)
                std::cout << d << " ";
            std::cout << "\n";
            // if (myDist.size() < myPath.back() + 1)
            myPathCost = myDist[myPath.back()]
                //    + myMaxNegCost * (myPath.size() - 1)
                ;
            // else
            //     myPathCost = -1;

            return myPath;
        }

        std::string cPathFinder::pathText()
        {
            std::stringstream ss;
            for (auto n : myPath)
            {
                std::string sn;
                sn = userName(n);

                if (sn == "???")
                    sn = std::to_string(n);
                ss << sn << " -> ";
            }

            if (myPath.size() && myDist.size() && myPathCost >= 0)
            {
                ss << " Cost is "
                   << myPathCost + myMaxNegCost * (myPath.size() - 1)
                   << "\n";
            }

            return ss.str();
        }

        void cPathFinder::span()
        {
            myPathCost = 0;
            int V = nodeCount();
            std::vector<bool> Q(V, false); // set true when node added to spanning tree
            mySpanTree.clear();
            mySpanTree.copyNodes(*this);

            // add initial arbitrary link
            int v = 0;
            int w = *adjacent(0).begin();
            mySpanTree.addLink(v, w);
            Q[v] = true;
            Q[w] = true;
            // std::cout << "adding " << name(v) << " " << name(w)
            //           << " " << cost(v, w) << "\n";

            // while nodes remain outside of span
            while (1)
            {
                int v;      // node in span
                int w = -1; // node not yet in span
                int min_cost = INT_MAX;

                // loop over nodes in span
                for (int kv = 0; kv < Q.size(); kv++)
                {
                    if (!Q[kv])
                        continue;

                    // loop over nodes not in span
                    for (int kw = 0; kw < Q.size(); kw++)
                    {
                        if (Q[kw])
                            continue;

                        if (!includes_link(kv, kw))
                            continue;

                        // find cheapest link that adds node to span
                        int linkCost = cost(kv, kw);
                        if (linkCost < min_cost)
                        {
                            min_cost = linkCost;
                            v = kv;
                            w = kw;
                        }
                    }
                }

                // check if any nodes were found not yet in span
                if (w == -1)
                    break;

                // add node to span
                Q[w] = true;
                mySpanTree.addLink(v, w);
                myPathCost += cost(v, w);
            }
        }

        std::string cPathFinder::spanText()
        {
            return mySpanTree.linksText();
        }

        void cPathFinder::depthFirst(
            int v,
            std::function<void(int v)> visitor)
        {
            myPath.clear();
            myPath.resize(nodeCount(), 0);
            myPred.clear();

            depthRecurse(v, visitor);
        }

        void cPathFinder::depthRecurse(
            int v,
            std::function<void(int v)> visitor)
        {
            visitor(v);

            // remember this node has been visted
            myPath[v] = 1;

            // look for new adjacent nodes
            for (int w : adjacent(v))
                if (!myPath[w])
                {
                    // search from new node
                    depthRecurse(w, visitor);
                }
        }

        void cPathFinder::breadth(
            std::function<void(int v, int p)> visitor)
        {
            if (!nodeCount())
                throw std::runtime_error("breadth called on empty graph");

            std::vector<bool> visited(nodeCount(), false);
            std::queue<int> Q;

            visited[myStart] = true;
            Q.push(myStart);

            while (Q.size())
            {
                int v = Q.front();
                Q.pop();
                for (int w : adjacent(v))
                {
                    if (!visited[w])
                    {
                        // reached a new node
                        visitor(w, v);
                        visited[w] = true;
                        Q.push(w);
                    }
                }
            }
        }

        void cPathFinder::tsp(const std::vector<int> &v)
        {
            if (!v.size())
            {
                tsp();
                return;
            }
        }
        std::vector<int> cPathFinder::tsp()
        {

            // calculate spanning tree
            span();

            // construct pathFinder from spanning tree
            cPathFinder pf(mySpanTree);

            // // depth first search of spanning tree
            pf.depthFirst(0, [this](int v)
                          {
                              // record new node on the search
                              myPath.push_back(v); });

            // return to starting point
            myPath.push_back(myPath[0]);

            // cost
            myPathCost = 0;
            int prev = -1;
            for (int n : myPath)
            {
                if (prev < 0)
                {
                    prev = n;
                    continue;
                }
                myPathCost += link(prev, n).myCost;
                prev = n;
            }

            return myPath;
        }

        void cPathFinder::cams()
        {
            myPath.clear();

            // working copy of input graph
            auto work = *this;

            // The nodes that connect leaf nodes to the rest of the graph must be in cover set
            for (int leaf = 0; leaf < nodeCount(); leaf++)
            {
                auto ns = adjacent(leaf);
                if (ns.size() > 1)
                    continue;

                // we have a leaf node
                // get node that connects it
                // add to cover set
                int leafcover = *ns.begin();
                myPath.push_back(leafcover);

                // remove all covered links from working graph
                for (int t : work.adjacent(leafcover))
                {
                    work.removeLink(leafcover, t);
                }
            }

            // loop until all links are covered
            while (work.linkCount())
            {
                // select first remaining uncovered link
                auto l = *(work.links().begin());
                int u = source(l);
                int v = target(l);

                if (u < 0 || v < 0)
                    throw std::runtime_error(
                        "cPathFinder::cams bad index");

                // add node with greatest degree to cover set
                auto sun = work.adjacent(u);
                auto svn = work.adjacent(v);
                if (svn.size() > sun.size())
                {
                    myPath.push_back(v);
                    for (int t : svn)
                    {
                        work.removeLink(v, t);
                        work.removeLink(t, v);
                    }
                }
                else
                {
                    myPath.push_back(u);
                    for (int t : sun)
                    {
                        work.removeLink(u, t);
                        work.removeLink(t, u);
                    }
                }
            }
            for (int n : myPath)
                node(n).myColor = "red";
            myPathCost = -1;
        }

        void cPathFinder::cliques()
        {
            // working copy on input graph
            auto work = *this;

            // store for maximal clique collection
            std::vector<std::vector<int>> vclique;

            while (1)
            {
                std::vector<int> clique;

                while (work.nodeCount())
                {
                    // std::cout << "work.nodeCount " << work.nodeCount() << " " << clique.size() << "\n";
                    if (!clique.size())
                    {
                        // start by moving an arbitrary node to the clique from the work graph
                        auto nit = work.nodes().begin();
                        clique.push_back(nit->first);
                        work.removeNode(nit->first);
                        continue;
                    }
                    bool found = false;

                    // loop over nodes remaining in work graph
                    for (auto &u : work.nodes())
                    {
                        // loop over nodes in clique
                        for (int v : clique)
                        {
                            if (work.includes_link(v, u.first) ||
                                work.includes_link(u.first, v))
                            {
                                // found node in work that is connected to clique nodes.
                                // move it to clique
                                // std::cout << "add " << myGraph.name(u.first) << "\n";
                                clique.push_back(u.first);
                                work.removeNode(u.first);
                                found = true;
                                break;
                            }
                        }
                        if (found)
                            break; // found a node to add to clique
                    }
                    if (!found)
                        break; // no more nodes can be added, the clique is maximal
                }

                if (!clique.size())
                    break; // did not find a clique

                // add to collection of maximal cliques
                vclique.push_back(clique);
            }

            // Display results
            std::stringstream ss;
            for (auto &c : vclique)
            {
                ss << "clique: ";
                for (int n : c)
                    ss << userName(n) << " ";
                ss << "\n";
            }
            myResults = ss.str();
            std::cout << myResults;
        }

        void cPathFinder::flows()
        {
            /*
    use link cost as maximum capacity
    use link value as used capacity
    use path cost as total flow
    */

            int totalFlow = 0;

            // backup graph
            cPathFinder bkup(*(graph::cGraph *)this);

            while (1)
            {
                // find path
                // std::cout << "links:\n" << linksText() << "\n";
                path();
                // std::cout << "pathsize " << myPath.size() << " ";
                if (!myPath.size())
                    break;
                // std::cout << "Path " << pathText() << "\n";

                // maximum flow through path
                int maxflow = INT_MAX;
                int u = -1;
                int v;
                for (int v : myPath)
                {
                    if (u >= 0)
                    {
                        double cap = findLink(u, v).myCost;
                        if (cap < maxflow)
                        {
                            maxflow = cap;
                        }
                    }

                    u = v;
                }

                // consume capacity
                u = -1;
                for (int v : myPath)
                {
                    if (u >= 0)
                    {
                        auto &l = findLink(u, v);
                        l.myCost -= maxflow;
                        if (l.myCost < 0.01)
                            removeLink(u, v);

                        bkup.link(u, v).myValue += maxflow;
                        if (!isDirected())
                            bkup.link(v, u).myValue += maxflow;
                    }
                    u = v;
                }

                totalFlow += maxflow;
            }

            // restore backup graph
            myG = bkup.graph();

            myPathCost = totalFlow;
            myResults = "total flow " + std::to_string(totalFlow);
            std::cout << myResults << "\n";

            std::stringstream ss;
            for (auto &n : nodes())
            {
                for (auto &l : n.second.myLink)
                {
                    if (!isDirected())
                        if (n.first > l.first)
                            continue;
                    ss << n.second.myName << " -- "
                       << userName(l.first) << " capacity "
                       << l.second.myCost << " used "
                       << l.second.myValue << "\n";
                }
            }
            std::cout << ss.str();
        }

        void cPathFinder::multiflows()
        {
            double totalmultiflow = 0;
            for (int s : mySource)
            {
                std::cout << "multisource " << userName(s) << "\n";
                myStart = s;
                flows();
                totalmultiflow += myPathCost;
            }
            myPathCost = totalmultiflow;
            myResults = "total flow " + std::to_string(totalmultiflow);
            std::cout << myResults << "\n";
        }

        void cPathFinder::waterValves(
            const std::vector<int> &valveTimes,
            bool pulse)
        {
            // time that water reaches each node
            std::vector<int> timeReached(nodeCount(), -1);

            std::vector<int> wait(nodeCount(), 0);

            myDist.resize(nodeCount(), -1);

            myPred.resize(nodeCount());

            // queue of nodes waiting to be searched from
            std::queue<int> Q;

            // start at the start
            timeReached[myStart] = 0;
            Q.push(myStart);

            // loop until all nodes searched from
            while (Q.size())
            {
                // next node to continue search from
                int v = Q.front();
                Q.pop();
                int Pt = timeReached[v];

                // std::cout << "search from " << node(v).myName << "\n";

                // loop over nodes that can be reached from v
                for (int w : adjacent(v))
                {
                    // wait for valve to open
                    int Wt = 0;

                    if (!pulse)
                    {
                        /**    assumes that valve opens at time specified
                         *     and remains open threafter
                         */
                        Wt = valveTimes[v] - Pt;
                        if (Wt < 0)
                            Wt = 0;
                    }
                    else
                    {
                        /** assumes that valves open instantaneously
                         * every t seconds
                         */
                        int t = valveTimes[v];
                        int k = 1;
                        while (k * t < Pt)
                            k++;
                        Wt = k * t - Pt;
                    }

                    // time to flow through pipe
                    int Lt = link(v, w).myCost;

                    // time to arrive at adjacent node
                    int Vt = Pt + Wt + Lt;

                    // std::cout << "visited " << node(w).myName
                    //           << " from " << node(v).myName
                    //           << " " << Pt << " " << Wt << " " << Lt << " " << Vt
                    //           << " " << timeReached[w] << "\n";

                    // if this is later than a previous visit, stop searching
                    if (timeReached[w] > -1)
                        if (Vt >= timeReached[w])
                            continue;

                    // std::cout << "reached " << node(w).myName << "\n";

                    // node w reached before any other route
                    timeReached[w] = Vt;
                    myPred[w] = v;
                    myDist[w] = Vt;
                    wait[v] = Wt;

                    // continue search from here
                    Q.push(w);
                }
            }
            std::cout
                << "Water reached " << node(myEnd).myName
                << " at time " << timeReached[myEnd] << "\n";

            pathPick(myEnd);

            std::cout << "node reached at, then waited\n";
            for (int n : myPath)
            {
                std::cout << userName(n) << "\t"
                          << myDist[n] << "\t"
                          << wait[n] << "\n";
            }
        }
        void cPathFinder::equiflows()
        {
            // source outflows
            int outlinkpercent = 100 / node(myStart).outdegree();
            for (auto &outlink : node(myStart).myLink)
                outlink.second.myValue = outlinkpercent;

            // do a breadth first search, updating flows as we go along
            breadth(
                [this](int v, int prev)
                {
                    static int totalFlow = INT_MAX;

                    if (v == myEnd)
                    {
                        // destination reached, report maximum flow
                        myPathCost = totalFlow;
                        return;
                    }

                    // sum the inputs
                    int sumInput = 0;
                    for (auto &inlink : inlinks(v))
                    {
                        sumInput += inlink.second.myValue;
                    }

                    // node outflows
                    int outValue = sumInput / node(v).outdegree();
                    for (auto &outlink : node(v).myLink)
                    {
                        outlink.second.myValue = outValue;

                        // check if this outflow reduces maximum flow through network
                        int x = outlink.second.myCost * 100 / outValue;
                        if (x < totalFlow)
                            totalFlow = x;
                    }
                });

            std::stringstream ss;
            ss << "total flow " << std::to_string(myPathCost) << "\n";
            for (auto &n : nodes())
            {
                for (auto &l : n.second.myLink)
                {
                    if (!isDirected())
                        if (n.first > l.first)
                            continue;
                    ss << n.second.myName << " -> "
                       << userName(l.first) << " capacity "
                       << l.second.myCost << " used "
                       << myPathCost * l.second.myValue / 100 << "\n";
                }
            }
            std::cout << ss.str();
            myResults = ss.str();
        }
        void cPathFinder::hills(
            const std::vector<std::vector<float>> &gheight)
        {

            // cost links according to change in height they incur
            int rowCount = gheight.size();
            if (!rowCount)
                throw std::runtime_error(
                    "cPathFinder::hills bad grid");
            int colCount = gheight[0].size();

            for (auto &l : links())
            {
                int source = l.first.first;
                int target = l.first.second;
                int srow = source / colCount;
                int scol = source - srow * colCount;
                int trow = target / colCount;
                int tcol = target - trow * colCount;
                float sh = gheight[srow][scol];
                float th = gheight[trow][tcol];
                float delta = th - sh;
                l.second.myCost = 1 + delta * delta;
            }

            path();

            myResults = pathText();
            std::cout << pathText();
        }

        void cPathFinder::allPaths()
        {
            std::vector<int> p;
            std::vector<std::vector<int>> vp;
            p.push_back(myStart);
            breadth(
                [&, this](int v, int prev)
                {
                    std::cout << userName(v) << ", ";
                    p.push_back(v);
                });
            std::cout << "\n";
            vp.push_back(p);
            myStart = p[p.size() - 2];
            p.clear();
            p.push_back(myStart);
            breadth(
                [&, this](int v, int prev)
                {
                    std::cout << userName(v) << ", ";
                    p.push_back(v);
                });
            std::cout << "\n";
        }
        void cPathFinder::PreReqs(
            const std::vector<std::string> &va)
        {
            std::set<int> setSkillsNeeded;

            // starting node
            start("0");

            // paths to all end nodes
            end(-1);

            // run Dijsktra
            paths(myStart);

            // loop over required skills
            for (auto &a : va)
            {
                // skills needed to get required skill
                auto path = pathPick(find(a));

                std::cout << "skill " << a << " needs ";
                for (int s : path)
                    std::cout << userName(s) << " ";
                std::cout << "\n";

                // loop over prerequsites
                for (auto s : path)
                {
                    // record skill if not already learned
                    setSkillsNeeded.insert(s);
                }
            }

            for (int s : setSkillsNeeded)
                node(s).myColor = "red";

            std::stringstream ss;
            ss << "Total skills needed "
               << setSkillsNeeded.size() << " ( ";
            for (int s : setSkillsNeeded)
                ss << userName(s) << " ";
            ss << " )";
            myResults = ss.str();
            std::cout << myResults << "\n";
        }

        bool cPathFinder::isConnected(
            int node1,
            int node2)
        {
            raven::set::cRunWatch aWatcher("isConnected");
            try
            {
                depthFirst(
                    node1,
                    [&](int v)
                    {
                        if (v == node2)
                            throw std::runtime_error("done");
                    });
                return false;
            }
            catch (std::runtime_error &e)
            {
                // std::cout << "catch " << e.what() << "\n";
                // if (e.what() == "done")
                return true;
                // throw e;
            }
        }

        void cPathFinder::karup()
        {
            raven::set::cRunWatch aWatcher("karup");
            std::cout << "karup on " << nodeCount() << " node graph\n";
            myPath.clear();

            // calculate initial values of B nodes
            std::multimap<int, int> mapValueNode;
            for (auto &b : nodes())
            {
                if (b.second.myName[0] != 'b')
                    continue;
                int value = 0;
                for (auto a : b.second.myLink)
                {
                    value += node(a.first).myCost;
                }
                value *= b.second.myCost;
                mapValueNode.insert(std::make_pair(value, b.first));
            }

            // while not all B nodes output
            while (mapValueNode.size())
            {
                raven::set::cRunWatch aWatcher("select");
                // std::cout << "B node\tValue\n";
                // for (auto &nv : mapValueNode)
                //     std::cout << name(nv.second) << "\t" << nv.first << "\n";

                // select node with highest value
                auto remove_it = --mapValueNode.end();
                int remove = remove_it->second;

                if (!remove_it->first)
                {
                    /** all remaining nodes have zero value
                     * all the links from B nodes to A nodes have been removed
                     * output remaining nodes in order of decreasing node weight
                     */
                    raven::set::cRunWatch aWatcher("Bunlinked");
                    std::multimap<int, int> mapNodeValueNode;
                    for (auto &nv : mapValueNode)
                    {
                        mapNodeValueNode.insert(
                            std::make_pair(
                                node(nv.second).myCost,
                                nv.second));
                    }
                    for (auto &nv : mapNodeValueNode)
                    {
                        myPath.push_back(nv.second);
                    }
                    break;
                }

                bool OK = true;
                int value = 0;
                {
                    raven::set::cRunWatch aWatcher("check");

                    // check that no nodes providing value have been removed

                    // std::cout << "checking neighbors of " << name(remove) << "\n";

                    auto &vl = node(remove).myLink;
                    for (auto it = vl.begin(); it != vl.end();)
                    {
                        if (!myG.count(it->first))
                        {
                            // A neighbour has been removed
                            OK = false;
                            it = vl.erase(it);
                        }
                        else
                        {
                            // A neighbour remains
                            value += node(it->first).myCost;
                            it++;
                        }
                    }
                }

                if (OK)
                {
                    raven::set::cRunWatch aWatcher("store");
                    // we have a node whose values is highest and valid

                    // store result
                    myPath.push_back(remove);

                    // remove neighbour A nodes
                    auto &ls = node(remove).myLink;
                    for (auto &l : ls)
                    {
                        myG.erase(l.first);
                    }
                    // remove the B node
                    // std::cout << "remove " << name( remove ) << "\n";
                    mapValueNode.erase(remove_it);
                }
                else
                {
                    // replace old value with new
                    raven::set::cRunWatch aWatcher("replace");
                    value *= node(remove).myCost;
                    mapValueNode.erase(remove_it);
                    mapValueNode.insert(std::make_pair(value, remove));
                }
            }
        }

        typedef std::pair<int, int> tribnode_t;              // tributary node ( node index, distance from crash site )
        typedef std::vector<tribnode_t> trib_t;              // tributary to crash node ( vector of tributary nodes)
        typedef std::pair<int, std::vector<trib_t>> crash_t; // crash node ( index, vector of tributaries )
        void cPathFinder::buildtributary(
            trib_t &trib,
            std::map<int, linkmap_t> &mapInLinks)
        {
            // std::cout << "=>buildtributary ";
            // raven::set::cRunWatch watcher("buildtributary");
            //  for (auto &tn : trib)
            //  {
            //      std::cout << tn.first << " ";
            //  }
            //  std::cout << "\n";

            // std::cout << linksText() << "\n";

            linkmap_t lm = mapInLinks[trib.back().first];
            if (!lm.size())
            {
                // std::cout << "trib end\n";
                return;
            }
            if (lm.size() > 1)
            {
                // std::cout << "trib split\n";
                return;
            }
            // std::cout << "adding " << userName(lm.begin()->first.first)
            //           << " to " << userName(trib.back().first) << "\n";

            trib.push_back(std::make_pair(
                lm.begin()->first.first,
                trib.back().second + 1));
            buildtributary(trib, mapInLinks);
        }

        void cPathFinder::srcnuzn()
        {
            raven::set::cRunWatch aWatcher("srcnuzn");

            // backup the full graph
            auto backup = myG;

            // identify and remove potential back edges
            std::vector<std::pair<int, int>> vforbidden;
            {
                raven::set::cRunWatch aWatcher("backedge_find");

                for (auto &n : nodes())
                {

                    for (int a : adjacent(n.first))
                    {
                        if (n.second.myCost - node(a).myCost >= 2)
                        {
                            // path must not contain this pair of nodes
                            vforbidden.push_back(std::make_pair(n.first, a));

                            // remove the edge
                            n.second.removeLink(a);
                        }
                    }
                }
            }

            try
            {
                // recursively visit all possible paths
                visitAllPaths(
                    myStart, myEnd,
                    [this, &vforbidden](int pathlength) -> int
                    {
                        raven::set::cRunWatch aWatcher("visitor");
                        // this "visitor" is called whenever a possible path is found

                        // for (int i = 0; i < pathlength; i++)
                        //     std::cout << userName(myPath[i]) << " ";
                        // std::cout << "\n";

                        for (int i = 0; i < pathlength; i++)
                        {
                            int n = myPath[i];
                            for (int ip = 0; ip < i; ip++)
                            {
                                int prev = myPath[ip];

                                // check if path contains any node pairs forbidden by backedges
                                for (auto &f : vforbidden)
                                {
                                    if (f.first == n && f.second == prev)
                                    {
                                        // std::cout << userName(f.first) << " " << userName(f.second)
                                        //           << " forbidden"
                                        //           <<" stop " << userName(myPath[i-1]) <<" "<< userName(myPath[i])
                                        //           << "\n";
                                        return f.first;
                                    }
                                }
                            }
                        }

                        // feasible path found, mark and throw exception to escape from recursion
                        myPath.resize(pathlength);
                        throw std::domain_error("srcnuzn_ok");
                    });

                // all possible paths visited witn no feasible found
                std::cout << "no feasible path\n";
                myPath.clear();
            }

            catch (std::domain_error &e)
            {
                // exception thrown, indicating a feasible path found
                std::cout << "srcnuzn_ok ";
                for (auto n : myPath)
                    std::cout << userName(n) << " ";
                std::cout << "\n";
            }

            // restore full path
            myG = backup;
        }

        void cPathFinder::srcnuzn_generate()
        {
            const int layerCount = 10;
            const int nodesLayer = 50;
            const int backsLayer = 4;
            clear();
            directed();
            myStart = findoradd("L0");
            node(myStart).myCost = 0;
            for (int layer = 1; layer < layerCount - 1; layer++)
            {
                for (int vl = 0; vl < nodesLayer; vl++)
                {
                    int n = findoradd(
                        "L" + std::to_string(layer) + "N" + std::to_string(vl));
                    node(n).myCost = layer;
                    if (layer == 1)
                        addLink(0, n);
                    else
                    {
                        for (int bv = 0; bv < nodesLayer; bv++)
                        {
                            addLink(
                                (layer - 2) * nodesLayer + bv + 1,
                                n);
                        }
                    }
                }
            }
            myEnd = findoradd("L" + std::to_string(layerCount - 1));
            node(myEnd).myCost = layerCount;
            for (int bv = 0; bv < nodesLayer; bv++)
            {
                addLink(
                    (layerCount - 3) * nodesLayer + bv + 1,
                    myEnd);
            }

            // add random back edges
            for (int sl = 3; sl < layerCount - 1; sl++)
            {
                for (int kb = 0; kb < backsLayer; kb++)
                {
                    int s = rand() % nodesLayer;
                    int d = rand() % nodesLayer;
                    int sn = (sl - 1) * nodesLayer + s + 1;
                    int dn = (sl - 3) * nodesLayer + d + 1;
                    std::cout << userName(sn) << " back to " << userName(dn) << "\n";
                    addLink(sn, dn);
                }
            }
        }

        void cPathFinder::visitAllPaths(
            int s, int d,
            std::function<int(int pathlength)> pathVisitor)
        {
            int V = nodeCount();

            // Mark all the vertices as not visited
            std::vector<bool> visited;
            visited.resize(V, false);

            myPath.resize(V, -1);
            int path_index = 0; // Initialize path[] as empty

            // Call the recursive
            visitAllPathsRecurse(s, visited, path_index,
                                 pathVisitor);
        }

        // A recursive function to print all paths from 'u' to myEnd.
        // visited[] keeps track of vertices in current path.
        // path[] stores actual vertices and path_index is current
        // index in path[]
        void cPathFinder::visitAllPathsRecurse(int u,
                                               std::vector<bool> &visited,
                                               int &path_index,
                                               std::function<int(int pthlength)> pathVisitor)
        {
            raven::set::cRunWatch aWatcher("visitAllPathsRecurse");

            // Mark the current node and store it in path[]
            // std::cout << "add " << userName(u) << " at " << path_index << "\n";
            visited[u] = true;
            myPath[path_index] = u;
            path_index++;

            // for( int i = 0; i < path_index; i++ )
            //     std::cout << userName(myPath[i]) << " ";
            // std::cout << " inc path\n";

            // If current vertex is same as destination, then vist current path
            if (u == myEnd)
            {
                int forbidden = pathVisitor(path_index);
            }

            else // If current vertex is not destination
            {
                // Recurse for all the vertices adjacent to current vertex
                for (auto a : adjacent(u))
                {
                    if (!visited[a])
                    {
                        // std::cout << "try " << userName(u) <<" " << userName(a) << "\n";
                        visitAllPathsRecurse(
                            a,
                            visited, path_index,
                            pathVisitor);
                    }
                }
            }

            // Remove current vertex from path[] and mark it as unvisited
            path_index--;
            visited[u] = false;
        }

        void cPathFinder::collision()
        {
            std::cout << "Finding collisions in " << nodeCount() << " node graph\n";

            raven::set::cRunWatch::Start();

            std::vector<crash_t> vCrash; // identified crash sites
            {
                raven::set::cRunWatch watcher("find crash sites");

                std::map<int, linkmap_t> mapInLinks;
                for (auto &n : nodes())
                {
                    mapInLinks[n.first] = inlinks(n.first);
                }

                for (auto &n : nodes())
                {
                    raven::set::cRunWatch watcher("tribs for crash site");
                    linkmap_t lm = mapInLinks[n.first];
                    if (lm.size() > 1)
                    {
                        // node with more than one incident edge
                        // is a potential crash site
                        // std::cout << "crash site " << userName(n.first) << "\n";
                        crash_t crash;
                        crash.first = n.first;

                        for (auto t : lm)
                        {
                            // find the nodes on this tributary
                            trib_t trib;
                            trib.push_back(std::make_pair(
                                t.first.first,
                                1));
                            buildtributary(trib, mapInLinks);
                            crash.second.push_back(trib);
                        }
                        vCrash.push_back(crash);
                    }
                }
            }

            // display crash site nodes and their tributaries
            if (vCrash.size() < 10)
            {
                for (auto &crash : vCrash)
                {
                    std::cout << userName(crash.first) << ": ";
                    for (auto &trib : crash.second)
                    {
                        std::cout << "{ ";
                        for (auto &n : trib)
                        {
                            // print tributary node and distance from crash
                            std::cout << "( " << userName(n.first) << " " << n.second << " ) ";
                        }
                        std::cout << " }, ";
                    }
                    std::cout << "\n";
                }
            }

            // search for colliding nodes

            for (int a = 0; a < nodeCount(); a++)
            {
                for (int b = a + 1; b < nodeCount(); b++)
                {
                    raven::set::cRunWatch watcher{"collision search"};

                    // loop over crash sites
                    for (auto &crash : vCrash)
                    {
                        bool afound, bfound;
                        afound = bfound = false;
                        int adist, bdist;

                        // loop over tributaries
                        for (auto &trib : crash.second)
                        {
                            // loop over trib nodes
                            for (auto &n : trib)
                            {
                                // check if still looking for a
                                if (!afound)
                                {
                                    if (a == n.first)
                                    {
                                        // a found, record distance
                                        afound = true;
                                        adist = n.second;

                                        // stop searching this trib
                                        break;
                                    }
                                }
                                if (!bfound)
                                {
                                    if (b == n.first)
                                    {
                                        bfound = true;
                                        bdist = n.second;
                                        break;
                                    }
                                }
                            }
                            if (afound && bfound)
                                break;
                        }
                        if (!(afound && bfound))
                        {
                            // they cannot collide at this crash site
                            // continue to next crash site
                            continue;
                        }
                        if (adist != bdist)
                        {
                            // they arrive at different times
                            // continue to next crash site
                            continue;
                        }
                        // success!
                        // std::cout << userName(a) << " and " << userName(b)
                        //           << " collide at " << userName(crash.first) << "\n";
                        break;
                    }
                }
            }
            raven::set::cRunWatch::Report();
        }
    }
}

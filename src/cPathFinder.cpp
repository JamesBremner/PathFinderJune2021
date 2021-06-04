#include "cPathFinder.h"

std::string cPathFinder::pathViz()
{
    return "";
}
void cPathFinder::start(const std::string &start)
{
}

cPathFinder::cEdge& cPathFinder::linkProps( int u, int v )
{
    return myLink[std::make_pair(u,v)];
}

int cPathFinder::linkCost( int u, int v )
{
    return linkProps(u,v).myCost;
}

void cPathFinder::path()
{
    int V = nodeCount();

    myDist.clear();
    myDist.resize(V);
    myPred.clear();
    myPred.resize(V);

    bool sptSet[V]; // sptSet[i] will be true if vertex i is included in shortest
    // path tree or shortest distance from src to i is finalized

    // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < V; i++)
        myDist[i] = INT_MAX, sptSet[i] = false;

    // Distance of source vertex from itself is always 0
    myDist[myStart] = 0;
    myPred[myStart] = 0;

    // Find shortest path for all vertices
    for (int count = 0; count < V - 1; count++)
    {
        // Pick the minimum distance vertex from the set of vertices not
        // yet processed. u is always equal to src in the first iteration.
        int min = INT_MAX, u;

        for (int v = 0; v < V; v++)
            if (sptSet[v] == false && myDist[v] <= min)
                min = myDist[v], u = v;

        // Mark the picked vertex as processed
        sptSet[u] = true;

        // Update dist value of the adjacent vertices of the picked vertex.
        for (int v : myGraph.all_neighbors(u))
        {
            if (sptSet[v])
                continue; // already processed

            // Update dist[v] only if total weight of path from src to  v through u is
            // smaller than current value of dist[v]
            int cost =  linkCost( u, v );
            if (myDist[u] + cost < myDist[v])
            {
                myDist[v] = myDist[u] + cost;
                myPred[v] = u;
            }
        }

        pathPick(myEnd);
    }
}

void cPathFinder::clear()
{
    myGraph.clear();
    myNode.clear();
    myLink.clear();
}

int cPathFinder::find(const std::string &name)
{
    for (int n = 0; n < nodeCount(); n++)
        if (myNode[n].myName == name)
            return n;
    return -1;
}

int cPathFinder::findoradd(const std::string &name)
{
    int n = find(name);
    if (n == -1)
    {
        n = nodeCount();
        myGraph.insert_vertex(n);
        myNode.push_back(cNode(name));
    }
    return n;
}

std::vector<int> cPathFinder::pathPick(int end)
{
    myPath.clear();
    // std::cout << "->cPathFinder::pathPick "
    //     << myStart <<" " << end << "\n";

    if (end < 0)
        throw std::runtime_error("cPathFinder::pathPick bad end node");
    if (myPred[end] == end)
        throw std::runtime_error("There is no path from " + std::to_string(myStart) + " to " + std::to_string(end));

    // pick out path, starting at goal and finishing at start
    myPath.push_back(end);
    int prev = end;
    while (1)
    {
        //std::cout << prev << " " << myPred[prev] << ", ";
        int next = myPred[prev];
        myPath.push_back(next);
        if (next == myStart)
            break;
        prev = next;
    }

    // reverse so path goes from start to goal
    std::reverse(myPath.begin(), myPath.end());

    return myPath;
}

std::string cPathFinder::nodeName(int n) const
{
}

void cPathFinder::addLink(
    int u,
    int v,
    float cost)
{
    if (u < 0 || v < 0)
        throw std::runtime_error("cPathFinder::addLink bad node");
    if (!myfDirected)
        myGraph.insert_undirected_edge(u, v);
    else
        myGraph.insert_edge(u, v);

    myLink.insert(std::make_pair(
        std::make_pair(u, v), cEdge(cost)));
}

void cPathFinder::addLink(
    const std::string &su,
    const std::string &sv,
    float cost)
{
    addLink(
        findoradd(su),
        findoradd(sv),
        cost);
}

int cPathFinder::linkCount()
{
    return myGraph.num_edges();
}
int cPathFinder::nodeCount()
{
    return myGraph.num_vertices();
}

std::string cPathFinder::linksText()
{
    std::stringstream ss;
    for (auto &e : myGraph.edge_list())
    {
        ss << "("
           << myNode[e.first].myName << ","
           << myNode[e.second].myName << ","
           << myLink[e].myCost
           << ") ";
    }
    return ss.str();
}

std::string cPathFinder::pathText()
{
    std::stringstream ss;
    for (auto n : myPath)
    {
        std::string sn;
        sn = myNode[n].myName;

        if (sn == "???")
            sn = std::to_string(n);
        ss << sn << " -> ";
        //ss << std::to_string(n) << " -> ";
    }

    if (myPath.size())
    {
        //std::cout << "dbg " << myDist[myPath.back()] << " " << myMaxNegCost << " " << myPath.size() << "\n";
        ss << " Cost is "
           << myDist[myPath.back()] + myMaxNegCost * (myPath.size() - 1)
           << "\n";
    }

    return ss.str();
}

std::string cPathFinder::spanText() {}
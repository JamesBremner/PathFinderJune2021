#include "cPathFinder.h"

std::string cPathFinder::pathViz()
{
    return "";
}
void cPathFinder::start(const std::string &start)
{
}

void cPathFinder::path()
{
}

void cPathFinder::clear()
{
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
        myNode.push_back( cNode( name ));
    }
    return n;
}

std::vector<int> cPathFinder::pathPick(int end)
{
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

    myLink.insert( std::make_pair(
        std::make_pair( u, v), cEdge( cost ) ));
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
    auto el = myGraph.edge_list();
    for( auto& e : el )
    {
                ss << "("
           << myNode[e.first].myName << ","
           << myNode[e.second].myName << ","
           << myLink[make_pair(e.first,e.second)].myCost
           << ") ";
    }
    return ss.str();
}

std::string cPathFinder::pathText() {}

std::string cPathFinder::spanText() {}
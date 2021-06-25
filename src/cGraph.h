#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

namespace raven {
namespace graph
{
    /// link properties
    class cLink
    {
    public:
        cLink(int c = 1)
            : myCost(c)
        {
        }
        int myCost;         // a constraint e.g. distance of a road, max xapacity of a pipe
        int myValue;        // a calculated value, e.g. actual flow through a pipe
    };

    /// node properties
    class cNode
    {
    public:
        cNode()
        :cNode(-1)
        {}
        cNode(
            int index,
            const std::string &name = "???")
            : myName(name)
            , myIndex( index )
        {
            if( myName == "???" )
                std::cout << "construct ???\n";
        }
        void removeLink(int dst)
        {
            auto it = myLink.find(dst);
            if (it != myLink.end())
                myLink.erase(it);
        }
        int linkCost(int dst)
        {
            auto it = myLink.find(dst);
            if (it == myLink.end())
                return INT_MAX;
            return it->second.myCost;
        }
        int outdegree()
        {
            return myLink.size();
        }
        std::string myName;
        int myIndex;
        int myCost;
        int myValue;
        std::string myColor;
        std::map<int, cLink> myLink;
    };

    typedef std::pair<std::pair<int, int>, cLink> link_t;

    class cGraph
    {
    public:

        typedef std::map<std::pair<int, int>, cLink> linkmap_t;

        void clear()
        {
            myG.clear();
            myfDirected = false;
        }
        void makeNodes( int count )
        {
            myG.clear();
            for( int k = 0; k < count; k++ )
                myG.insert( std::make_pair( k, cNode(k,std::to_string(k))));
        }
    /** set graph links type
     * 
     * @param[in] f true for directed, false for undirected, default directed
     * 
     * If not called, the graph will be undirected
     * 
     */
        void directed(bool f = true)
        {
            myfDirected = f;
        }
        bool isDirected() const
        {
            return myfDirected;
        }
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
            const std::string &srcname,
            const std::string &dstname,
            double cost = 1)
        {
            addLink(
                findoradd(srcname),
                findoradd(dstname),
                cost);
        }
        void addLink(int u, int v, double cost = 1)
        {
            if (0 > u || u > myG.size() || 0 > v || v > myG.size())
                throw std::runtime_error(
                    "addLink bad node index");
            myG.find(u)->second.myLink.insert(std::make_pair(v, cLink(cost)));
            if (!myfDirected)
                myG.find(v)->second.myLink.insert(std::make_pair(u, cLink(cost)));
        }
        void addLinkFast( int u, int v )
        {
            myG.find(u)->second.myLink.insert(std::make_pair(v, cLink()));
        }
/** Find node by name
 * 
 * @param[in] name
 * @return node index, -1 if named node does not exist
 */
        int find(const std::string &name)
        {
            for (auto n : myG)
            {
                if (n.second.myName == name)
                {
                    return n.first;
                }
            }
            return -1;
        }
/** Find or add node by name
 * 
 * @param[in] name
 * @return node index
 * 
 * If a node of specified name does not exist, it is added.
 */
        int findoradd(const std::string &name)
        {
            // search among the existing nodes
            int n = find(name);
            if (n < 0)
            {
                // node does not exist, create a new one
                // with a new index and add it to the graph
                n = myG.size();
                myG.insert(std::make_pair(n, cNode(n,name)));
            }
            return n;
        }

        cNode &findNode(int n)
        {
            auto it = myG.find(n);
            if (it == myG.end())
                throw std::runtime_error(
                    "cGraph::findNode bad index");
            return it->second;
        }

        cLink &findLink(int u, int v)
        {
            return myG.at(u).myLink.at(v);
        }

        void removeLink(int u, int v)
        {
            std::cout << "remove link " << name(u) <<" "<< name(v)  << "\n";
            try
            {
                myG.at(u).removeLink(v);
                if (!myfDirected)
                    myG.at(v).removeLink(u);
            }
            catch (...)
            {
                // just ignore requests to remove links that do not exist
            }
        }
        void removeNode(int n)
        {
            auto it = myG.find(n);
            if (it == myG.end())
                return;
            myG.erase(it);
        }

        /// Human readable list of links
        std::string linksText()
        {
            std::stringstream ss;
            for (auto &n : myG)
            {
                for (auto &l : n.second.myLink)
                {
                    if (!myfDirected)
                        if (n.first > l.first)
                            continue;
                    ss << n.second.myName << " -> "
                       << myG[l.first].myName << " cost " << l.second.myCost 
                       << " value " << l.second.myValue << "\n";
                }
            }
            return ss.str();
        }

        std::string nodesText() const
        {
            std::stringstream ss;
            for (auto &n : myG)
                 ss << n.second.myName 
                    << "(" << n.first << ") ";
            return ss.str();
        }

        std::map<int, cNode> &nodes()
        {
            return myG;
        }
        linkmap_t links() const
        {
            linkmap_t ret;
            for (auto &n : myG)
            {
                for (auto &l : n.second.myLink)
                {
                    ret.insert(std::make_pair(std::make_pair(n.first, l.first), l.second));
                }
            }
            return ret;
        }
        linkmap_t inlinks( int n )
        {
            linkmap_t ret;
            for( auto& l : links() ) {
                if( l.first.second == n ) {
                    ret.insert( l );
                }
            }
            return ret;
        }
        /** Reference to node from internal index
         * @param[in] i internal index
         * 
         * If node does not exist, exception thrown
         */
        cNode &node(int i)
        {
            return myG.at(i);
        }
        cLink &link(int u, int v)
        {
            auto itu = myG.find(u);
            if (itu == myG.end())
                throw std::runtime_error(
                    "link bad index");
            auto itv = itu->second.myLink.find(v);
            if (itv == itu->second.myLink.end())
                throw std::runtime_error(
                    "link bad index");
            return itv->second;
        }
        bool includes_link(int u, int v)
        {
            return includes_link(std::make_pair(u, v));
        }
        bool includes_link(const std::pair<int, int> &e)
        {
            auto itu = myG.find(e.first);
            if (itu == myG.end())
                return false;
            auto itv = itu->second.myLink.find(e.second);
            if (itv == itu->second.myLink.end())
                return false;
            return true;
        }
        /** link cost
 * @param[in] u node index
 * @param[in] v node index
 * @return cost of link between u and v
 * If u and v are not adjacent, returns INT_MAX
 */
int cost(int u, int v)
{
    try
    {
        return link(u, v).myCost;
    }
    catch (...)
    {
        return INT_MAX;
    }
}
        /** Adjacent nodes
         * @param[in] i index of node to find which nodes it connects to
         * @return vector of connected node indices
         */
        std::vector<int> adjacent(int i)
        {
            std::vector<int> ret;
            auto it = myG.find(i);
            if (it == myG.end())
                return ret;
            for (auto &l : it->second.myLink)
                ret.push_back(l.first);
            return ret;
        }
        int nodeCount() const
        {
            return myG.size();
        }
        int linkCount() const
        {
            return links().size();
        }

        const std::string &name(int i) const
        {
            //auto it = myG.find(i);

            // std::cout << "cGraph::name " << nodesText() << "\n";
            // std::cout << "looking for " << i << "\n";
            auto it = myG.begin();
            for( ; it != myG.end(); it++ )
                if( it->first == i )
                    break;

            if( it == myG.end() )
                throw std::runtime_error(
                    "cGraph::name bad index");

            return it->second.myName;
        }
        /// copy nodes, but not the links
        void copyNodes(const cGraph &other)
        {
            for (auto &n : other.myG)
            {
                myG.insert(std::make_pair(n.first, cNode(n.first,n.second.myName)));
            }
        }

        const std::map<int, cNode>& graph() const
        {
            return myG;
        }

    protected:
        std::map<int, cNode> myG; // the graph, keyed by internal node index

        /** myfDirected is true if the graph links are directed
         * 
         * Internally all links are always directed.  
         * When myfDirected is false, whenever a link is added
         * two directed links in opposite direction are added between the end nodes
         */
        bool myfDirected;


    };

int source( const link_t& link );

int target( const link_t& link );

}
}
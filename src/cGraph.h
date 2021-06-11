#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

namespace graph
{
    /// edge properties
    class cEdge
    {
    public:
        cEdge(int c = 1)
            : myCost(c)
        {
        }
        int myCost;
        int myValue;
    };

    /// node properties
    class cNode
    {
    public:
        cNode(const std::string &name = "???")
            : myName(name)
        {
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
        std::string myName;
        std::string myColor;
        std::map<int, cEdge> myLink;
    };
    class cGraph
    {
    public:
        typedef std::pair<std::pair<int, int>, cEdge> link_t;
        typedef std::map<std::pair<int, int>, cEdge> linkmap_t;

        void clear()
        {
            myG.clear();
            myfDirected = false;
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
            if (0 > u || u > myG.size() || 0 > v || v >> myG.size())
                throw std::runtime_error(
                    "addLink bad node index");
            myG.find(u)->second.myLink.insert(std::make_pair(v, cEdge(cost)));
            if (!myfDirected)
                myG.find(v)->second.myLink.insert(std::make_pair(u, cEdge(cost)));
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
            int n = find(name);
            if (n < 0)
            {
                n = myG.size();
                myG.insert(std::make_pair(n, cNode(name)));
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

        cEdge &findLink(int u, int v)
        {
            return myG.at(u).myLink.at(v);
        }

        void removeLink(int u, int v)
        {
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
                       << myG[l.first].myName << " cost "
                       << l.second.myCost << "\n";
                }
            }
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
        cNode &node(int i)
        {
            return myG[i];
        }
        cEdge &link(int u, int v)
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
        std::string &name(int i)
        {
            if (0 > i || i >= myG.size())
                throw std::runtime_error(
                    "cGraph::name bad index");
            return myG[i].myName;
        }
        /// copy nodes, but not the links
        void copyNodes(const cGraph &other)
        {
            for (auto &n : other.myG)
            {
                myG.insert(std::make_pair(n.first, cNode(n.second.myName)));
            }
        }

        const std::map<int, cNode>& graph() const
        {
            return myG;
        }

    protected:
        std::map<int, cNode> myG; // the graph, keyed by internal node index

        bool myfDirected;


    };
}
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
    };

    /// node properties
    class cNode
    {
    public:
        cNode(const std::string &name)
            : myName(name)
        {
        }
        cNode()
            : myName("???")
        {
        }
        void removeLink(int n)
        {
            auto it = myLink.find(n);
            if (it != myLink.end())
                myLink.erase(it);
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
        void directed(bool f = true)
        {
            myfDirected = f;
        }
        bool isDirected() const
        {
            return myfDirected;
        }
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

        std::string linksText()
        {
            std::stringstream ss;
            for (auto &n : myG)
            {
                for (auto &l : n.second.myLink)
                {
                    if( ! myfDirected )
                        if( n.first > l.first )
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
            for( auto& l : it->second.myLink )
                ret.push_back( l.first );
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
            for( auto& n : other.myG )
            {
                myG.insert(std::make_pair(n.first,cNode( n.second.myName )));
            }
        }

    private:
        
        std::map<int, cNode> myG; // the graph, keyed by internal node index

        bool myfDirected;
    };
}
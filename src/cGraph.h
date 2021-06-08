#pragma once
#include <string>
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
        std::string myName;
        std::string myColor;
    };
    class cGraph
    {
    public:
        typedef std::pair<std::pair<int, int>, cEdge> link_t;

        void clear()
        {
            myG.clear();
            myLink.clear();
            myAdj.clear();
        }
        void addLink(
            const std::string &srcname,
            const std::string &dstname,
            double cost = 1)
        {
            int nsrc = findoradd(srcname);
            int ndst = findoradd(dstname);
            myLink.insert(std::make_pair(std::make_pair(nsrc, ndst), cEdge(cost)));
        }
        void addLink(int u, int v, double cost = 1)
        {
            if (0 > u || u > myG.size() || 0 > v || v >> myG.size())
                throw std::runtime_error(
                    "addLink bad node index");
            myLink.insert(std::make_pair(std::make_pair(u, v), cEdge(cost)));
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

        void removeLink( int u, int v )
        {
            auto it = myLink.find( std::make_pair(u,v));
            if( it == myLink.end() )
                return;
            myLink.erase( it );
        }

        std::string linksText()
        {
            std::stringstream ss;
            for (auto l : myLink)
            {
                ss << linkText(l) << "\n";
            }
            return ss.str();
        }
        std::string linkText(const link_t &l)
        {
            std::stringstream ss;
            ss << myG[l.first.first].myName << " -> "
               << myG[l.first.second].myName
               << " cost " << l.second.myCost;
            return ss.str();
        }

        std::map<int, cNode> &nodes()
        {
            return myG;
        }
        std::map<std::pair<int, int>, cEdge> &links()
        {
            return myLink;
        }
        cNode &node(int i)
        {
            return myG[i];
        }
        cEdge &link(int u, int v)
        {
            return myLink[std::make_pair(u, v)];
        }
        bool includes_link(int u, int v)
        {
            return includes_link(std::make_pair(u, v));
        }
        bool includes_link(const std::pair<int, int> &e)
        {
            return (myLink.find(e) != myLink.end());
        }
        std::vector<int> adjacent(int i)
        {
            return myAdj[i];
        }
        int nodeCount() const
        {
            return myG.size();
        }
        int linkCount() const
        {
            return myLink.size();
        }

    private:
        std::map<int, cNode> myG;                    // the nodes
        std::map<std::pair<int, int>, cEdge> myLink; // the links
        std::vector<std::vector<int>> myAdj;         // the adjacency matrix
    };
}
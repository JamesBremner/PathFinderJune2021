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
            //myAdj.clear();
            myfDirected = false;
        }
        void directed(bool f = true)
        {
            myfDirected = f;
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
            myLink.insert(std::make_pair(std::make_pair(u, v), cEdge(cost)));
            if (!myfDirected)
                myLink.insert(std::make_pair(std::make_pair(v, u), cEdge(cost)));
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
            auto it = myLink.find(std::make_pair(u, v));
            if (it == myLink.end())
                return;
            myLink.erase(it);
            if (!myfDirected)
                removeLink(v, u);
        }

        std::string linksText()
        {
            std::stringstream ss;
            for (auto l : myLink)
            {
                ss << linkText(l);
            }
            return ss.str();
        }
        std::string linkText(const link_t &l)
        {
            if (!myfDirected)
                if (l.first.first > l.first.second)
                    return "";
            std::stringstream ss;
            ss << myG[l.first.first].myName << " -> "
               << myG[l.first.second].myName
               << " cost " << l.second.myCost
               << "\n";
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
        /** Adjacent nodes
         * @param[in] i index of node to find which nodes it connects to
         * @return vector of connected node indices
         */
        std::vector<int> adjacent(int i)
        {
            //return myAdj[i];
            std::vector<int> ret;
            for (auto &l : myLink)
                if (l.first.first == i)
                    ret.push_back(l.first.second);
            return ret;
        }
        int nodeCount() const
        {
            return myG.size();
        }
        int linkCount() const
        {
            return myLink.size();
        }
        std::string &name(int i)
        {
            if (0 > i || i >= myG.size())
                throw std::runtime_error(
                    "cGraph::name bad index");
            return myG[i].myName;
        }
        void copyNodes(const cGraph &other)
        {
            myG = other.myG;
        }

    private:
        std::map<int, cNode> myG;                    // the nodes
        std::map<std::pair<int, int>, cEdge> myLink; // the links
        //std::vector<std::vector<int>> myAdj;         // the adjacency matrix
        bool myfDirected;
    };
}
#include <math.h>
#include <sstream>
#include <algorithm>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "cPathFinderReader.h"
#include "cRunWatch.h"
#include "cMaze.h"

namespace raven
{
    namespace graph
    {

        eCalculation
        cPathFinderReader::open(
            const std::string &fname)
        {
            myFormat = eCalculation::none;
            myFile.close();
            myFile.open(fname);
            if (!myFile.is_open())
                return eCalculation::not_open;
            std::string line;
            getline(myFile, line);
            if (line.find("format") != 0)
                return eCalculation::none;
            if (line.find("costs") != -1)
            {
                costs();
                myFinder.path();
                std::cout << myFinder.pathText() << "\n";
                return eCalculation::costs;
            }
            else if (line.find("water") != -1)
            {
                bool pulse;
                auto t = valves(pulse);
                myFinder.waterValves(t, pulse);
                std::cout << myFinder.pathText() << "\n";
                return eCalculation::costs;
            }
            else if (line.find("spans") != -1)
            {
                costs();
                myFinder.span();
                std::cout << myFinder.spanText() << "\n";
                return eCalculation::spans;
            }
            else if (line.find("sales") != -1)
            {
                raven::set::cRunWatch::Start();
                myFinder.tsp(sales());
                raven::set::cRunWatch::Report();
                return eCalculation::sales;
            }
            else if (line.find("cams") != -1)
            {
                costs(false);
                myFinder.cams();
                return eCalculation::cams;
            }
            else if (line.find("cliques") != -1)
            {
                costs(false);
                return eCalculation::cliques;
            }
            else if (line.find("multiflows") != -1)
            {
                myFormat = eCalculation::multiflows;
                costs();
                myFinder.multiflows();
            }
            else if (line.find("equiflows") != -1)
            {
                myFormat = eCalculation::flows;
                costs();
                myFinder.equiflows();
            }
            else if (line.find("flows") != -1)
            {
                costs();
                myFinder.flows();
                return eCalculation::flows;
            }
            else if (line.find("hills") != -1)
            {
                myFinder.hills(orthogonalGrid());
                return eCalculation::hills;
            }
            else if (line.find("prerequisites") != -1)
            {
                myFormat = eCalculation::reqs;
                myFinder.PreReqs(singleParentTree());
            }
            else if (line.find("karupTimer") != -1)
            {
                KarupRandom();
                raven::set::cRunWatch::Start();
                myFinder.karup();
                raven::set::cRunWatch::Report();
            }
            else if (line.find("karup") != -1)
            {
                nodeCosts();
                myFinder.karup();
            }
            else if (line.find("amazon") != -1)
            {
                std::cout << "->amazon\n";
                links();
                std::cout << "<-amazon\n";
                raven::set::cRunWatch::Start();
                {
                    raven::set::cRunWatch aWatcher("DFS");
                    myFinder.depthFirst(0, [](int v) {});
                }
                raven::set::cRunWatch::Report();
            }
            else if (line.find("bonesi") != -1)
            {
                myFormat = eCalculation::bonesi;
                bonesi();
            }
            else if (line.find("collision") != -1)
            {
                collision();
                myFinder.collision();
                if (myFinder.nodeCount() < 100)
                    myFormat = eCalculation::costs;
                else
                {
                    // suppress visual display of large graphs
                    myFormat = eCalculation::collision;
                }
            }
            else if (line.find("maze") != -1)
            {
                cMaze maze;
                maze.read(myFile);
                maze.graph(myFinder);
                myFinder.path();
                myFormat = eCalculation::costs;
            }
            else if (line.find("allpaths") != -1)
            {
                costs();
                return eCalculation::allpaths;
            }
            else if (line.find("paths") != -1)
            {
                costs();
                myFormat = eCalculation::paths;
            }
            else if (line.find("generate_srcnuzn") != -1)
            {
                myFinder.srcnuzn_generate();
                return eCalculation::srcnuzn;
            }
            else if (line.find("srcnuzn") != -1)
            {
                nodecosts(true);
                return eCalculation::srcnuzn;
            }
            else if (line.find("pickup") != -1)
            {
                pickup();
                return eCalculation::pickup;
            }

            return myFormat;
        }

        std::vector<std::string> cPathFinderReader::ParseSpaceDelimited(
            const std::string &l)
        {
            std::vector<std::string> token;
            std::stringstream sst(l);
            std::string a;
            while (getline(sst, a, ' '))
                token.push_back(a);

            token.erase(
                remove_if(
                    token.begin(),
                    token.end(),
                    [](std::string t)
                    {
                        return (t.empty());
                    }),
                token.end());

            return token;
        }

        void cPathFinderReader::costs(
            bool weights,
            bool directed)
        {
            myFinder.clear();
            if (directed)
                myFinder.directed();
            int cost;
            int maxNegCost = 0;
            std::string line;
            while (std::getline(myFile, line))
            {
                std::cout << line << "\n";
                auto token = ParseSpaceDelimited(line);
                if (!token.size())
                    continue;
                switch (token[0][0])
                {
                case 'g':
                    if (myFinder.linkCount())
                        throw std::runtime_error("cPathFinderReader:: g ( directed ) must occurr before any links");
                    myFinder.directed();
                    break;

                case 'l':
                    if (weights && (token.size() != 4))
                        throw std::runtime_error("cPathFinder::read bad link line");
                    else if (3 > token.size() || token.size() > 4)
                        throw std::runtime_error("cPathFinder::read bad link line");
                    if (weights)
                        cost = atof(token[3].c_str());
                    else
                        cost = 1;
                    if (cost < maxNegCost)
                        maxNegCost = cost;
                    myFinder.addLink(
                        token[1],
                        token[2],
                        cost);
                    break;

                case 's':
                    if (token.size() != 2)
                        throw std::runtime_error("cPathFinder::read bad start line");
                    if (myFormat == eCalculation::multiflows)
                        myFinder.addSource(token[1]);
                    else
                        myFinder.start(token[1]);
                    break;

                case 'e':
                    if (token.size() != 2)
                        throw std::runtime_error("cPathFinder::read bad end line");
                    myFinder.end(token[1]);
                    break;
                }
            }
            if (maxNegCost < 0)
            {
                std::cout << "Negative link costs present\n"
                          << "Adding positive offset to all link costs\n";
                //        myFinder.makeCostsPositive(maxNegCost);
            }

            // std::cout << "<-costs\n" <<myFinder.linksText() << "\n";
        }

        void cPathFinderReader::nodecosts(
            bool directed)
        {
            myFinder.clear();
            if (directed)
                myFinder.directed();
            int cost;
            int maxNegCost = 0;
            std::string line;
            while (std::getline(myFile, line))
            {
                std::cout << line << "\n";
                auto token = ParseSpaceDelimited(line);
                if (!token.size())
                    continue;
                switch (token[0][0])
                {
                case 'g':
                    if (myFinder.linkCount())
                        throw std::runtime_error("cPathFinderReader:: g ( directed ) must occurr before any links");
                    myFinder.directed();
                    break;

                case 'n':
                    if (token.size() != 3)
                        throw std::runtime_error("cPathFinder::read bad node line");
                    myFinder.findNode(myFinder.findoradd(token[1])).myCost = atoi(token[2].c_str());
                    break;

                case 'l':
                    if (3 > token.size() || token.size() > 4)
                        throw std::runtime_error("cPathFinder::read bad link line");
                    cost = 1;
                    if (cost < maxNegCost)
                        maxNegCost = cost;
                    myFinder.addLink(
                        token[1],
                        token[2],
                        cost);
                    break;

                case 's':
                    if (token.size() != 2)
                        throw std::runtime_error("cPathFinder::read bad start line");
                    if (myFormat == eCalculation::multiflows)
                        myFinder.addSource(token[1]);
                    else
                        myFinder.start(token[1]);
                    break;

                case 'e':
                    if (token.size() != 2)
                        throw std::runtime_error("cPathFinder::read bad end line");
                    myFinder.end(token[1]);
                    break;
                }
            }
            if (maxNegCost < 0)
            {
                std::cout << "Negative link costs present\n"
                          << "Adding positive offset to all link costs\n";
                //        myFinder.makeCostsPositive(maxNegCost);
            }

            // std::cout << "<-costs\n" <<myFinder.linksText() << "\n";
        }

        void cPathFinderReader::pickup()
        {
            myFinder.clear();
            int cost;
            int maxNegCost = 0;
            std::string line;
            bool fd = false;
            bool fe = false;
            while (std::getline(myFile, line))
            {
                std::cout << line << "\n";
                auto token = ParseSpaceDelimited(line);
                if (!token.size())
                    continue;
                std::string nodeType;
                switch (token[0][0])
                {
                    case 'd': nodeType = "driver"; fd = true; break;
                    case 'c': nodeType = "cargo"; break;
                    case 'e': nodeType = "destination"; fe = true; break;
                }
                myFinder.findNode(myFinder.findoradd(token[3])).myColor = 
                    nodeType +" at " + token[1] + " " + token[2];
            }
            if( ! fd )
                throw std::runtime_error(
                    "Invalid pickup input - no driver");
            if( ! fe )
                throw std::runtime_error(
                    "Invalid pickup input - no e destination");
        }

        std::vector<int> cPathFinderReader::valves(
            bool &pulse)
        {
            std::vector<int> valveTimes;
            myFinder.clear();

            int cost;
            int maxNegCost = 0;
            std::string line;
            while (std::getline(myFile, line))
            {
                std::cout << line << "\n";
                auto token = ParseSpaceDelimited(line);
                if (!token.size())
                    continue;
                switch (token[0][0])
                {
                case 'g':
                    if (myFinder.linkCount())
                        throw std::runtime_error("cPathFinderReader:: g ( directed ) must occurr before any links");
                    myFinder.directed();
                    break;

                case 'l':
                    // link
                    if (valveTimes.size())
                        throw std::runtime_error("cPathFinder::read valve times must be after pipe specs");
                    if (token.size() != 4)
                        throw std::runtime_error("cPathFinder::read bad link line");

                    cost = atof(token[3].c_str());

                    if (cost < maxNegCost)
                        maxNegCost = cost;
                    myFinder.addLink(
                        token[1],
                        token[2],
                        cost);
                    break;

                case 's':
                    if (token.size() != 2)
                        throw std::runtime_error("cPathFinder::read bad start line");
                    if (myFormat == eCalculation::multiflows)
                        myFinder.addSource(token[1]);
                    else
                        myFinder.start(token[1]);
                    break;

                case 'e':
                    if (token.size() != 2)
                        throw std::runtime_error("cPathFinder::read bad end line");
                    myFinder.end(token[1]);
                    break;

                case 't':
                    // valve timings
                    if (token.size() < 2)
                        throw std::runtime_error("cPathFinder::read bad time line");
                    token.erase(token.begin());
                    if (token[0][0] == 'p')
                        pulse = true;
                    else if (token[0][0] == 'a')
                        pulse = false;
                    else
                        throw std::runtime_error("cPathFinder::read valve times must specify 'pulse' or 'always'");
                    token.erase(token.begin());
                    valveTimes.resize(myFinder.nodeCount());
                    int nodeIndex = 1;
                    for (auto &s : token)
                        valveTimes[myFinder.find(std::to_string(nodeIndex++))] = atoi(s.c_str());
                    break;
                }
            }
            if (maxNegCost < 0)
            {
                std::cout << "Negative link costs present\n"
                          << "Adding positive offset to all link costs\n";
                //        myFinder.makeCostsPositive(maxNegCost);
            }

            // std::cout << "<-costs\n" <<myFinder.linksText() << "\n";

            return valveTimes;
        }

        void cPathFinderReader::links()
        {
            myFinder.clear();
            myFinder.directed();
            myFinder.makeNodes(403394);
            std::string line;
            while (std::getline(myFile, line))
            {
                // std::cout << line << " | ";
                auto token = ParseSpaceDelimited(line);
                myFinder.addLinkFast(
                    atoi(token[0].c_str()),
                    atoi(token[1].c_str()));
            }
            // std::cout << "<-costs\n" <<myFinder.linksText() << "\n";
        }
        void cPathFinderReader::bonesi()
        {
            myFinder.clear();
            myFinder.directed();

            // read  JSON into property tree
            boost::property_tree::ptree tree;
            read_json(myFile, tree);

            // loop over source nodes
            for (auto &src : tree)
            {
                // loop over destination nodes
                for (auto &dst : tree.get_child(src.first).get_child("next"))
                {
                    // add link between source and destination
                    myFinder.addLink(src.first, dst.second.data());
                }
            }
        }
        void cPathFinderReader::nodeCosts()
        {
            myFinder.clear();
            int cost;
            std::string line;
            while (std::getline(myFile, line))
            {
                std::cout << line << "\n";
                auto token = ParseSpaceDelimited(line);
                if (!token.size())
                    continue;
                switch (token[0][0])
                {
                case 'n':
                    if (token.size() != 3)
                        throw std::runtime_error(
                            "bad node line");
                    myFinder.node(myFinder.findoradd(token[1])).myCost = atoi(token[2].c_str());
                    break;
                case 'l':
                    if (token.size() != 3)
                        throw std::runtime_error(
                            "bad link line");
                    myFinder.addLink(
                        token[1],
                        token[2],
                        1);
                    break;
                }
            }
        }

        void cPathFinderReader::KarupRandom()
        {
            const int nodeCount = 5e5;

            myFinder.makeNodes(2 * nodeCount);

            // generate a and b nodes
            for (int x = 0; x < nodeCount; x++)
            {
                myFinder.node(x).myName = "a" + std::to_string(x);
                myFinder.node(nodeCount + x).myName = "b" + std::to_string(x);
            }
            // link each b node to two random a nodes
            for (int x = 0; x < nodeCount; x++)
            {
                int a = rand() % (int)nodeCount;
                myFinder.addLink(nodeCount + x, a);
                a = rand() % (int)nodeCount;
                myFinder.addLink(nodeCount + x, a);
            }

            // std::cout << myFinder.linksText();
        }

        void cPathFinderReader::collision()
        {
            std::string line;
            getline(myFile, line);
            auto token = ParseSpaceDelimited(line);
            if (!token.size())
                throw std::runtime_error(
                    "cPathFinderReader::collision input format error");
            if (token[0] == "random")
            {
                const int nodeCount = atoi(token[1].c_str());
                myFinder.directed();
                myFinder.makeNodes(nodeCount);

                // link each node to one other chosen at random
                for (int x = 0; x < nodeCount; x++)
                    myFinder.addLink(x, rand() % nodeCount);
            }
            else
                costs(false, true);
        }

        std::vector<int> cPathFinderReader::sales()
        {
            struct sCity
            {
                int x;
                int y;
                std::string name;
            };
            std::vector<sCity> vCity;
            sCity city;

            std::vector<int> visit;

            enum class eInput
            {
                unknown,
                city,
                link,
                manhatten,
            } input = eInput::unknown;

            myFinder.clear();

            std::string line;
            while (std::getline(myFile, line))
            {
                std::cout << line << "\n";
                auto token = ParseSpaceDelimited(line);
                if (!token.size())
                    continue;
                if (token.size() != 4)
                    throw std::runtime_error("Input line must have 4 tokens");

                switch (token[0][0])
                {
                case 'm':
                    input = eInput::manhatten;
                    break;
                case 'c':
                    if (input == eInput::unknown)
                        input = eInput::city;
                    else if (input != eInput::city && input != eInput::manhatten)
                        throw std::runtime_error("Mixed input formats");
                    city.x = atoi(token[1].c_str());
                    city.y = atoi(token[2].c_str());
                    city.name = token[3];
                    vCity.push_back(city);
                    break;
                case 'l':
                    if (input == eInput::unknown)
                        input = eInput::link;
                    else if (input == eInput::city)
                        throw std::runtime_error("Mixed input formats");
                    myFinder.addLink(
                        myFinder.findoradd(token[1]),
                        myFinder.findoradd(token[2]),
                        atoi(token[3].c_str()));
                    break;
                case 'v':
                    token.erase(token.begin());
                    if (token.size() > myFinder.nodeCount())
                        throw std::runtime_error("More visit requests than nodes");
                    for (auto &v : token)
                    {
                        int iv = myFinder.find(v);
                        if (iv < 0)
                            throw std::runtime_error("Visit request to non existent node");
                        v.push_back(iv);
                    }
                    break;
                default:
                    std::cout << "ignored\n";
                    break;
                }
            }

            switch (input)
            {
            case eInput::city:
            {
                // link all the cities by the pythagorian distance between them
                for (int c1 = 0; c1 < vCity.size(); c1++)
                {
                    for (int c2 = c1 + 1; c2 < vCity.size(); c2++)
                    {
                        float dx = vCity[c1].x - vCity[c2].x;
                        float dy = vCity[c1].y - vCity[c2].y;
                        float d = sqrt(dx * dx + dy * dy);
                        // std::cout << vCity[c1].name << " " << vCity[c2].name << " "
                        //           << dx << " " << dy << " " << d << "\n";
                        myFinder.addLink(
                            vCity[c1].name,
                            vCity[c2].name,
                            d);
                    }
                }
            }
            break;

            case eInput::manhatten:
            {
                raven::set::cRunWatch aWatcher("CalculateManhattenDistances");
                for (int c1 = 0; c1 < vCity.size(); c1++)
                {
                    for (int c2 = 0; c2 < vCity.size(); c2++)
                    {
                        if (vCity[c1].name == vCity[c2].name)
                            continue;
                        // manhatten distance
                        float d =
                            fabs(vCity[c1].x - vCity[c2].x) + fabs(vCity[c1].y - vCity[c2].y);
                        myFinder.addLink(
                            vCity[c2].name,
                            vCity[c1].name,
                            d);
                    }
                }
            }
            break;

            default:
            {
                // add links with infinite cost between unlinked cities
                // myFinder.makeComplete();
            }
            break;
            }

            return visit;
        }
        std::vector<std::vector<float>> cPathFinderReader::orthogonalGrid()
        {
            if (!myFile.is_open())
                throw std::runtime_error(
                    "cPathFinderReader::orthogonalGrid file not open");

            const bool fDirected = true;

            myFinder.clear();
            myFinder.directed(fDirected);

            std::vector<std::vector<float>> grid;
            int RowCount = 0;
            int ColCount = -1;
            int start = -1;
            int end = -1;
            std::string line;
            while (std::getline(myFile, line))
            {
                std::cout << line << "\n";
                auto token = ParseSpaceDelimited(line);
                if (!token.size())
                    continue;
                switch (token[0][0])
                {
                case 'o':
                {
                    if (ColCount == -1)
                        ColCount = token.size() - 1;
                    else if (token.size() - 1 != ColCount)
                        throw std::runtime_error("Bad column count");
                    std::vector<float> row;
                    for (int k = 1; k < token.size(); k++)
                        row.push_back(atof(token[k].c_str()));
                    grid.push_back(row);
                }
                break;
                case 's':
                    if (token.size() != 3)
                        throw std::runtime_error("Bad start");
                    if (ColCount == -1)
                        throw std::runtime_error("Start node must be at end");
                    start =
                        (atoi(token[2].c_str()) - 1) * ColCount + atoi(token[1].c_str()) - 1;
                    break;
                case 'e':
                    if (token.size() != 3)
                        throw std::runtime_error("Bad end");
                    if (ColCount == -1)
                        throw std::runtime_error("End node must be at end");
                    end =
                        (atoi(token[2].c_str()) - 1) * ColCount + atoi(token[1].c_str()) - 1;
                    break;
                }
            }
            RowCount = grid.size();
            // std::cout << "<-cHillPathFinder::read " << myRowCount << "\n";

            // add nodes at each grid cell
            for (int row = 0; row < RowCount; row++)
            {
                for (int col = 0; col < ColCount; col++)
                {
                    int n = myFinder.findoradd(
                        myFinder.orthogonalGridNodeName(row, col));
                }
            }
            myFinder.start(start);
            myFinder.end(end);

            // link cells orthogonally
            for (int row = 0; row < RowCount; row++)
                for (int col = 0; col < ColCount; col++)
                {
                    int n = row * ColCount + col;

                    if (fDirected)
                    {
                        if (col > 0)
                        {
                            int left = row * ColCount + col - 1;
                            myFinder.addLink(n, left, 1);
                        }
                    }
                    if (col < ColCount - 1)
                    {
                        int right = row * ColCount + col + 1;
                        myFinder.addLink(n, right, 1);
                    }
                    if (fDirected)
                    {
                        if (row > 0)
                        {
                            int up = (row - 1) * ColCount + col;
                            myFinder.addLink(n, up, 1);
                        }
                    }
                    if (row < RowCount - 1)
                    {
                        int down = (row + 1) * ColCount + col;
                        myFinder.addLink(n, down, 1);
                    }
                }
            return grid;
        }
        std::vector<std::string> cPathFinderReader::singleParentTree()
        {
            myFinder.clear();
            if (!myFile.is_open())
                throw std::runtime_error(
                    "cPathFinderReader::singleParentTree file not open");
            std::string line;
            while (std::getline(myFile, line))
            {
                auto token = ParseSpaceDelimited(line);
                if (!token.size())
                    continue;
                switch (token[0][0])
                {

                case 't':
                {
                    token.erase(token.begin());
                    int child = 0;
                    for (auto &t : token)
                    {
                        if (!child)
                        {
                            child++;
                            continue;
                        }
                        myFinder.addLink(
                            myFinder.findoradd(t),
                            myFinder.findoradd(std::to_string(child++)),
                            1);
                    }
                }
                break;

                case 'a':
                    token.erase(token.begin());
                    return token;
                }
            }
            throw std::runtime_error("no A input");
        }
    }
}
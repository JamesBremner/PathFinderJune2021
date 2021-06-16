#include <math.h>
#include <sstream>
#include <algorithm>
#include "cPathFinderReader.h"

namespace raven { namespace graph {

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
    else if (line.find("spans") != -1)
    {
        costs();
        myFinder.span();
        std::cout << myFinder.spanText() << "\n";
        return eCalculation::spans;
    }
    else if (line.find("sales") != -1)
    {
        sales();
        myFinder.tsp();
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
    else if (line.find("multiflows") != -1) {
        myFormat = eCalculation::multiflows;
        costs();
        myFinder.multiflows();
    }
    else if (line.find("equiflows") != -1) {
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
    else if (line.find("hills") != -1) {
        myFinder.hills(orthogonalGrid());
        return eCalculation::hills;
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
            if( myFormat == eCalculation::multiflows )
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

    //std::cout << "<-costs\n" <<myFinder.linksText() << "\n";
}

void cPathFinderReader::sales()
{
    struct sCity
    {
        int x;
        int y;
        std::string name;
    };
    std::vector<sCity> vCity;
    sCity city;

    enum class eInput
    {
        unknown,
        city,
        link,
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
        case 'c':
            if (input == eInput::unknown)
                input = eInput::city;
            else if (input != eInput::city)
                throw std::runtime_error("Mixed input formats");
            city.x = atoi(token[1].c_str());
            city.y = atoi(token[2].c_str());
            city.name = token[3];
            vCity.push_back(city);
            break;
        case 'l':
            if (input == eInput::unknown)
                input = eInput::link;
            else if (input != eInput::link)
                throw std::runtime_error("Mixed input formats");
            myFinder.addLink(
                myFinder.findoradd(token[1]),
                myFinder.findoradd(token[2]),
                atoi(token[3].c_str()));
            break;
        default:
            std::cout << "ignored\n";
            break;
        }
    }

    if (input == eInput::city)
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
    else
    {
        // add links with infinite cost between unlinked cities
       // myFinder.makeComplete();
    }
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
                orthogonalGridNodeName(row,col));
        }
    }
    myFinder.start( start );
    myFinder.end( end );

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
std::string cPathFinderReader::orthogonalGridNodeName(
    int row, int col)
{
    return "c" + std::to_string(col + 1) + "r" + std::to_string(row + 1);
}
std::vector<std::string> cPathFinderReader::singleParentTree()
{
    std::vector<std::string> ret;
    return ret;
}

}
}
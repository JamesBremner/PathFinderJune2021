#include <math.h>
#include <sstream>
#include <algorithm>
#include "cPathFinderReader.h"

cPathFinderReader::eFormat
cPathFinderReader::open(
    const std::string &fname)
{
    eFormat ret = eFormat::none;
    myFile.close();
    myFile.open(fname);
    if (!myFile.is_open())
        return eFormat::not_open;
    std::string line;
    getline(myFile, line);
    if (line.find("format") != 0)
        return eFormat::none;
    if (line.find("costs") != -1)
    {
        costs();
        myFinder.path();
        std::cout << myFinder.pathText() << "\n";
        return eFormat::costs;
    }
    else if (line.find("spans") != -1)
    {
        costs();
        myFinder.span();
        std::cout << myFinder.spanText() << "\n";
        return eFormat::spans;
    }
    else if (line.find("sales") != -1)
    {
        sales();
        myFinder.tsp();
        return eFormat::sales;
    }
    else if (line.find("cams") != -1)
    {
        costs(false);
        myFinder.cams();
        return eFormat::cams;
    }
    return ret;
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

std::vector<std::string> cPathFinderReader::singleParentTree()
{
    std::vector<std::string> ret;
    return ret;
}

#include "cPathFinderReader.h"

cPathFinderReader::eFormat
cPathFinderReader::open(
    const std::string &fname)
{
    eFormat ret = eFormat::none;
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

    std::cout << myFinder.linksText() << "\n";
}

std::vector<std::string> cPathFinderReader::singleParentTree()
{
}

#include <iostream>

#include "cPathFinderReader.h"
#include "cGraph.h"
#include "cPathFinder.h"

cPathFinder finder;
cPathFinderReader reader(finder);

main()
{
    reader.open("../dat/costs.txt");
    std::string expected("1 -> 2 -> 3 -> 4 ->  Cost is 3\n");
    if (expected != finder.pathText())
        throw std::runtime_error("costs.txt failed");

    reader.open("../dat/costs2.txt");
    expected = "1 -> 2 -> 4 ->  Cost is 2\n";
    if (expected != finder.pathText())
        throw std::runtime_error("costs2.txt failed");

    reader.open("../dat/span.txt");
    expected = "2 -> 1 cost 1\n"
               "2 -> 3 cost 1\n"
               "3 -> 4 cost 1\n";
    if( expected != finder.spanText() )
        throw std::runtime_error( "span.txt failed");

    //std::cout << "t2\n|" << finder.spanText() << "|\n";

    std::cout << "All tests passed\n";
}
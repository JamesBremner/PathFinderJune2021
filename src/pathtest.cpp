// PathFinder unit tests

#include <iostream>

#include "cPathFinderReader.h"
#include "cGraph.h"
#include "cPathFinder.h"

cPathFinder finder;
cPathFinderReader reader(finder);

main()
{
    // Dijsktra - long path to avoid high cost link
    reader.open("../dat/costs.txt");
    std::string expected("1 -> 2 -> 3 -> 4 ->  Cost is 3\n");
    if (expected != finder.pathText())
        throw std::runtime_error("costs.txt failed");

    // Dijsktra - all link costs equal so choose shortest path
    reader.open("../dat/costs2.txt");
    expected = "1 -> 2 -> 4 ->  Cost is 2\n";
    if (expected != finder.pathText())
        throw std::runtime_error("costs2.txt failed");

    // Spanning tree
    reader.open("../dat/span.txt");
    expected = "2 -> 1 cost 1\n"
               "2 -> 3 cost 1\n"
               "3 -> 4 cost 1\n";
    if( expected != finder.spanText() )
        throw std::runtime_error( "span.txt failed");

    // Metric approx travelling salesman
    reader.open("../dat/sales.txt");
    expected = "b -> a -> f -> d -> e -> c -> b ->  Cost is 21\n";
    if (expected != finder.pathText())
        throw std::runtime_error("sales.txt failed");

    //std::cout << "t2\n|" << finder.pathText() << "|\n";

    std::cout << "All tests passed\n";
}
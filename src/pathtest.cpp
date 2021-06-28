// PathFinder unit tests

#include <iostream>

#include "cPathFinderReader.h"
#include "cGraph.h"
#include "cPathFinder.h"

raven::graph::cPathFinder finder;
raven::graph::cPathFinderReader reader(finder);

main()
{
    std::string expected;

    // std::cout << "TEST Amazon0601.txt\n";
    // reader.open("../dat/Amazon0601.txt");

    std::cout << "TEST karup.txt\n";
    reader.open("../dat/karup.txt");
    expected = "by -> bz -> bx -> ";
    if (expected != finder.pathText())
        throw std::runtime_error("karup.txt failed");

    // std::cout << "karup timing test\n";
    //  reader.open("../dat/karupTimer.txt");
    // int i;
    // std::cin >> i;


    // prerequisites
    std::cout << "TEST t2.txt\n";
    reader.open("../dat/t2.txt");
    expected = "Total skills needed 5 ( 0 2 3 5 6  )";
    if (expected != finder.resultsText())
        throw std::runtime_error("t2.txt failed");

    // link cover
    std::cout << "TEST cover3.txt\n";
    reader.open("../dat/cover3.txt");
    expected = "3 -> 1 -> 2 -> ";
    if (expected != finder.pathText())
        throw std::runtime_error("cover3.txt failed");

    // Dijsktra - long path to avoid high cost link
    reader.open("../dat/costs.txt");
    expected = "1 -> 2 -> 3 -> 4 ->  Cost is 3\n";
    if (expected != finder.pathText())
        throw std::runtime_error("costs.txt failed");

    // Dijsktra - all link costs equal so choose shortest path
    reader.open("../dat/costs2.txt");
    expected = "1 -> 2 -> 4 ->  Cost is 2\n";
    if (expected != finder.pathText())
        throw std::runtime_error("costs2.txt failed");

    // Spanning tree
    std::cout << "TEST span.txt\n";
    reader.open("../dat/span.txt");
    expected = "2 -> 1 cost 1 value 0\n"
               "2 -> 3 cost 1 value 0\n"
               "3 -> 4 cost 1 value 0\n";
    if (expected != finder.spanText())
        throw std::runtime_error("span.txt failed");

    // Metric approx travelling salesman
    std::cout << "TEST sales.txt\n";
    reader.open("../dat/sales.txt");
    expected = "b -> a -> f -> d -> e -> c -> b ->  Cost is 16\n";
    std::cout << finder.pathText() << "\n";
    if (expected != finder.pathText())
        throw std::runtime_error("sales.txt failed");

    std::cout << "TEST cliques.txt\n";
    reader.open("../dat/cliques.txt");
    finder.cliques();
    expected = "clique: 5 1 3 7 \n"
               "clique: 8 2 6 4 \n";
    if (expected != finder.resultsText())
        throw std::runtime_error("cliques.txt failed");

    reader.open("../dat/flows2.txt");
    expected = "total flow 500";
    if (expected != finder.resultsText())
        throw std::runtime_error("flows2.txt failed");

    // flow in directed graph
    reader.open("../dat/flows3.txt");
    expected = "total flow 500";
    if (expected != finder.resultsText())
        throw std::runtime_error("flows3.txt failed");

    reader.open("../dat/flows7.txt");
    expected = "total flow 9";
    if (finder.resultsText().find(expected) == -1)
        throw std::runtime_error("flows7.txt failed");

    // equal flows https://stackoverflow.com/questions/67908818
    reader.open("../dat/flows8.txt");
    expected = "total flow 8";
    if (finder.resultsText().find(expected) == -1)
        throw std::runtime_error("flows8.txt failed");

    reader.open("../dat/hill.txt");
    expected = "c1r1 -> c1r2 -> c1r3 -> c2r3 -> c3r3 ->  Cost is 4\n";
    if (finder.resultsText().find(expected) == -1)
        throw std::runtime_error("hill.txt failed");

    //reader.open("../dat/flows4.txt");
    //reader.open("../dat/flows5.txt");

    // std::cout << "e\n|" << expected << "|\n";
    // std::cout << "t2\n|" << finder.resultsText() << "|\n";

    std::cout << "\nAll tests passed\n";
}
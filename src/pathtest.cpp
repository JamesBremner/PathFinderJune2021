#include "cPathFinder.h"
#include "cPathFinderReader.h"

    cPathFinder finder;
    cPathFinderReader reader( finder );

main()
{
    reader.open( "../dat/costs.txt");
    std::string expected( "1 -> 2 -> 3 -> 4 ->  Cost is 3\n" );
    if( expected != finder.pathText() )
        throw std::runtime_error( "costs.txt failed");

    reader.open( "../dat/costs2.txt");
    expected = "1 -> 2 -> 4 ->  Cost is 2\n";
    if( expected != finder.pathText() )
        throw std::runtime_error( "costs2.txt failed");

    //std::cout <<"t2 "<< finder.pathText() << "\n";

    std::cout << "All tests passed\n";
}
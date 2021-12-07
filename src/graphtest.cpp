#include "cutest.h"
#include "cGraph.h"

using namespace raven::graph;

TEST(links)
{
    cGraph g;

    g.clear();
    g.directed(false);
    g.addLink("1", "0");
    g.addLink("2", "1");
    g.addLink("3", "0");
    g.addLink("4", "3");

    CHECK_EQUAL(4, g.linkCount());

    g.clear();
    g.directed();
    g.addLink("b", "a", 1);
    g.addLink("c", "b", 2);
    g.addLink("d", "a", 3);
    g.addLink("e", "d", 4);

    CHECK_EQUAL(4, g.linkCount());

    auto lm = g.links();

    CHECK_EQUAL(4,lm[std::make_pair(g.find("e"),g.find("d"))].myCost);
    CHECK_EQUAL(3,lm[std::make_pair(g.find("d"),g.find("s"))].myCost);
    CHECK_EQUAL(2,lm[std::make_pair(g.find("c"),g.find("b"))].myCost);
    CHECK_EQUAL(1,lm[std::make_pair(g.find("b"),g.find("a"))].myCost);

    auto inl = g.inlinks( g.find("a") );
    auto lba = inl[std::make_pair(g.find("b"),g.find("a"))];

    CHECK_EQUAL(2,inl.size());
    // CHECK_EQUAL("b",g.userName(source(lba)));
    // CHECK_EQUAL("a",g.userName(target(lba)));

}

int main()
{
    raven::set::UnitTest::RunAllTests();

    return 1;
}

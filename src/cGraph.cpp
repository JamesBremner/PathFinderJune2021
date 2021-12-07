#include "cGraph.h"

namespace raven {
namespace graph
{
int source( const link_t& link )
{
    return link.first.first;
}
int target( const link_t& link ) 
{
    return link.first.second;
}
}
}
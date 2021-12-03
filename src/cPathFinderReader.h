#include <string>
#include "cPathFinder.h"

namespace raven
{
    namespace graph
    {

        /// Read input files into pathfinder
        class cPathFinderReader
        {
        public:
            cPathFinderReader(cPathFinder &finder)
                : myFinder(finder)
            {
            }
            void set(const std::string &fname)
            {
                myfname = fname;
            }

            /** Open file, determine format and read data into pathfinder
     * @param[in] fname to open
     * @return format
     */
            eCalculation open(
                const std::string &fname);

            /** Read input file with costed links 
     * 
     *  @param[in] weights true if required, false if not, default true
     *  @param[in] directed true to create directed graph, default false
     * 
     *  format: https://github.com/JamesBremner/PathFinder/wiki/Costs 
     * 
     * If weights is false, then input lines may have just three columns,
     * and cost of 1 will be assigned to link
     */
            void costs(
                bool weights = true,
                bool directed = false);

            /** Read input file specifying pipes and valves
         */
            std::vector<int> valves(
                bool &pulse);

            /** read input file with uncosted links
     * 
     * This expects nothing but links in src dst format
     * 
     * It assumes that the graph nodes have already been created
     * 
     * There is no error checking - intended for fast reading very large graphs
     */
            void links();

            void bonesi();

            /// read tree
            std::vector<std::string> singleParentTree();

            /** read cities with locations or links
             * @return vector of node indices to be visited
             * 
             * If return vector is empty, all cities must be visted
             */
            std::vector<int> sales();

            /** read orthogonal grid of cell values
     * @return grid of cell values
     */
            std::vector<std::vector<float>>
            orthogonalGrid();

            /// read uncosted links, multiple links from one source on one line
            void multi();

            void shaun();

            void islands();

            void nodeCosts();

            void KarupRandom();

            void CollisionRandom();

        private:
            cPathFinder &myFinder;
            std::string myfname;
            std::ifstream myFile;
            eCalculation myFormat;

            /** Parse Space Delimited line
 * @param[in] l line from a space delimited text file
 * @return vector of strings containing the columns extracted from line
 */
            std::vector<std::string> ParseSpaceDelimited(
                const std::string &l);

            /** Create a name for a grid node
     * @param[in] 0-based row position
     * @param[in] 0-based col position
     * @return 1 based node name, format "r<row+1>c<col+1>"
     */
            std::string orthogonalGridNodeName(
                int row, int col);
        };
    }
}

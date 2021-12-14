namespace raven
{
    namespace graph
    {
        // forward definition of class finding paths through graphs
        class cPathFinder;

        /// represents a 2D grid maze
        class cMaze
        {
        public:
            /// read ascii art maze specification
            void read(std::ifstream &file);

            /** convert maze to a graph with nodes and links
             * @param[out] finder  the graph
             */
            void graph(cPathFinder &finder);

            /// get number of rows in maze grid
            int rowCount() const
            {
                return (int)myMaze.size();
            }

            /// get number of columns in maze grid
            int colCount() const
            {
                if (!rowCount())
                    return 0;
                return (int)myMaze[0].size();
            }

        private:

            /// represents a single cell in a maze
            class cCell
            {
            public:
                bool top, down, right, left; ///< true if wall present

                /// CTOR - build walls all around cell
                cCell(bool w = true)
                    : top(w), down(w), right(w), left(w)
                {
                }
            };

            /// the maze represented as a 2D vector of calls
            std::vector<std::vector<cCell>> myMaze;

            /// 0-based indices of start and end cells
            int myStart, myEnd;        
        };
    }
}
namespace raven
{
    namespace graph
    {
        class cPathFinder;

        class cMaze
        {
        public:
            void read(std::ifstream &file);
            void graph(cPathFinder &finder);
            int rowCount() const
            {
                return (int)myMaze.size();
            }
            int colCount() const
            {
                if (!rowCount())
                    return 0;
                return (int)myMaze[0].size();
            }

        private:
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
            std::vector<std::vector<cCell>> myMaze;
            int myStart, myEnd;         // 0-based indices of start and end cells
        };
    }
}
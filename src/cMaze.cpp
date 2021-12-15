#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>
#include "cMaze.h"
#include "cPathFinder.h"
namespace raven
{
    namespace graph
    {
        void cMaze::read(std::ifstream &file)
        {
            char str1[100], str2[100], str3[100];
            file.getline(str1, 100);
            std::string firstline(str1);
            if (firstline.find("gen") != -1)
            {
                generate(firstline);
                return;
            }
            file.getline(str2, 100);
            file.getline(str3, 100);
            int line = 1, cell = 1;
            while (!file.eof())
            {
                std::vector<cCell> vCell;

                cell = 1;
                std::cout << "line " << line << std::endl;
                int i = 0;
                bool top, down, right, left, start, end;
                top = down = right = left = false;
                int j = 0, k = 1;
                std::cout << "strlen = " << std::strlen(str1) << std::endl;
                while (i < (int)std::strlen(str1) - 1)
                {
                    cCell C;
                    top = down = right = left = start = end = false;
                    if (str1[i] == '+')
                        i++; //new cell
                    if (str1[i] == '-')
                        top = true;
                    else if (str1[i] == ' ')
                        top = false;
                    i = i + 3; //wall above
                    if (str2[j] == '|')
                        left = true;
                    else if (str2[j] == ' ')
                        left = false;
                    else if (str2[j] == 's')
                    {
                        left = true;
                        myStart = cell - 1;
                    }
                    j = j + 4; //left wall
                    if (str2[j] == '|')
                        right = true;
                    else if (str2[j] == ' ')
                        right = false; //right wall
                    else if (str2[j] == 'e')
                    {
                        right = true;
                        myEnd = cell - 1;
                    }
                    if (str3[k] == ' ')
                        down = false;
                    else if (str3[k] == '-')
                        down = true;
                    k = k + 4; //wall below
                    //cout << "cell = " << cell << endl;
                    cell++;
                    //            cout << "Top :  " << top << "\t";
                    //            cout << "down :  " << down << endl;
                    //            cout << "right :  " << right << "\t";
                    //            cout << "left :  " << left << endl;
                    //            cout << endl << endl;
                    C.top = top;
                    C.down = down;
                    C.right = right;
                    C.left = left;
                    vCell.push_back(C);
                }
                myMaze.push_back(vCell);

                std::strcpy(str1, str3);
                std::cout << str1 << std::endl;
                file.getline(str2, 100);
                file.getline(str3, 100);
                std::cout << str2 << std::endl;
                std::cout << str3 << std::endl;
                line++;
            }
        }
        void cMaze::graph(cPathFinder &finder)
        {
            // add nodes at each grid cell
            for (int row = 0; row < rowCount(); row++)
            {
                for (int col = 0; col < colCount(); col++)
                {
                    int n = finder.findoradd(
                        finder.orthogonalGridNodeName(row, col));
                }
            }

            // link nodes that have a door between them
            for (int row = 0; row < rowCount(); row++)
            {
                for (int col = 0; col < colCount(); col++)
                {
                    int n = row * colCount() + col;
                    if (!myMaze[row][col].right)
                    {
                        finder.addLink(n, n + 1);
                    }
                    if (!myMaze[row][col].down)
                    {
                        finder.addLink(n, n + colCount());
                    }
                    if (n == myStart)
                    {
                        finder.addLink(
                            "START", finder.orthogonalGridNodeName(row, col));
                    }
                    if (n == myEnd)
                    {
                        finder.addLink(
                            finder.orthogonalGridNodeName(row, col), "END");
                    }
                }
            }
            finder.start("START");
            finder.end("END");
        }
        void cMaze::generate(const std::string &cmd)
        {
            int p = cmd.find("-rows");
            if (p == -1)
                throw std::runtime_error(
                    "cMaze::generate rows undefined");
            int rows = atoi(cmd.substr(p + 5).c_str());
            p = cmd.find("-cols");
            if (p == -1)
                throw std::runtime_error(
                    "cMaze::generate cols undefined");
            int cols = atoi(cmd.substr(p + 5).c_str());
            p = cmd.find("-file");
            if (p == -1)
                throw std::runtime_error(
                    "cMaze::generate output file undefined"); 
            int q = cmd.find(" ",p+6);
            std::string fname = cmd.substr(p+6,q - p - 6);
            if (cmd.find("bin") != -1)
                generate_binary_tree(rows, cols);
            else if (cmd.find("rec") != -1)
                generate_recursive_init(rows, cols);
            else
                throw std::runtime_error(
                    "cMaze::generate algorithm undefined");
            std::ofstream f( fname );
            if( !f.is_open() )
            throw std::runtime_error(
                    "cMaze::generate cannot open output file");
            f << "format maze\n";
            for( auto& s : displayText() )
                f << s << "\n";
        }
        void cMaze::generate_binary_tree(int row_count, int col_count)
        {
            for (int row = 0; row < row_count; row++)
            {
                std::vector<cCell> vCell;
                for (int col = 0; col < col_count; col++)
                {
                    cCell C;

                    if (row == 0)
                    {
                        C.left = false;
                    }
                    else if (col == 0)
                    {
                        C.top = false;
                    }
                    else
                    {
                        int r = rand() % 2;
                        if (r)
                            C.left = false;
                        else
                            C.top = false;
                    }
                    vCell.push_back(C);
                }
                myMaze.push_back(vCell);
            }
            // select random exit
            if (rand() % 2)
            {
                int row = rand() % row_count;
                myMaze[row][col_count - 1].right = false;
            }
            else
            {
                int col = rand() & col_count;
                myMaze[row_count - 1][col].down = false;
            }
        }
void cMaze::generate_recursive_init(int row_count, int col_count)
{
    // construct initial chamber, maze with no walls except around edges
    for( int r = 0; r < row_count; r++ )
    {
        std::vector< cCell > vr;
        for( int c = 0; c < col_count; c++ )
        {
            cCell C( false );
            if( r == 0 )
                C.top = true;
            if( r == row_count-1 )
                C.down = true;
            if( c == 0 )
                C.left = true;
            if( c == col_count )
                C.right = true;
            vr.push_back( C );
        }
        myMaze.push_back( vr );
    }
    // open entrance and exit in walls around edges
    int StartRow = rand()%row_count;
    myStart = StartRow * col_count;
    int EndRow   = rand()%row_count;
    myEnd = EndRow * col_count + col_count-1;

    // myMaze[StartRow][0].left = false;
    // myMaze[EndRow][col_count].right = false;

    generate_recursive( 0, 0, col_count, row_count );
}
void cMaze::generate_recursive( int x, int y, int w, int h )
{
    // check for recurion completed
    if( w == 1 && h == 1 )
        return;

    // raise four walls
    int newx = w / 2;
    int newy = h / 2;
    for( int r = 0; r < h; r++ )
    {
        myMaze[y+r][x+newx].left = true;
    }
    for( int c = 0; c < w; c++ )
    {
        myMaze[y+newy][x+c].top = true;
    }
    // open passage in three of four walls
    int wnop = rand() % 4;
    if( wnop != 0 )
        myMaze[y+newy][x+rand() % newx].top = false;
    if( wnop != 1 )
        myMaze[y+newy][x+newx+(rand()%(w-newx))].top = false;
    if( wnop != 2 )
        myMaze[y+rand()%newy][x+newx].left = false;
    if( wnop != 3 )
        myMaze[y+newy+(rand()%(h-newy))][x+newx].left = false;

    // recurse into four smaller chambers
    generate_recursive( x, y, newx, newy );
    generate_recursive( x + newx, y, w - newx, newy );
    generate_recursive( x, y + newy, newx, h - newy );
    generate_recursive( x + newx, y + newy, w - newx, h - newy );

}
std::vector< std::string > cMaze::displayText()
{
    std::cout << myMaze.size() << " rows " << myMaze[0].size() << " cols\n";
    std::vector< std::string > vss;
    std::stringstream s1, s2, s3;
    int kr = 0;
    for( auto& r : myMaze )
    {
        int kc = 0;
        for( auto& c : r )
        {
            s1 << "+";
            if( c.top )
                s1 << "---";
            else
                s1 << "   ";
            if( c.left )
                s2 << "|";
            else
                s2 << " ";
            if( onPath(kc,kr))
                s2 << " * ";
            else
                s2 << "   ";
            if( kc == myMaze[0].size()-1 )
            {
                s1 << "+";
                if( c.right )
                    s2 << "|";
                else
                    s2 << " ";
            }
            if( kr == myMaze.size() - 1 )
            {
                s3 << "+";
                if( c.down )
                    s3 << "---";
                else
                    s3 << "   ";
                if( kc == myMaze[0].size()-1 )
                    s3 << "+";
            }
            kc++;
        }

//        cout << s1.str() << "\n";
//        cout << s2.str() << "\n";
        vss.push_back( s1.str() );
        vss.push_back( s2.str() );
        s1.str("");
        s2.str("");
        if( kr == myMaze.size() - 1 )
        {
            //cout << s3.str() << "\n";
            vss.push_back( s3.str() );
        }
        kr++;
    }

    return vss;
}


    }
}
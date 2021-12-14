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
                        myStart = cell-1;
                    }
                    j = j + 4; //left wall
                    if (str2[j] == '|')
                        right = true;
                    else if (str2[j] == ' ')
                        right = false; //right wall
                    else if (str2[j] == 'e') {   
                        right = true; 
                        myEnd = cell-1;
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
                    if( ! myMaze[row][col].right ) {
                        finder.addLink( n, n+1 );
                    }
                    if( ! myMaze[row][col].down ) {
                        finder.addLink( n, n+colCount() );
                    }
                    if( n == myStart ) {
                        finder.addLink(
                            "START", finder.orthogonalGridNodeName(row, col));
                    }
                    if( n == myEnd ) {
                        finder.addLink(
                            finder.orthogonalGridNodeName(row, col), "END" );
                    }
                }
            }
            finder.start( "START");
            finder.end( "END" );
        }
    }
}
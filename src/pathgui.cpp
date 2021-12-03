#include <filesystem>
#include <set>
#include "wex.h"
#include "inputbox.h"
#include "window2file.h"
#include <gdiplus.h>
#include "cPathFinder.h"
#include "cPathFinderReader.h"

// enum class eOption
// {
//     none,
//     costs,
//     req,
//     sales,
//     span,
//     cams,
//     ortho,
//     islands,
// };

raven::graph::eCalculation opt = raven::graph::eCalculation::none;

void replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void RunDOT(raven::graph::cPathFinder &finder)
{
    // temp firectory is usually
    // C:\Users\<userName>\AppData\Local\Temp

    auto path = std::filesystem::temp_directory_path();
    std::cout << "RunDOT " << path <<"\n";
    auto gdot = path / "g.dot";
    std::ofstream f(gdot);
    if (!f.is_open())
        throw std::runtime_error("Cannot open " + gdot.string());

using raven::graph::eCalculation;
    switch (opt)
    {
    case eCalculation::costs:
    case eCalculation::sales:
    case eCalculation::bonesi:
        f << finder.pathViz() << "\n";
        break;
    case eCalculation::spans:
        f << finder.spanViz() << "\n";
        break;
    case eCalculation::cams:
    case eCalculation::reqs:
        f << finder.camsViz() << "\n";
        break;
    default:
        std::cout << "RunDOT bad option\n";
        return;
    }

    f.close();

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // char cmd[100];
    // snprintf(cmd, 99, "dot -Kfdp -n -Tpng -o sample.png g.dot");

    std::string scmd = "dot -Kfdp -n -Tpng -o ";
    auto sample = path / "sample.png";
    scmd += sample.string() + " " + gdot.string();

    // Retain keyboard focus, minimize module2 window
    si.wShowWindow = SW_SHOWNOACTIVATE | SW_MINIMIZE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USEPOSITION;
    si.dwX = 600;
    si.dwY = 200;

    if (!CreateProcess(NULL,                 // No module name (use command line)
                       (char *)scmd.c_str(), // Command line
                       NULL,                 // Process handle not inheritable
                       NULL,                 // Thread handle not inheritable
                       FALSE,                // Set handle inheritance to FALSE
                       CREATE_NEW_CONSOLE,   //  creation flags
                       NULL,                 // Use parent's environment block
                       NULL,                 // Use parent's starting directory
                       &si,                  // Pointer to STARTUPINFO structure
                       &pi)                  // Pointer to PROCESS_INFORMATION structure
    )
    {
        int syserrno = GetLastError();
        if (syserrno == 2)
        {
            wex::msgbox mb(
                "Cannot find executable file\n"
                "Is graphViz installed?");

            return;
        }
        wex::msgbox mb("system error");
        // SetStatusText(wxString::Format("Sysem error no (%d)\n", GetLastError()));
        // wchar_t *lpMsgBuf;
        // FormatMessage(
        //     FORMAT_MESSAGE_ALLOCATE_BUFFER |
        //         FORMAT_MESSAGE_FROM_SYSTEM |
        //         FORMAT_MESSAGE_IGNORE_INSERTS,
        //     NULL,
        //     (DWORD)syserrno,
        //     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        //     (LPWSTR)&lpMsgBuf,
        //     0, NULL);

        // char errorbuf[200];
        // snprintf(errorbuf, 199,
        //          "Error is %S",
        //          lpMsgBuf);
        // LocalFree(lpMsgBuf);

        return;
    }

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    Sleep(1000);
}

void doPreReqs(
    raven::graph::cPathFinder &finder,
    raven::graph::cPathFinderReader &reader)
{
    std::set<int> setSkillsNeeded;

    // read input. va are the required skills
    auto va = reader.singleParentTree();

    // starting node
    finder.start("0");

    // paths to all end nodes
    finder.end(-1);

    // run Dijsktra
    finder.path();

    // loop over required skills
    for (auto &a : va)
    {
        // skill 0 does not need to be learned
        if (a == "0")
            continue;

        std::cout << "skill " << a << " needs ";

        //skills needed to get reuired skill
        auto path = finder.pathPick(finder.find(a));

        for (int s : path)
            std::cout << finder.name(s) << " ";
        std::cout << "\n";

        //loop over prerequsites
        for (auto s : path)
        {
            //record skill if not already learned
            setSkillsNeeded.insert(s);
        }
    }

    std::cout << "Total skills needed "
              << setSkillsNeeded.size() << " ( ";
    for (int s : setSkillsNeeded)
        std::cout << finder.name(s) << " ";
    std::cout << " )\n";
}

int main()
{
    raven::graph::cPathFinder finder;

    // construct top level application window
    wex::gui &form = wex::maker::make();
    form.move({50, 50, 800, 800});
    form.text("Path Finder GUI");

    wex::panel &graphPanel = wex::maker::make<wex::panel>(form);
    graphPanel.move(0, 50, 800, 750);

    wex::multiline &editPanel = wex::maker::make<wex::multiline>(form);
    editPanel.move(0, 0, 800, 800);
    editPanel.text("");
    editPanel.fontName("courier");
    editPanel.fontHeight(18);

    wex::menubar mbar(form);
    wex::menu mfile(form);
    mfile.append("Edit", [&](const std::string &title)
                 {
                     wex::filebox fb(form);
                     auto fname = fb.open();
                     if (fname.empty())
                         return;
                     std::ifstream f(fname);
                     if (!f.is_open())
                         throw std::runtime_error("Cannot open " + fname);
                     std::stringstream buffer;
                     buffer << f.rdbuf();
                     auto snr = buffer.str();
                     replaceAll(snr, "\n", "\r\n");
                     editPanel.text(snr);
                     editPanel.show();
                     graphPanel.show(false);
                     editPanel.update();
                 });
    mfile.append("Save", [&](const std::string &title)
                 {
                     wex::filebox fb(form);
                     auto fname = fb.save();
                     if (fname.empty())
                         return;
                     std::ofstream f(fname);
                     if (!f.is_open())
                     {
                         wex::msgbox m("Cannot open " + fname);
                         return;
                     }
                     auto s = editPanel.text();
                     replaceAll(s, "\r\n", "\n");
                     f << s;
                 });
    mfile.append("Calculate", [&](const std::string &title)
                 {
                     using raven::graph::cPathFinderReader;
                     finder.clear();
                     wex::filebox fb(form);
                     auto fname = fb.open();
                     if (fname.empty())
                         return;
                     cPathFinderReader reader(finder);

                     try
                     {
                         using raven::graph::eCalculation;
                         switch (reader.open(fname))
                         {
                         case eCalculation::none:
                             throw std::runtime_error(
                                 "File format not specified");
                         case eCalculation::not_open:
                             throw std::runtime_error(
                                 "Cannot open " + fname);

                         case eCalculation::costs:
                         case eCalculation::hills:
                         case eCalculation::gsingh:
                         case eCalculation::shaun:
                         case eCalculation::flows:
                         case eCalculation::multiflows:
                         case eCalculation::bonesi:
                            opt = eCalculation::costs;
                             break;
                        case eCalculation::reqs:
                            opt = eCalculation::reqs;
                            break;
                        case eCalculation::cliques:
                            finder.cliques();
                            opt = eCalculation::costs;
                             break;
                         case eCalculation::sales:
                             opt = eCalculation::sales;
                             break;
                         case eCalculation::spans:
                             opt = eCalculation::spans;
                             break;
                         case eCalculation::cams:
                             opt = eCalculation::cams;
                             break;
                         case eCalculation::islands:
                             opt = eCalculation::islands;
                             break;
                         case eCalculation::maze_ascii_art:
                             break;

                         default:
                             throw std::runtime_error(
                                 "UNrecognized file format");
                         }
                     }
                     catch (std::runtime_error &e)
                     {
                         wex::msgbox m(e.what());
                         return;
                     }

                     RunDOT(finder);
                     editPanel.show(false);
                     form.text("Path Finder GUI " + fname);

                     form.update();
                 });
    mbar.append("File", mfile);

    form.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            wex::shapes s(ps);
            using raven::graph::eCalculation;
            switch (opt)
            {
            case eCalculation::costs:
            case eCalculation::sales:
            case eCalculation::cams:
                s.text(
                    finder.pathText(),
                    {5, 5});
                break;
            case eCalculation::spans:
                s.text(
                    finder.spanText(),
                    {5, 5});
                break;
            case eCalculation::islands:
                // s.text(
                //     "There are " + std::to_string( finder.islandCount() ) + " islands",
                //     {5, 5});
                break;
            case eCalculation::reqs:
                   s.text(
                    finder.resultsText(),
                    {5, 5});
                break;         
            }
        });

    graphPanel.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            wex::window2file w2f;
            auto path = std::filesystem::temp_directory_path();
            auto sample = path / "sample.png";
            w2f.draw(graphPanel, sample.string());
        });

    std::error_code ec;
    std::filesystem::remove(
        std::filesystem::temp_directory_path() / "sample.png", ec);

    // show the application
    form.show();

    //Pass the control of the application to the windows message queue.
    form.run();

    return 0;
}
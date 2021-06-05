#include <filesystem>
#include <set>
#include "wex.h"
#include "inputbox.h"
#include "window2file.h"
#include <gdiplus.h>
#include "cPathFinder.h"
#include "cPathFinderReader.h"

enum class eOption
{
    none,
    costs,
    req,
    sales,
    span,
    cams,
    ortho,
    islands,
};

eOption opt = eOption::none;

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

void RunDOT(cPathFinder &finder)
{
    auto path = std::filesystem::temp_directory_path();
    auto gdot = path / "g.dot";
    std::ofstream f(gdot);
    if (!f.is_open())
        throw std::runtime_error("Cannot open " + gdot.string());

    switch (opt)
    {
    case eOption::costs:
    case eOption::sales:
        f << finder.pathViz() << "\n";
        break;
    case eOption::span:
        f << finder.spanViz() << "\n";
        break;
    default:
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
            wex::msgbox mb("Cannot find executable file");
            // NotifyUserOfError(wxString::Format("Cannot find executable file %S in working directory\n",
            //                                    exec_filename));
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
    cPathFinder &finder,
    cPathFinderReader &reader)
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
            std::cout << finder.nodeName(s) << " ";
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
        std::cout << finder.nodeName(s) << " ";
    std::cout << " )\n";
}

int main()
{
    cPathFinder finder;

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
                     finder.clear();
                     wex::filebox fb(form);
                     auto fname = fb.open();
                     if (fname.empty())
                         return;
                     cPathFinderReader reader(finder);

                     try
                     {
                         switch (reader.open(fname))
                         {
                         case cPathFinderReader::eFormat::none:
                             throw std::runtime_error(
                                 "File format not specified");
                         case cPathFinderReader::eFormat::not_open:
                             throw std::runtime_error(
                                 "Cannot open " + fname);

                         case cPathFinderReader::eFormat::costs:
                         case cPathFinderReader::eFormat::hills:
                         case cPathFinderReader::eFormat::cams:
                         case cPathFinderReader::eFormat::gsingh:
                         case cPathFinderReader::eFormat::shaun:
                         case cPathFinderReader::eFormat::flows:
                             break;
                         case cPathFinderReader::eFormat::sales:
                             opt = eOption::sales;
                             break;
                         case cPathFinderReader::eFormat::spans:
                             opt = eOption::span;
                             break;
                         case cPathFinderReader::eFormat::islands:
                             opt = eOption::islands;
                             break;
                         case cPathFinderReader::eFormat::maze_ascii_art:
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

    form.events().draw([&](PAINTSTRUCT &ps)
                       {
                           wex::shapes s(ps);
                           switch (opt)
                           {
                           case eOption::costs:
                           case eOption::sales:
                               s.text(
                                   finder.pathText(),
                                   {5, 5});
                               break;
                           case eOption::span:
                               s.text(
                                   finder.spanText(),
                                   {5, 5});
                               break;
                           case eOption::islands:
                               // s.text(
                               //     "There are " + std::to_string( finder.islandCount() ) + " islands",
                               //     {5, 5});
                               break;
                           }
                       });

    graphPanel.events().draw([&](PAINTSTRUCT &ps)
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
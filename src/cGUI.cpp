#include <wex.h>
#include "cGUI.h"
#include <inputbox.h>

cGUI::cGUI()
    : fm(wex::maker::make())
{
    fm.move({50, 50, 700, 700});
    fm.text("SPH");

    eventHandlers();
    menus();

    fm.show();
    fm.run();
}

void cGUI::eventHandlers()
{
    fm.events().timer(
        [this](int id)
        {
            const int stepsBetweenDisplayUpdastes = 5;

            for (int k = 0; k < stepsBetweenDisplayUpdastes; k++)
                theSim.simStep();

            fm.update();
        });

    fm.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            theSim.draw(S);
        });
}

void cGUI::menus()
{
    wex::menubar mb(fm);

    // myFileMenu = new wex::menu(fm);
    // myFileMenu->append(
    //     "Open",
    //     [&](const std::string &title)
    //     {
    //         wex::filebox fb(fm);
    //         auto fn = fb.open();
    //         if (fn.empty())
    //             return;
    //         std::ifstream t(fn);
    //         std::stringstream buffer;
    //         buffer << t.rdbuf();
    //         // mySolver.input(buffer.str());
    //         fm.update();
    //     });

    // myFileMenu->append(
    //     "Save",
    //     [&](const std::string &title)
    //     {
    //         wex::filebox fb(fm);
    //         auto fn = fb.save();
    //         if (fn.empty())
    //             return;
    //         std::ofstream of(fn);
    //         if (!of.is_open())
    //         {
    //             wex::msgbox("Cannot open " + fn);
    //             return;
    //         }
    //         // mySolver.save( of );
    //     });
    // mb.append("File", *myFileMenu);

    myRunMenu = new wex::menu(fm);
    myRunMenu->append(
        "Simulation",
        [&](const std::string &title)
        {
            myUpdateTimer = new wex::timer(fm, 100);

        });
    mb.append("Run", *myRunMenu);
}

void cSPHsim::draw(wex::shapes &S)
{
    for (auto &p : particles)
    {
        S.rectangle({(int)p.pos.x - 1, (int)p.pos.y - 1, 3, 3});
    }
}
#include <wex.h>
#include "cGUI.h"
#include <inputbox.h>

cGUI::cGUI()
    : fm(wex::maker::make())
{
    fm.move({50, 50, 700, 700});
    fm.text("SPH");

    theSim.simStep();

    eventHandlers();
    menus();

    myUpdateTimer = new wex::timer(fm, 20);

    fm.show();
    fm.run();
}

void cGUI::eventHandlers()
{
    fm.events().timer(
        [this](int id) {
            theSim.simStep();
            fm.update();
        });

    fm.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            theSim.draw(S);
        });

    fm.events().click(
        [&]
        {
            auto ms = fm.getMouseStatus();
            wex::msgbox(
                "Click at " + std::to_string(ms.x) + ", " + std::to_string(ms.y));
        });
}

void cGUI::menus()
{
    wex::menubar mb(fm);

    myFileMenu = new wex::menu(fm);
    myFileMenu->append(
        "Open",
        [&](const std::string &title)
        {
            wex::filebox fb(fm);
            auto fn = fb.open();
            if (fn.empty())
                return;
            std::ifstream t(fn);
            std::stringstream buffer;
            buffer << t.rdbuf();
            // mySolver.input(buffer.str());
            fm.update();
        });

    myFileMenu->append(
        "Save",
        [&](const std::string &title)
        {
            wex::filebox fb(fm);
            auto fn = fb.save();
            if (fn.empty())
                return;
            std::ofstream of(fn);
            if (!of.is_open())
            {
                wex::msgbox("Cannot open " + fn);
                return;
            }
            // mySolver.save( of );
        });
    mb.append("File", *myFileMenu);

    myEditMenu = new wex::menu(fm);
    myEditMenu->append(
        "Specifications",
        [&](const std::string &title)
        {
            //  mySolver.editSpecs(fm);
            fm.update();
        });
    mb.append("Edit", *myEditMenu);
}

void cSPHsim::draw(wex::shapes &S)
{
    for (auto &p : particles)
    {
        S.rectangle({(int)p.pos.x - 1, (int)p.pos.y - 1, 3, 3});
    }
}
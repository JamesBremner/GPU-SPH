#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "Particle.h"
#include "cSPHsim.h"

class cGUI
{
public:

    cGUI();

private:
    wex::gui &fm;
    wex::menu *myFileMenu;
    wex::menu *myEditMenu;

    cSPHsim theSim;

    void menus();
    void eventHandlers();
    void draw( wex::shapes& S );
};
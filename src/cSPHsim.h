#pragma once

namespace wex {
    class shapes;
}

class cSPHsim
{
    public:
    cSPHsim();
    int simStep();
    void draw( wex::shapes& S );

    std::vector<Particle> particles;
};
#pragma once

namespace wex {
    class shapes;
}

class cSPHsim
{
    public:
    int simStep();
    void draw( wex::shapes& S );

    std::vector<Particle> particles;
};
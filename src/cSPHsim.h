#pragma once

namespace wex
{
    class shapes;
}

class cSPHsim
{
public:
    cSPHsim();
    int simStep();
    void draw(wex::shapes &S);

    std::vector<Particle> particles;

private:
    float poly6(float r, float C, float hh);

    float density_to_pressure(float density);

    cxy poly6_grad(cxy dp, float r, float C6, float hh);

    float viscosity_laplacian(float r, float visc_const);

    /// @brief Impose speed limit
    /// @param[in/out] vel
    ///
    /// Why is there s speed limit ( less than c )?

    void speedLimit(cxy &vel);

    void NewtonLaw2(
        Particle &p,
        const cxy &force,
        float mass);

    void handle_boundaries(int id);
};
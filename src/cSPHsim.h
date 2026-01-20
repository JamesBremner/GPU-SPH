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

    /// @brief calculate particle seperation distance
    /// @param[out] separation distance between particles ( -1 if irrelevant, i.e > smoothing radius )
    /// @param[in] pos1 location first particle
    /// @param[in] pos2 location second particle
    ///
    /// This avoid the square root extraction if particles are far apart
    /// but in general it is an expensive operation
    /// so do not make redundant calls to this.

    std::vector<float> particleSeparation( int focusIndex );

    bool unitTests();
};
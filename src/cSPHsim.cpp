#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <random>

#include "Particle.h"
#include "cGUI.h"

const float PI = 3.14159265358979323846;

const int height = 600;
const int width = 600;
const int screen_width = width;
const int screen_height = height;
unsigned int num_particles = 1024;
const unsigned int WORKGROUP_SIZE = 256;

float viscocity = 10.0f;
const float mass = 1.0f;
const float smoothing_radius = 16.f;
const float smoothing_radius_squared = smoothing_radius * smoothing_radius;
const float gravity = 200.0f;
const float timeStep = 0.005f;
const int stepsPerRender = 1;
const float max_speed = 200.0f;
const float size = 20;

const float rest_density = 8.0f;
const float gas_constant = 200.0f;
const float damping = 0.6f;
const float force = 9.81f; // the gravitational constant. see https://github.com/JimaBob/GPU-SPH/issues/5

void cSPHsim::init()
{
    calcViscConst();
}
void cSPHsim::calcViscConst()
{
    C = 4.0 / (PI * pow(smoothing_radius, 8));
    C6 = -6 * C;
    visc_const = (45.0f / (PI * pow(smoothing_radius, 6)));
}

std::vector<float> cSPHsim::particleSeparation(int focusIndex)
{
    std::vector<float> ret;
    for (int i = 0; i < num_particles; i++)
    {
        float d2 = particles[focusIndex].pos.dist2(particles[i].pos);
        if (d2 > smoothing_radius_squared)
        {
            ret.push_back(-1);
        }
        else
        {
            ret.push_back(sqrt(d2));
        }
    }
    return ret;
}

float cSPHsim::poly6(float r)
{
    if (r < 0)
        return 0.0;
    float val = smoothing_radius_squared - r * r;
    return C * val * val * val;
}

float cSPHsim::density_to_pressure(float density)
{
    return gas_constant * (density - rest_density);
}

cxy cSPHsim::poly6_grad(cxy dp, float r)
{
    if (r < 0)
        return cxy(0, 0);
    float val = smoothing_radius_squared - r * r;
    float scale = C6 * val * val;
    dp *= scale;
    return dp;
}

float cSPHsim::viscosity_laplacian(float r, float visc_const)
{
    if (r < 0)
        return 0.f;
    return visc_const * (smoothing_radius - r);
}

void cSPHsim::speedLimit(cxy &vel)
{

    // avoid extracting square root unless speed limit exceeded
    const float maxSpeedSquared = 40000;
    float ss = vel.x * vel.x + vel.y * vel.y;
    if (ss <= maxSpeedSquared)
        return;

    // reduce speed
    vel *= 200 / sqrt(ss);
}

void cSPHsim::NewtonLaw2(
    Particle &p,
    const cxy &force,
    float mass)
{
    cxy deltaVelocity(force);
    deltaVelocity *= 1.0f / mass;
    p.vel += deltaVelocity;
}

cSPHsim::cSPHsim()
{
    // Particles Setup
    particles = Particle::generate(
        num_particles, width / 4, height / 4,
        cxy(width / 4, height / 4));
}

void cSPHsim::handle_boundaries(int id)
{
    const float boundary_damping = 0.8;
    const int particle_size = 0; // see https://github.com/JimaBob/GPU-SPH/issues/5#issuecomment-3765761865

    // Left boundary
    if (particles[id].pos.x - particle_size < 0.0)
    {
        particles[id].pos.x = particle_size;
        particles[id].vel.x = abs(particles[id].vel.x) * boundary_damping;
    }
    // Right boundary
    if (particles[id].pos.x + particle_size > width)
    {
        particles[id].pos.x = width - particle_size;
        particles[id].vel.x = -abs(particles[id].vel.x) * boundary_damping;
    }
    // Bottom boundary
    if (particles[id].pos.y - particle_size < 0.0)
    {
        particles[id].pos.y = particle_size;
        particles[id].vel.y = abs(particles[id].vel.y) * boundary_damping;
    }
    // Top boundary
    if (particles[id].pos.y + particle_size > height)
    {
        particles[id].pos.y = height - particle_size;
        particles[id].vel.y = -abs(particles[id].vel.y) * boundary_damping;
    }
}

int cSPHsim::simStep()
{

    // loop over the particles
    for (int id = 0; id < num_particles; id++)
    {

        std::vector<float> vParticleSeparations = particleSeparation(id);

        // Compute density
        float rho = 0.0;
        for (int i = 0; i < num_particles; i++)
        {
            rho += mass * poly6( vParticleSeparations[i] );

        }
        particles[id].density = std::max(rho, 0.000001f);
        float P = density_to_pressure(particles[id].density);

        // Compute pressure

        cxy f(0, 0);
        cxy visc(0, 0);

        for (int i = 0; i < num_particles; i++)
        { // Needs neighbourhood search still
            if (i == id)
                continue;

            cxy dp = particles[i].pos - particles[id].pos;
            float r = vParticleSeparations[i];

            if (r < 0)
                continue;
            if (r < 0.0001)
                r = 0.0001;

            float rhoi = std::max(particles[i].density, 0.000001f);
            float Pi = density_to_pressure(rhoi);
            cxy grad = poly6_grad(dp, r);

            float coeff = -mass * (P + Pi) / (2.0 * rhoi);
            grad *= coeff;
            f += grad;

            cxy v = particles[i].vel - particles[id].vel;
            v *= viscosity_laplacian(r, visc_const) / rhoi;
            visc += v;
        }
        // Apply forces/accelerate
        cxy vmv = visc;
        vmv *= viscocity * mass;
        f += vmv;

        // Calculate mass of the particle
        // ( I do not understand this calculation
        // see https://github.com/JimaBob/GPU-SPH/issues/6 )
        float massOfParticle = particles[id].density + force / mass;

        NewtonLaw2(particles[id], f, massOfParticle);

        // impose speed limit
        speedLimit(particles[id].vel);

        // Move and process boundaries

        cxy v(particles[id].vel);
        v *= timeStep;
        particles[id].pos += v;

        handle_boundaries(id);
    }

    return 0;
}

bool cSPHsim::unitTests()
{
    cxy pos1(1, 2);
    cxy pos2(4, 6);

    particles.clear();
    particles.emplace_back(1, 2);
    particles.emplace_back(4, 6);
    auto vSeps = particleSeparation(0);
    if (vSeps[1] != 5)
        return false;

    particles[1] = Particle(301, 402);
    particleSeparation(0);
    if (vSeps[1] != -1)
        return false;

    return true;
}
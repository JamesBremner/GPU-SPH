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
const float gravity = 200.0f;
const float timeStep = 0.005f;
const int stepsPerRender = 1;
const float max_speed = 200.0f;
const float size = 20;

const float rest_density = 8.0f;
const float gas_constant = 200.0f;
const float damping = 0.6f;
const float force = 9.81f; // the gravitational constant. see https://github.com/JimaBob/GPU-SPH/issues/5

float cSPHsim::poly6(float r, float C, float hh)
{
    if (r >= smoothing_radius)
        return 0.0;
    float val = hh - r * r;
    return C * val * val * val;
}

float cSPHsim::density_to_pressure(float density)
{
    return gas_constant * (density - rest_density);
}

cxy cSPHsim::poly6_grad(cxy dp, float r, float C6, float hh)
{
    if (r >= smoothing_radius || r == 0.f)
        return cxy(0, 0);
    float val = hh - r * r;
    float scale = C6 * val * val;
    dp *= scale;
    return dp;
}

float cSPHsim::viscosity_laplacian(float r, float visc_const)
{
    if (r >= smoothing_radius)
        return 0.f;
    return visc_const * (smoothing_radius - r);
}



void cSPHsim::speedLimit(cxy &vel)
{

    // avoid extracting square root unless sepeed limit exceeded
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
    cxy deltaVelocity( force );
    deltaVelocity *= 1.0f / mass;
    p.vel += deltaVelocity;
}

cSPHsim::cSPHsim()
{
    // Particles Setup
    particles = Particle::generate(
        num_particles, width, height);
}

int cSPHsim::simStep()
{

    // loop over the particles
    for (int id = 0; id < num_particles; id++)
    {

        const float C = 4.0 / (PI * pow(smoothing_radius, 8));
        const float C6 = -6 * C;
        const float hh = smoothing_radius * smoothing_radius;
        const float visc_const = (45.0f / (PI * pow(smoothing_radius, 6)));

        // Compute density
        float rho = 0.0;
        for (int i = 0; i < num_particles; i++)
        {
            float r = sqrt(particles[id].pos.dist2(particles[i].pos));
            rho += mass * poly6(r, C, hh);
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
            float r = sqrt(particles[id].pos.dist2(particles[i].pos));

            if (r < 0.0001)
                r = 0.0001;
            if (r >= smoothing_radius || r == 0.0)
                continue;

            float rhoi = std::max(particles[i].density, 0.000001f);
            float Pi = density_to_pressure(rhoi);
            cxy grad = poly6_grad(dp, r, C6, hh);

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
    }

    return 0;
}


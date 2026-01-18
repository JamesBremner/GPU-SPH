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


int cSPHsim::simStep() {
    // Particles Setup
    particles = Particle::generate(
            num_particles, width, height);

    // loop over the particles
     for ( int id = 0; id < num_particles; id++) 
     {

    //     const float C = 4.0 / (PI * pow(smoothing_radius, 8));
    //     const float C6 = -6 * C;
    //     const float hh = smoothing_radius * smoothing_radius;
    //     const float visc_const = (45.0f / (PI * pow(smoothing_radius, 6)));

    //     // Compute density
    //     float rho = 0.0;
    //     for (int i = 0; i < num_particles; i++)
    //     {
    //         // float r = length(particles[id].pos - particles[i].pos);
    //         float r = 
    //         rho += mass * poly6(r, C, hh);
    //     }
    //     particles[id].density = max(rho, 0.000001);
    //     float P = density_to_pressure(particles[id].density);

    //     // Compute pressure

    //     vec2 f = vec2(0.0);
    //     vec2 visc = vec2(0.0);

    //     for (int i = 0; i < num_particles; i++)
    //     { // Needs neighbourhood search still
    //         if (i == id)
    //             continue;

    //         vec2 dp = particles[i].pos - particles[id].pos;
    //         float r = length(dp);

    //         if (r < 0.0001)
    //             r = 0.0001;
    //         if (r >= smoothing_radius || r == 0.0)
    //             continue;

    //         float rhoi = max(particles[i].density, 0.000001);
    //         float Pi = density_to_pressure(rhoi);
    //         vec2 grad = poly6_grad(dp, r, C6, hh);

    //         float coeff = -mass * (P + Pi) / (2.0 * rhoi);
    //         f += coeff * grad;

    //         visc += (particles[i].vel - particles[id].vel) * viscosity_laplacian(r, visc_const) / rhoi;
    //     }
     }

    return 0;
}
main()
{

    cGUI theGUI;
    return 0;
    
}


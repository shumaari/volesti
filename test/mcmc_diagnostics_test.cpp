// VolEsti (volume computation and sampling library)

// Copyright (c) 2012-2020 Vissarion Fisikopoulos
// Copyright (c) 2018-2020 Apostolos Chalkis

// Licensed under GNU LGPL.3, see LICENCE file


// Edited by HZ on 11.06.2020 - mute doctest.h
#include "doctest.h"
#include <fstream>
#include <iostream>

#include <boost/random.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>

#include "misc/misc.h"

#include "random_walks/random_walks.hpp"

#include "volume/volume_sequence_of_balls.hpp"
#include "volume/volume_cooling_gaussians.hpp"
#include "volume/volume_cooling_balls.hpp"
#include "generators/known_polytope_generators.h"
#include "sampling/sampling.hpp"

#include "diagnostics/effective_sample_size.hpp"
#include "diagnostics/multivariate_psrf.hpp"
#include "diagnostics/univariate_psrf.hpp"
#include "diagnostics/interval_psrf.hpp"
#include "diagnostics/effective_sample_size.hpp"
#include "diagnostics/geweke.hpp"
#include "diagnostics/raftery.hpp"

template
<
    typename MT,
    typename WalkType,
    typename Polytope
>
MT get_samples(Polytope &P)
{
    typedef typename Polytope::PointType Point;
    typedef typename Polytope::NT NT;

    typedef BoostRandomNumberGenerator<boost::mt19937, NT, 3> RNGType;

    unsigned int walkL = 10, numpoints = 10000, nburns = 0, d = P.dimension();
    RNGType rng(d);
    Point StartingPoint(d);
    std::list<Point> randPoints;

    uniform_sampling<WalkType>(randPoints, P, rng, walkL, numpoints,
                               StartingPoint, nburns);

    MT samples(d, numpoints);
    unsigned int jj = 0;

    for (typename std::list<Point>::iterator rpit = randPoints.begin(); rpit!=randPoints.end(); rpit++, jj++)
    {
        samples.col(jj) = (*rpit).getCoefficients();
    }

    return samples;
}

template <typename NT>
void call_test_psrf(){
    typedef Cartesian<NT>    Kernel;
    typedef typename Kernel::Point    Point;
    typedef HPolytope<Point> Hpolytope;
    typedef Eigen::Matrix<NT,Eigen::Dynamic,Eigen::Dynamic> MT;
    typedef Eigen::Matrix<NT,Eigen::Dynamic,1> VT;
    Hpolytope P;
    unsigned int d = 10;

    std::cout << "--- Testing psrf on Billiard Walk and H-cube10" << std::endl;
    P = generate_cube<Hpolytope>(d, false);
    P.ComputeInnerBall();

    MT samples = get_samples<MT, AcceleratedBilliardWalk>(P);

    NT score = multivariate_psrf<NT, VT>(samples);

    std::cout<<"psrf = "<<score<<std::endl;
    CHECK(score < 1.1);
}

template <typename NT>
void call_test_univariate_psrf(){
    typedef Cartesian<NT>    Kernel;
    typedef typename Kernel::Point    Point;
    typedef HPolytope<Point> Hpolytope;
    typedef Eigen::Matrix<NT,Eigen::Dynamic,Eigen::Dynamic> MT;
    typedef Eigen::Matrix<NT,Eigen::Dynamic,1> VT;
    Hpolytope P;
    unsigned int d = 10;

    std::cout << "--- Testing psrf on Billiard Walk and H-cube10" << std::endl;
    P = generate_cube<Hpolytope>(d, false);
    P.ComputeInnerBall();

    MT samples = get_samples<MT, AcceleratedBilliardWalk>(P);

    VT score = univariate_psrf<NT, VT>(samples);

    std::cout<<"univariate psrf = "<<score.transpose()<<std::endl;
    CHECK(score.maxCoeff() < 1.1);
}

template <typename NT>
void call_test_interval_psrf(){
    typedef Cartesian<NT>    Kernel;
    typedef typename Kernel::Point    Point;
    typedef HPolytope<Point> Hpolytope;
    typedef Eigen::Matrix<NT,Eigen::Dynamic,Eigen::Dynamic> MT;
    typedef Eigen::Matrix<NT,Eigen::Dynamic,1> VT;
    Hpolytope P;
    unsigned int d = 10;
    NT alpha = 0.05;

    std::cout << "--- Testing psrf on Billiard Walk and H-cube10" << std::endl;
    P = generate_cube<Hpolytope>(d, false);
    P.ComputeInnerBall();

    MT samples = get_samples<MT, AcceleratedBilliardWalk>(P);

    VT score = interval_psrf<VT>(samples, alpha);

    std::cout<<"interval psrfs = "<<score.transpose()<<std::endl;
    CHECK(score.maxCoeff() < 1.1);
}

template <typename NT>
void call_test_ess(){
    typedef Cartesian<NT>    Kernel;
    typedef typename Kernel::Point    Point;
    typedef HPolytope<Point> Hpolytope;
    typedef Eigen::Matrix<NT,Eigen::Dynamic,Eigen::Dynamic> MT;
    typedef Eigen::Matrix<NT,Eigen::Dynamic,1> VT;
    Hpolytope P;
    unsigned int d = 10, min_ess;

    std::cout << "--- Testing psrf on Billiard Walk and H-cube10" << std::endl;
    P = generate_cube<Hpolytope>(d, false);
    P.ComputeInnerBall();

    MT samples = get_samples<MT, AcceleratedBilliardWalk>(P);

    VT score = effective_sample_size<NT, VT>(samples, min_ess);

    std::cout<<"ess = "<<score.transpose()<<std::endl;
    CHECK(score.minCoeff() > 100);
}

template <typename NT>
void call_test_geweke(){
    typedef Cartesian<NT>    Kernel;
    typedef typename Kernel::Point    Point;
    typedef HPolytope<Point> Hpolytope;
    typedef Eigen::Matrix<NT,Eigen::Dynamic,Eigen::Dynamic> MT;
    typedef Eigen::Matrix<NT,Eigen::Dynamic,1> VT;
    Hpolytope P;
    unsigned int d = 10;

    std::cout << "--- Testing psrf on Billiard Walk and H-cube10" << std::endl;
    P = generate_cube<Hpolytope>(d, false);
    P.ComputeInnerBall();

    MT samples = get_samples<MT, AcceleratedBilliardWalk>(P);

    NT frac_1 = 0.1, frac_2 = 0.5;
    bool score = perform_geweke<VT>(samples, frac_1, frac_2);

    std::cout<<"geweke = "<<score<<std::endl;
    CHECK(score == true);
}

template <typename NT>
void call_test_raftery(){
    typedef Cartesian<NT>    Kernel;
    typedef typename Kernel::Point    Point;
    typedef HPolytope<Point> Hpolytope;
    typedef Eigen::Matrix<NT,Eigen::Dynamic,Eigen::Dynamic> MT;
    typedef Eigen::Matrix<NT,Eigen::Dynamic,1> VT;
    Hpolytope P;
    unsigned int d = 10;

    std::cout << "--- Testing psrf on Billiard Walk and H-cube10" << std::endl;
    P = generate_cube<Hpolytope>(d, false);
    P.ComputeInnerBall();

    MT samples = get_samples<MT, AcceleratedBilliardWalk>(P);

    NT _q = 0.025;
    NT _r = 0.01;
    NT _s = 0.95;

    MT res = perform_raftery<VT>(samples, _q, _r, _s);

    std::cout<<"raftery walk length to get indepedent samples, 1st coordinate = "<<res(0,2)<<std::endl;
    CHECK(res(0,2) < 6);
}


TEST_CASE("psrf") {
    call_test_psrf<double>();
}

TEST_CASE("univariate_psrf") {
    call_test_univariate_psrf<double>();
}

TEST_CASE("interval_psrf") {
    call_test_interval_psrf<double>();
}

TEST_CASE("ess") {
    call_test_ess<double>();
}

TEST_CASE("geweke") {
    call_test_geweke<double>();
}

TEST_CASE("raftery") {
    call_test_raftery<double>();
}

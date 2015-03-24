/** @file genepool.h

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 3/22/2015</p>
*/

#ifndef GENEPOOL_H
#define GENEPOOL_H

#include <iostream>
#include <cstddef>
#include <vector>
#include <memory>

class Gene;

class GenePool
{
public:
    GenePool();
    ~GenePool();

    void dump(size_t count = 0, std::ostream& out = std::cout) const;

    // ---------------- control ----------------

    void initialize(const std::vector<Gene*>& genes);

    void evaluate();

    void nextGeneration();

    // ----------- selecting genes -------------

    const std::vector<std::shared_ptr<Gene>>& genes() const;

    Gene * getBest() const;

    /** Returns the best @p count number of individuals */
    std::vector<std::shared_ptr<Gene>> getBest(size_t count) const;

    /** Returns best of @p tries number of genes */
    Gene * getBestRandom(size_t tries) const;

    // ------ random functions for genes -------

    /** Returns random number in [0,1) */
    double rnd();
    double rnd(double mi, double ma);
    int rnd(int mi, int ma);
    bool rnd_prob(double prob);

private:

    struct Private;
    Private * p_;
};

#endif // GENEPOOL_H

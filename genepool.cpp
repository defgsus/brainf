/** @file genepool.cpp

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 3/22/2015</p>
*/

#include <vector>
#include <list>
#include <memory>
#include <random>
#include <algorithm>
#include <iomanip>
#include <chrono>

#include "genepool.h"
#include "gene.h"

struct GenePool::Private
{
    Private(GenePool * pool)
        : parent        (pool)
        , mt            (std::chrono::system_clock::now().time_since_epoch().count())
    {
    }

    GenePool * parent;
    std::vector<std::shared_ptr<Gene>> genes;
    std::mt19937_64 mt;
};



GenePool::GenePool()
    : p_    (new Private(this))
{
}

GenePool::~GenePool()
{
    delete p_;
}


void GenePool::dump(size_t count, std::ostream &out) const
{
    out << std::setw(5) << "gen."
        << " " << std::setw(16) << "eval"
        << " gene\n";
    auto list = getBest(p_->genes.size());
    size_t k=0;
    for (auto g : list)
    {
        out << std::setw(5) << g->generation()
            << " " << std::setw(16) << g->fitness()
            << " " << g->toString() << std::endl;
        if (count > 0 && ++k >= count)
            break;
    }
}



void GenePool::initialize(const std::vector<Gene*>& genes)
{
    p_->genes.clear();
    for (Gene * g : genes)
    {
        g->p_pool_ = this;
        g->p_gen_ = 0;
        g->initialize();
        // add to pool
        p_->genes.push_back(std::unique_ptr<Gene>(g));
    }
}

void GenePool::evaluate()
{
    for (auto & g : p_->genes)
        g->p_fit_ = g->evaluate();
}

void GenePool::nextGeneration()
{
    if (p_->genes.empty())
        return;

    const size_t
            num = p_->genes.size(),
    // number of individuals to reproduce
            num_rep = num / 3;

    auto all = genes();
    auto best = getBest(num_rep);

    p_->genes.clear();

    // copy the very best
    size_t num_cpy = std::min(best.size(), size_t(5));
    for (size_t i = 0; i<num_cpy; ++i)
        p_->genes.push_back(best[i]);

    // mutate the rest
    while (p_->genes.size() < num)
    {
        int i = rnd(0, int(best.size())-1);
        auto g = best[i]->clone();

        // cross-breed with someone
        if (rnd_prob(.03))
        {
#if 1
            int j = rnd(0, int(best.size())-1);
            g->cross(best[j].get());
#else
            int j = rnd(0, int(all.size())-1);
            g->cross(all[j].get());
#endif
        }
        else
        // mutate
        do
        {
            g->mutate(rnd(0.0001, 0.5),
                      rnd(0.0001, 0.1));
        } while (*g == best[i].get());

        ++g->p_gen_;
        p_->genes.push_back(std::shared_ptr<Gene>(g));
    }
}



Gene * GenePool::getBest() const
{
    if (p_->genes.empty())
        return 0;

    Gene * b = p_->genes[0].get();
    double f = b->fitness();
    for (auto & g : p_->genes)
    if (g->fitness() > f)
    {
        b = g.get();
        f = b->fitness();
    }

    return b;
}

const std::vector<std::shared_ptr<Gene>> & GenePool::genes() const
{
    return p_->genes;
}

std::vector<std::shared_ptr<Gene>> GenePool::getBest(size_t count) const
{
    std::vector<std::shared_ptr<Gene>> best = genes();

    std::stable_sort(best.begin(), best.end(),
              [=](const std::shared_ptr<Gene>& l, const std::shared_ptr<Gene>& r)
    {
        return l->fitness() > r->fitness();
    });

    best.resize(std::min(best.size(), count));
    return best;
}







double GenePool::rnd()
{
    return double(p_->mt()) / p_->mt.max();
}

double GenePool::rnd(double mi, double ma)
{
    return mi + rnd() / std::max(0.0000000001, ma - mi);
}

int GenePool::rnd(int mi, int ma)
{
    const int mo = std::max(1, ma - mi + 1);
    return mi + int(uint64_t(p_->mt()) % uint64_t(mo));
}

bool GenePool::rnd_prob(double prob)
{
    return rnd() < prob;
}

/** @file gene.h

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 3/22/2015</p>
*/

#ifndef GENE_H
#define GENE_H

#include <cstddef>
#include <string>

class GenePool;

class Gene
{
    friend class GenePool;

public:
    Gene();
    virtual ~Gene() { }

    size_t generation() const { return p_gen_; }
    double fitness() const { return p_fit_; }

    GenePool * pool() const { return p_pool_; }

    // ---------- virtual interface --------

    /** Return an exact copy as new instance */
    virtual Gene * clone() const = 0;

    virtual std::string toString() const = 0;

    virtual void initialize() = 0;
    virtual bool operator == (const Gene * other) const = 0;

    virtual void mutate(double amt, double prob) = 0;
    virtual void cross(const Gene * other) = 0;

    virtual double evaluate() = 0;


private:

    GenePool * p_pool_;
    size_t p_gen_;
    double p_fit_;

};

#endif // GENE_H

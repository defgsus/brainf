/** @file brainfgene.h

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 3/22/2015</p>
*/

#ifndef BRAINFGENE_H
#define BRAINFGENE_H

#include <list>

#include "gene.h"
#include "brainf.h"

class BrainfGene : public Gene
{
public:
    BrainfGene();

    // ---------- virtual interface --------

    BrainfGene * clone() const;

    std::string toString() const override;

    void initialize() override;
    bool operator == (const Gene * other) const override;

    void mutate(double amt, double prob) override;
    void cross(const Gene * other) override;

    double evaluate() override;

    /** Create a brainfuck interpreter with current code */
    Brainf_uint8 getBrainf() const;

private:

    BrainfOpcode rndOpcode_(bool includeLoops);
    void addOpcode_();
    void removeOpcode_();
    void insert_(size_t pos, BrainfOpcode op);

    void simplify_(std::vector<BrainfOpcode>&);

    static double strCompare_(const std::string& a, const std::string& b);

    std::vector<BrainfOpcode> code_;

    bool enableLoops_;
};

#endif // BRAINFGENE_H

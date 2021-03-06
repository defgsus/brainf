/** @file brainfgene.cpp

    @brief

    <p>(c) 2015, stefan.berke@modular-audio-graphics.com</p>
    <p>All rights reserved</p>

    <p>created 3/22/2015</p>
*/

#include <cmath>

#include "brainfgene.h"
#include "genepool.h"

#define MAX_STEPS 500

BrainfGene::BrainfGene()
    : enableLoops_      (true)
{
}

BrainfGene * BrainfGene::clone() const
{
    auto g = new BrainfGene(*this);
    return g;
}

std::string BrainfGene::toString() const
{
    auto bf = getBrainf();
    bf.run(MAX_STEPS);
    return
            "{" + bf.outputString(true) + "} "
            + bf.codeString()
            ;
}

void BrainfGene::initialize()
{
    code_.clear();

    const int si = pool()->rnd(4, 10);
    for (int i=0; i<si; ++i)
        code_.push_back(rndOpcode_(false));
}

bool BrainfGene::operator == (const Gene * o) const
{
    const BrainfGene * other = dynamic_cast<const BrainfGene*>(o);
    if (!other)
        return false;
    if (code_.size() != other->code_.size())
        return false;
    for (size_t i=0; i<code_.size(); ++i)
        if (code_[i] != other->code_[i])
            return false;
    return true;
}

void BrainfGene::mutate(double amt, double prob)
{
    size_t num = amt * pool()->rnd(0, 10);

    if (pool()->rnd_prob(prob))
        for (size_t i=0; i<num; ++i)
            if (pool()->rnd_prob(prob))
                removeOpcode_();

    if (pool()->rnd_prob(prob))
        for (size_t i=0; i<num; ++i)
            if (pool()->rnd_prob(prob))
                addOpcode_();

    if (code_.empty())
        addOpcode_();

    // change some opcodes (except loop brackets)

    //double prob1 = prob / code_.size();
    num = amt * pool()->rnd(0, 10);
    for (size_t j = 0; j<num; ++j)
    if (pool()->rnd_prob(prob))
    {
        size_t i = pool()->rnd(0, int(code_.size()) - 1);
        if (   code_[i] != BFO_BEGIN
            && code_[i] != BFO_END)
            code_[i] = rndOpcode_(false);
    }

    simplify_(code_);
}

void BrainfGene::cross(const Gene * o)
{
    const BrainfGene * other = dynamic_cast<const BrainfGene*>(o);
    if (!other)
        return;

    // concat two code pieces
    const int
            x0 = pool()->rnd(1, int(code_.size()*2/3)),
            x1 = pool()->rnd(int(other->code_.size()/3), int(other->code_.size()));

    /// @todo This does not check for matching loop brackets
    code_.resize(x0);
    for (size_t i = x1; i < other->code_.size(); ++i)
        code_.push_back(other->code_[i]);

    simplify_(code_);
}

void BrainfGene::simplify_(std::vector<BrainfOpcode>& code)
{
    if (code.empty())
        return;

    // remove leading tape moves
    int i=0;
    while (i < (int)code.size() &&
           (code[i] == BFO_LEFT || code[i] == BFO_RIGHT))
        ++i;
    if (i>0)
        code.erase(code.begin(), code.begin() + i);

    if (code.size() < 2)
        return;

    // keep everything up to last '.'
    i = code.size() - 1;
    for (; i > 0; --i)
        if (code[i] == BFO_OUT)
            break;
    if (i > 0)
    {
        // but include trailing ']'
        int j=i;
        for (; j<int(code.size()); ++j)
            if (code[j] == BFO_END)
                i = j;

        code.resize(std::min(code.size(), size_t(i + 1)));
    }

    if (code.size() < 2)
        return;

    for (size_t i = 0; i < code.size(); )
    {
        // remove obvious two-char noops
        if (i < code.size() - 1)
        {
            auto nextop = code[i+1];
            if ((code[i] == BFO_INC && nextop == BFO_DEC)
             || (code[i] == BFO_DEC && nextop == BFO_INC)
             || (code[i] == BFO_RIGHT && nextop == BFO_LEFT)
             || (code[i] == BFO_LEFT && nextop == BFO_RIGHT)
             || (code[i] == BFO_BEGIN && nextop == BFO_END)
                )
            {
                code.erase(code.begin() + i, code.begin() + i + 2);
                continue;
            }
        }

        ++i;
    }
}


double BrainfGene::evaluate()
{
#if 1
    auto bf = getBrainf();
    bf.run(MAX_STEPS);

    const std::string str = bf.outputString();
    //double f = strCompare_(str, "Hello, world!");
    //double f = strCompare_(str, "Hello, hello, hello, world!");
    //double f = strCompare_(str, "abcabcabcabcabc");
    //double f = strCompare_(str, "0123456789");
    double f = strCompare_(str, "brainf***");
    //double f = strCompare_(str, "words words words");
    //double f = strCompare_(str, "Hello, world! brainfuck autogenerated code rulez!");
#else
    auto bf = getBrainf(), bf2 = getBrainf();
    std::vector<unsigned char> inp;
    for (int i=1; i<20; ++i)
        inp.push_back(i * 11);
    bf2.setInput(inp);
    bf.run(MAX_STEPS);
    bf2.run(MAX_STEPS);
    double f =   strCompare_(bf.toString(), "Hello, world!")
               * strCompare_(bf2.toString(), "brainf***");
    //f /= 2.;
#endif
    // size penalty
    //f /= (1. + 0.01 * std::max(0, int(code_.size()) - 20));
    //f /= (1. + 0.002 * code_.size());
    return f * 100.;
}

Brainf_uint8 BrainfGene::getBrainf() const
{
    std::vector<unsigned char> inp;
    for (int i=1; i<20; ++i)
        inp.push_back(i * 7);

    Brainf_uint8 bf;
    bf.setCode(code_);
    bf.setInput(inp);
    return bf;
}

double BrainfGene::strCompare_(const std::string &a, const std::string &b)
{
    double diff = 0.;

#if 1
    const size_t
            minsize = std::min(a.size(), b.size()),
            maxsize = std::max(a.size(), b.size());

    for (size_t i=0; i<minsize; ++i)
    {
        diff += std::abs(double(a[i]) - double(b[i])) / (256. + 5. * i);
    }
    diff += maxsize - minsize;
#else
    const size_t maxsize = std::max(a.size(), b.size());

    for (size_t i=0; i<maxsize; ++i)
    {
        double ai = i < a.size() ? (unsigned char)a[i] : 255.,
               bi = i < b.size() ? (unsigned char)b[i] : 255.;
        diff += std::abs(ai - bi) / (256. + 5. * i);
    }

#endif

    return 1. / (1. + diff * diff);
}


BrainfOpcode BrainfGene::rndOpcode_(bool includeLoops)
{
    if (includeLoops)
        return BrainfOpcode(pool()->rnd(BFO_LEFT, BFO_END));
    else
        return BrainfOpcode(pool()->rnd(BFO_LEFT, BFO_OUT));
}

void BrainfGene::addOpcode_()
{
    int pos = pool()->rnd(0, code_.size());
    BrainfOpcode op = rndOpcode_(enableLoops_);

    if (op == BFO_BEGIN || op == BFO_END)
    {
        /// @todo quite unsmart insert of a loop here
        insert_(pos, BFO_END);
        // left,right,inc,dec,in (are all valid loop enders)
        insert_(pos, BrainfOpcode(pool()->rnd(BFO_LEFT, BFO_IN)));
        insert_(pos, BFO_BEGIN);
    }
    else
        insert_(pos, op);
}

void BrainfGene::removeOpcode_()
{
    if (code_.size() < 2)
        return;

    int pos = pool()->rnd(0, code_.size()-1);
    if (code_[pos] != BFO_BEGIN
     && code_[pos] != BFO_END)
        code_.erase(code_.begin() + pos);
    // remove begin and end statement
    else if (code_[pos] == BFO_BEGIN)
    {
//        std::cout << "rem [ " << pos << std::endl;
        code_.erase(code_.begin() + pos);

        // find end bracket
        size_t i = pos;
        int lvl = 1;
        while (i < code_.size())
        {
            if (code_[i] == BFO_BEGIN)
                ++lvl;
            else
            if (code_[i] == BFO_END && --lvl == 0)
            {
                code_.erase(code_.begin() + i);
                return;
            }
            ++i;
        }
//        std::cout << "not found ]" << std::endl;
    }
    else if (code_[pos] == BFO_END)
    {
//        std::cout << "rem ] " << pos << std::endl;
        code_.erase(code_.begin() + pos);

        // find start bracket
        int i = pos, lvl = 1;
        while (i >= 0)
        {
            --i;
            if (code_[i] == BFO_END)
                ++lvl;
            else
            if (code_[i] == BFO_BEGIN && (--lvl == 0))
            {
                code_.erase(code_.begin() + i);
                return;
            }
        }
//        std::cout << "not found [" << std::endl;
    }
}

void BrainfGene::insert_(size_t pos, BrainfOpcode op)
{
    if (pos >= code_.size())
        code_.push_back(op);
    else
        code_.insert(code_.begin() + pos, op);
}

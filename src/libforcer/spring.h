#pragma once

#include "iforcer.h"

namespace sdfibm::forcer {

class Spring : public IForcer, _creator<Spring>
{
public:
    virtual Force generate(const scalar&, const vector&, const vector&, const quaternion&, const vector&) override final;

    Spring(const dictionary& para)
    {
        pivot = para.lookup("pivot");
        try {
            k = Foam::readScalar(para.lookup("k"));
            l = Foam::readScalar(para.lookup("l"));
        } catch (const std::exception& e) {
            std::cout << "Problem in creating Spring force" << ' '
                      << e.what() << std::endl;
            std::exit(1);
        }
    }
    virtual ~Spring() override final {}
    TYPENAME("Spring")
    virtual std::string description() const override {return "Spring forcer with a pivot, stiffness (k), and rest length (l)";}
private:
    vector pivot; 
    scalar k;
    scalar l;
};

IForcer::Force Spring::generate(
        const scalar& time,
        const vector& position,
        const vector& velocity,
        const quaternion& orientation,
        const vector& omega
)
{
    vector r = position - pivot;
    auto force {vector::zero};
    if (mag(r) > SMALL)
        force = -k * r * (1.0 - l/mag(r));
    return {force, vector::zero};
}

}
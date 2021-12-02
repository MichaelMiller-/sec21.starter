#pragma once

#include <QAbstractTransition>

template <typename Event, typename Compare = std::equal_to<>>
class GenericTransition : public QAbstractTransition
{
    using value_t = typename Event::value_t;

    const value_t value;
    const Compare compare{};
public:
    GenericTransition(value_t v) : value{std::move(v)} {}

protected:
    bool eventTest(QEvent *e) override
    {
        if (e->type() != Event::type_v)
            return false;
        return compare(value, static_cast<Event*>(e)->value);
    }
    void onTransition(QEvent *) override {}
};

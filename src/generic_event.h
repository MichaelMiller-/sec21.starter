#pragma once

#include <QEvent>

template <typename T, auto I>
struct GenericEvent : public QEvent
{
    using value_t = T;

    static constexpr auto id_v = QEvent::User + I;
    static constexpr auto type_v = QEvent::Type(QEvent::User + I);

    T value;

    GenericEvent(T v) : QEvent{type_v}, value{std::move(v)} {}
};

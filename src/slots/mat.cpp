#include "mat.hpp"
namespace dt::df::plugin::opencv
{
void MatSlot::accept(const BaseSlot *slot)
{
    auto input_slot = dynamic_cast<const MatSlot *>(slot);
    if (input_slot)
    {
        accept(input_slot->value());
    }
}

void MatSlot::accept(cv::Mat *val)
{
    mat_ = val;
    valueChanged();
}

cv::Mat *MatSlot::value() const
{
    return mat_;
}

bool MatSlot::canConnect(const BaseSlot *const slot) const
{
    return dynamic_cast<const MatSlot *const>(slot);
}

void MatSlot::connectEvent()
{
    BaseSlot::connectEvent();
    valueChanged();
}
} // namespace dt::df::plugin::opencv

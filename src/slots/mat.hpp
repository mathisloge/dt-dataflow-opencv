#pragma once
#include <dt/df/core/base_slot.hpp>
#include <dt/df/core/graph_manager.hpp>
#include <opencv2/core.hpp>
namespace dt::df::plugin::opencv
{
class MatSlot : public BaseSlot
{
  public:
    static constexpr std::string_view kKey = "OpenCvMatSlot";
    using BaseSlot::BaseSlot;
    void accept(const BaseSlot *slot) override;
    void accept(cv::Mat *);
    cv::Mat *value() const;
    bool canConnect(const BaseSlot *const slot) const override;
    void connectEvent() override;

  private:
    cv::Mat *mat_ = nullptr;
};
} // namespace dt::df::plugin::opencv

#pragma once
#include <dt/df/core/base_node.hpp>
#include <dt/df/core/graph_manager.hpp>
#include <opencv2/core.hpp>
namespace dt::df::plugin::opencv
{
class MatOutputNode final : public BaseNode
{
  public:
    static constexpr const char *kKey = "OpenCVMatOutputNode";
    static constexpr const char *kName = "OpenCV Mat";

  public:
    MatOutputNode(IGraphManager &graph_manager);
    MatOutputNode(IGraphManager &graph_manager, const nlohmann::json &json);

  private:
    void initSlots();
    static Slots CreateOutputs(IGraphManager &graph_manager);

  private:
    cv::Mat mat_;
};
} // namespace dt::df::plugin::opencv

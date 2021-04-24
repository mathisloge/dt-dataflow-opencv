#pragma once
#include <dt/df/core/base_node.hpp>
#include <dt/df/core/graph_manager.hpp>
#include <opencv2/core.hpp>
namespace dt::df::plugin::opencv
{
class MatCopyNode final : public BaseNode
{
  public:
    static constexpr const char *kKey = "OpenCVMatCopyNode";
    static constexpr const char *kName = "OpenCV copy Mat";

  public:
    MatCopyNode(IGraphManager &graph_manager);
    MatCopyNode(IGraphManager &graph_manager, const nlohmann::json &json);
    void calculate() override;
  private:
    void initSlots();
    static Slots CreateInputs(IGraphManager &graph_manager);
    static Slots CreateOutputs(IGraphManager &graph_manager);

  private:
    cv::Mat mat_;
};
} // namespace dt::df::plugin::opencv

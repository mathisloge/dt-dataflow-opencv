#pragma once
#include <mutex>
#include <dt/df/core/base_node.hpp>
#include <dt/df/core/graph_manager.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>

namespace dt::df::plugin::opencv
{
class MatDisplayNode final : public BaseNode
{
  public:
    static constexpr const char *kKey = "OpenCVMatDisplayNode";
    static constexpr const char *kName = "OpenCV display Mat";

  public:
    MatDisplayNode(IGraphManager &graph_manager);
    MatDisplayNode(IGraphManager &graph_manager, const nlohmann::json &json);

  private:
    void initSlots();
    void renderCustomContent() override;
    static Slots CreateInputs(IGraphManager &graph_manager);

  private:
    cv::Mat *mat_;
    std::mutex tex_mtx_;
    bool mat_changed_;
    cv::ogl::Texture2D texture_;
};
} // namespace dt::df::plugin::opencv

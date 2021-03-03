#pragma once
#include <map>
#include <dt/df/core/base_node.hpp>
#include <dt/df/core/graph_manager.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "../slots/mat.hpp"
namespace dt::df::plugin::opencv
{
class ColorCvtNode final : public BaseNode
{
  public:
    static constexpr const char *kKey = "OpenCVColorCvtNode";
    static constexpr const char *kName = "OpenCV color convert";

  public:
    ColorCvtNode(IGraphManager &graph_manager);
    ColorCvtNode(IGraphManager &graph_manager, const nlohmann::json &json);

  private:
    void initSlots();
    void renderCustomContent() override;
    static Slots CreateInputs(IGraphManager &graph_manager);
    static Slots CreateOutputs(IGraphManager &graph_manager);

  private:
    cv::Mat *mat_;
    std::shared_ptr<MatSlot> output_slot_;
    static const std::map<cv::ColorConversionCodes, std::string_view> kColorVals;
    cv::ColorConversionCodes selected_color_;
};
} // namespace dt::df::plugin::opencv

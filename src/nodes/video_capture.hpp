#pragma once
#include <thread>
#include <dt/df/core/base_node.hpp>
#include <dt/df/core/graph_manager.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include "../slots/mat.hpp"
namespace dt::df::plugin::opencv
{
class VideoCaptureNode final : public BaseNode
{
  public:
    static constexpr const char *kKey = "OpenCVVideoCaptureNode";
    static constexpr const char *kName = "OpenCV VideoCapture";

  public:
    VideoCaptureNode(IGraphManager &graph_manager);
    VideoCaptureNode(IGraphManager &graph_manager, const nlohmann::json &json);
    ~VideoCaptureNode();

  private:
    void initSlots();
    void ioFnc();
    static Slots CreateInputs(IGraphManager &graph_manager);
    static Slots CreateOutputs(IGraphManager &graph_manager);

  private:
    cv::Mat *mat_;
    int device_id_;
    int api_id_;
    std::string file_name_;
    bool use_device_; //! if false, file will be used
    bool input_has_changed_;
    cv::VideoCapture cap_;
    bool should_capture_;
    std::thread cap_thread_;

    std::shared_ptr<MatSlot> mat_out_slot_;
};
} // namespace dt::df::plugin::opencv

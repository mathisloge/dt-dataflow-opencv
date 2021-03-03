#pragma once
#include <thread>
#include <dt/df/core/base_node.hpp>
#include <dt/df/core/graph_manager.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include "../bounded_buffer.hpp"
#include "../slots/mat.hpp"
namespace dt::df::plugin::opencv
{
class VideoWriterNode final : public BaseNode
{
  public:
    static constexpr const char *kKey = "OpenCVVideoWriterNode";
    static constexpr const char *kName = "OpenCV Video writer";

  public:
    VideoWriterNode(IGraphManager &graph_manager);
    VideoWriterNode(IGraphManager &graph_manager, const nlohmann::json &json);
    ~VideoWriterNode();

  private:
    void initSlots();
    void ioFnc();
    static Slots CreateInputs(IGraphManager &graph_manager);
    static Slots CreateOutputs(IGraphManager &graph_manager);

  private:
    std::mutex file_mtx_;
    std::string file_name_;
    bool file_changed_;
    cv::VideoWriter writer_;
    bool should_capture_;
    bool should_write_;
    bounded_buffer<cv::Mat> buffer_;
    std::thread cap_thread_;
    char fourcc_[4];
    double fps_;
    cv::Size frame_size_;
    bool is_color_;
};
} // namespace dt::df::plugin::opencv

#include "video_writer.hpp"
#include <iostream>
#include <dt/df/core/number_slot.hpp>
#include <dt/df/core/string_slot.hpp>
#include "../slots/mat.hpp"
namespace dt::df::plugin::opencv
{
VideoWriterNode::VideoWriterNode(IGraphManager &graph_manager)
    : BaseNode{graph_manager, kKey, kName, CreateInputs(graph_manager), CreateOutputs(graph_manager)}
    , file_changed_{false}
    , should_capture_{true}
    , should_write_{true}
    , buffer_{100}
    , cap_thread_{std::bind(&VideoWriterNode::ioFnc, this)}
{
    initSlots();
}
VideoWriterNode::VideoWriterNode(IGraphManager &graph_manager, const nlohmann::json &json)
    : BaseNode{graph_manager, json}
    , file_changed_{false}
    , should_capture_{true}
    , should_write_{true}
    , buffer_{10}
    , cap_thread_{std::bind(&VideoWriterNode::ioFnc, this)}
{
    initSlots();
}

void VideoWriterNode::initSlots()
{
    inputByLocalId(0)->subscribe([this](const BaseSlot *slot) {
        auto mat = static_cast<const MatSlot *>(slot)->value();
        // std::cout << "got frame1" << std::endl;
        if (should_capture_ && should_write_ && writer_.isOpened())
        {
            // std::cout << "got frame2" << std::endl;
            buffer_.push_front(*mat);
        }
    });
    inputByLocalId(1)->subscribe([this](const BaseSlot *slot) {
        const auto new_file = static_cast<const StringSlot *>(slot)->value();
        std::unique_lock<std::mutex> l{file_mtx_};
        if (new_file != file_name_)
        {
            file_name_ = new_file;
            file_changed_ = true;
            buffer_.push_front(cv::Mat{}); // push to unlock task
        }
    });

    inputByLocalId(2)->subscribe([this](const BaseSlot *slot) {
        should_write_ = static_cast<bool>(static_cast<const NumberSlot *>(slot)->value());
    });

    inputByLocalId(3)->subscribe([this](const BaseSlot *slot) {
        auto fourcc_str = static_cast<const StringSlot *>(slot)->value();
        std::unique_lock<std::mutex> l{file_mtx_};
        std::cout << "got fourcc : " << fourcc_str.size() << " " << fourcc_str << std::endl;
        if (fourcc_str.size() >= 4 && (fourcc_[0] != fourcc_str[0] || fourcc_[1] != fourcc_str[1] ||
                                       fourcc_[2] != fourcc_str[2] || fourcc_[3] != fourcc_str[3]))
        {
            fourcc_[0] = fourcc_str[0];
            fourcc_[1] = fourcc_str[1];
            fourcc_[2] = fourcc_str[2];
            fourcc_[3] = fourcc_str[3];
        }
        file_changed_ = true;
        buffer_.push_front(cv::Mat{}); // push to unlock task
    });

    inputByLocalId(4)->subscribe([this](const BaseSlot *slot) {
        const auto new_fps = static_cast<const NumberSlot *>(slot)->value();
        // std::cout << "got fps1 : " << new_fps << std::endl;
        if (std::abs(new_fps - fps_) > std::numeric_limits<double>::epsilon())
        {
            // std::cout << "got fps2 : " << new_fps << std::endl;
            std::unique_lock<std::mutex> l{file_mtx_};
            fps_ = new_fps;
            file_changed_ = true;
            buffer_.push_front(cv::Mat{}); // push to unlock task
        }
    });

    inputByLocalId(5)->subscribe([this](const BaseSlot *slot) {
        const auto val = static_cast<const NumberSlot *>(slot)->value();
        if (val != frame_size_.width)
        {
            std::unique_lock<std::mutex> l{file_mtx_};
            frame_size_.width = val;
            file_changed_ = true;
            buffer_.push_front(cv::Mat{}); // push to unlock task
        }
    });

    inputByLocalId(6)->subscribe([this](const BaseSlot *slot) {
        const auto val = static_cast<const NumberSlot *>(slot)->value();
        if (val != frame_size_.height)
        {
            std::unique_lock<std::mutex> l{file_mtx_};
            frame_size_.height = val;
            file_changed_ = true;
            buffer_.push_front(cv::Mat{}); // push to unlock task
        }
    });
    inputByLocalId(7)->subscribe([this](const BaseSlot *slot) {
        const auto val = static_cast<bool>(static_cast<const NumberSlot *>(slot)->value());
        if (val != is_color_)
        {
            std::unique_lock<std::mutex> l{file_mtx_};
            is_color_ = val;
            file_changed_ = true;
            buffer_.push_front(cv::Mat{}); // push to unlock task
        }
    });
}

void VideoWriterNode::calculate()
{}

void VideoWriterNode::ioFnc()
{
    while (should_capture_)
    {
        if (!should_write_) //! \todo change to conditional variable
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }
        if (file_changed_)
        {
            std::unique_lock<std::mutex> l{file_mtx_};
            file_changed_ = false;
            if (!file_name_.empty())
            {
                const int fourcc_code = cv::VideoWriter::fourcc(fourcc_[0], fourcc_[1], fourcc_[2], fourcc_[3]);
                std::cout << "fourcc " << fourcc_code << std::endl;
                const bool opened = writer_.open(file_name_, fourcc_code, fps_, frame_size_, is_color_);
                std::cout << "opened : " << opened << std::endl;
            }
        }

        if (!file_changed_ && should_capture_ && should_write_ && writer_.isOpened())
        {
            cv::Mat mat;
            buffer_.pop_back(&mat);
            if (!mat.empty())
            {
                // std::cout << "write" << std::endl;
                writer_.write(mat);
            }
        }
    }
    if (writer_.isOpened())
        writer_.release();
    std::cout << "endig write thread" << std::endl;
}

VideoWriterNode::~VideoWriterNode()
{
    should_capture_ = false;
    buffer_.push_front(cv::Mat{}); // push to unlock task
    if (cap_thread_.joinable())
        cap_thread_.join();
}

Slots VideoWriterNode::CreateInputs(IGraphManager &graph_manager)
{
    try
    {
        const auto &mat_slot_fac = graph_manager.getSlotFactory("OpenCvMatSlot");
        const auto &str_slot_fac = graph_manager.getSlotFactory("StringSlot");
        const auto &int_slot_fac = graph_manager.getSlotFactory("IntSlot");
        const auto &bool_slot_fac = graph_manager.getSlotFactory("BoolSlot");
        const auto &floating_slot_fac = graph_manager.getSlotFactory("FloatingSlot");

        return Slots{
            mat_slot_fac(graph_manager, SlotType::input, "mat", 0, SlotFieldVisibility::without_connection),
            str_slot_fac(graph_manager, SlotType::input, "file", 1, SlotFieldVisibility::without_connection),
            bool_slot_fac(graph_manager, SlotType::input, "write", 2, SlotFieldVisibility::without_connection),
            str_slot_fac(graph_manager, SlotType::input, "fourcc", 3, SlotFieldVisibility::without_connection),
            floating_slot_fac(graph_manager, SlotType::input, "fps", 4, SlotFieldVisibility::without_connection),
            int_slot_fac(graph_manager, SlotType::input, "frame width", 5, SlotFieldVisibility::without_connection),
            int_slot_fac(graph_manager, SlotType::input, "frame height", 6, SlotFieldVisibility::without_connection),
            bool_slot_fac(graph_manager, SlotType::input, "is color", 7, SlotFieldVisibility::without_connection)};
    }
    catch (...)
    {}
    return Slots{};
}

Slots VideoWriterNode::CreateOutputs(IGraphManager &graph_manager)
{
    return Slots{};
}

} // namespace dt::df::plugin::opencv

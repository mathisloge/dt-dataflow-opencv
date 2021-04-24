#include "video_capture.hpp"
#include <iostream>
#include <dt/df/core/number_slot.hpp>
#include <dt/df/core/string_slot.hpp>
#include "../slots/mat.hpp"
namespace dt::df::plugin::opencv
{
VideoCaptureNode::VideoCaptureNode(IGraphManager &graph_manager)
    : BaseNode{graph_manager, kKey, kName, CreateInputs(graph_manager), CreateOutputs(graph_manager)}
    , mat_{nullptr}
    , device_id_{0}
    , api_id_{cv::CAP_ANY}
    , use_device_{false}
    , should_capture_{true}
    , input_has_changed_{false}
    , cap_thread_{std::bind(&VideoCaptureNode::ioFnc, this)}
{
    initSlots();
}
VideoCaptureNode::VideoCaptureNode(IGraphManager &graph_manager, const nlohmann::json &json)
    : BaseNode{graph_manager, json}
    , mat_{nullptr}
    , device_id_{0}
    , api_id_{cv::CAP_ANY}
    , use_device_{false}
    , should_capture_{true}
    , input_has_changed_{false}
    , cap_thread_{std::bind(&VideoCaptureNode::ioFnc, this)}
{
    initSlots();
}

void VideoCaptureNode::initSlots()
{
    std::static_pointer_cast<MatSlot>(inputByLocalId(0))->subscribe([this](const BaseSlot *slot) {
        mat_ = static_cast<const MatSlot *>(slot)->value();
    });
    std::static_pointer_cast<NumberSlot>(inputByLocalId(1))->subscribe([this](const BaseSlot *slot) {
        device_id_ = static_cast<const NumberSlot *>(slot)->value();
        if (use_device_)
            input_has_changed_ = true;
    });
    std::static_pointer_cast<NumberSlot>(inputByLocalId(2))->subscribe([this](const BaseSlot *slot) {
        api_id_ = static_cast<const NumberSlot *>(slot)->value();
        if (use_device_)
            input_has_changed_ = true;
    });
    std::static_pointer_cast<StringSlot>(inputByLocalId(3))->subscribe([this](const BaseSlot *slot) {
        file_name_ = static_cast<const StringSlot *>(slot)->value();
        if (!use_device_)
            input_has_changed_ = true;
    });
    std::static_pointer_cast<NumberSlot>(inputByLocalId(4))->subscribe([this](const BaseSlot *slot) {
        use_device_ = static_cast<const NumberSlot *>(slot)->value();
        input_has_changed_ = true;
    });
    mat_out_slot_ = std::static_pointer_cast<MatSlot>(outputByLocalId(0));
    fps_out_slot_ = std::static_pointer_cast<NumberSlot>(outputByLocalId(1));
    width_out_slot_ = std::static_pointer_cast<NumberSlot>(outputByLocalId(2));
    height_out_slot_ = std::static_pointer_cast<NumberSlot>(outputByLocalId(3));
}

void VideoCaptureNode::ioFnc()
{
    double rate = 0;
    using Clock = std::chrono::high_resolution_clock;
    while (should_capture_)
    {
        if (input_has_changed_)
        {
            if (use_device_)
                cap_.open(device_id_, api_id_);
            else
                cap_.open(file_name_, api_id_);
            input_has_changed_ = false;
        }
        if (mat_)
        {

            const auto ts = Clock::now();
            if (cap_.read(*mat_) && !mat_->empty())
            {
                rate = cap_.get(cv::CAP_PROP_FPS);
                mat_out_slot_->accept(mat_);

                const auto current_frame = cap_.get(cv::CAP_PROP_POS_FRAMES);
                const auto current_time = cap_.get(cv::CAP_PROP_POS_MSEC);
                const auto fps = current_frame / (current_time / 1e3);

                fps_out_slot_->setValue(fps);
                width_out_slot_->setValue(mat_->size().width);
                height_out_slot_->setValue(mat_->size().height);

                std::this_thread::sleep_until(ts + std::chrono::nanoseconds(static_cast<int>((1. / fps) * 1e9)));
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void VideoCaptureNode::calculate()
{}

VideoCaptureNode::~VideoCaptureNode()
{
    should_capture_ = false;
    if (cap_thread_.joinable())
        cap_thread_.join();
}

Slots VideoCaptureNode::CreateInputs(IGraphManager &graph_manager)
{
    try
    {
        const auto &int_slot_fac = graph_manager.getSlotFactory("IntSlot");
        const auto &bool_slot_fac = graph_manager.getSlotFactory("BoolSlot");
        const auto &str_slot_fac = graph_manager.getSlotFactory("StringSlot");
        const auto &mat_slot_fac = graph_manager.getSlotFactory("OpenCvMatSlot");
        return Slots{
            mat_slot_fac(graph_manager, SlotType::input, "mat", 0, SlotFieldVisibility::without_connection),
            int_slot_fac(graph_manager, SlotType::input, "device", 1, SlotFieldVisibility::without_connection),
            int_slot_fac(graph_manager, SlotType::input, "device api", 2, SlotFieldVisibility::without_connection),
            str_slot_fac(graph_manager, SlotType::input, "file", 3, SlotFieldVisibility::without_connection),
            bool_slot_fac(graph_manager, SlotType::input, "use device", 4, SlotFieldVisibility::without_connection)};
    }
    catch (...)
    {}
    return Slots{};
}

Slots VideoCaptureNode::CreateOutputs(IGraphManager &graph_manager)
{
    try
    {
        const auto &mat_slot_fac = graph_manager.getSlotFactory("OpenCvMatSlot");
        const auto &int_slot_fac = graph_manager.getSlotFactory("IntSlot");
        const auto &float_slot_fac = graph_manager.getSlotFactory("FloatingSlot");
        return Slots{
            mat_slot_fac(graph_manager, SlotType::output, "mat", 0, SlotFieldVisibility::never),
            float_slot_fac(graph_manager, SlotType::output, "fps", 1, SlotFieldVisibility::never),
            int_slot_fac(graph_manager, SlotType::output, "width", 2, SlotFieldVisibility::never),
            int_slot_fac(graph_manager, SlotType::output, "height", 3, SlotFieldVisibility::without_connection)};
    }
    catch (...)
    {}
    return Slots{};
}

} // namespace dt::df::plugin::opencv

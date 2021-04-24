#include "color_cvt.hpp"
#include <imgui.h>

#include "../slots/mat.hpp"
namespace dt::df::plugin::opencv
{
ColorCvtNode::ColorCvtNode(IGraphManager &graph_manager)
    : BaseNode{graph_manager, kKey, kName, Slots{CreateInputs(graph_manager)}, Slots{CreateOutputs(graph_manager)}}
{
    initSlots();
}
ColorCvtNode::ColorCvtNode(IGraphManager &graph_manager, const nlohmann::json &json)
    : BaseNode{graph_manager, json}
{
    initSlots();
}

void ColorCvtNode::calculate()
{
    auto in_mat = std::static_pointer_cast<MatSlot>(inputByLocalId(0))->value();
    if (in_mat && !in_mat->empty())
    {
        const auto prev_it = kColorVals.find(selected_color_);
        const cv::ColorConversionCodes color_conv_code{prev_it != kColorVals.end() ? prev_it->first
                                                                                   : cv::COLOR_BGR2RGB};
        cv::cvtColor(*in_mat, *in_mat, color_conv_code);
        output_slot_->accept(in_mat);
    }
}

void ColorCvtNode::renderCustomContent()
{
    ImGui::PushID(this);
    auto prev_it = kColorVals.find(selected_color_);
    ImGui::SetNextItemWidth(120.f);
    if (ImGui::BeginCombo("Color convert", prev_it != kColorVals.end() ? prev_it->second.data() : ""))
    {
        for (const auto &color_val : kColorVals)
        {
            const bool is_selected = (selected_color_ == color_val.first);
            if (ImGui::Selectable(color_val.second.data(), is_selected))
                selected_color_ = color_val.first;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::PopID();
}

void ColorCvtNode::initSlots()
{
    selected_color_ = cv::COLOR_BGR2RGB;
    output_slot_ = std::static_pointer_cast<MatSlot>(outputByLocalId(0));
}

Slots ColorCvtNode::CreateInputs(IGraphManager &graph_manager)
{
    try
    {
        const auto &mat_slot_fac = graph_manager.getSlotFactory("OpenCvMatSlot");
        return Slots{mat_slot_fac(graph_manager, SlotType::input, "mat", 0, SlotFieldVisibility::never)};
    }
    catch (...)
    {}
    return Slots{};
}

Slots ColorCvtNode::CreateOutputs(IGraphManager &graph_manager)
{
    try
    {
        const auto &mat_slot_fac = graph_manager.getSlotFactory("OpenCvMatSlot");
        return Slots{mat_slot_fac(graph_manager, SlotType::output, "mat", 0, SlotFieldVisibility::never)};
    }
    catch (...)
    {}
    return Slots{};
}

const std::map<cv::ColorConversionCodes, std::string_view> ColorCvtNode::kColorVals = {
    {cv::COLOR_BGR2RGB, "BGR2RGB"}, {cv::COLOR_BGR2RGBA, "BGR2RGBA"}, {cv::COLOR_BGR2GRAY, "COLOR_BGR2GRAY"}};

} // namespace dt::df::plugin::opencv

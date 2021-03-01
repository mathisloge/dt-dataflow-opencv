#include "mat_display.hpp"
#include <imgui.h>
#include "../slots/mat.hpp"

namespace dt::df::plugin::opencv
{
MatDisplayNode::MatDisplayNode(IGraphManager &graph_manager)
    : BaseNode{graph_manager, kKey, kName, CreateInputs(graph_manager), Slots{}}
    , mat_{nullptr}
{
    initSlots();
}
MatDisplayNode::MatDisplayNode(IGraphManager &graph_manager, const nlohmann::json &json)
    : BaseNode{graph_manager, json}
    , mat_{nullptr}
{
    initSlots();
}

void MatDisplayNode::initSlots()
{
    std::static_pointer_cast<MatSlot>(inputByLocalId(0))->subscribe([this](const BaseSlot *slot) {
        auto in_mat = static_cast<const MatSlot *>(slot)->value();
        std::unique_lock<std::mutex> l{tex_mtx_};
        mat_ = in_mat;
        mat_changed_ = true;
    });
}

void MatDisplayNode::renderCustomContent()
{
    ImGui::Text("Current Mat:");
    ImGui::Value("W", mat_ ? mat_->size().width : 0);
    ImGui::Value("H", mat_ ? mat_->size().height : 0);
    if (mat_changed_ && mat_)
    {
        std::unique_lock<std::mutex> l{tex_mtx_};
        mat_changed_ = false;
        texture_.copyFrom(*mat_);
        //cv::ogl::convertToGLTexture2D(*mat_, texture_);
    }
    //ImGui::Image(reinterpret_cast<void *>(texture_.texId()), ImVec2(texture_.size().width, texture_.size().height));
}

Slots MatDisplayNode::CreateInputs(IGraphManager &graph_manager)
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
} // namespace dt::df::plugin::opencv

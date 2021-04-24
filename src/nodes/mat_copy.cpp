#include "mat_copy.hpp"
#include "../slots/mat.hpp"
namespace dt::df::plugin::opencv
{
MatCopyNode::MatCopyNode(IGraphManager &graph_manager)
    : BaseNode{graph_manager, kKey, kName, Slots{CreateInputs(graph_manager)}, Slots{CreateOutputs(graph_manager)}}
{
    initSlots();
}
MatCopyNode::MatCopyNode(IGraphManager &graph_manager, const nlohmann::json &json)
    : BaseNode{graph_manager, json}
{
    initSlots();
}

void MatCopyNode::initSlots()
{
    std::static_pointer_cast<MatSlot>(outputByLocalId(0))->accept(&mat_);
}

void MatCopyNode::calculate()
{
    auto copy_mat = std::static_pointer_cast<MatSlot>(inputByLocalId(0))->value();
    if (copy_mat)
    {
        copy_mat->copyTo(mat_);
        std::static_pointer_cast<MatSlot>(outputByLocalId(0))->valueChanged();
    }
}

Slots MatCopyNode::CreateInputs(IGraphManager &graph_manager)
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

Slots MatCopyNode::CreateOutputs(IGraphManager &graph_manager)
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

} // namespace dt::df::plugin::opencv

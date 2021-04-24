#include "mat_output.hpp"
#include "../slots/mat.hpp"
namespace dt::df::plugin::opencv
{
MatOutputNode::MatOutputNode(IGraphManager &graph_manager)
    : BaseNode{graph_manager, kKey, kName, Slots{}, Slots{CreateOutputs(graph_manager)}}
{
    initSlots();
}
MatOutputNode::MatOutputNode(IGraphManager &graph_manager, const nlohmann::json &json)
    : BaseNode{graph_manager, json}
{
    initSlots();
}

void MatOutputNode::initSlots()
{
    std::static_pointer_cast<MatSlot>(outputByLocalId(0))->accept(&mat_);
}

void MatOutputNode::calculate()
{}

Slots MatOutputNode::CreateOutputs(IGraphManager &graph_manager)
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

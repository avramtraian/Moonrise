/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/MathUtilities.h>
#include <AND/NumericLimits.h>
#include <GUI/DockTree.h>

namespace GUI {

//=================================================================================================
// HORIZONTAL SPLIT DOCK NODE.
//=================================================================================================

void HSplitDockNode::set_left_child(NonnullRefPtr<DockNode> const& child, Badge<ContainerDockNode>)
{
    m_left_child = child;
    m_left_child->set_parent_node(this);
}

void HSplitDockNode::set_right_child(NonnullRefPtr<DockNode> const& child, Badge<ContainerDockNode>)
{
    m_right_child = child;
    m_right_child->set_parent_node(this);
}

Optional<u32> HSplitDockNode::calculate_min_size_x() const
{
    auto left_min_size = m_left_child->calculate_min_size_x();
    auto right_min_size = m_right_child->calculate_min_size_x();

    if (!left_min_size.has_value() && !right_min_size.has_value())
        return {};

    return left_min_size.value_or(0) + right_min_size.value_or(0);
}

Optional<u32> HSplitDockNode::calculate_max_size_x() const
{
    auto left_max_size = m_left_child->calculate_max_size_x();
    auto right_max_size = m_right_child->calculate_max_size_x();

    if (!left_max_size.has_value() || !right_max_size.has_value())
        return {};

    return left_max_size.value() + right_max_size.value();
}

Optional<u32> HSplitDockNode::calculate_min_size_y() const
{
    auto left_min_size = m_left_child->calculate_min_size_y();
    auto right_min_size = m_right_child->calculate_min_size_y();

    if (!left_min_size.has_value() && !right_min_size.has_value())
        return {};

    return max(left_min_size.value_or(0), right_min_size.value_or(0));
}

Optional<u32> HSplitDockNode::calculate_max_size_y() const
{
    auto left_max_size = m_left_child->calculate_max_size_y();
    auto right_max_size = m_right_child->calculate_max_size_y();

    if (!left_max_size.has_value() && !right_max_size.has_value())
        return {};

    static constexpr u32 max_size = NumericLimits<u32>::max();
    return min(left_max_size.value_or(max_size), right_max_size.value_or(max_size));
}

//=================================================================================================
// VERTICAL SPLIT DOCK NODE.
//=================================================================================================

void VSplitDockNode::set_top_child(NonnullRefPtr<DockNode> const& child, Badge<ContainerDockNode>)
{
    m_top_child = child;
    m_top_child->set_parent_node(this);
}

void VSplitDockNode::set_bottom_child(NonnullRefPtr<DockNode> const& child, Badge<ContainerDockNode>)
{
    m_bottom_child = child;
    m_bottom_child->set_parent_node(this);
}

Optional<u32> VSplitDockNode::calculate_min_size_x() const
{
    auto top_min_size = m_top_child->calculate_min_size_x();
    auto bottom_min_size = m_bottom_child->calculate_min_size_x();

    if (!top_min_size.has_value() && !bottom_min_size.has_value())
        return {};

    return max(top_min_size.value_or(0), bottom_min_size.value_or(0));
}

Optional<u32> VSplitDockNode::calculate_max_size_x() const
{
    auto top_max_size = m_top_child->calculate_max_size_x();
    auto bottom_max_size = m_bottom_child->calculate_max_size_x();

    if (!top_max_size.has_value() && !bottom_max_size.has_value())
        return {};

    static constexpr u32 max_size = NumericLimits<u32>::max();
    return min(top_max_size.value_or(max_size), bottom_max_size.value_or(max_size));
}

Optional<u32> VSplitDockNode::calculate_min_size_y() const
{
    auto top_min_size = m_top_child->calculate_min_size_y();
    auto bottom_min_size = m_bottom_child->calculate_min_size_y();

    if (!top_min_size.has_value() && !bottom_min_size.has_value())
        return {};

    return top_min_size.value_or(0) + bottom_min_size.value_or(0);
}

Optional<u32> VSplitDockNode::calculate_max_size_y() const
{
    auto top_max_size = m_top_child->calculate_max_size_y();
    auto bottom_max_size = m_bottom_child->calculate_max_size_y();

    if (!top_max_size.has_value() || !bottom_max_size.has_value())
        return {};

    return top_max_size.value() + bottom_max_size.value();
}

//=================================================================================================
// CONTAINER DOCK NODE.
//=================================================================================================

void ContainerDockNode::set_panel(NonnullRefPtr<Panel> const& panel)
{
    m_panel = panel;
}

ContainerDockNode& ContainerDockNode::insert_left(NonnullRefPtr<Panel> const& panel)
{
    // Create the new split node.
    auto new_split_node = HSplitDockNode::construct();
    new_split_node->set_dock_tree(*m_dock_tree);
    new_split_node->set_parent_node(m_parent_node);

    // Create the new container node.
    auto new_container_node = ContainerDockNode::construct();
    new_container_node->set_dock_tree(*m_dock_tree);
    new_container_node->set_parent_node(new_split_node.get());
    new_container_node->set_panel(panel);

    new_split_node->set_left_child(new_container_node, {});
    new_split_node->set_right_child(adopt_nonnull(*this), {});
    update_split_parent(new_split_node);

    return *new_container_node;
}

ContainerDockNode& ContainerDockNode::insert_right(NonnullRefPtr<Panel> const& panel)
{
    // Create the new split node.
    auto new_split_node = HSplitDockNode::construct();
    new_split_node->set_dock_tree(*m_dock_tree);
    new_split_node->set_parent_node(m_parent_node);

    // Create the new container node.
    auto new_container_node = ContainerDockNode::construct();
    new_container_node->set_dock_tree(*m_dock_tree);
    new_container_node->set_parent_node(new_split_node.get());
    new_container_node->set_panel(panel);

    new_split_node->set_left_child(adopt_nonnull(*this), {});
    new_split_node->set_right_child(new_container_node, {});
    update_split_parent(new_split_node);

    return *new_container_node;
}

ContainerDockNode& ContainerDockNode::insert_top(NonnullRefPtr<Panel> const& panel)
{
    // Create the new split node.
    auto new_split_node = VSplitDockNode::construct();
    new_split_node->set_dock_tree(*m_dock_tree);
    new_split_node->set_parent_node(m_parent_node);

    // Create the new container node.
    auto new_container_node = ContainerDockNode::construct();
    new_container_node->set_dock_tree(*m_dock_tree);
    new_container_node->set_parent_node(new_split_node.get());
    new_container_node->set_panel(panel);

    new_split_node->set_top_child(new_container_node, {});
    new_split_node->set_bottom_child(adopt_nonnull(*this), {});
    update_split_parent(new_split_node);

    return *new_container_node;
}

ContainerDockNode& ContainerDockNode::insert_bottom(NonnullRefPtr<Panel> const& panel)
{
    // Create the new split node.
    auto new_split_node = VSplitDockNode::construct();
    new_split_node->set_dock_tree(*m_dock_tree);
    new_split_node->set_parent_node(m_parent_node);

    // Create the new container node.
    auto new_container_node = ContainerDockNode::construct();
    new_container_node->set_dock_tree(*m_dock_tree);
    new_container_node->set_parent_node(new_split_node.get());
    new_container_node->set_panel(panel);

    new_split_node->set_top_child(adopt_nonnull(*this), {});
    new_split_node->set_bottom_child(new_container_node, {});
    update_split_parent(new_split_node);

    return *new_container_node;
}

void ContainerDockNode::update_split_parent(NonnullRefPtr<DockNode> const& new_split_node)
{
    ASSERT(m_parent_node == new_split_node.get());

    if (new_split_node->parent_node()) {
        // The current container node has a valid parent node, which guarantees that the
        // parent node is either a horizontal or vertical split node. We have to update the
        // parent children to reflect the newly inserted split node.

        auto* parent = new_split_node->parent_node();
        if (parent->is_horizontal_split()) {
            auto& horizontal_split = parent->as_horizontal_split();
            if (&horizontal_split.left_child() == this)
                horizontal_split.set_left_child(new_split_node, {});
            else if (&horizontal_split.right_child() == this)
                horizontal_split.set_right_child(new_split_node, {});
            else
                ASSERT_NOT_REACHED;
        } else if (parent->is_vertical_split()) {
            auto& vertical_split = parent->as_vertical_split();
            if (&vertical_split.top_child() == this)
                vertical_split.set_top_child(new_split_node, {});
            else if (&vertical_split.bottom_child() == this)
                vertical_split.set_bottom_child(new_split_node, {});
            else
                ASSERT_NOT_REACHED;
        } else {
            ASSERT_NOT_REACHED;
        }
    } else {
        // The current container node does not have a parent node, which means that it represents
        // the root node of the dock tree. In this situation, it is guaranteed that the parent
        // object is the dock tree object itself. We have to update the dock tree root to reflect
        // the newly inserted split node.

        ASSERT(&m_dock_tree->root_node() == this);
        m_dock_tree->set_root_node(new_split_node);
    }
}

Optional<u32> ContainerDockNode::calculate_min_size_x() const
{
    return m_panel->calculate_min_size_x();
}

Optional<u32> ContainerDockNode::calculate_max_size_x() const
{
    return m_panel->calculate_max_size_x();
}

Optional<u32> ContainerDockNode::calculate_min_size_y() const
{
    return m_panel->calculate_min_size_y();
}

Optional<u32> ContainerDockNode::calculate_max_size_y() const
{
    return m_panel->calculate_max_size_y();
}

//=================================================================================================
// DOCK TREE.
//=================================================================================================

ContainerDockNode& DockTree::set_root_panel(NonnullRefPtr<Panel> const& panel)
{
    ASSERT(!m_root_node.is_valid());
    auto container_node = ContainerDockNode::construct();
    container_node->set_dock_tree(*this);
    container_node->set_panel(panel);
    m_root_node = container_node;
    return *container_node;
}

void DockTree::set_root_node(NonnullRefPtr<DockNode> const& node)
{
    m_root_node = node;
}

} // namespace GUI

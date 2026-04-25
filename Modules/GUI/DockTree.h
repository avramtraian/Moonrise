/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Badge.h>
#include <GUI/Panel.h>
#include <GUI/Widget.h>

namespace GUI {

//=================================================================================================
// DOCK NODE.
//=================================================================================================

class DockNode;
class HSplitDockNode;
class VSplitDockNode;
class ContainerDockNode;
class DockTree;

class DockNode : public Widget {
    GUI_OBJECT(DockNode, Object);

public:
    virtual bool is_horizontal_split() const { return false; }
    virtual bool is_vertical_split() const { return false; }
    virtual bool is_container() const { return false; }

    HSplitDockNode& as_horizontal_split()
    {
        ASSERT(is_horizontal_split());
        return reinterpret_cast<HSplitDockNode&>(*this);
    }

    VSplitDockNode& as_vertical_split()
    {
        ASSERT(is_vertical_split());
        return reinterpret_cast<VSplitDockNode&>(*this);
    }

    ContainerDockNode& as_container()
    {
        ASSERT(is_container());
        return reinterpret_cast<ContainerDockNode&>(*this);
    }

public:
    DockNode* parent_node() const { return m_parent_node; }
    void set_parent_node(DockNode* parent_node) { m_parent_node = parent_node; }

    DockTree& dock_tree() const { return *m_dock_tree; }
    void set_dock_tree(DockTree& dock_tree) { m_dock_tree = &dock_tree; }

public:
    // FIXME: Correctly implement this method!
    virtual Gfx::IntSize calculate_preferred_size() const override { return { 0, 0 }; }

protected:
    DockNode* m_parent_node;
    DockTree* m_dock_tree;
};

//=================================================================================================
// HORIZONTAL SPLIT DOCK NODE.
//=================================================================================================

class HSplitDockNode : public DockNode {
    GUI_OBJECT(HSplitDockNode, DockNode);

public:
    virtual bool is_horizontal_split() const override { return true; }

    DockNode& left_child() { return *m_left_child; }
    DockNode& right_child() { return *m_right_child; }

    void set_left_child(NonnullRefPtr<DockNode> const&, Badge<ContainerDockNode>);
    void set_right_child(NonnullRefPtr<DockNode> const&, Badge<ContainerDockNode>);

    virtual Optional<u32> calculate_min_size_x() const override;
    virtual Optional<u32> calculate_max_size_x() const override;

    virtual Optional<u32> calculate_min_size_y() const override;
    virtual Optional<u32> calculate_max_size_y() const override;

private:
    RefPtr<DockNode> m_left_child;
    RefPtr<DockNode> m_right_child;
};

//=================================================================================================
// VERTICAL SPLIT DOCK NODE.
//=================================================================================================

class VSplitDockNode : public DockNode {
    GUI_OBJECT(VSplitDockNode, DockNode);

public:
    virtual bool is_vertical_split() const override { return true; }

    DockNode& top_child() { return *m_top_child; }
    DockNode& bottom_child() { return *m_bottom_child; }

    void set_top_child(NonnullRefPtr<DockNode> const&, Badge<ContainerDockNode>);
    void set_bottom_child(NonnullRefPtr<DockNode> const&, Badge<ContainerDockNode>);

    virtual Optional<u32> calculate_min_size_x() const override;
    virtual Optional<u32> calculate_max_size_x() const override;

    virtual Optional<u32> calculate_min_size_y() const override;
    virtual Optional<u32> calculate_max_size_y() const override;

private:
    RefPtr<DockNode> m_top_child;
    RefPtr<DockNode> m_bottom_child;
};

//=================================================================================================
// CONTAINER DOCK NODE.
//=================================================================================================

class ContainerDockNode : public DockNode {
    GUI_OBJECT(ContainerDockNode, DockNode);

public:
    RefPtr<Panel> panel() const { return m_panel; }
    void set_panel(NonnullRefPtr<Panel> const&);

    ContainerDockNode& insert_left(NonnullRefPtr<Panel> const&);
    ContainerDockNode& insert_right(NonnullRefPtr<Panel> const&);
    ContainerDockNode& insert_top(NonnullRefPtr<Panel> const&);
    ContainerDockNode& insert_bottom(NonnullRefPtr<Panel> const&);

    virtual Optional<u32> calculate_min_size_x() const override;
    virtual Optional<u32> calculate_max_size_x() const override;

    virtual Optional<u32> calculate_min_size_y() const override;
    virtual Optional<u32> calculate_max_size_y() const override;

private:
    void update_split_parent(NonnullRefPtr<DockNode> const& new_split_node);

private:
    RefPtr<Panel> m_panel;
};

//=================================================================================================
// DOCK TREE.
//=================================================================================================

class DockTree : public Widget {
    GUI_OBJECT(DockTree, Widget);

public:
    ContainerDockNode& set_root_panel(NonnullRefPtr<Panel> const&);

    DockNode& root_node() const { return *m_root_node; }
    void set_root_node(NonnullRefPtr<DockNode> const&);

public:
    // FIXME: Correctly implement this method!
    virtual Gfx::IntSize calculate_preferred_size() const override { return { 0, 0 }; }

private:
    RefPtr<DockNode> m_root_node;
};

} // namespace GUI

/*
 * Copyright (c) 2021, Tim Flynn <trflynn89@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/Forward.h>
#include <LibWeb/Layout/ReplacedBox.h>

namespace Web::Layout {

class LabelableNode : public ReplacedBox {
    GC_CELL(LabelableNode, ReplacedBox);

public:
    Painting::LabelablePaintable* paintable();
    Painting::LabelablePaintable const* paintable() const;

protected:
    LabelableNode(DOM::Document& document, DOM::Element& element, GC::Ref<CSS::ComputedProperties> style)
        : ReplacedBox(document, element, move(style))
    {
    }

    virtual ~LabelableNode() = default;
};

}

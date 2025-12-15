#include <gtest/gtest.h>

#include <test/helpers/snapshot_tests.h>

using namespace engine;

namespace engine {

    // Goal:
    // We want some kind of immediate mode style declarative UI.
    // Three phases: declare UI elements, compute UI layout, render UI layout.

    // UI elements: text, image, container

    // The UI element hiearchy declared by the application programmer
    struct UIDocument {
    };

    // The computed layout of elements (positions + sizes)
    struct UILayout {
    };

    // Renders a UILayout onto the screen
    class UIRenderer {
        // void draw_ui(Renderer* renderer, const UILayout& layout);
    };

    // The application programmer interface
    class UISystem {
        // void on_input(...); // some kind of input event

        // void begin_element(...);
        // void end_element(...);
    };

    // Questions:
    // How do we handle mouse and keyboard input?
    // Mouse input probably easiest:
    // - Track position of mouse in screen space (taking zoom etc. into account)
    // - Hovering an element and pressing up triggers click and changes focus to element
    // Keyboard / Gamepad navigation:
    // - Boxes are laid out with a main layout axis (horizontally or vertically)
    // - Each box has an index along axis
    // - Only one box is focused at a time (default index 0)
    // - Pressing forward / backward will move focus along axis
    // - Some kind of event for customizing focus change?
    //      - Need a way to go from e.g. side menu to menu content, which has to be customized by
    //        application programmer since we can't meaningfully predict the navigation since where
    //        we should go depends on the semantics of the content of the screen.


} // namespace engine

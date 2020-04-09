/*
  ==============================================================================

   Copyright 2020 Art+Logic, Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Tag.h"

struct ASharedTagButtonLookAndFeel;


/**
 * A tag button represents a clickable tag in the GUI.
 * It displays the tag name.
 * TODO: Display example count and child dismiss button.
 */
class ATagButton : public TextButton
{
public:
   constexpr static const float kCornerSize = 10.0f;
   constexpr static const float kWidth = 109.0f;
   constexpr static const float kHeight = 24.0f;
   constexpr static const float kTextSize = 14.0f;
   
   explicit ATagButton(const ATag& tag);
   ~ATagButton() override;

   constexpr const ATag& Tag() const noexcept { return fTag; }
   void SetTag(const ATag& tag) noexcept;
   
   String TagName() const;

   /** Handler for click on the close image contained within the button. */
   std::function<void()> OnCloseClick;

   constexpr bool ShowClose() const noexcept
   {
      return fShowClose;
   }

   void SetShowClose(bool doShow);

   void paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted,
                    bool shouldDrawButtonAsDown) override;

   void mouseDown(const MouseEvent& e) override;
   
private:

   Rectangle<int> CloseBounds() const;
   
   SharedResourcePointer<ASharedTagButtonLookAndFeel> fLookAndFeel;
   
   ATag fTag;
   bool fShowClose;

   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATagButton)
};

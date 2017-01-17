/*
Copyright 2016 fuzz, Inc. All rights reserved. 
   http://www.fuzz.co.jp

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of fuzz, Inc. nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FUZZ, INC. BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/***********************************************************************//**
	@file
***************************************************************************/
#pragma once

#include "wxump/Application.hpp"
#include "wxump/Layout.hpp"

namespace wxump {
/***********************************************************************//**
	@brief レイアウト描画
***************************************************************************/
class LayoutRenderer {
 private:
  std::unique_ptr<HaiImage> haiImage_;
  std::shared_ptr<wxDC> dc_;

 public:
  LayoutRenderer();
  ~LayoutRenderer();

  void loadHaiImage(Application::HaiSize size);

  void beginRender(std::shared_ptr<wxDC> dc);
  void endRender();

  void renderHai(const LayoutPos& layoutPos, 
                 const HaiObject& object);

  void renderText(const LayoutRect& layoutRect, 
                  const wxString& text, 
                  const wxColour& color = *wxWHITE, 
                  float alignment = 0.0f);

  void renderBitmap(const LayoutPos& pos, 
                    const wxBitmap& bitmap);

  void renderRect(const LayoutRect& layoutRect, 
                  const wxColour& color, 
                  const wxColour& edgeColor, 
                  const LayoutValue& layoutRadius = LayoutValue());
  
  void renderLine(const LayoutPos& start,
                  const LayoutPos& end,
                  const wxColour& colour = *wxBLACK);

  wxPoint getPos(const LayoutPos& pos) const;
  wxSize getSize(const LayoutSize& size) const;
  wxRect getRect(const LayoutRect& rect) const;

  wxPoint getIndex(const LayoutRect& rect, const wxPoint& pos) const;

 private:
  int getLayoutValue(const LayoutValue& layout) const;
  wxDC& getDc() const;

  static wxBitmap ApplyColor(const wxBitmap& bitmap, 
                             const wxColour& color);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}

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

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

namespace wxump {
/***********************************************************************//**
	@brief リザルトウィンドウ
***************************************************************************/
class ResultWindow
  : public wxPanel, 
    public Application::Listener
{
  typedef wxPanel super;

 private:
  enum {
    RESULT_RYUUKYUKU,
    RESULT_AGARI,
    RESULT_GAME_END,
  };
  enum {
    TOTAL_STR,
    TOTAL_FU,
    TOTAL_HAN,
    TOTAL_MAX
  };

 private:
  std::shared_ptr<Client> client_;
  std::shared_ptr<const Player> player_;
  std::unique_ptr<wxBitmap> bitmap_;
  LayoutRect cursor_;
  size_t resultType_;
  bool isResultPoint;

 public:
  ResultWindow(wxWindow* parent,
               std::shared_ptr<Client> client = nullptr);
  virtual ~ResultWindow();
  
  UMP_ACCESSOR(Player, player_);
  UMP_ACCESSOR(IsResultPoint, isResultPoint);
  static const LayoutSize& GetWinSize();
 protected:
  void onReceiveCommand(const ump::Command& command) override;
  void onChangeLayout(const LayoutRenderer& renderer) override;

 private:
  UMP_GETTER(Client, client_);
  void show();
  void hide();
  void clear();
  
  void onMouse(wxMouseEvent& event);
  void onPaint(wxPaintEvent& event);

  void onAgari(const ump::Command& command);
  void onPoint(const ump::Command& command);
  void onGameEnd(const ump::Command& command);
  
  LayoutRenderer& beginRender(int resultType);
  void endRender(LayoutRenderer& renderer);

  void renderText(LayoutRenderer& renderer, 
                  const wxColour& color, 
                  const wxString& text, 
                  const wxString& rightText = wxEmptyString);
  void renderTitleText(LayoutRenderer& renderer,
                        const wxColour& color,
                        const wxString& text);
  void renderAgariHais(LayoutRenderer& renderer);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}

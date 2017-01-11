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
#include "wxump/Application.hpp"
#include "wxump/Client.hpp"
#include "wxump/ClientPlayer.hpp"
#include "wxump/Conversion.hpp"
#include "wxump/HaiObject.hpp"
#include "wxump/HandRenderer.hpp"
#include "wxump/Layout.hpp"
#include "wxump/LayoutRenderer.hpp"
#include "wxump/Player.hpp"
#include "wxump/ResultWindow.hpp"

namespace wxump {
static const size_t INVALID_SELECT  = std::numeric_limits<size_t>::max();
static const LayoutValue HEIGHT = LayoutValue(0, 2, 3);
static const LayoutSize INFO_SIZE(HEIGHT, HEIGHT);
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
ClientPlayer::ClientPlayer(std::shared_ptr<Client> client, 
                           std::shared_ptr<const ump::mj::Player> player)
: super(client, player),
  cursorIndex_(INVALID_SELECT),
  nakiIndex_(INVALID_SELECT)
{
  
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
ClientPlayer::~ClientPlayer() {

}
/***********************************************************************//**
	@brief マウス操作
  @param[in] renderer 描画クラス
  @param[in] event マウスイベント
  @param[in] pos 描画部分のマウス位置
***************************************************************************/
void ClientPlayer::onMouse(const LayoutRenderer& renderer,
                           const wxMouseEvent& event,
                           const wxPoint& pos) {
  static const LayoutRect MENZEN_RECT(LayoutPos(HEIGHT, 
                                                LayoutValue(0, 1, 2)), 
                                      LayoutSize(LayoutValue(1, 0, 0), 
                                                 LayoutValue(0, 1, 0)));
  auto index = renderer.getIndex(MENZEN_RECT, pos);
  if(index.y == 0 &&
     index.x >= 0 &&
     index.x < getPlayer()->getMenzen().size()) {
    setCursorIndex(index.x);
    if(event.LeftDown()) {
      setNakiIndex(Application::Get()->onSelectHai(index.x));
    }
  }
  else {
    setCursorIndex(INVALID_SELECT);
  }
}
/***********************************************************************//**
	@copydoc Player::renderInfo
***************************************************************************/
void ClientPlayer::renderInfo(LayoutRenderer& renderer,
                        const LayoutPos& offset) {
  static const LayoutSize MARGIN(LayoutValue(0, 0, 1), 
                                 LayoutValue(0, 0, 1));
  auto player = getPlayer();
  auto pos = offset + MARGIN;
  auto size = INFO_SIZE - MARGIN * 2;
  renderer.renderRect(LayoutRect(pos, size), 
                      wxColour(0, 0, 0, 64), 
                      wxNullColour);
  {
    LayoutRect rect(pos, LayoutSize(size.width, LayoutValue(0, 0.5, 0)));
    if(auto zikaze = player->getZikaze()) {
      renderer.renderText(rect, Conversion::GetHaiString(zikaze) + "家",
      isTurn() ? *wxYELLOW : *wxWHITE);
    }
    rect.pos.y += rect.size.height;
    rect.size.height = LayoutValue(0, 0.25, 0);
    renderer.renderText(rect, player->getName());
  }
  {
    LayoutRect rect(pos + LayoutSize(LayoutValue(), size.height), 
                    LayoutSize(size.width, LayoutValue(0, 0.5, 0)));
    rect.pos.y -= rect.size.height;
    renderer.renderText(rect, wxString::Format("%d", player->getPoint()), 
                        *wxWHITE, 1.0f);
  }
}
/***********************************************************************//**
	@copydoc Player::renderHais
***************************************************************************/
void ClientPlayer::renderHais(LayoutRenderer& renderer,
                              const LayoutPos& offset) const {
  HandRenderer hand(getClient(), getPlayer());
  hand.renderKawa(renderer, offset);
  LayoutPos pos = offset + LayoutSize(LayoutValue(), LayoutValue(0, 1, 1));
  hand.renderMenzen(renderer, pos, getCursorIndex(), getNakiIndex());
  pos = pos +
        LayoutSize((ResultWindow::GetWinSize().width -
                    LayoutValue(1, 0, 0)),
                    LayoutValue());
  hand.renderAllMentsu(renderer, pos);
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

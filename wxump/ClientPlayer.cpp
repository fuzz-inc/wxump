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
  select_(INVALID_SELECT)
{
  
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
ClientPlayer::~ClientPlayer() {

}
/***********************************************************************//**
	@brief マウス操作
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
    setSelect(index.x);
    if(event.LeftDown()) {
      Application::Get()->onSelectHai(index.x);
    }
  }
  else {
    setSelect(INVALID_SELECT);
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
      renderer.renderText(rect, Conversion::getHaiString(zikaze) + "家",
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
	@copydoc Player::renderHand
***************************************************************************/
void ClientPlayer::renderHand(LayoutRenderer& renderer,
                              const LayoutPos& offset,
                              bool isResult) const{
  {
    LayoutPos pos = offset;
    auto& menzen = getPlayer()->getMenzen();
    for(size_t i = 0, n = menzen.size(); i < n; i++) {
      renderer.renderHai(pos, 
                         HaiObject(menzen.at(i)).
                         setSelect(i == getSelect() && !isResult));
      pos.x.w += 1;
    }
  }
  {
    LayoutPos pos = offset +
        LayoutSize((ResultWindow::GetWinSize().width -
                    LayoutValue(1, 0, 0)),
                    LayoutValue());
    if(isResult) {
      auto& lastSutehai = player_->getGame()->getLastSutehai();
      if(lastSutehai->isRon()) {
        renderer.renderHai(pos, HaiObject(lastSutehai->getHai()));
        pos.x -= LayoutValue(0, 0, 1);
      }
    }
    for(size_t i = 0, n = player_->countMentsu(); i < n; i++) {
      pos = renderMentsu(renderer, pos, player_->getMentsu(i));
      pos.x.margin -= 1;
    }
  }
}
/***********************************************************************//**
	@copydoc Player::renderKawa
***************************************************************************/
void ClientPlayer::renderKawa(LayoutRenderer& renderer,
                        const LayoutPos& offset) const {
  LayoutPos pos = offset;
  auto& kawa = player_->getKawa();
  auto clientPlayer(getClient()->getPlayer());
  for(size_t i = 0, n = kawa.size(); i < n; i++) {
    auto& sutehai = kawa.at(i);
    renderer.renderHai(pos, 
                       HaiObject(sutehai.getHai()).
                       setTsumogiri(sutehai.isTsumogiri()).
                       setRichi(sutehai.isRichi()).
                       setNaki(sutehai.isNaki()));
    pos.x += sutehai.isRichi()
      ? LayoutValue(0, 1, 0)
      : LayoutValue(1, 0, 0);
    if(i % 6 == 5) {
      pos.x.margin += 1;
    }
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

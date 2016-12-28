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
#include "wxump/Conversion.hpp"
#include "wxump/HaiObject.hpp"
#include "wxump/Layout.hpp"
#include "wxump/LayoutRenderer.hpp"
#include "wxump/Player.hpp"
#include "wxump/ResultWindow.hpp"
#include "wxump/TakuWindow.hpp"

namespace wxump {
static const LayoutValue HEIGHT = LayoutValue(0, 2, 3);
static const LayoutSize INFO_SIZE(HEIGHT, HEIGHT);
static const size_t SUTEHAI_MAX = 6 * 4;
static const LayoutSize HAND_SIZE(LayoutValue(SUTEHAI_MAX - 1, 1, 
                                              (SUTEHAI_MAX + 5) / 6), 
                                  HEIGHT);
static const LayoutValue WIDTH(INFO_SIZE.width + HAND_SIZE.width);
static const LayoutSize SIZE(WIDTH, HEIGHT);

static const LayoutSize COMMENT_SIZE(LayoutValue(4, 0, 0), 
                                     LayoutValue(1, 0, 0));
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
Player::Player(std::shared_ptr<Client> client, 
               std::shared_ptr<const ump::mj::Player> player)
  : client_(client), 
    player_(player)
{
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Player::~Player() {
}
/***********************************************************************//**
	@brief コメントのリセット
***************************************************************************/
void Player::resetComment() {
  setComment(wxEmptyString);
}
/***********************************************************************//**
	@brief 描画関数
  @param[in] renderer 描画クラス
  @param[in] offset 描画位置
***************************************************************************/
void Player::onPaint(LayoutRenderer& renderer, 
                     const LayoutPos& offset) {
  LayoutPos handPos = offset + LayoutSize(INFO_SIZE.width, 
                                          LayoutValue(0, 0, 1));
  renderer.renderRect(LayoutRect(offset, GetSize()), 
                      wxNullColour, 
                      wxColour(0, 64, 0));
  renderInfo(renderer, offset);
  renderHais(renderer, handPos);
  if(!comment_.IsEmpty()) {
    static const LayoutSize PADDING(LayoutSize(LayoutValue(0, 0, 1), 
                                               LayoutValue(0, 0, 1)));
    renderer.renderRect(LayoutRect(handPos - PADDING, 
                                   COMMENT_SIZE + PADDING * 2), 
                        wxColour(255, 255, 255, 200), 
                        *wxBLACK, 
                        LayoutValue(0, 0, 2));
    renderer.renderText(LayoutRect(handPos, COMMENT_SIZE), 
                        comment_, *wxBLACK, 0.5f);
  }
}
/***********************************************************************//**
	@brief 引数と同じサーバープレイヤーか確かめる
  @param[in] player 
  @return 同じならtrue
***************************************************************************/
const bool Player::isMjPlayer(const ump::mj::Player* player) const{
  return player_.get() == player;
}
/***********************************************************************//**
	@brief プレイヤー情報部分の描画を行う
  @param[in] renderer 描画クラス
  @param[in] offset 位置
***************************************************************************/
void Player::renderInfo(LayoutRenderer& renderer, 
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
    auto clientPlayer = getClient()->getPlayer();
    rect.size.height *= 0.7f;
    rect.pos.y -= rect.size.height;
    auto diff = player->getPoint() - clientPlayer->getPoint();
    renderer.renderText(rect, 
                        (diff != 0)
                        ? wxString::Format("%+d", diff)
                        : "-", 
                        wxColour(200, 200, 200), 
                        1.0f);
  }
}
/***********************************************************************//**
	@brief プレイヤーの捨て牌、手牌を表示
  @param[in] renderer 描画クラス
  @param[in] offset 位置
***************************************************************************/
void Player::renderHais(LayoutRenderer& renderer,
                        const LayoutPos& offset) const {
  renderKawa(renderer, offset);
  LayoutPos pos = offset + LayoutSize(LayoutValue(), LayoutValue(0, 1, 1));
  renderHand(renderer, pos);
}
/***********************************************************************//**
	@brief プレイヤーの捨て牌表示
  @param[in] renderer 描画クラス
  @param[in] offset 位置
***************************************************************************/
void Player::renderKawa(LayoutRenderer& renderer,
                        const LayoutPos& offset) const {
  LayoutPos pos = offset;
  auto& kawa = player_->getKawa();
  auto clientPlayer(getClient()->getPlayer());
  int calc = 155 * sinf((countTime_*0.1)*M_PI_2);
  int alpha = 255;
  alpha -= (calc >= 0) ? calc : 0;
  for(size_t i = 0, n = kawa.size(); i < n; i++) {
    auto& sutehai = kawa.at(i);
    auto hai = sutehai.getHai();
    bool isNakiQ = isTurn() &&
                   player_->getGame()->getLastSutehai() == &sutehai &&
                   (clientPlayer->canPon(hai) ||
                   clientPlayer->canChi(hai) ||
                   clientPlayer->canKan(hai) ||
                   clientPlayer->canRon(hai));
    renderer.renderHai(pos, 
                       HaiObject(sutehai.getHai()).
                       setTsumogiri(sutehai.isTsumogiri()).
                       setRichi(sutehai.isRichi()).
                       setNaki(sutehai.isNaki()).
                       setNakiQ(isNakiQ).setAlpha(alpha));
    pos.x += sutehai.isRichi()
      ? LayoutValue(0, 1, 0)
      : LayoutValue(1, 0, 0);
    if(i % 6 == 5) {
      pos.x.margin += 1;
    }
  }
}
/***********************************************************************//**
	@brief プレイヤーの手牌を表示する
  @param[in] renderer 描画クラス
  @param[in] offset 位置
  @param[in] isResult リザルト画面に表示するならtrue
***************************************************************************/
void Player::renderHand(LayoutRenderer& renderer, 
                        const LayoutPos& offset,
                        bool isResult) const{
  {
    LayoutPos pos = offset;
    auto& menzen = getPlayer()->getMenzen();
    for(size_t i = 0, n = menzen.size(); i < n; i++) {
      renderer.renderHai(pos, HaiObject(menzen.at(i)));
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
	@brief 鳴き牌の表示
  @param[in] renderer 描画クラス
  @param[in] pos 描画位置
  @param[in] mentsu 鳴き牌
***************************************************************************/
LayoutPos Player::renderMentsu(LayoutRenderer& renderer, 
                               LayoutPos pos, 
                               const ump::mj::Mentsu& mentsu) const{
  for(int i = int(mentsu.size() - 1); i >= 0; i--) {
    pos.x.w -= 1;
    renderer.renderHai(pos, 
                       HaiObject(mentsu.isKantsu() && 
                                 mentsu.isAnko() && 
                                 (i == 0 || i == 3)
                                 ? ump::mj::Hai::GetUnknown()
                                 : mentsu.at(i)));
  }
  return pos;
}
/***********************************************************************//**
	@brief
***************************************************************************/
const bool Player::isTurn() const {
  return player_->getSeat() == getClient()->getTurn();
}
/***********************************************************************//**
	@brief
***************************************************************************/
const LayoutValue& Player::GetWidth() {
  return WIDTH;
}
/***********************************************************************//**
	@brief
***************************************************************************/
const LayoutValue& Player::GetHeight() {
  return HEIGHT;
}
/***********************************************************************//**
	@brief プレイヤーのサイズを求める関数
  @return プレイヤーのサイズ
***************************************************************************/
const LayoutSize& Player::GetSize() {
  return SIZE;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

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
#include "wxump/HaiRender.hpp"
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
static const LayoutValue HALF_HEIGHT(0, 0.5, 0);
static const LayoutValue QUARTER_HEIGHT(0, 0.25, 0);
static const float DIFF_POINT_SIZE_RATE = 0.7f;
/***********************************************************************//**
	@brief コンストラクタ
  @param[in] client クライアント
  @param[in] player サーバーで管理しているプレイヤー
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
  renderComment(renderer, handPos);
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
  auto pos = offset + MARGIN;
  auto size = INFO_SIZE - MARGIN * 2;
  renderer.renderRect(LayoutRect(pos, size), 
                      wxColour(0, 0, 0, 64), 
                      wxNullColour);
  
  renderName(renderer, pos, size);
  renderPoint(renderer, pos, size);
}
/***********************************************************************//**
	@brief プレイヤーの捨て牌、手牌を表示
  @param[in] renderer 描画クラス
  @param[in] offset 位置
***************************************************************************/
void Player::renderHais(LayoutRenderer& renderer,
                        const LayoutPos& offset) const {
  HaiRender render(renderer, getClient());
  LayoutPos pos = offset;
  renderKawa(render, pos);
  
  pos = pos + LayoutSize(LayoutValue(), LayoutValue(0, 1, 1));
  renderMenzen(render, pos);
  renderMentsu(render, pos);
}
/***********************************************************************//**
	@brief コメント表示
  @param[in] renderer 描画クラス
  @param[in] offset 位置
***************************************************************************/
void Player::renderComment(LayoutRenderer& renderer,
                           const LayoutPos& offset) const {
  if(!comment_.IsEmpty()) {
    static const LayoutSize PADDING(LayoutSize(LayoutValue(0, 0, 1), 
                                               LayoutValue(0, 0, 1)));
    renderer.renderRect(LayoutRect(offset - PADDING,
                                   COMMENT_SIZE + PADDING * 2), 
                        wxColour(255, 255, 255, 200), 
                        *wxBLACK, 
                        LayoutValue(0, 0, 2));
    renderer.renderText(LayoutRect(offset, COMMENT_SIZE),
                        comment_, *wxBLACK, 0.5f);
  }
}
/***********************************************************************//**
	@brief 自家と名前の表示
  @param[in] renderer 描画クラス
  @param[in] offset 位置
  @param[in] size
***************************************************************************/
void Player::renderName(LayoutRenderer& renderer,
                        const LayoutPos& offset,
                        const LayoutSize& size) const {
  LayoutRect rect(offset, LayoutSize(size.width, HALF_HEIGHT));
  if(auto zikaze = getPlayer()->getZikaze()) {
    renderer.renderText(rect,
                        Conversion::GetHaiString(zikaze) + "家",
                        isTurn() ? *wxYELLOW : *wxWHITE);
  }
  rect.pos.y += rect.size.height;
  rect.size.height = QUARTER_HEIGHT;
  renderer.renderText(rect, getPlayer()->getName());
}
/***********************************************************************//**
	@brief ポイント、操作プレイヤーとの差を表示
  @param[in] renderer 描画クラス
  @param[in] offset 位置
  @param[in] size
***************************************************************************/
void Player::renderPoint(LayoutRenderer& renderer,
                        const LayoutPos& offset,
                        const LayoutSize& size) const {
  LayoutRect rect(offset + LayoutSize(LayoutValue(), size.height),
                  LayoutSize(size.width, HALF_HEIGHT));
  rect.pos.y -= rect.size.height;
  renderer.renderText(rect, wxString::Format("%d", getPlayer()->getPoint()),
                      *wxWHITE, 1.0f);
  auto clientPlayer = getClient()->getPlayer();
  auto diff = getPlayer()->getPoint() - clientPlayer->getPoint();
  rect.size.height *= DIFF_POINT_SIZE_RATE;
  rect.pos.y -= rect.size.height;
  renderer.renderText(rect, 
                      (diff != 0)
                      ? wxString::Format("%+d", diff)
                      : "-", 
                      wxColour(200, 200, 200), 
                      1.0f);
}
/***********************************************************************//**
	@brief 捨て牌の表示
  @param[in] renderer 描画クラス
  @param[in] offset 位置
***************************************************************************/
void Player::renderKawa(HaiRender& render,
                        const LayoutPos& offset) const {
  LayoutPos pos = offset;
  int calc = 155 * sinf((countTime_*0.1)*M_PI_2);
  size_t alpha = 255 - ((calc >= 0) ? calc : 0);
  render.renderKawa(pos, getPlayer()->getKawa(), isTurn(), alpha);
}
/***********************************************************************//**
	@brief 手牌の表示
  @param[in] renderer 描画クラス
  @param[in] offset 位置
***************************************************************************/
void Player::renderMenzen(HaiRender& render,
                          const LayoutPos& offset) const {
  render.renderMenzen(offset, getPlayer()->getMenzen());
}
/***********************************************************************//**
	@brief 鳴き牌を表示
  @param[in] renderer 描画クラス
  @param[in] offset 位置
***************************************************************************/
void Player::renderMentsu(HaiRender& render,
                          const LayoutPos& offset) const {
  LayoutPos pos = offset + LayoutSize((ResultWindow::GetWinSize().width -
                                          LayoutValue(1, 0, 0)),
                                        LayoutValue());
  for(size_t i = 0, n = getPlayer()->countMentsu(); i < n; i++) {
    pos = render.renderMentsu(pos, getPlayer()->getMentsu(i));
  }
}
/***********************************************************************//**
	@brief 自身の手番か調べる
  @return 手番ならtrue
***************************************************************************/
const bool Player::isTurn() const {
  return player_->getSeat() == getClient()->getTurn();
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

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
#include "wxump/HaiObject.hpp"
#include "wxump/HandRenderer.hpp"
#include "wxump/Layout.hpp"
#include "wxump/LayoutRenderer.hpp"
#include "wxump/Player.hpp"
#include "wxump/ResultWindow.hpp"

namespace wxump {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
HandRenderer::HandRenderer(std::shared_ptr<Client> client,
                            std::shared_ptr<const ump::mj::Hand> hand)
  : client_(client),
    hand_(hand)
{
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
HandRenderer::~HandRenderer() {
}
/***********************************************************************//**
	@brief メンゼン牌表示
  @param[in] renderer 描画関数
  @param[in] offset 位置
  @param[in] index カーソルの差す番号
***************************************************************************/
void HandRenderer::renderMenzen(LayoutRenderer& renderer,
                                const wxump::LayoutPos& offset,
                                size_t index,
                                size_t selected) const {
  LayoutPos pos = offset;
  auto& menzen = getHand()->getMenzen();
  for(size_t i = 0, n = menzen.size(); i < n; i++) {
    renderer.renderHai(pos, HaiObject(menzen.at(i)).
                            setSelect(i == index).
                            setSelected(i == selected));
    pos.x.w += 1;
  }
}
/***********************************************************************//**
	@brief 鳴き牌全て表示
  @param[in] renderer 描画クラス
  @param[in] offset 位置
***************************************************************************/
void HandRenderer::renderAllMentsu(LayoutRenderer& renderer,
                                    const LayoutPos& offset) const {
  LayoutPos pos = offset;
  auto hand = getHand();
  for(size_t i = 0, n = hand->countMentsu(); i < n; i++) {
    pos = renderMentsu(renderer, pos, hand->getMentsu(i));
    pos.x.margin -= 1;
  }
}
/***********************************************************************//**
	@brief 捨て牌表示
  @param[in] renderer 描画クラス
  @param[in] offset 位置
  @param[in] isTurn 表示プレイヤーのターンならtrue(操作プレイヤーはfalse)
  @param[in] alpha 鳴ける牌のアルファ値
***************************************************************************/
void HandRenderer::renderKawa(LayoutRenderer& renderer,
                              const LayoutPos& offset,
                              bool isTurn, size_t alpha) const {
  LayoutPos pos = offset;
  auto& kawa = getHand()->getKawa();
  auto client = getClient();
  auto clientPlayer(client->getPlayer());
  for(size_t i = 0, n = kawa.size(); i < n; i++) {
    auto& sutehai = kawa.at(i);
    bool isNakiQ = isTurn &&
                    client->getLastSutehai() == &sutehai &&
                    (clientPlayer->canPon(sutehai.getHai()) ||
                     clientPlayer->canChi(sutehai.getHai()) ||
                     clientPlayer->canKan(sutehai.getHai()) ||
                     clientPlayer->canRon(sutehai.getHai()));
    renderer.renderHai(pos, 
                       HaiObject(sutehai.getHai()).
                       setTsumogiri(sutehai.isTsumogiri()).
                       setRichi(sutehai.isRichi()).
                       setNaki(sutehai.isNaki() || sutehai.isRon()).
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
	@brief 全プレイヤーで最後に捨てられた牌を表示する
  @param[in] renderer 描画クラス
  @param[in] offset 位置
  @return 描画後の次の牌表示位置
***************************************************************************/
LayoutPos HandRenderer::renderRonHai(LayoutRenderer& renderer,
                                      const LayoutPos& offset) const {
  LayoutPos pos = offset;
  auto& lastSutehai = getClient()->getLastSutehai();
  if(lastSutehai->isRon()) {
    renderer.renderHai(pos, HaiObject(lastSutehai->getHai()));
    pos.x -= LayoutValue(0, 0, 1);
  }
  return pos;
}
/***********************************************************************//**
	@brief 鳴き牌表示
  @param[in] renderer 描画クラス
  @param[in] pos 位置
  @param[in] mentsu 描画する鳴き牌
***************************************************************************/
LayoutPos HandRenderer::renderMentsu(LayoutRenderer& renderer,
                                      LayoutPos pos,
                                      const ump::mj::Mentsu& mentsu) const {
  for(int i = static_cast<int>(mentsu.size() - 1); i >= 0; i--) {
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
	$Id$
***************************************************************************/
}

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
#include "wxump/Client.hpp"
#include "wxump/HaiObject.hpp"
#include "wxump/HaiRender.hpp"
#include "wxump/Layout.hpp"
#include "wxump/LayoutRenderer.hpp"

namespace wxump {
/***********************************************************************//**
	@brief 捨て牌の表示
  @param[in] offset 描画位置
  @param[in] kawa 牌
  @param[in] isTurn 描画するプレイヤーのターンならtrue
  @param[in] alpha 設定する透明度
  @return 描画後位置(右)
***************************************************************************/
LayoutPos HaiRender::renderKawa(const LayoutPos& offset,
                                const ump::mj::Kawa& kawa,
                                bool isTurn,
                                size_t alpha) const {
  assert(client_);
  LayoutPos pos = offset;
  for(size_t i = 0, n = kawa.size(); i < n; i++) {
    auto& sutehai = kawa.at(i);
    bool isNakiQ = isTurn &&
                   client_->getLastSutehai() == &sutehai &&
                   canClientNaki(sutehai.getHai());
    renderer_.renderHai(pos, HaiObject(sutehai).
                              setNakiQ(isNakiQ).
                              setAlpha(alpha));
    pos.x += sutehai.isRichi()
              ? LayoutValue(0, 1, 0)
              : LayoutValue(1, 0, 0);
    if(i % 6 == 5) {
      pos.x.margin += 1;
    }
  }
  pos.x.margin += 1;
  return pos;
}
/***********************************************************************//**
	@brief 牌表示
  @param[in] offset 描画位置
  @param[in] hai 牌
***************************************************************************/
void HaiRender::renderHai(const LayoutPos& offset,
                          const ump::mj::Hai* hai) const {
  renderer_.renderHai(offset, HaiObject(hai));
}
/***********************************************************************//**
	@brief メンツ表示
  @param[in] offset 描画位置
  @param[in] mentsu メンツ
  @return 描画後位置(左)
***************************************************************************/
LayoutPos HaiRender::renderMentsu(const LayoutPos& offset,
                                  const ump::mj::Mentsu& mentsu) const {
  LayoutPos pos = offset;
  for(int i = static_cast<int>(mentsu.size() - 1); i >= 0; i--) {
    renderer_.renderHai(pos,
                        HaiObject(mentsu.isKantsu() &&
                                  mentsu.isAnko() &&
                                  (i == 0 || i == 3)
                                  ? ump::mj::Hai::GetUnknown()
                                  : mentsu.at(i)));
    pos.x.w -= 1;
  }
  pos.x.margin -= 1;
  return pos;
}
/***********************************************************************//**
	@brief メンゼン表示
  @param[in] offset 描画位置
  @param[in] menzen メンゼン牌
  @param[in] cursor カーソルが指す牌番号
  @param[in] choice 鳴きで選択した牌番号
  @return 描画後位置(右)
***************************************************************************/
LayoutPos HaiRender::renderMenzen(const LayoutPos& offset,
                                  const ump::mj::HaiArray& menzen,
                                  const ump::mj::HaiArray& richiableHais,
                                  size_t cursor,
                                  size_t choice) const {
  LayoutPos pos = offset;
  for(size_t i = 0, n = menzen.size(); i < n; i++) {
    bool richiableHai = richiableHais.hasSame(menzen.at(i));
    renderer_.renderHai(pos,
                        HaiObject(menzen.at(i)).
                          setSelect(i == cursor).
                          setSelected(i == choice).
                          setRichiable(richiableHai));
    pos.x.w += 1;
  }
  pos.x.margin += 1;
  return pos;
}
/***********************************************************************//**
	@brief 最後の捨て牌（ロン牌）表示
  @param[in] offset 描画位置
  @param[in] hai ロン牌
  @return 次の描画位置（左）
***************************************************************************/
LayoutPos HaiRender::renderLastSutehai(const LayoutPos& offset,
                                       const ump::mj::Sutehai& hai) const {
  LayoutPos pos = offset;
  if(hai.isRon()) {
    renderer_.renderHai(pos, HaiObject(hai.getHai()));
    pos.x -= LayoutValue(1, 0, 1);
  }
  return pos;
}
/***********************************************************************//**
	@brief ドラ表示
  @param[in] offset 描画位置
  @param[in] dora ドラ牌
  @return 次の描画位置（左）
***************************************************************************/
LayoutPos HaiRender::renderDora(const LayoutPos& offset,
                                const ump::mj::HaiArray& dora) const {
  LayoutPos pos = offset;
  for(auto hai : dora) {
    pos.x.w -= 1;
    renderer_.renderHai(pos, HaiObject(hai));
  }
  return pos;
}
/***********************************************************************//**
	@brief クライアントが対象の牌で行動できるかチェック
  @param[in] hai 牌
  @return 鳴けるならtrue
***************************************************************************/
bool HaiRender::canClientNaki(const ump::mj::Hai* hai) const {
  auto clientPlayer(client_->getPlayer());
  return (clientPlayer->canPon(hai) ||
          clientPlayer->canChi(hai) ||
          clientPlayer->canKan(hai) ||
          clientPlayer->canRon(hai));
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

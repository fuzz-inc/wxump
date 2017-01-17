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
#include "wxump/TakuInfo.hpp"

namespace wxump {
static const LayoutValue HEIGHT(0, 1, 2);
/***********************************************************************//**
	@brief コンストラクタ
  @param[in] client クライアント
***************************************************************************/
TakuInfo::TakuInfo(std::shared_ptr<Client> client)
  : client_(client)
{
  Application::Get()->attachListener(this);
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
TakuInfo::~TakuInfo() {
}
/***********************************************************************//**
	@copydoc Application::Listner::onReceiveCommand
***************************************************************************/
void TakuInfo::onReceiveCommand(const ump::Command& command) {
  switch(command.getType()) {
  case ump::Command::TYPE_DORA:
    setDora(command);
    break;
  case ump::Command::TYPE_KYOKUSTART:
    resetDora();
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@brief 描画
  @param[in] renderer 描画クラス
***************************************************************************/
void TakuInfo::onPaint(LayoutRenderer& renderer) {
  if(auto bakaze = getClient()->getBakaze()) {
    auto client = getClient();
    LayoutRect rect(LayoutPos(LayoutValue(0, 0, 1), LayoutValue(0, 0, 1)),
                    LayoutSize(GetSize().width, LayoutValue(0, 1, 0)));
    renderer.renderText(rect, 
                  wxString::Format("%s%d局",
                                   Conversion::GetHaiString(bakaze),
                                   static_cast<int>(client->getOya() + 1)));
    rect.pos.x += LayoutValue(0, 3, 0);
    rect.pos.y += LayoutValue(0, 0.5, 0);
    rect.size.height *= 0.5;
    wxString text;
    int renchan = client->getRenchan();
    if(renchan > 0) {
      text = wxString::Format("%d本場 ", renchan);
    }
    text.Append(wxString::Format("残り%d",
                                  static_cast<int>(client->getRest())));
    renderer.renderText(rect, text);
    renderDora(renderer);
  }
}
/***********************************************************************//**
	@brief 表示するドラをセットする
  @param[in] command 受信コマンド
***************************************************************************/
void TakuInfo::setDora(const ump::Command& command) {
  auto hai = ump::mj::Hai::Get(command.getArg(0).c_str());
  if(doraPt_ < dora_.size()) {
    dora_[doraPt_] = hai;
  }
  else {
    dora_.push_back(hai);
  }
  doraPt_++;
}
/***********************************************************************//**
	@brief ドラをリセットする
***************************************************************************/
void TakuInfo::resetDora() {
  dora_.clear();
  doraPt_ = 2;
  for(size_t i = 0; i < 7; i++) {
    dora_.push_back(ump::mj::Hai::GetUnknown());
  }
}
/***********************************************************************//**
	@brief ドラを表示する
	@param[in] renderer 描画クラス
***************************************************************************/
void TakuInfo::renderDora(LayoutRenderer& renderer) {
  LayoutPos pos(GetSize().width - LayoutValue(0, 0, 1), 
                LayoutValue(0, 0, 1));
  for(auto hai : dora_) {
    pos.x.w -= 1;
    renderer.renderHai(pos, HaiObject(hai));
  }
}
/***********************************************************************//**
	@brief 高さ
***************************************************************************/
LayoutValue TakuInfo::GetHeight() {
  return HEIGHT;
}
/***********************************************************************//**
	@brief サイズ
***************************************************************************/
LayoutSize TakuInfo::GetSize() {
  return LayoutSize(Player::GetSize().width, HEIGHT);
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

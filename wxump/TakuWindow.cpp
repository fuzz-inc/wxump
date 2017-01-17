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
#include "wxump/ControlPlayer.hpp"
#include "wxump/ControlWindow.hpp"
#include "wxump/Conversion.hpp"
#include "wxump/HaiObject.hpp"
#include "wxump/Layout.hpp"
#include "wxump/LayoutRenderer.hpp"
#include "wxump/Player.hpp"
#include "wxump/ResultWindow.hpp"
#include "wxump/TakuWindow.hpp"
#include "wxump/TakuInfo.hpp"

namespace wxump {
static const size_t SUTEHAI_MAX = 6 * 4;
static const LayoutValue HAND_WIDTH(SUTEHAI_MAX - 1, 1,
                                    (SUTEHAI_MAX + 5) / 6);
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
TakuWindow::TakuWindow(wxWindow* parent, std::shared_ptr<Client> client)
  : super(parent, wxID_ANY, wxDefaultPosition,
          Application::Get()->getLayoutRenderer().getSize(GetSize())), 
    client_(client),
    timer_(new wxTimer(this)),
    type_(ump::Command::TYPE_NULL)
{
  SetBackgroundColour(wxColor(0, 128, 0));
  Bind(wxEVT_MOTION, &TakuWindow::onMouse, this);
  Bind(wxEVT_LEFT_DOWN, &TakuWindow::onMouse, this);
  Bind(wxEVT_RIGHT_DOWN, &TakuWindow::onMouse, this);
  Bind(wxEVT_PAINT, &TakuWindow::onPaint, this);
  Bind(wxEVT_TIMER, &TakuWindow::onTimer, this);
  timer_->Start(static_cast<int>(1000.0 / Application::Get()->getFPS()));
  Application::Get()->attachListener(this);
  info_.reset(new TakuInfo(client));
  result_.reset(new ResultWindow(parent, client_));
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
TakuWindow::~TakuWindow() {
  timer_->Stop();
  delete timer_;
}
/***********************************************************************//**
	@copydoc Application::Listner::onReceiveCommand
***************************************************************************/
void TakuWindow::onReceiveCommand(const ump::Command& command) {
  resetComment();
  switch(command.getType()) {
  case ump::Command::TYPE_GAMESTART:
    onGameStart();
    break;
  case ump::Command::TYPE_AGARI:
    onAgari(command);
    break;
  case ump::Command::TYPE_SAY:
    onSay(command);
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@copydoc Application::Listener::onChangeLayout
***************************************************************************/
void TakuWindow::onChangeLayout(const LayoutRenderer& renderer) {
  SetMinClientSize(renderer.getSize(GetSize()));
}
/***********************************************************************//**
	@brief プレイヤーのコメントをセットする
  @param[in] command サーバーからのコマンド
***************************************************************************/
void TakuWindow::onSay(const ump::Command& command) {
  auto seat = ump::Command::StringToSeat(command.getArg(0).c_str());
  players_.at(seat)->
    setComment(Conversion::GetSayString(command.getArg(1).c_str()));
}
/***********************************************************************//**
	@brief 全てのプレイヤーのコメントをリセット
***************************************************************************/
void TakuWindow::resetComment() {
  for(auto player : players_) {
    player->resetComment();
  }
}
/***********************************************************************//**
	@brief 一定時間の間隔で通る関数
  @param[in] 時間イベント
***************************************************************************/
void TakuWindow::onTimer(wxTimerEvent &event) {
  onUpdate();
  Refresh();
}
/***********************************************************************//**
	@brief アップデート
***************************************************************************/
void TakuWindow::onUpdate() {
  auto command = client_->getCommand();
  if(type_ == command.getType() &&
     (command.getType() == ump::Command::TYPE_NAKI_Q ||
     command.getType() == ump::Command::TYPE_SUTEHAI_Q)) {
    countFlashTime_++;
  }
  else {
    countFlashTime_ = 0;
  }
  for(auto player : players_) {
    player->setTime(countFlashTime_);
  }
  type_ = command.getType();
}
/***********************************************************************//**
	@brief 特定のマウス操作に反応して通る関数
  @param[in] マウスイベント
***************************************************************************/
void TakuWindow::onMouse(wxMouseEvent& event) {
  auto pos = event.GetPosition();
  auto& renderer = Application::Get()->getLayoutRenderer();
  LayoutRect rect(LayoutPos(LayoutValue(), TakuInfo::GetHeight()),
                  Player::GetSize());
  if(controlPlayer_) {
    for(int i = 0, n = static_cast<int>(players_.size() - 1); i < n; i++) {
      rect.pos.y += rect.size.height;
    }
    auto renderRect = renderer.getRect(rect);
    controlPlayer_->onMouse(renderer, event, pos - renderRect.GetPosition());
  }
  if(event.RightDown()) {
    Application::Get()->onCancel();
  }
}
/***********************************************************************//**
	@brief 描画クラス
  @param 描画イベント
***************************************************************************/
void TakuWindow::onPaint(wxPaintEvent &event) {
  std::lock_guard<std::mutex> lock(Application::Get()->getMutex());
  auto dc = std::make_shared<wxPaintDC>(this);
  auto& renderer = Application::Get()->getLayoutRenderer();
  renderer.beginRender(dc);
  info_->onPaint(renderer);
  LayoutPos pos(LayoutValue(), TakuInfo::GetHeight());
  for(size_t i = 1, n = players_.size(); i <= n; i++) {
    auto player = players_.at((getClient()->getSeat() + i) % n);
    player->onPaint(renderer, pos);
    pos.y += Player::GetSize().height;
  }
  renderer.endRender();
}
/***********************************************************************//**
	@brief ゲーム開始時の処理
***************************************************************************/
void TakuWindow::onGameStart() {
  players_.clear();
  for(auto player : getClient()->getPlayers()) {
    if(getClient()->getPlayer() == player) {
      controlPlayer_ = std::make_shared<ControlPlayer>(getClient(), player);
      players_.push_back(controlPlayer_);
    }
    else {
      players_.push_back(std::make_shared<Player>(getClient(), player));
    }
  }
}
/***********************************************************************//**
	@brief アガリ時の処理
***************************************************************************/
void TakuWindow::onAgari(const ump::Command& command) {
  result_->setPlayer(client_->getPlayer(command.getArg(0).c_str()));
}
/***********************************************************************//**
	@brief 卓のサイズを求める関数
  @return 卓のサイズ
***************************************************************************/
LayoutSize TakuWindow::GetSize() {
  return LayoutSize(Player::GetSize().width,
                    Player::GetSize().height * 4 + TakuInfo::GetHeight());
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

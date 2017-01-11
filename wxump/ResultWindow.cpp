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
#include "wxump/HandRenderer.hpp"
#include "wxump/Layout.hpp"
#include "wxump/LayoutRenderer.hpp"
#include "wxump/Player.hpp"
#include "wxump/ResultWindow.hpp"

namespace wxump {
static const LayoutSize SIZE(LayoutValue(4 * 4 + 2 + 1, 0, 4 + 1),
                             LayoutValue(0, 9, 2));
static const LayoutSize WINDOW_SIZE =
  SIZE + LayoutSize(LayoutValue(0, 0, 2), LayoutValue(0, 0, 2));
static const LayoutValue POINT_STR_WIDTH(10, 0, 1);
static const LayoutValue YAKU_STR_WIDTH(7, 0, 1);
static const LayoutValue POINT_STR_POSX =
  (SIZE.width - POINT_STR_WIDTH) * 0.5;
static const LayoutValue YAKU_STR_POSX =
  (SIZE.width - YAKU_STR_WIDTH) * 0.5;
enum RESULT {
  RESULT_RYUUKYUKU,
  RESULT_AGARI,
  RESULT_GAME_END,
};
enum TOTAL{
  TOTAL_STR,
  TOTAL_FU,
  TOTAL_HAN,
  TOTAL_MAX
};
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
ResultWindow::ResultWindow(wxWindow* parent,
                           std::shared_ptr<Client> client)
  : super(parent, wxID_ANY, wxDefaultPosition, 
          Application::Get()->getLayoutRenderer().getSize(WINDOW_SIZE)), 
    client_(client),
    isResultPoint(false)
{
  SetWindowStyle(wxSTAY_ON_TOP);
  Hide();
  Bind(wxEVT_LEFT_DOWN, &ResultWindow::onMouse, this);
  Bind(wxEVT_MOUSEWHEEL, &ResultWindow::onMouse, this);
  Bind(wxEVT_PAINT, &ResultWindow::onPaint, this);
  Application::Get()->attachListener(this);
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
ResultWindow::~ResultWindow() {
}
/***********************************************************************//**
	@copydoc Application::Listner::onReceiveCommand
***************************************************************************/
void ResultWindow::onReceiveCommand(const ump::Command& command) {
  switch(command.getType()) {
  case ump::Command::TYPE_GAMEEND:
    onGameEnd(command);
    show();
    break;
  case ump::Command::TYPE_KYOKUSTART:
    hide();
    break;
  case ump::Command::TYPE_KYOKUEND:
    break;
  case ump::Command::TYPE_AGARI:
    onAgari(command);
    setIsResultPoint(true);
    break;
  case ump::Command::TYPE_POINT:
    onPoint(command);
    break;
  case ump::Command::TYPE_READY_Q:
    show();
    break;
  case ump::Command::TYPE_RYUKYOKU:
    setIsResultPoint(true);
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@brief ウィンドウサイズ変更処理
  @param[in] renderer 描画クラス
***************************************************************************/
void ResultWindow::onChangeLayout(const LayoutRenderer& renderer) {
  SetClientSize(renderer.getSize(WINDOW_SIZE));

  if(bitmap_) {
    bitmap_->SetWidth(Application::Get()->getLayoutRenderer().
                      getSize(SIZE).GetWidth());
    bitmap_->SetHeight(Application::Get()->getLayoutRenderer().
                      getSize(SIZE).GetHeight());
  }
}
/***********************************************************************//**
	@brief ウィンドウ表示時
***************************************************************************/
void ResultWindow::show() {
  CenterOnParent();
  auto& renderer = beginRender(RESULT_RYUUKYUKU);
  switch (resultType_) {
  case RESULT_RYUUKYUKU:
    renderTitleText(renderer, *wxWHITE, "流局");
    break;
  case RESULT_AGARI:
    renderTitleText(renderer, *wxWHITE, "結果");
    break;
  case RESULT_GAME_END:
    renderTitleText(renderer, *wxWHITE, "終局");
    break;
  default:
    break;
  }
  endRender(renderer);
  super::Show();
}
/***********************************************************************//**
	@brief ウィンドウを隠す処理＋初期化
***************************************************************************/
void ResultWindow::hide() {
  clear();
  player_ = nullptr;
  addMargin_ = 0;
  setIsResultPoint(false);
  super::Hide();
}
/***********************************************************************//**
	@brief リザルト情報初期化
***************************************************************************/
void ResultWindow::clear() {
  bitmap_.reset();
}
/***********************************************************************//**
	@brief マウス処理
  @param[in] event マウスイベント
***************************************************************************/
void ResultWindow::onMouse(wxMouseEvent &event) {
  if(event.LeftDown()) {
    client_->replyCommand(ump::Command(ump::Command::TYPE_YES));
    Show(false);
  }
  else if(auto delta = event.GetWheelRotation()) {
    int cursor = Application::Get()->getLayoutRenderer().
                  getPos(cursor_.pos).y;
    int height = Application::Get()->getLayoutRenderer().getSize(SIZE).y;
    if(cursor >= height) {
      addMargin_ -= delta;
      if(addMargin_ < 0) {
        addMargin_ = 0;
      }
      int maxMargin = (cursor - height) * 0.25;
      if(addMargin_ > maxMargin) {
        addMargin_ = maxMargin;
      }
    }
  }
}
/***********************************************************************//**
	@brief 描画
  @param[in] event 描画イベント
***************************************************************************/
void ResultWindow::onPaint(wxPaintEvent& event) {
  auto& renderer = Application::Get()->getLayoutRenderer();
  renderer.beginRender(std::make_shared<wxPaintDC>(this));
  renderer.renderRect(LayoutRect(LayoutPos(), WINDOW_SIZE), 
                      wxColour(0, 0, 0, 200),
                      wxNullColour);
  if(bitmap_) {
    renderer.renderBitmap(LayoutPos(LayoutValue(0, 0, 1), 
                                    LayoutValue(0, 0, 1 - addMargin_)),
                          *bitmap_);
  }
  renderer.endRender();
}
/***********************************************************************//**
	@brief アガった時
  @param[in] command サーバーコマンド
***************************************************************************/
void ResultWindow::onAgari(const ump::Command& command) {
  clear();
  auto& renderer = beginRender(RESULT_AGARI);
  int totalHan = 0;
  renderAgariHais(renderer);
  for(size_t i = 3, n = command.countArg(); i < n; i += 2) {
    int han = wxAtoi(command.getArg(i + 1));
    renderText(renderer, *wxWHITE,
               command.getArg(i), 
               wxString::Format("%d翻", han));
    totalHan += han;
  }
  LayoutPos start(POINT_STR_POSX, cursor_.pos.y);
  LayoutPos end(start.x + POINT_STR_WIDTH, cursor_.pos.y);
  renderer.renderLine(start, end, *wxWHITE);
  renderText(renderer, *wxWHITE, wxEmptyString,
             wxString::Format("%s符 %d翻", 
                              command.getArg(2),
                              totalHan));
  wxString agari = command.getArg(1);
  if(wxAtoi(agari) != 0) {
    agari += "点";
  }
  renderText(renderer, *wxWHITE, wxEmptyString, agari);
  endRender(renderer);
}
/***********************************************************************//**
	@brief 点数移動がある時
  @param[in] command サーバーコマンド
***************************************************************************/
void ResultWindow::onPoint(const ump::Command& command) {
  if(getIsResultPoint()) {
    auto& renderer = beginRender(RESULT_RYUUKYUKU);
    auto player = client_->getPlayer(command.getArg(0).c_str());
    wxColour color = isClientPlayer(player) ? *wxCYAN : *wxWHITE;
    auto zikaze = player->getZikaze();
    cursor_.pos.x = POINT_STR_POSX;
    cursor_.size.width = POINT_STR_WIDTH;
    renderText(renderer, color,
               wxString::Format("%s家 %s", 
                                Conversion::GetHaiString(zikaze),
                                player->getName()), 
               command.getArg(1));
    endRender(renderer);
  }
}
/***********************************************************************//**
	@brief ゲーム終了時
  @param[in] command サーバーコマンド
***************************************************************************/
void ResultWindow::onGameEnd(const ump::Command &command) {
  clear();
  auto& renderer = beginRender(RESULT_GAME_END);
  auto client = getClient();
  cursor_.pos.x = POINT_STR_POSX;
  cursor_.size.width = POINT_STR_WIDTH;
  for(int i = 0; i < client->countPlayer(); i++) {
    auto seat = command.getArg(i << 1).c_str();
    auto player = client->getPlayer(seat);
    wxColour color = isClientPlayer(player) ? *wxCYAN : *wxWHITE;
    wxString numStr = Conversion::GetSeatNumber(seat) + ":";
    renderText(renderer, color,
               wxString::Format("%i位 ", i + 1) + numStr +
                                player->getName(),
               command.getArg((i << 1) + 1));
  }
  endRender(renderer);
}
/***********************************************************************//**
	@brief 描画開始
  @param[in] リザルトの種類(アガリ、流局、終局)
  @return 描画クラス
***************************************************************************/
LayoutRenderer& ResultWindow::beginRender(int resultType) {
  auto& renderer = Application::Get()->getLayoutRenderer();
  if(!bitmap_) {
    LayoutSize SIZEx2 = LayoutSize(SIZE.width, SIZE.height * 2);
    bitmap_.reset(new wxBitmap(renderer.getSize(SIZEx2)));
    cursor_ = LayoutRect(LayoutPos(YAKU_STR_POSX,
                                    LayoutValue(0, 1.5, 0)),
                         LayoutSize(YAKU_STR_WIDTH,
                                    LayoutValue(0, 0.5, 0)));
    resultType_ = resultType;
  }
  renderer.beginRender(std::make_shared<wxMemoryDC>(*bitmap_));
  return renderer;
}
/***********************************************************************//**
	@brief 描画終了
  @param 終了する描画クラス
***************************************************************************/
void ResultWindow::endRender(LayoutRenderer& renderer) {
  renderer.endRender();
}
/***********************************************************************//**
	@brief テキスト左右描画
  @param[in] renderer 描画クラス
  @param[in] color 色
  @param[in] text 左詰めテキスト
  @param[in] rightText 右詰めテキスト
***************************************************************************/
void ResultWindow::renderText(LayoutRenderer& renderer, 
                              const wxColour& color, 
                              const wxString& text, 
                              const wxString& rightText) {
  renderer.renderText(cursor_, text, color);
  renderer.renderText(cursor_, rightText, color, 1.0f);
  cursor_.pos.y += cursor_.size.height;
}
/***********************************************************************//**
	@brief テキスト中央描画
  @param[in] renderer 描画クラス
  @param[in] color 色
  @param[in] text テキスト
***************************************************************************/
void ResultWindow::renderTitleText(LayoutRenderer& renderer,
                                    const wxColour& color,
                                    const wxString& text) {
  LayoutRect rect(LayoutPos(POINT_STR_POSX,
                            LayoutValue(0, 0.5, 0)),
                  LayoutSize(POINT_STR_WIDTH,
                             LayoutValue(0, 0.75, 0)));
  renderer.renderText(rect, text, color, 0.5f);
}
/***********************************************************************//**
	@brief アガったプレイヤーの牌を描画
  @param[in] renderer 描画クラス
***************************************************************************/
void ResultWindow::renderAgariHais(LayoutRenderer& renderer) {
  assert(getPlayer());
  LayoutPos offset(LayoutValue(), cursor_.pos.y);
  HandRenderer hand(getClient(), getPlayer());
  hand.renderMenzen(renderer, offset);
  offset = offset + LayoutSize((ResultWindow::GetWinSize().width -
                                  LayoutValue(1, 0, 0)),
                                LayoutValue());
  offset = hand.renderRonHai(renderer, offset);
  hand.renderAllMentsu(renderer, offset);
  cursor_.pos.y += LayoutValue(0, 1, 1);
}
/***********************************************************************//**
	@brief
***************************************************************************/
bool ResultWindow::isClientPlayer(
    std::shared_ptr<const ump::mj::Player> player) const{
  return player == client_->getPlayer();
}
/***********************************************************************//**
	@brief リザルト画面のサイズを求める
  @return リザルト画面サイズ
***************************************************************************/
const LayoutSize& ResultWindow::GetWinSize() {
  return SIZE;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

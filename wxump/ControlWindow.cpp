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
#include "wxump/ControlWindow.hpp"
#include "wxump/Conversion.hpp"
#include "wxump/Id.hpp"
#include "wxump/Layout.hpp"
#include "wxump/Player.hpp"

namespace wxump {
static const size_t INVALID_SELECT  = std::numeric_limits<size_t>::max();
/***********************************************************************//**
	@brief コンストラクタ
  @param[in] parent 親ウィンドウ
  @param[in] client クライアント
***************************************************************************/
ControlWindow::ControlWindow(wxWindow* parent,
                             std::shared_ptr<Client> client)
  : super(parent),
    client_(client),
    choice_(INVALID_SELECT)
{
  createButton();
  SetBackgroundColour(wxColour(0, 128, 0));
  Application::Get()->attachListener(this);
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
ControlWindow::~ControlWindow() {
}
/***********************************************************************//**
	@copydoc Application::Listner::onReceiveCommand
***************************************************************************/
void ControlWindow::onReceiveCommand(const ump::Command& command) {
  switch(command.getType()) {
  case ump::Command::TYPE_KYOKUSTART:
    setKyokuStart();
    break;
  case ump::Command::TYPE_KYOKUEND:
    setButtonDisable();
    break;
  case ump::Command::TYPE_NAKI_Q:
    doAutoAgari(command);
    doAutoNakiCancel(command);
    setButtonEnable(command);
    break;
  case ump::Command::TYPE_SAY:
    setButtonDisable();
    break;
  case ump::Command::TYPE_SUTEHAI_Q:
    doAutoSutehai(command);
    doAutoAgari(command);
    setButtonEnable(command);
    setChoice(INVALID_SELECT);
    break;
  case ump::Command::TYPE_TSUMO:
  case ump::Command::TYPE_SUTEHAI:
    setButtonDisable();
    setChoice(INVALID_SELECT);
    break;
  case ump::Command::TYPE_TENPAI_Q:
    doAutoSayTenpai(command);
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@brief 牌を選択した
	@param[in] index 選択した牌の番号
***************************************************************************/
void ControlWindow::onSelectHai(size_t index) {
  auto client = getClient();
  auto player = client->getPlayer();
  auto menzen(player->getMenzen());
  switch(client->getCommand().getType()) {
  case ump::Command::TYPE_SUTEHAI_Q:
    if(isToggle(BUTTON_KAN)) {
      auto hai = menzen.at(index);
      if(player->findKakanMentsu(hai)) {
        doReply(ump::Command(ump::Command::TYPE_KAKAN).
                append(hai->toString()));
      }
      else if(menzen.countSame(hai) == 4) {
        ump::mj::HaiArray hais;
        for(int i = 0; i < 4; i++) {
          hais.append(menzen.removeSame(hai));
        }
        doReply(ump::Command(ump::Command::TYPE_ANKAN).
                append(hais.toString()));
      }
      setToggle(BUTTON_KAN, false);
    }
    else {
      doSutehai(index);
    }
    break;
  case ump::Command::TYPE_NAKI_Q:
    if(isToggle(BUTTON_PON) || isToggle(BUTTON_CHI)) {
      if(choice_ == INVALID_SELECT) {
        setChoice(index);
      }
      else if(choice_ == index) {
        setChoice(INVALID_SELECT);
      }
      else {
        auto hai = ump::mj::Hai::Get(client->getCommand().
                                      getArg(0).c_str());
        ump::mj::HaiArray hais;
        hais.append(menzen.at(choice_)).
             append(menzen.at(index));
        if(isToggle(BUTTON_PON)) {
          if(player->canPon(hais, hai)) {
            doReply(ump::Command(ump::Command::TYPE_PON).
                    append(hais.toString()));
          }
          setToggle(BUTTON_PON, false);
        }
        else if(isToggle(BUTTON_CHI)) {
          if(player->canChi(hais, hai)) {
            doReply(ump::Command(ump::Command::TYPE_CHI).
                    append(hais.toString()));
          }
          setToggle(BUTTON_CHI, false);
        }
        setChoice(INVALID_SELECT);
      }
    }
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@brief キャンセルした
***************************************************************************/
void ControlWindow::onCancel() {
  auto client = getClient();
  switch(client->getCommand().getType()) {
  case ump::Command::TYPE_NAKI_Q:
    doReply(ump::Command(ump::Command::TYPE_NO));
    setChoice(INVALID_SELECT);
    break;
  case ump::Command::TYPE_READY_Q:
    doReply(ump::Command(ump::Command::TYPE_YES));
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@brief ボタンの作成
***************************************************************************/
void ControlWindow::createButton() {
  static const size_t BUTTON_MAX = BUTTON_BOTTOM - BUTTON_TOP - 1;
  static const wxString LABELS[BUTTON_MAX] = {
    "自動和了", 
    "鳴かない", 
    "テンパイ", 
    "リーチ", 
    "和了", 
    "ポン", 
    "チー", 
    "カン", 
    "キャンセル"
  };
  auto sizer = new wxGridSizer(BUTTON_MAX);
  for(int i = 0; i < BUTTON_MAX; i++) {
    int id = BUTTON_TOP + 1 + i;
    switch(id) {
    case BUTTON_AGARI:
    case BUTTON_CANCEL:
      {
        auto button = new wxButton(this, id, LABELS[i]);
        button->Bind(wxEVT_BUTTON, &ControlWindow::onButton, this);
        buttons_[id] = button;
        sizer->Add(button, wxSizerFlags().Expand().Proportion(1));
      }
      break;
    default:
      {
        auto button = new wxToggleButton(this, id, LABELS[i]);
        button->Bind(wxEVT_TOGGLEBUTTON, 
                     &ControlWindow::onToggleButton, this);
        buttons_[id] = button;
        sizer->Add(button, wxSizerFlags().Expand().Proportion(1));
      }
      break;
    }
  }
  SetSizerAndFit(sizer);
}
/***********************************************************************//**
	@brief 局開始時
***************************************************************************/
void ControlWindow::setKyokuStart() {
  setToggle(BUTTON_AUTO_AGARI, false);
  setToggle(BUTTON_AUTO_SAY_TENPAI, true);
  setToggle(BUTTON_AUTO_NAKI_CANCEL, false);
  setToggle(BUTTON_RICHI, false);
}
/***********************************************************************//**
	@brief 押せるボタンを決める
  @param[in] command 受信コマンド
***************************************************************************/
void ControlWindow::setButtonEnable(const ump::Command& command) {
  bool canRichi = command.hasArg(ump::Command::TYPE_RICHI);
  bool canRon = command.hasArg(ump::Command::TYPE_RON);
  bool canTsumo = command.hasArg(ump::Command::TYPE_TSUMO);
  bool canPon = command.hasArg(ump::Command::TYPE_PON);
  bool canChi = command.hasArg(ump::Command::TYPE_CHI);
  bool canKan = command.hasArg(ump::Command::TYPE_KAN);
  bool canAnkan = command.hasArg(ump::Command::TYPE_ANKAN);
  bool canKakan = command.hasArg(ump::Command::TYPE_KAKAN);
  bool enable[] = {
    canRichi,
    canRon || canTsumo,
    canPon,
    canChi,
    canKan || canAnkan || canKakan,
    canPon || canChi || canKan || canRon || canRichi
  };
  const int BUTTON_MAX = BUTTON_BOTTOM - BUTTON_RICHI;
  for(int i = 0; i < BUTTON_MAX; i++) {
    int id = BUTTON_RICHI + i;
    buttons_[id]->Enable(enable[i]);
  }

  if(canTsumo) {
    buttons_[BUTTON_AGARI]->SetLabel("ツモ");
  }
  if(canRon) {
    buttons_[BUTTON_AGARI]->SetLabel("ロン");
  }
}
/***********************************************************************//**
	@brief 全てのボタンを押せないようにする
***************************************************************************/
void ControlWindow::setButtonDisable() {
  setToggle(BUTTON_RICHI, false);
  for(int i = BUTTON_AGARI; i < BUTTON_BOTTOM; i++) {
    buttons_[i]->Disable();
  }
  buttons_[BUTTON_AGARI]->SetLabel("アガリ");
}
/***********************************************************************//**
	@brief 鳴き手動選択
  @param[in] index 選択した牌番号
***************************************************************************/
void ControlWindow::setChoice(size_t index) {
  choice_ = index;
  Application::Get()->setChoice(choice_);
}
/***********************************************************************//**
	@brief ポンボタンを押した処理
  @param[in] event コマンドイベント
***************************************************************************/
void ControlWindow::onPonDown(wxCommandEvent &event) {
  auto client = getClient();
  auto command = client->getCommand();
  auto hai = ump::mj::Hai::Get(command.getArg(0).c_str());
  hai = ump::mj::Hai::Get(hai->getColor(), hai->getNumber());
  auto player = client->getPlayer();
  auto menzen(player->getMenzen());
  size_t all_num = menzen.countSame(hai);
  size_t red_num = all_num - menzen.countEqual(hai);
  if(red_num == 0 ||  // 赤牌なし
     all_num == 2 ||  // 2枚のみ
     red_num == all_num) {  // 赤牌のみ
    ump::mj::HaiArray hais;
    hais.append(menzen.removeSame(hai)).
         append(menzen.removeSame(hai));
    doReply(ump::Command(ump::Command::TYPE_PON).
            append(hais.toString()));
    setToggle(BUTTON_PON, false);
  }
}
/***********************************************************************//**
	@brief チーボタンを押した処理
  @param[in] event コマンドイベント
***************************************************************************/
void ControlWindow::onChiDown(wxCommandEvent &event) {
  auto command = client_->getCommand();
  auto hai = ump::mj::Hai::Get(command.getArg(0).c_str());
  hai = ump::mj::Hai::Get(hai->getColor(), hai->getNumber());
  auto player = getClient()->getPlayer();
  auto menzen(player->getMenzen());
  bool shift_m1 = menzen.isInclude(hai->shift(-1));
  bool shift_m2 = menzen.isInclude(hai->shift(-2));
  bool shift_p1 = menzen.isInclude(hai->shift(1));
  bool shift_p2 = menzen.isInclude(hai->shift(2));
  // チーできる組が一組じゃないなら手動
  if(!((shift_p1 && shift_m1) && (shift_p2 || shift_m2))) {
    const ump::mj::Hai* hai1 = nullptr;
    const ump::mj::Hai* hai2 = nullptr;
    if(shift_m1 && shift_m2) {
      hai1 = hai->shift(-1);
      hai2 = hai->shift(-2);
    }
    else if(shift_p1 && shift_p2) {
      hai1 = hai->shift(1);
      hai2 = hai->shift(2);
    }
    else if(shift_p1 && shift_m1) {
      hai1 = hai->shift(1);
      hai2 = hai->shift(-1);
    }
    
    size_t all_num1 = menzen.countSame(hai1);
    size_t all_num2 = menzen.countSame(hai2);
    size_t red_num1 = all_num1 - menzen.countEqual(hai1);
    size_t red_num2 = all_num2 - menzen.countEqual(hai2);
    if((red_num1 == 0 || all_num1 == red_num1) && // 赤牌なしか赤牌のみ
       (red_num2 == 0 || all_num2 == red_num2)) { // ゞ
      ump::mj::HaiArray hais;
      hais.append(menzen.removeSame(hai1)).
           append(menzen.removeSame(hai2));
      doReply(ump::Command(ump::Command::TYPE_CHI).
              append(hais.toString()));
      setToggle(BUTTON_CHI, false);
    }
  }
}
/***********************************************************************//**
	@brief カンボタンを押した処理
  @param[in] event コマンドイベント
***************************************************************************/
void ControlWindow::onKanDown(wxCommandEvent &event) {
  auto client = getClient();
  auto player = client->getPlayer();
  auto command = client->getCommand();
  auto menzen(player->getMenzen());
  
  if(command.hasArg(ump::Command::TYPE_ANKAN) ||
          command.hasArg(ump::Command::TYPE_KAKAN)) {
    auto kanableHais = player->getKanPattern();
    if(kanableHais.size() == 1) {
      if(command.hasArg(ump::Command::TYPE_KAKAN)) {
        auto hai = menzen.removeSame(kanableHais[0].at(0));
        doReply(ump::Command(ump::Command::TYPE_KAKAN).
                append(hai->toString()));
      }
      else if(command.hasArg(ump::Command::TYPE_ANKAN)) {
        ump::mj::HaiArray hais = kanableHais[0];
        doReply(ump::Command(ump::Command::TYPE_ANKAN).
                append(hais.toString()));
      }
      setToggle(BUTTON_KAN, false);
    }
  }
  else {
    auto hai = ump::mj::Hai::Get(command.getArg(0).c_str());
    auto hais = player->getKanPattern(hai);
    auto test = hais.getHais().toString();
    doReply(ump::Command(ump::Command::TYPE_KAN).
            append(hais[0].toString()));
    setToggle(BUTTON_KAN, false);
  }
}
/***********************************************************************//**
	@brief 各種ボタンを押した時
  @param[in] event コマンドイベント
***************************************************************************/
void ControlWindow::onButton(wxCommandEvent& event) {
  switch(event.GetId()) {
  case BUTTON_AGARI:
    switch(getClient()->getCommand().getType()) {
    case ump::Command::TYPE_SUTEHAI_Q:
      doReply(ump::Command(ump::Command::TYPE_TSUMO));
      break;
    case ump::Command::TYPE_NAKI_Q:
      doReply(ump::Command(ump::Command::TYPE_RON));
      break;
    default:
      break;
    }
    break;
  case BUTTON_CANCEL:
    Application::Get()->onCancel();
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@brief 各種トグルボタンを押した時
  @param[in] event コマンドイベント
***************************************************************************/
void ControlWindow::onToggleButton(wxCommandEvent& event) {
  setSwitchToggle(event.GetId());
  switch(event.GetId()) {
  case BUTTON_PON:
    onPonDown(event);
    break;
  case BUTTON_CHI:
    onChiDown(event);
    break;
  case BUTTON_KAN:
    onKanDown(event);
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@brief ボタンのトグル状態をセットする
	@param[in] id ボタンのID
	@param[in] value 押下状態のとき真
***************************************************************************/
void ControlWindow::setToggle(int id, bool value) {
  if(auto button = dynamic_cast<wxToggleButton*>(buttons_.at(id))) {
    button->SetValue(value);
  }
}
/***********************************************************************//**
	@brief ボタンのトグル状態を取得する
	@param[in] id ボタンのID
	@return ボタンが押下状態のとき真
***************************************************************************/
bool ControlWindow::isToggle(int id) const {
  if(auto button = dynamic_cast<wxToggleButton*>(buttons_.at(id))) {
    return button->GetValue();
  }
  return false;
}
/***********************************************************************//**
	@brief 指定以外のトグル状態をfalseにする
  @param[in] id falseにしない例外ID
***************************************************************************/
void ControlWindow::setSwitchToggle(int id) {
  int toggleKind[] = {BUTTON_AGARI, BUTTON_PON, BUTTON_CHI, BUTTON_KAN};
  for(auto kind : toggleKind) {
    if(kind != id) {
      setToggle(kind, false);
    }
  }
}
/***********************************************************************//**
	@brief 牌を捨てる
  @param[in] index 選択した牌番号
***************************************************************************/
void ControlWindow::doSutehai(const size_t& index) {
  auto player = getClient()->getPlayer();
  auto menzen(player->getMenzen());
  ump::Command reply(ump::Command::TYPE_SUTEHAI);
  reply.append(menzen.at(index)->toString());
  if(index == menzen.size() - 1) {
    reply.append("tsumogiri");
  }
  if(isToggle(BUTTON_RICHI)) {
    reply.append(ump::Command::TYPE_RICHI);
  }
  doReply(reply);
}
/***********************************************************************//**
	@brief クライアントに返答する
  @param[in] command 返答用コマンド
***************************************************************************/
void ControlWindow::doReply(const ump::Command& command) {
  getClient()->replyCommand(command);
}
/***********************************************************************//**
	@brief 自動でアガる処理
  @param[in] command 受信コマンド
***************************************************************************/
void ControlWindow::doAutoAgari(const ump::Command& command) {
  if(isToggle(BUTTON_AUTO_AGARI)) {
    if(command.hasArg(ump::Command::TYPE_TSUMO)) {
      doReply(ump::Command(ump::Command::TYPE_TSUMO));
    }
    else if(command.hasArg(ump::Command::TYPE_RON)) {
      doReply(ump::Command(ump::Command::TYPE_RON));
    }
  }
}
/***********************************************************************//**
	@brief 自動で鳴かない処理
  @param[in] command 受信コマンド
***************************************************************************/
void ControlWindow::doAutoNakiCancel(const ump::Command& command) {
  if(isToggle(BUTTON_AUTO_NAKI_CANCEL) && 
     !command.hasArg(ump::Command::TYPE_RON)) {
    doReply(ump::Command(ump::Command::TYPE_NO));
  }
}
/***********************************************************************//**
	@brief 自動でテンパイ宣言しない処理
  @param[in] command 受信コマンド
***************************************************************************/
void ControlWindow::doAutoSayTenpai(const ump::Command& command) {
  doReply(ump::Command(isToggle(BUTTON_AUTO_SAY_TENPAI)
                        ? ump::Command::TYPE_YES
                        : ump::Command::TYPE_NO));
}
/***********************************************************************//**
	@brief リーチ時に自動でツモ牌を捨てる
  @param[in] command 受信コマンド
***************************************************************************/
void ControlWindow::doAutoSutehai(const ump::Command& command) {
  auto& player = getClient()->getPlayer();
  if(player->isRichi() &&
    !(command.hasArg(ump::Command::TYPE_TSUMO) ||
      command.hasArg(ump::Command::TYPE_ANKAN))) {
    doSutehai(player->getMenzen().size() - 1);
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

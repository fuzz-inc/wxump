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
#include "wxump/Config.hpp"
#include "wxump/LayoutRenderer.hpp"
#include "wxump/MainFrame.hpp"
#include "wxump/Server.hpp"

namespace wxump {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
Application::Application()
  : mainFrame_(nullptr), 
    mjConfig_(std::make_shared<ump::server::Config>()),
    fps_(30)
{
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Application::~Application() {
}
/***********************************************************************//**
	@brief 
***************************************************************************/
std::shared_ptr<ump::socket::Socket> Application::createSocket() const {
  return std::make_shared<ump::socket::TcpSocket>();
  //return std::make_shared<ump::socket::SslSocket>();
}
/***********************************************************************//**
	@brief リスナーを登録する
***************************************************************************/
void Application::attachListener(Listener* listener) {
  listeners_.push_back(listener);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Application::onReceiveCommand(const ump::Command& command) {
  CallAfter([&, command]() {
      for(auto listener : listeners_) {
        listener->onReceiveCommand(command);
      }
    });
}
/***********************************************************************//**
	@brief 牌選択処理
***************************************************************************/
void Application::onSelectHai(size_t index) {
  for(auto listener : listeners_) {
    listener->onSelectHai(index);
  }
}
/***********************************************************************//**
	@brief キャンセルボタン処理
***************************************************************************/
void Application::onCancel() {
  for(auto listener : listeners_) {
    listener->onCancel();
  }
}
/***********************************************************************//**
	@brief 牌のサイズを変更する
	@param[in] size 牌のサイズ
***************************************************************************/
void Application::changeHaiSize(HaiSize size) {
  auto& renderer = getLayoutRenderer();
  renderer.loadHaiImage(size);
  for(auto listener : listeners_) {
    listener->onChangeLayout(renderer);
  }
}
/***********************************************************************//**
	@brief 初期化
***************************************************************************/
bool Application::OnInit() {
  wxLog::SetActiveTarget(new wxLogStderr());
  wxInitAllImageHandlers();
  config_.reset(new Config());
  layoutRenderer_.reset(new LayoutRenderer());
  layoutRenderer_->loadHaiImage(HaiSize::SMALL);
  mainFrame_ = new MainFrame();
  mainFrame_->Show();
  SetTopWindow(mainFrame_);
  {
    auto server = std::make_shared<ump::server::Server>(createSocket());
    server->setConfig(mjConfig_);
    server_.reset(new ump::server::Thread(server));
    server_->start();
  }
  return super::OnInit();
}
/***********************************************************************//**
	@brief コマンドラインオプションを設定
***************************************************************************/
void Application::OnInitCmdLine(wxCmdLineParser& parser) {
  super::OnInitCmdLine(parser);
  parser.AddOption("m", "member", "number of player", 
                   wxCMD_LINE_VAL_NUMBER);
  parser.AddSwitch("O", "open", "open mode");
  parser.AddOption("L", "log", "log file prefix", wxCMD_LINE_VAL_STRING);
  parser.AddOption("a", "aka", "number of aka(5) dora", 
                   wxCMD_LINE_VAL_NUMBER);
}
/***********************************************************************//**
	@brief コマンドラインオプションを解析
***************************************************************************/
bool Application::OnCmdLineParsed(wxCmdLineParser& parser) {
//  {
//    long num;
//    if(parser.Found("m", &num)) {
//      mjConfig_->setPlayerMax(num);
//    }
//  }
  if(parser.FoundSwitch("O") == wxCMD_SWITCH_ON) {
    mjConfig_->setOpen(true);
  }
  {
    wxString prefix;
    if(parser.Found("L", &prefix)) {
      mjConfig_->setLogPrefix(prefix.ToStdString());
    }
  }
  {
    long num = 1;
    parser.Found("a", &num);
    mjConfig_->
      setDoraHaiNum(ump::mj::Hai::Get(ump::mj::Hai::COLOR_MANZU, 5), num).
      setDoraHaiNum(ump::mj::Hai::Get(ump::mj::Hai::COLOR_PINZU, 5), num).
      setDoraHaiNum(ump::mj::Hai::Get(ump::mj::Hai::COLOR_SOUZU, 5), num);
  }
  return super::OnCmdLineParsed(parser);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
int Application::OnExit() {
  server_.reset();
  return super::OnExit();
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

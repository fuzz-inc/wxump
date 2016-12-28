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
#include "wxump/Config.hpp"
#include "wxump/ConfigDialog.hpp"
#include "wxump/ControlWindow.hpp"
#include "wxump/Id.hpp"
#include "wxump/Layout.hpp"
#include "wxump/LogWindow.hpp"
#include "wxump/MainFrame.hpp"
#include "wxump/TakuWindow.hpp"

namespace wxump {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
MainFrame::MainFrame()
  : super(0, wxID_ANY, wxT("wxUMP")),
    client_(std::make_shared<Client>(Application::Get())), 
    takuWindow_(new TakuWindow(this, client_)), 
    controlWindow_(new ControlWindow(this, client_))
{
  setupMenuBar();
  setupToolBar();
  auto vbox = new wxBoxSizer(wxVERTICAL);
  vbox->Add(takuWindow_, wxSizerFlags().Expand().Proportion(1));
  vbox->Add(controlWindow_, wxSizerFlags().Expand());
  SetSizerAndFit(vbox);
  Application::Get()->attachListener(this);
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
MainFrame::~MainFrame() {
}
/***********************************************************************//**
	@copydoc Application::Listener::onReceiveCommand
***************************************************************************/
void MainFrame::onReceiveCommand(const ump::Command& command) {
  switch(command.getType()) {
  case ump::Command::TYPE_GAMESTART:
    doGameStart();
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@copydoc Application::Listener::onChangeLayout
***************************************************************************/
void MainFrame::onChangeLayout(const LayoutRenderer& renderer) {
  Fit();
}
/***********************************************************************//**
	@brief メニューバーのセットアップ
***************************************************************************/
void MainFrame::setupMenuBar() {
  wxMenuBar* menuBar = new wxMenuBar();
  {
    wxMenu* menu = new wxMenu();
    menu->Append(MENU_GAME_CONNECT, wxT("Connect server"));
    menu->Append(MENU_GAME_BOT, wxT("Run BOT"));
    menu->AppendSeparator();
    menu->Append(MENU_GAME_CONFIG, wxT("Config"));
    menu->AppendSeparator();
    menu->Append(MENU_GAME_QUIT, wxT("Quit"));
    menuBar->Append(menu, wxT("Game"));
  }
  {
    auto menu = new wxMenu();
    {
      auto subMenu = new wxMenu();
      subMenu->AppendRadioItem(MENU_VIEW_HAI_SIZE_SMALL, wxT("Small"));
      subMenu->AppendRadioItem(MENU_VIEW_HAI_SIZE_MIDDLE, wxT("Middle"));
      subMenu->AppendRadioItem(MENU_VIEW_HAI_SIZE_LARGE, wxT("Large"));
      menu->AppendSubMenu(subMenu, wxT("Hai size"));
    }
    menuBar->Append(menu, wxT("View"));
  }
  {
    auto menu = new wxMenu();
    menu->AppendCheckItem(MENU_WINDOW_LOG, wxT("Log"));
    menuBar->Append(menu, wxT("Window"));
  }
  SetMenuBar(menuBar);
  Bind(wxEVT_MENU_OPEN, &MainFrame::onOpenMenu, this);
  Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::onMenuSelect, this);
}
/***********************************************************************//**
	@brief ツールバーのセットアップ
***************************************************************************/
void MainFrame::setupToolBar() {
  auto toolBar = CreateToolBar();
  toolBar->AddControl(createButton(toolBar, MENU_GAME_CONNECT, 
                                   wxT("Connect")));
  toolBar->AddControl(createButton(toolBar, MENU_GAME_BOT, 
                                   wxT("BOT")));
  toolBar->Realize();
}
/***********************************************************************//**
	@brief 
***************************************************************************/
wxButton* MainFrame::createButton(wxWindow* parent, 
                                  wxWindowID id, 
                                  const wxString& label) {
  auto button = new wxButton(parent, id, label);
  button->Bind(wxEVT_BUTTON, &MainFrame::onMenuSelect, this);
  return button;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void MainFrame::onOpenMenu(wxMenuEvent& event) {
  
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void MainFrame::onMenuSelect(wxCommandEvent& event) {
  const auto& config = Application::Get()->getConfig();
  switch(event.GetId()) {
  case MENU_GAME_CONNECT:
    if(!client_->isOpen()) {
      client_->open(config.getHost(), config.getPort());
    }
    break;
  case MENU_GAME_BOT:
    {
      auto bot = std::make_shared
        <ump::client::Bot>(Application::Get()->getMjConfig(), 
                           Application::Get()->createSocket());
      bot->open(config.getHost(), config.getPort());
    }
    break;
  case MENU_GAME_CONFIG:
    {
      ConfigDialog dialog(this);
      dialog.ShowModal();
    }
    break;
  case MENU_GAME_QUIT:
    Close();
    break;
  case MENU_WINDOW_LOG:
    //auiManager_.GetPane(log_).Show(event.IsChecked());
    //auiManager_.Update();
    break;
  case MENU_VIEW_HAI_SIZE_SMALL:
    Application::Get()->changeHaiSize(Application::HaiSize::SMALL);
    break;
  case MENU_VIEW_HAI_SIZE_MIDDLE:
    Application::Get()->changeHaiSize(Application::HaiSize::MIDDLE);
    break;
  case MENU_VIEW_HAI_SIZE_LARGE:
    Application::Get()->changeHaiSize(Application::HaiSize::LARGE);
    break;
  default:
    break;
  }
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void MainFrame::doGameStart() {
  
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

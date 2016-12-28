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
#pragma once

#include "wxump/Application.hpp"

namespace wxump {
/***********************************************************************//**
	@brief ボタン操作を扱うクラス
***************************************************************************/
class ControlWindow
  : public wxPanel, 
    public Application::Listener
{
  typedef wxPanel super;
  
 private:
  std::shared_ptr<Client> client_;
  std::map<int, wxAnyButton*> buttons_;
  size_t choosing_;
  
 public:
  ControlWindow(wxWindow* parent,
                std::shared_ptr<Client> client);
  ~ControlWindow() override;
  
 protected:
  void onReceiveCommand(const ump::Command& command) override;
  void onSelectHai(size_t index) override;
  void onCancel() override;

 private:
  UMP_GETTER(Client, client_);

  void onPaint(wxPaintEvent& event);
  void onPonDown(wxCommandEvent& event);
  void onChiDown(wxCommandEvent& event);
  void onKanDown(wxCommandEvent& event);
  
  void createButton();
  
  void setKyokuStart();
  void setButtonEnable(const ump::Command command);
  void setButtonDisable();

  void onButton(wxCommandEvent& event);
  void onToggleButton(wxCommandEvent& event);

  void setToggle(int id, bool value);
  bool isToggle(int id) const;
  void setSwitchToggle(int id);
  
  bool doAutoAgari(const ump::Command& command);
  bool doAutoNakiCancel(const ump::Command& command);
  void doAutoSayTenpai(const ump::Command& command);
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}

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
#include "wxump/ConfigDialog.hpp"

namespace wxump {
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
ConfigDialog::ConfigDialog(wxWindow* parent)
  : super(parent, wxID_ANY, "Config"), 
    host_(new wxTextCtrl(this, wxID_ANY)), 
    port_(new wxTextCtrl(this, wxID_ANY))
{
  auto& config = Application::Get()->getConfig();
  auto sizer = new wxBoxSizer(wxVERTICAL);
  {
    //auto grid = new wxFlexGridSizer(2, 2, wxSize(4, 4));
    auto grid = new wxGridSizer(2, 2, wxSize(4, 4));

    grid->Add(new wxStaticText(this, wxID_ANY, "Host:"));
    host_->SetValue(config.getHost());
    grid->Add(host_);

    grid->Add(new wxStaticText(this, wxID_ANY, "Port:"));
    port_->SetValue(wxString::Format("%d", config.getPort()));
    grid->Add(port_);
                             
    sizer->Add(grid, wxSizerFlags().Expand().Border(wxALL, 16));
  }
  sizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 
             wxSizerFlags().Expand());
  SetSizerAndFit(sizer);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void ConfigDialog::EndModal(int id) {
  super::EndModal(id);
  if(id == wxID_OK) {
    auto& config = Application::Get()->getConfig();
    config.setHost(host_->GetValue());
    {
      long value;
      if(port_->GetValue().ToLong(&value)) {
        config.setPort(int(value));
      }
    }
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

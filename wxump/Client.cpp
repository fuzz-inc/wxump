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

namespace wxump {
/***********************************************************************//**
	@brief コンストラクタ
	@param[in] application アプリケーション
***************************************************************************/
Client::Client(Application* application)
  : super(application->getMjConfig(), 
          application->createSocket()), 
    application_(application)
{}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
Client::~Client() {
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Client::replyCommand(const ump::Command& command) {
  super::replyCommand(command, getCommand());
}
/***********************************************************************//**
	@brief コマンド受信
***************************************************************************/
void Client::onReceiveCommand(const ump::Command& command) {
  std::lock_guard<std::mutex> lock(application_->getMutex());
  command_ = command;
  wxLogMessage("[UMP] > %s", 
               wxString::FromUTF8(command.toString(false).c_str()).c_str());
  super::onReceiveCommand(command);
  switch(command.getType()) {
  case ump::Command::TYPE_HELLO:
    replyCommand(ump::Command(ump::Command::TYPE_HELLO).
                 append(std::string("ump=") + 
                        ump::Version::Get().toString()));
    break;
  default:
    break;
  }
  application_->onReceiveCommand(command);
}
/***********************************************************************//**
	@brief 
***************************************************************************/
void Client::onReplyCommand(const ump::Command& command) {
  wxLogMessage("[UMP] < %s", 
               wxString::FromUTF8(command.toString(false).c_str()).c_str());
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

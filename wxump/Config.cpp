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
#include "wxump/Config.hpp"

namespace wxump {
static const long VERSION = 20160929;

static const wxString HOST("HOST");
static const wxString PORT("PORT");
static const wxString PERSPECTIVE("PERSPECTIVE");

static const wxString DEFAULT_HOST("localhost");
static const int DEFAULT_PORT = 1326;
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
Config::Config()
  : super("jp.co.fuzz.wxump")
{
  if(ReadLong("VERSION", 0) != VERSION) {
    DeleteAll();
    Write("VERSION", VERSION);
  }
}
/***********************************************************************//**
	@brief 
  @param[in] value
***************************************************************************/
Config& Config::setHost(const wxString& value) {
  Write(HOST, value);
  return *this;
}
/***********************************************************************//**
	@brief
***************************************************************************/
wxString Config::getHost() const {
  return Read(HOST, DEFAULT_HOST);
}
/***********************************************************************//**
	@brief 
  @param[in] port
***************************************************************************/
Config& Config::setPort(int port) {
  Write(PORT, long(port));
  return *this;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
int Config::getPort() const {
  long value;
  Read(PORT, &value, long(DEFAULT_PORT));
  return int(value);
}
/***********************************************************************//**
	@brief 
  @param[in] value
***************************************************************************/
Config& Config::setPerspective(const wxString& value) {
  Write(PERSPECTIVE, value);
  return *this;
}
/***********************************************************************//**
	@brief 
***************************************************************************/
wxString Config::getPerspective() const {
  return Read(PERSPECTIVE, wxEmptyString);
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

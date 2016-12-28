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
#include "wxump/Conversion.hpp"

namespace wxump {
/***********************************************************************//**
	@brief 牌の表記文字を求める関数
  @param[in] 牌クラス
  @return 牌の表記文字
***************************************************************************/
wxString Conversion::getHaiString(const ump::mj::Hai* hai) {
  int color = hai->getColor();
  int number = hai->getNumber();
  wxString str = "?";
  
  if(color == 3) {
    switch(number) {
      case 1:
        str = "東";
        break;
      case 2:
        str = "南";
        break;
      case 3:
        str = "西";
        break;
      case 4:
        str = "北";
        break;
      case 5:
        str = "白";
        break;
      case 6:
        str = "發";
        break;
      case 7:
        str = "中";
        break;
    }
  }
  else {
    str = wxString(wxT("%i"), number);
    switch(color) {
      case 0:
        str.append("萬");
        break;
      case 1:
        str.append("筒");
        break;
      case 2:
        str.append("索");
        break;
    }
  }
  return str;
}
/***********************************************************************//**
	@brief サーバーコマンドからプレイヤーコメントを求める関数
  @param[in] コマンド文字列
  @return プレイヤーコメント文字
***************************************************************************/
wxString Conversion::getSayString(const char* chr) {
  wxString comment("エラー");
  const char* str[] = {
    "pon", "chi", "kan", "ankan", "kakan",
    "ron", "tsumo", "richi", "tenpai", "noten"
  };
  const wxString say[] = {
    "ポン", "チー", "カン", "カン", "カン",
    "ロン", "ツモ", "リーチ", "テンパイ", "ノーテン"
  };
  for(int i = 0; i < 10; i++) {
    if(!wxStrcmp(chr, str[i])) {
      comment = say[i];
      break;
    }
  }
  
  return comment;
}
/***********************************************************************//**
	@brief サーバーコマンドから卓の席番号を求める関数
  @param[in] chr コマンド文字列
  @return 席番号
***************************************************************************/
wxString Conversion::getSeatNumber(const char* chr) {
  wxString seatNumber;
  const char* str[] = {"A", "B", "C", "D"};
  for(int i = 0; i < 4; i++) {
    if(wxStrchr(chr, *str[i])) {
      seatNumber = wxString::Format(wxT("%iP"), i+1);
      break;
    }
  }
  return seatNumber;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}


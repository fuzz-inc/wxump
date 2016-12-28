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
#include "wxump/HaiImage.hpp"

namespace wxump {
/***********************************************************************//**
	@brief コンストラクタ
  @param[in] 通常文字牌の画像アドレス
  @param[in] 赤文字牌の画像アドレス
  @param[in] 牌の画像アドレス
***************************************************************************/
HaiImage::HaiImage(const char* pathNormal, 
                   const char* pathRed,
                   const char* pathBasis) {
  wxImage baseImage(pathBasis);
  wxASSERT(baseImage.IsOk());
  width_ = baseImage.GetWidth() / 2;
  height_ = baseImage.GetHeight();
  wxBitmap baseBitmap(baseImage.GetSubImage(wxRect(0, 0, width_, height_)));
  bitmaps_[ump::mj::Hai::GetUnknown()] = 
    wxBitmap(baseImage.GetSubImage(wxRect(width_, 0, width_, height_)));
  setupBitmap(pathNormal, baseBitmap, false);
  setupBitmap(pathRed, baseBitmap, true);
}
/***********************************************************************//**
	@brief 指定の牌画像を取得
  @param[in] hai 牌
  @return 牌のbitmap画像
***************************************************************************/
const wxBitmap& HaiImage::getBitmap(const ump::mj::Hai* hai) const {
  return bitmaps_.at(hai);
}
/***********************************************************************//**
	@brief 画像を切り分け保存
  @param[in] path 文字画像アドレス
  @param[in] wxBitmap 牌画像
  @param[in] isDora 赤文字の時true
***************************************************************************/
void HaiImage::setupBitmap(const char* path, 
                           const wxBitmap& baseBitmap, 
                           bool isDora) {
  wxImage image(path);
  wxASSERT(image.IsOk());
  wxASSERT(image.GetWidth() >= width_ * 9);
  wxASSERT(image.GetHeight() >= height_ * ump::mj::Hai::COLOR_MAX);
  for(int color = 0; color < ump::mj::Hai::COLOR_MAX; color++) {
    for(int num = 1; num <= 9; num++) {
      auto hai = ump::mj::Hai::Get(color, num, isDora);
      wxBitmap bitmap(baseBitmap);
      wxMemoryDC dc(bitmap);
      dc.DrawBitmap(wxBitmap(image.GetSubImage(wxRect((num - 1) * width_, 
                                                      color * height_, 
                                                      width_, height_))), 
                    0, 0);
      bitmaps_[hai] = bitmap;
    }
  }
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}

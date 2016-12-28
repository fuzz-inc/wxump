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

namespace wxump {
/***********************************************************************//**
	@brief 牌のサイズに応じた画面レイアウトの単位クラス
***************************************************************************/
class LayoutValue {
 public:
  float w;		/**< 牌の横サイズの数 */
  float h;		/**< 牌の縦サイズの数 */
  float margin;   /**< マージンの数 */
  
 public:
  LayoutValue(float w = 0, float h = 0, float margin = 0)
    : w(w),
      h(h),
      margin(margin)
  {}
  ~LayoutValue() = default;
  
  LayoutValue operator+(const LayoutValue& rhs) const {
    return LayoutValue(w + rhs.w, h + rhs.h, margin + rhs.margin);
  }
  
  LayoutValue operator-(const LayoutValue& rhs) const {
    return LayoutValue(w - rhs.w, h - rhs.h, margin - rhs.margin);
  }

  LayoutValue operator*(float rhs) const {
    return LayoutValue(w * rhs, h * rhs, margin * rhs);
  }

  LayoutValue& operator+=(const LayoutValue& rhs) {
    w += rhs.w;
    h += rhs.h;
    margin += rhs.margin;
    return *this;
  }

  LayoutValue& operator-=(const LayoutValue& rhs) {
    w -= rhs.w;
    h -= rhs.h;
    margin -= rhs.margin;
    return *this;
  }

  LayoutValue& operator*=(float rhs) {
    w *= rhs;
    h *= rhs;
    margin *= rhs;
    return *this;
  }

  LayoutValue operator-() const {
    return LayoutValue(-w, -h, -margin);
  }
};
/***********************************************************************//**
	@brief 牌のサイズの応じた画面レイアウトのサイズを表すクラス
***************************************************************************/
class LayoutSize {
 public:
  LayoutValue width;    /**< 幅 */
  LayoutValue height;   /**< 高さ */

 public:
  LayoutSize() = default;
  LayoutSize(const LayoutValue& width, const LayoutValue& height)
    : width(width), 
      height(height)
  {}
  ~LayoutSize() = default;

  LayoutSize operator+(const LayoutSize& rhs) const {
    return LayoutSize(width + rhs.width, height + rhs.height);
  }

  LayoutSize operator-(const LayoutSize& rhs) const {
    return LayoutSize(width - rhs.width, height - rhs.height);
  }

  LayoutSize operator*(float rhs) const {
    return LayoutSize(width * rhs, height * rhs);
  }
};
/***********************************************************************//**
	@brief 牌のサイズに応じた画面レイアウトの位置を表すクラス
***************************************************************************/
class LayoutPos {
 public:
  LayoutValue x;    /**< X座標 */
  LayoutValue y;    /**< Y座標 */

 public:
  LayoutPos() = default;
  LayoutPos(const LayoutValue& x, const LayoutValue& y)
    : x(x), 
      y(y)
  {}
  ~LayoutPos() = default;

  LayoutPos operator+(const LayoutPos& rhs) const {
    return LayoutPos(x + rhs.x, y + rhs.y);
  }

  LayoutPos operator+(const LayoutSize& rhs) const {
    return LayoutPos(x + rhs.width, y + rhs.height);
  }

  LayoutPos operator-(const LayoutPos& rhs) const {
    return LayoutPos(x - rhs.x, y - rhs.y);
  }

  LayoutPos operator-(const LayoutSize& rhs) const {
    return LayoutPos(x - rhs.width, y - rhs.height);
  }
};
/***********************************************************************//**
	@brief 牌のサイズに応じた画面レイアウトの短形を表すクラス
***************************************************************************/
class LayoutRect {
 public:
  LayoutPos pos;    /**< 位置 */
  LayoutSize size;    /**< サイズ */

 public:
  LayoutRect() = default;
  LayoutRect(const LayoutPos& pos, const LayoutSize& size)
    : pos(pos), 
      size(size)
  {}
  ~LayoutRect() = default;
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}

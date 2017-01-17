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
	@brief 牌の描画法をまとめたクラス
***************************************************************************/
class HaiRender {
 private:
  LayoutRenderer& renderer_;
  const std::shared_ptr<Client> client_;
  
 public:
  HaiRender(LayoutRenderer& renderer,
            std::shared_ptr<Client> client = nullptr)
    : renderer_(renderer),
      client_(client)
  {}
  ~HaiRender() = default;
  
  LayoutPos renderKawa(const LayoutPos& offset,
                       const ump::mj::Kawa& kawa,
                       bool isTurn = false,
                       size_t alpha = 255) const;
  LayoutPos renderMentsu(const LayoutPos& offset,
                         const ump::mj::Mentsu& mentsu) const;
  LayoutPos renderMenzen(const LayoutPos& offset,
                         const ump::mj::HaiArray& menzen,
                         const ump::mj::HaiArray& richiableHais = ump::mj::HaiArray(),
                         size_t cursor = std::numeric_limits<size_t>::max(),
                         size_t choice = std::numeric_limits<size_t>::max()
                         ) const;
  LayoutPos renderLastSutehai(const LayoutPos& offset,
                              const ump::mj::Sutehai& hai) const;
  
  LayoutPos renderDora(const LayoutPos& offset,
                       const ump::mj::HaiArray& dora) const;

};
/***********************************************************************//**
	$Id$
***************************************************************************/
}

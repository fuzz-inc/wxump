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
	@brief プレイヤークラス
***************************************************************************/
class Player {
 protected:
  std::shared_ptr<Client> client_;
  std::shared_ptr<const ump::mj::Player> player_;
  wxString comment_;
  int countTime_;
  
 public:
  Player(std::shared_ptr<Client> client, 
         std::shared_ptr<const ump::mj::Player> player);
  virtual ~Player();

  UMP_SETTER(Comment, comment_);
  UMP_SETTER(Time, countTime_);
  void resetComment();

  void onPaint(LayoutRenderer& renderer, 
               const LayoutPos& offset);
  
  virtual void renderHand(LayoutRenderer& renderer,
                          const LayoutPos& offset,
                          bool isResult = false) const;
  const bool isMjPlayer(const ump::mj::Player* player) const;
  
  static const LayoutValue& GetWidth();
  static const LayoutValue& GetHeight();
  static const LayoutSize& GetSize();
  
 protected:
  UMP_GETTER(Client, client_);
  UMP_GETTER(Player, player_);

  virtual void renderInfo(LayoutRenderer& renderer,
                          const LayoutPos& offset);
  
  void renderHais(LayoutRenderer& renderer,
                  const LayoutPos& offset) const;
  virtual void renderKawa(LayoutRenderer& renderer,
                          const LayoutPos& offset) const;

  LayoutPos renderMentsu(LayoutRenderer& renderer, 
                         LayoutPos pos, 
                         const ump::mj::Mentsu& mentsu) const;
  
  const bool isTurn() const;
  
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}

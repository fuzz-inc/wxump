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
	@brief 
***************************************************************************/
class Application
  : public wxApp
{
  typedef wxApp super;

 public:
  enum HaiSize {
    SMALL, 
    MIDDLE, 
    LARGE
  };

  class Listener;

 private:
  std::unique_ptr<Config> config_;
  std::unique_ptr<LayoutRenderer> layoutRenderer_;
  MainFrame* mainFrame_;
  std::shared_ptr<ump::server::Config> mjConfig_;
  std::unique_ptr<ump::server::Thread> server_;
  std::mutex mutex_;
  std::vector<Listener*> listeners_;
  size_t choice_; /**< 選択した牌番号 */
  int fps_;
  
 public:
  Application();
  ~Application() override;

  Config& getConfig() const {
    return *config_;
  }

  LayoutRenderer& getLayoutRenderer() const {
    return *layoutRenderer_;
  }

  MainFrame* getMainFrame() const {
    return mainFrame_;
  }

  UMP_GETTER(MjConfig, mjConfig_);

  std::shared_ptr<ump::socket::Socket> createSocket() const;
  
  std::mutex& getMutex() {
    return mutex_;
  }
  
  const int getFPS() const {
    return fps_;
  }
  
  UMP_ACCESSOR(Choice, choice_);

  void attachListener(Listener* listener);

  void onReceiveCommand(const ump::Command& command);
  void onSelectHai(size_t index);
  void onCancel();

  void changeHaiSize(HaiSize size);

  static Application* Get() {
    return static_cast<Application*>(super::GetInstance());
  }

 protected:
  bool OnInit() override;
  void OnInitCmdLine(wxCmdLineParser& parser) override;
  bool OnCmdLineParsed(wxCmdLineParser& parser) override;
  int OnExit() override;
};
/***********************************************************************//**
	@brief 
***************************************************************************/
class Application::Listener {
 public:
  Listener() = default;
  virtual ~Listener() = default;

  /**
   * UMPコマンドを受信したときの処理
   * @param[in] command UMPコマンド
   */
  virtual void onReceiveCommand(const ump::Command& command) {}

  /**
   *
   */
  virtual void onSelectHai(size_t index){}

  /**
   *
   */
  virtual void onCancel() {}

  /**
   * レイアウトが変更されたときの処理
   * @param[in] renderer レンダラー
   */
  virtual void onChangeLayout(const LayoutRenderer& renderer) {}
};
/***********************************************************************//**
	$Id$
***************************************************************************/
}

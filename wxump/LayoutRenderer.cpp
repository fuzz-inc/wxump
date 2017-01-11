/***********************************************************************//**
	@file
***************************************************************************/
#include "wxump/HaiImage.hpp"
#include "wxump/HaiObject.hpp"
#include "wxump/Layout.hpp"
#include "wxump/LayoutRenderer.hpp"

namespace wxump {
static const int MARGIN = 8;
/***********************************************************************//**
	@brief コンストラクタ
***************************************************************************/
LayoutRenderer::LayoutRenderer() {
}
/***********************************************************************//**
	@brief デストラクタ
***************************************************************************/
LayoutRenderer::~LayoutRenderer() {
}
/***********************************************************************//**
	@brief 描画を開始する
	@param[in] dc デバイスコンテキスト
***************************************************************************/
void LayoutRenderer::beginRender(std::shared_ptr<wxDC> dc) {
  wxASSERT(!dc_);
  dc_ = dc;
}
/***********************************************************************//**
	@brief 描画を終了する
***************************************************************************/
void LayoutRenderer::endRender() {
  dc_.reset();
}
/***********************************************************************//**
	@brief 牌画像を読み込む
	@param[in] index 牌のサイズ
***************************************************************************/
void LayoutRenderer::loadHaiImage(Application::HaiSize index) {
  static const wxString TABLES[] = {
    "36x54",
    "40x60",
    "48x72"
  };
  wxString prefix = wxStandardPaths::Get().GetResourcesDir() + 
    "/asset/" + TABLES[index] + "/";
  haiImage_.reset(new HaiImage(prefix + "design00a.png", 
                               prefix + "design00b.png", 
                               prefix + "base00.png"));
}
/***********************************************************************//**
	@brief 牌を描画する
	@param[in] layoutPos 描画位置
	@param[in] object 牌オブジェクト
***************************************************************************/
void LayoutRenderer::renderHai(const LayoutPos& layoutPos, 
                               const HaiObject& object) {
  auto pos = getPos(layoutPos);
  if(object.isSelect()) {
    pos.y -= 4;
  }
  wxBitmap bitmap(haiImage_->getBitmap(object.getHai()));
  size_t blue = 0xff;
  size_t alpha = 0xff;
  size_t green = 0xff;
  if(object.isNakiQ()) {
    alpha = object.getAlpha();
  }
  else if(object.isNaki()) {
    alpha = 0x7f;
  }
  if(object.isTsumogiri()) {
    blue = 0xc0;
  }
  if(object.isSelected()) {
    green = 0xc0;
    blue = 0xc0;
  }
  bitmap = ApplyColor(bitmap, wxColour(0xff, green, blue, alpha));
  if(object.isRichi()) {
    bitmap = wxBitmap(bitmap.ConvertToImage().Rotate90(false));
    pos.y += haiImage_->getHeight() - haiImage_->getWidth();
  }
  getDc().DrawBitmap(bitmap, pos);
}
/***********************************************************************//**
	@brief テキストを描画する
	@param[in] layoutRect 描画範囲(widthが負のときは右寄せ)
	@param[in] text テキスト
	@param[in] color テキストの色
	@param[in] alignment アライメント
***************************************************************************/
void LayoutRenderer::renderText(const LayoutRect& layoutRect, 
                                const wxString& text, 
                                const wxColour& color, 
                                float alignment) {
  auto& dc = getDc();
  auto rect = getRect(layoutRect);
  wxFont font(dc.GetFont());
  font.SetPixelSize(wxSize(rect.GetHeight(), rect.GetHeight()));
  dc.SetFont(font);
  dc.SetTextForeground(color);
  auto textSize = dc.GetTextExtent(text);
  rect.x += (rect.width - textSize.GetWidth()) * alignment;
  dc.DrawText(text, rect.GetPosition());
}
/***********************************************************************//**
	@brief ビットマップを描画する
	@param[in] pos 描画位置
	@param[in] bitmap ビットマップ
***************************************************************************/
void LayoutRenderer::renderBitmap(const LayoutPos& pos, 
                                  const wxBitmap& bitmap) {
  getDc().DrawBitmap(bitmap, getPos(pos));
}
/***********************************************************************//**
	@brief 矩形を描画する
	@param[in] layoutRect 矩形
	@param[in] color 塗り潰す色
	@param[in] edgeColor 縁の色
	@param[in] layoutRadius 丸み
***************************************************************************/
void LayoutRenderer::renderRect(const LayoutRect& layoutRect, 
                                const wxColour& color, 
                                const wxColour& edgeColor, 
                                const LayoutValue& layoutRadius) {
  auto& dc = getDc();
  dc.SetBrush((color.Alpha() > 0) ? wxBrush(color) : wxNullBrush);
  dc.SetPen(edgeColor.Alpha() > 0 ? wxPen(edgeColor) : wxNullPen);
  auto rect = getRect(layoutRect);
  auto radius = getLayoutValue(layoutRadius);
  if(radius > 0) {
    dc.DrawRoundedRectangle(rect, radius);
  }
  else {
    dc.DrawRectangle(rect);
  }
}
/***********************************************************************//**
	@brief 線を描画する
  @param[in] start 端点1
  @param[in] end 端点2
  @param[in] color 色
***************************************************************************/
void LayoutRenderer::renderLine(const LayoutPos& start,
                                const LayoutPos& end,
                                const wxColour& color) {
  auto& dc = getDc();
  dc.SetPen((color.Alpha() > 0) ? wxPen(color) : wxNullPen);
  dc.DrawLine(getPos(start), getPos(end));
}
/***********************************************************************//**
	@brief 牌単位の位置座標から実数値の位置座標を取得する
  @param[in] pos 牌単位の位置座標
  @return 実数値の位置座標
***************************************************************************/
wxPoint LayoutRenderer::getPos(const LayoutPos& pos) const {
  return wxPoint(getLayoutValue(pos.x), getLayoutValue(pos.y));
}
/***********************************************************************//**
	@brief 牌単位のサイズから実数値のサイズを取得する
  @param[in] size 牌単位のサイズ
  @return 実数値のサイズ
***************************************************************************/
wxSize LayoutRenderer::getSize(const LayoutSize& size) const {
  return wxSize(getLayoutValue(size.width), getLayoutValue(size.height));
}
/***********************************************************************//**
	@brief 牌単位の矩形から実数値の矩形を取得する
  @param[in] size 牌単位の矩形
  @return 実数値の矩形
***************************************************************************/
wxRect LayoutRenderer::getRect(const LayoutRect& rect) const {
  return wxRect(getPos(rect.pos), getSize(rect.size));
}
/***********************************************************************//**
	@brief 指定範囲を基準とした位置座標を取得する
  @param[in] layoutRect 描画範囲の短形
  @param[in] pos 全体を基準とした位置座標
  @return 指定範囲を基準とした位置座標
***************************************************************************/
wxPoint LayoutRenderer::getIndex(const LayoutRect& layoutRect, 
                                 const wxPoint& pos) const {
  auto rect = getRect(layoutRect);
  return wxPoint(std::floor(float(pos.x - rect.x) / rect.width), 
                 std::floor(float(pos.y - rect.y) / rect.height));
}
/***********************************************************************//**
	@brief 牌単位の数値から実数値を求める
  @param[in] layout 牌単位の数値
  @return 実数値
***************************************************************************/
int LayoutRenderer::getLayoutValue(const LayoutValue& layout) const {
  wxASSERT(haiImage_);
  return layout.w * haiImage_->getWidth() + 
    layout.h * haiImage_->getHeight() + 
    layout.margin * MARGIN;
}
/***********************************************************************//**
	@brief デバイスコンテキストの取得
***************************************************************************/
wxDC& LayoutRenderer::getDc() const {
  wxASSERT(dc_);
  return *dc_;
}
/***********************************************************************//**
	@brief 画像の色を変更する
  @param[in] bitmap 画像
  @param[in] color 色
  @return 変更した画像のコピー
***************************************************************************/
wxBitmap LayoutRenderer::ApplyColor(const wxBitmap& bitmap, 
                                    const wxColour& color) {
  if(color.Red() == 255 &&
     color.Green() == 255 &&
     color.Blue() == 255 &&
     color.Alpha() == 255) {
    return bitmap;
  }
  wxBitmap copy = 
    bitmap.GetSubBitmap(wxRect(0, 0, bitmap.GetWidth(), bitmap.GetHeight()));
  wxAlphaPixelData data(copy);
  auto pixel = data.GetPixels();
  float rateAlpha = color.Alpha() / 255.0f;
  float rateRed = color.Red() / 255.0f * rateAlpha;
  float rateGreen = color.Green() / 255.0f * rateAlpha;
  float rateBlue = color.Blue() / 255.0f * rateAlpha;
  for(size_t i = 0, n = bitmap.GetWidth() * bitmap.GetHeight(); i < n; i++) {
    pixel.Red() *= rateRed;
    pixel.Green() *= rateGreen;
    pixel.Blue() *= rateBlue;
    pixel.Alpha() *= rateAlpha;
    
    pixel++;
  }
  return copy;
}
/***********************************************************************//**
	$Id$
***************************************************************************/
}
